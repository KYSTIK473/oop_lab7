#ifndef FACTORY_NPC_HPP
#define FACTORY_NPC_HPP

#include "npc.hpp"
#include "rogue.hpp"
#include "orc.hpp"
#include "werewolf.hpp"
#include <memory>
#include <string>
#include <stdexcept>

class NPCFactory {
public:
    static std::shared_ptr<NPC> createNPC(const std::string& type, 
                                          const std::string& name, 
                                          int x, int y) {
        if (type == "Rogue" || type == "Разбойник") {
            return std::make_shared<Rogue>(name, x, y);
        } else if (type == "Orc" || type == "Орк") {
            return std::make_shared<Orc>(name, x, y);
        } else if (type == "Werewolf" || type == "Оборотень") {
            return std::make_shared<Werewolf>(name, x, y);
        }
        throw std::invalid_argument("Unknown NPC type: " + type);
    }
    
    static bool isValidType(const std::string& type) {
        return type == "Rogue" || type == "Разбойник" ||
               type == "Orc" || type == "Орк" ||
               type == "Werewolf" || type == "Оборотень";
    }
    
    static void printAvailableTypes() {
        std::cout << "Available NPC types:" << std::endl;
        std::cout << "  - Rogue (Разбойник)" << std::endl;
        std::cout << "  - Orc (Орк)" << std::endl;
        std::cout << "  - Werewolf (Оборотень)" << std::endl;
    }
};

#endif