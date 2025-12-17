#include "core.hpp"
#include "factory_npc.hpp"
#include <iostream>
#include <cstdlib>
#include <ctime>
#include <limits>

void printMenu() {
    std::cout << "\n=== Balagur Fate 3 - Dungeon Editor ===" << std::endl;
    std::cout << "1. Add new NPC" << std::endl;
    std::cout << "2. Print all NPCs" << std::endl;
    std::cout << "3. Save to file" << std::endl;
    std::cout << "4. Load from file" << std::endl;
    std::cout << "5. Simulate battle" << std::endl;
    std::cout << "6. Show NPC info" << std::endl;
    std::cout << "7. Show battle rules (Variant 10)" << std::endl;
    std::cout << "8. Clear dungeon" << std::endl;
    std::cout << "0. Exit" << std::endl;
    std::cout << "=======================================" << std::endl;
    std::cout << "Choice: ";
}

void printBattleRules() {
    std::cout << "\n=== Battle Rules (Variant 10) ===" << std::endl;
    std::cout << "1. Rogue kills Werewolves" << std::endl;
    std::cout << "2. Werewolf kills Rogues" << std::endl;
    std::cout << "3. Orc kills Rogues" << std::endl;
    std::cout << "================================" << std::endl;
}

void clearInput() {
    std::cin.clear();
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}

int main() {
    std::srand(std::time(nullptr));
    Core dungeonCore;
    
    int choice;
    bool running = true;
    
    dungeonCore.addNPC("Rogue", "Shadow", 50, 50);
    dungeonCore.addNPC("Orc", "Grom", 150, 150);
    dungeonCore.addNPC("Werewolf", "Fenrir", 250, 250);
    dungeonCore.addNPC("Rogue", "Bandit", 350, 350);
    dungeonCore.addNPC("Werewolf", "Lycan", 450, 450);
    
    while (running) {
        printMenu();
        std::cin >> choice;
        clearInput();
        
        switch (choice) {
            case 1: {
                std::string type, name;
                int x, y;
                
                NPCFactory::printAvailableTypes();
                std::cout << "Enter NPC type: ";
                std::getline(std::cin, type);
                
                std::cout << "Enter NPC name: ";
                std::getline(std::cin, name);
                
                std::cout << "Enter X coordinate (0-500): ";
                std::cin >> x;
                std::cout << "Enter Y coordinate (0-500): ";
                std::cin >> y;
                clearInput();
                
                if (dungeonCore.addNPC(type, name, x, y)) {
                    std::cout << "NPC added successfully!" << std::endl;
                } else {
                    std::cout << "Failed to add NPC!" << std::endl;
                }
                break;
            }
            
            case 2:
                dungeonCore.printAll();
                break;
                
            case 3: {
                std::string filename;
                std::cout << "Enter filename to save: ";
                std::getline(std::cin, filename);
                if (filename.empty()) filename = "dungeon_save.txt";
                
                if (dungeonCore.saveToFile(filename)) {
                    std::cout << "Saved successfully!" << std::endl;
                }
                break;
            }
            
            case 4: {
                std::string filename;
                std::cout << "Enter filename to load: ";
                std::getline(std::cin, filename);
                if (filename.empty()) filename = "dungeon_save.txt";
                
                if (dungeonCore.loadFromFile(filename)) {
                    std::cout << "Loaded successfully!" << std::endl;
                }
                break;
            }
            
            case 5: {
                double range;
                std::cout << "Enter battle range: ";
                std::cin >> range;
                clearInput();
                
                std::cout << "Simulating battle..." << std::endl;
                dungeonCore.simulateBattle(range);
                std::cout << "Battle simulation complete!" << std::endl;
                break;
            }
            
            case 6:
                std::cout << dungeonCore.npcInfo() << std::endl;
                break;
                
            case 7:
                printBattleRules();
                break;
                
            case 8:
                dungeonCore.saveToFile("backup.txt");
                dungeonCore.loadFromFile("empty.txt");
                std::cout << "Dungeon cleared! Backup saved to backup.txt" << std::endl;
                break;
                
            case 0:
                running = false;
                std::cout << "Exiting..." << std::endl;
                break;
                
            default:
                std::cout << "Invalid choice! Try again." << std::endl;
        }
    }
    
    return 0;
}