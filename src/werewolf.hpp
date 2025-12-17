#ifndef WEREWOLF_HPP
#define WEREWOLF_HPP

#include "npc.hpp"

class Werewolf : public NPC {
public:
    Werewolf(const std::string& name, int x, int y);
    
    void accept(NPCVisitor& visitor) override;
    std::string getType() const override { return "Werewolf"; }
};

#endif