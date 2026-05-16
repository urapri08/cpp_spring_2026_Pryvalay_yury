#pragma once
#include "Interfaces.h"
#include <string>

class Animal : public IAlive, public IInventory {
protected:
    std::string name;
    int food;
    int number;
    bool healthy;
public:
    Animal(std::string n, int f, int num, bool h) : name(n), food(f), number(num), healthy(h) {}
    int getFood() const override { return food; }
    int getNumber() const override { return number; }
    std::string getName() const override { return name; }
    bool isHealthy() const { return healthy; }
};

class Predator : public Animal {
public:
    Predator(std::string n, int f, int num, bool h) : Animal(n, f, num, h) {}
};

class Herbivore : public Animal, public IPlayerAnimal {
protected:
    int kindness;
public:
    Herbivore(std::string n, int f, int num, int k, bool h) : Animal(n, f, num, h), kindness(k) {}
    bool canPlay() const override { return kindness > 5; }
};

class Monkey : public Herbivore {
public:
    Monkey(std::string n, int f, int num, int k, bool h) : Herbivore(n, f, num, k, h) {}
};

class Rabbit : public Herbivore {
public:
    Rabbit(std::string n, int f, int num, int k, bool h) : Herbivore(n, f, num, k, h) {}
};

class Tiger : public Predator {
public:
    Tiger(std::string n, int f, int num, bool h) : Predator(n, f, num, h) {}
};

class Wolf : public Predator {
public:
    Wolf(std::string n, int f, int num, bool h) : Predator(n, f, num, h) {}
};

class Thing : public IInventory {
protected:
    int number;
    std::string name;
public:
    Thing(int num, std::string n) : number(num), name(n) {}
    int getNumber() const override { return number; }
    std::string getName() const override { return name; }
};

class Table : public Thing {
public:
    Table(int num) : Thing(num, "Стол") {}
};

class Computer : public Thing {
public:
    Computer(int num) : Thing(num, "Компьютер") {}
};