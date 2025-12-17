#include "rogue.hpp"
#include "visitor_simulate_fight.hpp"

Rogue::Rogue(const std::string& name, int x, int y) 
    : NPC(name, x, y) {}

void Rogue::accept(NPCVisitor& visitor) {
    visitor.visit(*this);
}