#ifndef ORC_HPP
#define ORC_HPP

#include "npc.hpp"

class Orc : public NPC {
public:
    Orc(const std::string& name, int x, int y);
    
    void accept(NPCVisitor& visitor) override;
    std::string getType() const override { return "Orc"; }
};

#endif