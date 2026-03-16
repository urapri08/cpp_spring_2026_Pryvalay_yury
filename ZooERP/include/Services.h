#pragma once
#include "Interfaces.h"
#include "Domain.h"
#include <vector>
#include <iostream>

class VetClinic : public IVetClinic {
public:
  bool checkHealth(const Animal& animal) override {
    return animal.isHealthy();
  }
};

class IZoo : public IService {
public:
  virtual void addAnimal(Animal* animal) = 0;
  virtual void addThing(Thing* thing) = 0;
  virtual int getTotalFood() const = 0;
  virtual void printContactZoo() const = 0;
  virtual void printInventory() const = 0;
};

class Zoo : public IZoo {
private:
  IVetClinic* clinic;
  std::vector<Animal*> animals;
  std::vector<Thing*> things;

public:
  Zoo(IVetClinic* c) : clinic(c) {}
  ~Zoo() {
    for (auto a : animals) {
      delete a;
    }
    for (auto t : things) {
      delete t;
    }
  }

  void addAnimal(Animal* animal) override {
    if (!animal) return;

    if (clinic->checkHealth(*animal)) {
      animals.push_back(animal);
      std::cout << "[+] " << animal->getName() << " успешно прошел медосмотр и добавлен в зоопарк.\n";
    }
    else {
      std::cout << "[-] " << animal->getName() << " болен Клиника его не пропустила.\n";
      delete animal;
    }
  }

  void addThing(Thing* thing) override {
    if (!thing) return;
    things.push_back(thing);
  }

  int getTotalFood() const override {
    int total_food = 0;
    for (const auto a : animals) {
      total_food += a->getFood();
    }
    return total_food;
  }

  void printContactZoo() const override {
    std::cout << "\n Животные для контактного зоопарка\n";
    for (const auto a : animals) {
      if (auto player = dynamic_cast<IPlayerAnimal*>(a)) {
        if (player->canPlay()) {
          std::cout << a->getName() << " (Инв. номер: " << a->getNumber() << ")\n";
        }
      }
    }
  }

  void printInventory() const override {
    std::cout << "\nИнвкнтаризация Зоопарка\n";
    for (const auto a : animals) {
      std::cout << "Животное: " << a->getName() << " [Номер:" << a->getNumber() << "]\n";
    }
    for (const auto t : things) {
      std::cout << "Вещь: " << t->getName() << " [Номер:" << t->getNumber() << "]\n";
    }
  }
};