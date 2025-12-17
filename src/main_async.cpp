#include "async_game.hpp"
#include <iostream>
#include <thread>
#include <chrono>

int main() {
    std::cout << "=== Balagur Fate 3 - Async Version ===" << std::endl;
    std::cout << "NPC Types: Rogue, Orc, Werewolf, Pegasus" << std::endl;
    std::cout << "Battle Rules:" << std::endl;
    std::cout << "  1. Rogue kills Werewolves" << std::endl;
    std::cout << "  2. Werewolf kills Rogues" << std::endl;
    std::cout << "  3. Orc kills Rogues" << std::endl;
    std::cout << "  4. Pegasus doesn't kill anyone" << std::endl;
    std::cout << "\nMovement rules:" << std::endl;
    std::cout << "  - Rogue: move 10, kill distance 10" << std::endl;
    std::cout << "  - Orc: move 20, kill distance 10" << std::endl;
    std::cout << "  - Werewolf: move 40, kill distance 5" << std::endl;
    std::cout << "  - Pegasus: move 30, kill distance 10" << std::endl;
    std::cout << "======================================\n" << std::endl;
    
    std::cout << "Initializing game with 50 NPCs..." << std::endl;
    
    AsyncGame game;
    
    std::cout << "Starting game for 30 seconds..." << std::endl;
    
    try {
        game.run();
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}