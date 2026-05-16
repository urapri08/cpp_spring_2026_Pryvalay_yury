#pragma once
#include "Interfaces.h"
#include <unordered_map>
#include <string>
#include <functional>
#include <memory>
#include <stdexcept>

class SimpleDI {
private:
    std::unordered_map<std::string, std::function<std::shared_ptr<IService>(SimpleDI&)>> factories;
    std::unordered_map<std::string, std::shared_ptr<IService>> singletons;

public:
    template<typename T>
    void registerSingleton(const std::string& name, std::function<std::shared_ptr<T>(SimpleDI&)> factory) {
        factories[name] = [this, name, factory](SimpleDI& c) -> std::shared_ptr<IService> {
            if (singletons.find(name) == singletons.end()) {
                singletons[name] = factory(c);
            }
            return singletons[name];
        };
    }

    template<typename T>
    void registerTransient(const std::string& name, std::function<std::shared_ptr<T>(SimpleDI&)> factory) {
        factories[name] = [factory](SimpleDI& c) -> std::shared_ptr<IService> {
            return factory(c);
        };
    }

    template<typename T>
    std::shared_ptr<T> resolve(const std::string& name) {
        if (factories.find(name) == factories.end()) {
            throw std::runtime_error("Ошибка: Сервис " + name + " не найден!");
        }
        return std::dynamic_pointer_cast<T>(factories[name](*this));
    }
};