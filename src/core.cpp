#include "core.hpp"

Core::Core() {
    attach(new ConsoleObserver());
    attach(new FileObserver("log.txt"));
}

Core::~Core() {
}

bool Core::isNameUnique(const std::string& name) const {
    for (const auto& npc : npcs) {
        if (npc->getName() == name) {
            return false;
        }
    }
    return true;
}

bool Core::addNPC(const std::string& type, const std::string& name, int x, int y) {
    try {
        if (!isNameUnique(name)) {
            notify("Failed to add NPC: name '" + name + "' is already taken");
            return false;
        }
        
        auto npc = NPCFactory::createNPC(type, name, x, y);
        npcs.push_back(npc);
        
        std::string message = "Added " + type + " '" + name + 
                             "' at (" + std::to_string(x) + ", " + 
                             std::to_string(y) + ")";
        notify(message);
        return true;
    } catch (const std::exception& e) {
        notify("Failed to add NPC: " + std::string(e.what()));
        return false;
    }
}

bool Core::saveToFile(const std::string& filename) {
    std::ofstream file(filename);
    if (!file.is_open()) {
        notify("Failed to open file for writing: " + filename);
        return false;
    }
    
    int aliveCount = 0;
    for (const auto& npc : npcs) {
        if (npc->isAlive()) {
            file << npc->save() << std::endl;
            aliveCount++;
        }
    }
    
    file.close();
    notify("Saved " + std::to_string(aliveCount) + " NPCs to " + filename);
    return true;
}

bool Core::loadFromFile(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        notify("Failed to open file for reading: " + filename);
        return false;
    }
    
    npcs.clear();
    std::string line;
    int count = 0;
    
    while (std::getline(file, line)) {
        auto npc = NPC::load(line);
        if (npc) {
            npcs.push_back(npc);
            count++;
        }
    }
    
    file.close();
    notify("Loaded " + std::to_string(count) + " NPCs from " + filename);
    return true;
}

void Core::printAll() const {
    std::cout << "\n=== NPC List (" << npcs.size() << " total) ===" << std::endl;
    if (npcs.empty()) {
        std::cout << "No NPCs in dungeon" << std::endl;
        return;
    }
    
    int aliveCount = 0;
    for (const auto& npc : npcs) {
        npc->print();
        if (npc->isAlive()) aliveCount++;
    }
    std::cout << "Alive: " << aliveCount << "/" << npcs.size() << std::endl;
}

void Core::simulateBattle(double range) {
    notify("Starting battle simulation with range " + std::to_string(range));
    
    auto npcsCopy = npcs;
    
    for (size_t i = 0; i < npcsCopy.size(); i++) {
        auto attacker = npcsCopy[i];
        if (!attacker->isAlive()) continue;
        
        for (size_t j = 0; j < npcsCopy.size(); j++) {
            if (i == j) continue;
            
            auto defender = npcsCopy[j];
            if (!defender->isAlive()) continue;
            
            if (attacker->distanceTo(*defender) > range) continue;
            
            BattleVisitor visitor(attacker, range);
            visitor.setDefender(defender);
            
            defender->accept(visitor);
            
            if (visitor.didBattleOccur()) {
                std::string message = attacker->getType() + " '" + 
                                     attacker->getName() + "' defeated " +
                                     defender->getType() + " '" + 
                                     defender->getName() + "'";
                notify(message);
            }
        }
    }
    
    size_t before = npcs.size();
    npcs.erase(std::remove_if(npcs.begin(), npcs.end(),
               [](const std::shared_ptr<NPC>& npc) { return !npc->isAlive(); }),
               npcs.end());
    size_t after = npcs.size();
    
    notify("Battle finished. Removed " + std::to_string(before - after) + " dead NPCs");
}

size_t Core::getNPCCount() const { 
    return npcs.size(); 
}

size_t Core::getAliveCount() const {
    return std::count_if(npcs.begin(), npcs.end(),
                        [](const std::shared_ptr<NPC>& npc) { return npc->isAlive(); });
}

std::string Core::npcInfo() const {
    std::string info = "NPC Information:\n";
    for (const auto& npc : npcs) {
        info += "  " + npc->getType() + " '" + npc->getName() + 
               "' at (" + std::to_string(npc->getX()) + ", " + 
               std::to_string(npc->getY()) + ") - " +
               (npc->isAlive() ? "Alive" : "Dead") + "\n";
    }
    return info;
}