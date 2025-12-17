#include "orc.hpp"
#include "visitor_simulate_fight.hpp"

Orc::Orc(const std::string& name, int x, int y) 
    : NPC(name, x, y) {}

void Orc::accept(NPCVisitor& visitor) {
    visitor.visit(*this);
}