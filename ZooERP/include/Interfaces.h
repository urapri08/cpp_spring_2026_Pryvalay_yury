#pragma once
#include <string>

class IAlive {
public:
  virtual ~IAlive() = default;
  virtual int getFood() const = 0;
};

class IInventory {
public:
  virtual ~IInventory() = default;
  virtual int getNumber() const = 0;
  virtual std::string getName() const = 0;
};
class IPlayerAnimal {
public:
  virtual ~IPlayerAnimal() = default;
  virtual bool canPlay() const = 0;
};

class Animal;

class IService {
public:
  virtual ~IService() = default;
};

class IVetClinic : public IService {
public:
  virtual bool checkHealth(const Animal& animal) = 0;
};