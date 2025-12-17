#include "npc.hpp"
#include "visitor_simulate_fight.hpp"
#include "factory_npc.hpp"

NPC::NPC(const std::string& name, int x, int y) 
    : name(name), x(x), y(y), alive(true) {
    if (x < 0 || x > 500 || y < 0 || y > 500) {
        throw std::invalid_argument("Coordinates must be between 0 and 500");
    }
}

void NPC::print() const {
    std::cout << getType() << " '" << name 
              << "' at (" << x << ", " << y 
              << ") - " << (alive ? "Alive" : "Dead") << std::endl;
}

void NPC::setPosition(int newX, int newY) {
    if (newX >= 0 && newX <= 500 && newY >= 0 && newY <= 500) {
        x = newX;
        y = newY;
    }
}

double NPC::distanceTo(const NPC& other) const {
    int dx = x - other.x;
    int dy = y - other.y;
    return std::sqrt(dx*dx + dy*dy);
}

std::string NPC::save() const {
    std::ostringstream oss;
    oss << getType() << "," << name << "," << x << "," << y << "," << alive;
    return oss.str();
}

std::shared_ptr<NPC> NPC::load(const std::string& data) {
    std::string type, name;
    int x, y;
    bool alive;
    
    size_t pos1 = data.find(',');
    size_t pos2 = data.find(',', pos1 + 1);
    size_t pos3 = data.find(',', pos2 + 1);
    size_t pos4 = data.find(',', pos3 + 1);
    
    if (pos1 == std::string::npos || pos2 == std::string::npos || 
        pos3 == std::string::npos || pos4 == std::string::npos) {
        return nullptr;
    }
    
    type = data.substr(0, pos1);
    name = data.substr(pos1 + 1, pos2 - pos1 - 1);
    x = std::stoi(data.substr(pos2 + 1, pos3 - pos2 - 1));
    y = std::stoi(data.substr(pos3 + 1, pos4 - pos3 - 1));
    alive = std::stoi(data.substr(pos4 + 1)) != 0;
    
    auto npc = NPCFactory::createNPC(type, name, x, y);
    if (npc && !alive) {
        npc->die();
    }
    return npc;
}