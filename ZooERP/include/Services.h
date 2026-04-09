#pragma once
#include "Interfaces.h"
#include "Domain.h"
#include <vector>
#include <iostream>
#include <memory>

class VetClinic : public IVetClinic {
public:
    bool checkHealth(const Animal& animal) override {
        return animal.isHealthy();
    }
};

class IZoo : public IService {
public:
    virtual void addAnimal(std::shared_ptr<Animal> animal) = 0;
    virtual void addThing(std::shared_ptr<Thing> thing) = 0;
    virtual int getTotalFood() const = 0;
    virtual void printContactZoo() const = 0;
    virtual void printInventory() const = 0;
};

class Zoo : public IZoo {
private:
    std::shared_ptr<IVetClinic> clinic;
    std::vector<std::shared_ptr<Animal>> animals;
    std::vector<std::shared_ptr<Thing>> things;

public:
    Zoo(std::shared_ptr<IVetClinic> c) : clinic(c) {}

    void addAnimal(std::shared_ptr<Animal> animal) override {
        if (!animal) return;
        
        if (clinic->checkHealth(*animal)) {
            animals.push_back(animal);
            std::cout << "[+] " << animal->getName() << " прошел медосмотр и добавлен в зоопарк.\n";
        } else {
            std::cout << "[-] " << animal->getName() << " болен! Клиника его не пропустила.\n";
        }
    }

    void addThing(std::shared_ptr<Thing> thing) override {
        if (!thing) return;
        things.push_back(thing);
        std::cout << "[+] " << thing->getName() << " добавлен в инвентарь.\n";
    }

    int getTotalFood() const override {
        int total_food = 0;
        for (const auto& a : animals) {
            total_food += a->getFood();
        }
        return total_food;
    }

    void printContactZoo() const override {
        std::cout << "\n--- Животные для контактного зоопарка ---\n";
        for (const auto& a : animals) {
            if (auto player = std::dynamic_pointer_cast<IPlayerAnimal>(a)) {
                if (player->canPlay()) {
                    std::cout << a->getName() << " (Инв. номер: " << a->getNumber() << ")\n";
                }
            }
        }
    }

    void printInventory() const override {
        std::cout << "\n--- Инвентаризация ---\n";
        for (const auto& a : animals) {
            std::cout << "Животное: " << a->getName() << " [Номер: " << a->getNumber() << "]\n";
        }
        for (const auto& t : things) {
            std::cout << "Вещь: " << t->getName() << " [Номер: " << t->getNumber() << "]\n";
        }
    }
};