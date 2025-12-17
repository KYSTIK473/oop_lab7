#include "werewolf.hpp"
#include "visitor_simulate_fight.hpp"

Werewolf::Werewolf(const std::string& name, int x, int y) 
    : NPC(name, x, y) {}

void Werewolf::accept(NPCVisitor& visitor) {
    visitor.visit(*this);
}