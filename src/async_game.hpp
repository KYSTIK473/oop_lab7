#ifndef ASYNC_GAME_HPP
#define ASYNC_GAME_HPP

#include <atomic>
#include <thread>
#include <mutex>
#include <shared_mutex>
#include <condition_variable>
#include <vector>
#include <queue>
#include <random>
#include <chrono>
#include <iostream>
#include <map>
#include <algorithm>
#include "async_npc.hpp"

class AsyncGame {
private:
    std::vector<std::shared_ptr<AsyncNPC>> npcs;
    mutable std::shared_mutex npcs_mutex;
    
    std::queue<std::pair<std::shared_ptr<AsyncNPC>, std::shared_ptr<AsyncNPC>>> battle_queue;
    std::mutex battle_mutex;
    std::condition_variable battle_cv;
    
    std::queue<std::string> log_queue;
    std::mutex log_mutex;
    std::condition_variable log_cv;
    
    std::atomic<bool> running{true};
    std::atomic<int> game_time{0};
    
    std::random_device rd;
    std::mt19937 gen;
    
    const int MAP_WIDTH = 100;
    const int MAP_HEIGHT = 100;
    
    struct NPCRules {
        int move_distance;
        int kill_distance;
    };
    
    std::map<std::string, NPCRules> rules = {
        {"Rogue", {10, 10}},
        {"Orc", {20, 10}},
        {"Werewolf", {40, 5}},
        {"Pegasus", {30, 10}}
    };
    
    void log_message(const std::string& message) {
        std::lock_guard<std::mutex> lock(log_mutex);
        log_queue.push(message);
        log_cv.notify_one();
    }
    
public:
    AsyncGame() : gen(rd()) {
        std::uniform_int_distribution<> type_dist(0, 3);
        std::uniform_int_distribution<> coord_dist(0, MAP_WIDTH - 1);
        
        std::vector<std::string> types = {"Rogue", "Orc", "Werewolf", "Pegasus"};
        
        for (int i = 0; i < 50; ++i) {
            int type_idx = type_dist(gen);
            std::string type = types[type_idx];
            std::string name = type + "_" + std::to_string(i);
            int x = coord_dist(gen);
            int y = coord_dist(gen);
            
            auto npc = std::make_shared<AsyncNPC>(name, x, y);
            npc->type = type;
            
            std::lock_guard<std::shared_mutex> lock(npcs_mutex);
            npcs.push_back(npc);
        }
        
        log_message("Game initialized with " + std::to_string(npcs.size()) + " NPCs");
    }
    
    int get_move_distance(const std::string& type) const {
        auto it = rules.find(type);
        return it != rules.end() ? it->second.move_distance : 0;
    }
    
    int get_kill_distance(const std::string& type) const {
        auto it = rules.find(type);
        return it != rules.end() ? it->second.kill_distance : 0;
    }
    
    bool can_kill(const std::string& attacker_type, const std::string& defender_type) {
        if (attacker_type == "Rogue" && defender_type == "Werewolf") {
            return true;
        } else if (attacker_type == "Werewolf" && defender_type == "Rogue") {
            return true;
        } else if (attacker_type == "Orc" && defender_type == "Rogue") {
            return true;
        }
        return false;
    }
    
    int roll_dice() {
        std::uniform_int_distribution<> dist(1, 6);
        return dist(gen);
    }
    
    void movement_thread() {
        std::uniform_int_distribution<> dir_dist(-1, 1);
        std::uniform_int_distribution<> sleep_dist(10, 100);
        
        while (running) {
            std::this_thread::sleep_for(std::chrono::milliseconds(sleep_dist(gen)));
            
            std::shared_lock<std::shared_mutex> read_lock(npcs_mutex);
            auto npcs_copy = npcs;
            read_lock.unlock();
            
            for (auto& npc : npcs_copy) {
                if (!npc->isAlive()) continue;
                
                std::string type = npc->type;
                if (rules.find(type) == rules.end()) continue;
                
                int move_dist = rules[type].move_distance;
                int kill_dist = rules[type].kill_distance;
                
                int dx = dir_dist(gen);
                int dy = dir_dist(gen);
                int new_x = npc->getX() + dx * move_dist;
                int new_y = npc->getY() + dy * move_dist;
                
                new_x = std::max(0, std::min(MAP_WIDTH - 1, new_x));
                new_y = std::max(0, std::min(MAP_HEIGHT - 1, new_y));
                
                npc->setPosition(new_x, new_y);
                
                for (auto& other : npcs_copy) {
                    if (npc == other || !other->isAlive()) continue;
                    
                    double distance = npc->distanceTo(*other);
                    if (distance <= kill_dist) {
                        if (can_kill(type, other->type)) {
                            std::lock_guard<std::mutex> lock(battle_mutex);
                            battle_queue.push({npc, other});
                            battle_cv.notify_one();
                        }
                    }
                }
            }
        }
    }
    
    void battle_thread() {
        while (running) {
            std::pair<std::shared_ptr<AsyncNPC>, std::shared_ptr<AsyncNPC>> battle;
            
            {
                std::unique_lock<std::mutex> lock(battle_mutex);
                battle_cv.wait(lock, [this]() { 
                    return !battle_queue.empty() || !running; 
                });
                
                if (!running && battle_queue.empty()) break;
                
                if (!battle_queue.empty()) {
                    battle = battle_queue.front();
                    battle_queue.pop();
                } else {
                    continue;
                }
            }
            
            auto& attacker = battle.first;
            auto& defender = battle.second;
            
            if (!attacker->isAlive() || !defender->isAlive()) continue;
            
            int attack_power = roll_dice();
            int defense_power = roll_dice();
            
            {
                std::lock_guard<std::mutex> cout_lock(log_mutex);
                if (attack_power > defense_power) {
                    defender->die();
                    log_message(attacker->type + " " + attacker->getName() + 
                               " killed " + defender->type + " " + defender->getName() +
                               " (" + std::to_string(attack_power) + " vs " + 
                               std::to_string(defense_power) + ")");
                } else {
                    log_message(attacker->type + " " + attacker->getName() + 
                               " failed to kill " + defender->type + " " + defender->getName() +
                               " (" + std::to_string(attack_power) + " vs " + 
                               std::to_string(defense_power) + ")");
                }
            }
        }
    }
    
    void logger_thread() {
        while (running) {
            std::string message;
            
            {
                std::unique_lock<std::mutex> lock(log_mutex);
                log_cv.wait_for(lock, std::chrono::milliseconds(100), 
                              [this]() { return !log_queue.empty() || !running; });
                
                if (!running && log_queue.empty()) break;
                
                if (!log_queue.empty()) {
                    message = log_queue.front();
                    log_queue.pop();
                } else {
                    continue;
                }
            }
            
            std::lock_guard<std::mutex> cout_lock(log_mutex);
            std::cout << "[LOG] " << message << std::endl;
        }
    }
    
    void print_map() {
        std::lock_guard<std::mutex> cout_lock(log_mutex);
        
        std::cout << "\n=== Game Time: " << game_time << "s ===" << std::endl;
        std::cout << "Map (" << MAP_WIDTH << "x" << MAP_HEIGHT << "):\n";
        
        std::vector<std::vector<char>> map(MAP_HEIGHT, std::vector<char>(MAP_WIDTH, '.'));
        
        std::shared_lock<std::shared_mutex> lock(npcs_mutex);
        for (const auto& npc : npcs) {
            if (!npc->isAlive()) continue;
            
            int x = npc->getX();
            int y = npc->getY();
            
            if (x >= 0 && x < MAP_WIDTH && y >= 0 && y < MAP_HEIGHT) {
                char symbol = '.';
                if (npc->type == "Rogue") symbol = 'R';
                else if (npc->type == "Orc") symbol = 'O';
                else if (npc->type == "Werewolf") symbol = 'W';
                else if (npc->type == "Pegasus") symbol = 'P';
                
                map[y][x] = symbol;
            }
        }
        lock.unlock();
        
        int display_size = 20;
        for (int y = 0; y < display_size; ++y) {
            for (int x = 0; x < display_size; ++x) {
                int map_x = x * MAP_WIDTH / display_size;
                int map_y = y * MAP_HEIGHT / display_size;
                std::cout << map[map_y][map_x];
            }
            std::cout << std::endl;
        }
        
        int alive = 0;
        int rogues = 0, orcs = 0, werewolves = 0, pegasus = 0;
        
        lock.lock();
        for (const auto& npc : npcs) {
            if (npc->isAlive()) {
                alive++;
                if (npc->type == "Rogue") rogues++;
                else if (npc->type == "Orc") orcs++;
                else if (npc->type == "Werewolf") werewolves++;
                else if (npc->type == "Pegasus") pegasus++;
            }
        }
        lock.unlock();
        
        std::cout << "\nStatistics:" << std::endl;
        std::cout << "Alive: " << alive << "/" << npcs.size() << std::endl;
        std::cout << "Rogues: " << rogues << std::endl;
        std::cout << "Orcs: " << orcs << std::endl;
        std::cout << "Werewolves: " << werewolves << std::endl;
        std::cout << "Pegasus: " << pegasus << std::endl;
        std::cout << "=========================\n" << std::endl;
    }
    
    void run() {
        log_message("Starting async game...");
        
        std::thread movement(&AsyncGame::movement_thread, this);
        std::thread battle(&AsyncGame::battle_thread, this);
        std::thread logger(&AsyncGame::logger_thread, this);
        
        auto start_time = std::chrono::steady_clock::now();
        
        while (game_time < 30 && running) {
            auto now = std::chrono::steady_clock::now();
            game_time = std::chrono::duration_cast<std::chrono::seconds>(now - start_time).count();
            
            if (game_time < 30) {
                print_map();
                std::this_thread::sleep_for(std::chrono::seconds(1));
            }
        }
        
        running = false;
        
        battle_cv.notify_all();
        log_cv.notify_all();
        
        if (movement.joinable()) movement.join();
        if (battle.joinable()) battle.join();
        if (logger.joinable()) logger.join();
        
        std::lock_guard<std::mutex> cout_lock(log_mutex);
        std::cout << "\n=== GAME OVER ===" << std::endl;
        std::cout << "Final survivors:" << std::endl;
        
        std::shared_lock<std::shared_mutex> lock(npcs_mutex);
        for (const auto& npc : npcs) {
            if (npc->isAlive()) {
                std::cout << "  " << npc->type << " '" << npc->getName() 
                          << "' at (" << npc->getX() << ", " << npc->getY() << ")" << std::endl;
            }
        }
        
        int survivors = 0;
        for (const auto& npc : npcs) {
            if (npc->isAlive()) {
                survivors++;
            }
        }
        
        std::cout << "\nTotal survivors: " 
                  << survivors
                  << " out of " << npcs.size() << std::endl;
    }
    
    ~AsyncGame() {
        running = false;
        battle_cv.notify_all();
        log_cv.notify_all();
    }
};

#endif