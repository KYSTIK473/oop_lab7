#ifndef NPC_HPP
#define NPC_HPP

#include <iostream>
#include <string>
#include <memory>
#include <vector>
#include <cmath>
#include <fstream>
#include <sstream>

class NPCVisitor;

class NPC {
protected:
    std::string name;
    int x;
    int y;
    bool alive;
    
public:
    std::string type;
    
    NPC(const std::string& name, int x, int y) 
        : name(name), x(x), y(y), alive(true) {
        if (x < 0 || x > 500 || y < 0 || y > 500) {
            throw std::invalid_argument("Coordinates must be between 0 and 500");
        }
    }
    
    virtual ~NPC() = default;
    
    virtual void accept(NPCVisitor& visitor) = 0;
    virtual std::string getType() const { return type; }
    
    virtual void print() const {
        std::cout << getType() << " '" << name 
                  << "' at (" << x << ", " << y 
                  << ") - " << (alive ? "Alive" : "Dead") << std::endl;
    }
    
    const std::string& getName() const { return name; }
    int getX() const { return x; }
    int getY() const { return y; }
    bool isAlive() const { return alive; }
    
    void setPosition(int newX, int newY) {
        if (newX >= 0 && newX <= 500 && newY >= 0 && newY <= 500) {
            x = newX;
            y = newY;
        }
    }
    void die() { alive = false; }
    
    double distanceTo(const NPC& other) const {
        int dx = x - other.x;
        int dy = y - other.y;
        return std::sqrt(dx*dx + dy*dy);
    }
    
    virtual std::string save() const {
        std::ostringstream oss;
        oss << getType() << "," << name << "," << x << "," << y << "," << alive;
        return oss.str();
    }
    
    static std::shared_ptr<NPC> load(const std::string& data) {
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
        
        auto npc = std::make_shared<NPC>(name, x, y);
        npc->type = type;
        if (!alive) {
            npc->die();
        }
        return npc;
    }
};

#endif