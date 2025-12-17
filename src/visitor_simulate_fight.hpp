#ifndef VISITOR_SIMULATE_FIGHT_HPP
#define VISITOR_SIMULATE_FIGHT_HPP

#include "npc.hpp"
#include "rogue.hpp"
#include "orc.hpp"
#include "werewolf.hpp"
#include <memory>

class NPCVisitor {
public:
    virtual ~NPCVisitor() = default;
    virtual void visit(Rogue& rogue) = 0;
    virtual void visit(Orc& orc) = 0;
    virtual void visit(Werewolf& werewolf) = 0;
};

class BattleVisitor : public NPCVisitor {
private:
    std::shared_ptr<NPC> attacker;
    std::shared_ptr<NPC> defender;
    double battleRange;
    bool battleOccurred;
    
public:
    BattleVisitor(std::shared_ptr<NPC> attacker, double range)
        : attacker(attacker), battleRange(range), battleOccurred(false) {}
    
    void setDefender(std::shared_ptr<NPC> def) { defender = def; }
    bool didBattleOccur() const { return battleOccurred; }
    void reset() { battleOccurred = false; defender = nullptr; }
    
    void visit(Rogue& rogue) override {
        if (!defender || !attacker->isAlive() || !rogue.isAlive()) return;
        
        if (attacker->distanceTo(rogue) > battleRange) return;
        
        if (attacker->getType() == "Orc" && defender->getType() == "Rogue") {
            if (rogue.isAlive()) {
                rogue.die();
                battleOccurred = true;
            }
        }
        else if (attacker->getType() == "Werewolf" && defender->getType() == "Rogue") {
            if (rogue.isAlive()) {
                rogue.die();
                battleOccurred = true;
            }
        }
    }
    
    void visit(Orc& orc) override {
        if (!defender || !attacker->isAlive() || !orc.isAlive()) return;
        
        if (attacker->distanceTo(orc) > battleRange) return;
    }
    
    void visit(Werewolf& werewolf) override {
        if (!defender || !attacker->isAlive() || !werewolf.isAlive()) return;
        
        if (attacker->distanceTo(werewolf) > battleRange) return;
        
        if (attacker->getType() == "Rogue" && defender->getType() == "Werewolf") {
            if (werewolf.isAlive()) {
                werewolf.die();
                battleOccurred = true;
            }
        }
    }
};

#endif