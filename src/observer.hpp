#ifndef OBSERVER_HPP
#define OBSERVER_HPP

#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <memory>
#include <algorithm>

class Observer {
public:
    virtual ~Observer() = default;
    virtual void update(const std::string& message) = 0;
};

class ConsoleObserver : public Observer {
public:
    void update(const std::string& message) override {
        std::cout << "[EVENT] " << message << std::endl;
    }
};

class FileObserver : public Observer {
private:
    std::ofstream logFile;
    
public:
    FileObserver(const std::string& filename) {
        logFile.open(filename, std::ios::app);
    }
    
    ~FileObserver() {
        if (logFile.is_open()) {
            logFile.close();
        }
    }
    
    void update(const std::string& message) override {
        if (logFile.is_open()) {
            logFile << "[EVENT] " << message << std::endl;
        }
    }
};

class Observable {
private:
    std::vector<Observer*> observers;
    
public:
    void attach(Observer* observer) {
        observers.push_back(observer);
    }
    
    void detach(Observer* observer) {
        observers.erase(std::remove(observers.begin(), observers.end(), observer), 
                       observers.end());
    }
    
    void notify(const std::string& message) {
        for (auto observer : observers) {
            observer->update(message);
        }
    }
    
    virtual ~Observable() = default;
};

#endif