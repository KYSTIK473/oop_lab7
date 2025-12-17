#ifndef CORE_HPP
#define CORE_HPP

#include "npc.hpp"
#include "factory_npc.hpp"
#include "visitor_simulate_fight.hpp"
#include "observer.hpp"
#include <vector>
#include <memory>
#include <fstream>
#include <algorithm>
#include <string>

class Core : public Observable {
private:
    std::vector<std::shared_ptr<NPC>> npcs;
    
    bool isNameUnique(const std::string& name) const;
    
public:
    Core();
    ~Core();
    
    bool addNPC(const std::string& type, const std::string& name, int x, int y);
    bool saveToFile(const std::string& filename);
    bool loadFromFile(const std::string& filename);
    void printAll() const;
    void simulateBattle(double range);
    
    size_t getNPCCount() const;
    size_t getAliveCount() const;
    std::string npcInfo() const;
};

#endif