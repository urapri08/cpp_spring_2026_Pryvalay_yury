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
    Animal(std::string n, int f, int num, bool h = true) : name(n), food(f), number(num), healthy(h) {}
    int getFood() const override { return food; }
    int getNumber() const override { return number; }
    std::string getName() const override { return name; }
    bool isHealthy() const { return healthy; }
};
class Herbo : public Animal, public IPlayerAnimal {
protected:
    int kindness;
public:
    Herbo(std::string n, int f, int num, int k, bool h = true) : Animal(n, f, num, h), kindness(k) {}
    bool canPlay() const override { return kindness > 5; }
};

class Predator : public Animal {
public:
    Predator(std::string n, int f, int num, bool h = true) : Animal(n, f, num, h) {}
};

class Monkey : public Herbo {
public:
    Monkey(std::string n, int f, int num, int k, bool h = true) : Herbo(n, f, num, k, h) {}
};

class Rabbit : public Herbo {
public:
    Rabbit(std::string n, int f, int num, int k, bool h = true) : Herbo(n, f, num, k, h) {}
};

class Tiger : public Predator {
public:
    Tiger(std::string n, int f, int num, bool h = true) : Predator(n, f, num, h) {}
};

class Wolf : public Predator {
public:
    Wolf(std::string n, int f, int num, bool h = true) : Predator(n, f, num, h) {}
};

class Thing : public IInventory {
protected:
    std::string name;
    int number;
public:
    Thing(std::string n, int num) : name(n), number(num) {}
    int getNumber() const override { return number; }
    std::string getName() const override { return name; }
};

class Table : public Thing {
public:
    Table(int num) : Thing("Стол", num) {}
};
class Computer : public Thing {
public:
    Computer(int num) : Thing("Компьютер", num) {}
};