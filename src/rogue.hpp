#ifndef ROGUE_HPP
#define ROGUE_HPP

#include "npc.hpp"

class Rogue : public NPC {
public:
    Rogue(const std::string& name, int x, int y);
    
    void accept(NPCVisitor& visitor) override;
    std::string getType() const override { return "Rogue"; }
};

#endif