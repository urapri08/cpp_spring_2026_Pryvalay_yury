#pragma once

#include "Interfaces.h" 
#include <unordered_map>
#include <string>
#include <functional>
#include <stdexcept>

class SimpleDI {
private:
  std::unordered_map<std::string, std::function<IService* (SimpleDI&)>> factories;
  std::unordered_map<std::string, IService*> singletons;

public:
  ~SimpleDI() {
    for (auto& pair : singletons) {
      delete pair.second;
    }
  }

  template<typename T>
  void registerSingleton(const std::string& name, std::function<T* (SimpleDI&)> factory) {
    factories[name] = [this, name, factory](SimpleDI& c) -> IService* {
      if (singletons.find(name) == singletons.end()) {
        singletons[name] = factory(c);
      }
      return singletons[name];
      };
  }

  template<typename T>
  T* resolve(const std::string& name) {
    if (factories.find(name) == factories.end()) {
      throw std::runtime_error("Ошибка: Срвис ненайден");
    }
    return static_cast<T*>(factories[name](*this));
  }
};