#include "../include/DIContainer.h"
#include "../include/Services.h"
#include "../include/Domain.h"
#include <iostream>
#include <string>
#include <locale>

void printMenu() {
  std::cout << "\n=== ERP Московского Зоопарка ===\n";
  std::cout << "1. Добавить обезьяну\n";
  std::cout << "2. Добавить кролика\n";
  std::cout << "3. Добавить тигра\n";
  std::cout << "4. Добавить стол\n";
  std::cout << "5. Добавить компьютер\n";
  std::cout << "6. Посчитать требуемую еду\n";
  std::cout << "7. Вывести контактный зоопарк\n";
  std::cout << "8. Вывести инвентаризацию\n";
  std::cout << "0. Выход\n";
  std::cout << "Ваш выбор: ";
}

int main() {

  setlocale(LC_ALL, "");

  SimpleDI container;
  container.registerSingleton<IVetClinic>("VetClinic", [](SimpleDI&) {
    return std::make_shared<VetClinic>();
    });

  container.registerSingleton<IZoo>("Zoo", [](SimpleDI& c) {
    return std::make_shared<Zoo>(c.resolve<IVetClinic>("VetClinic"));
    });

  auto zoo = container.resolve<IZoo>("Zoo");
  int choice = -1;

  while (choice != 0) {
    printMenu();
    if (!(std::cin >> choice)) {
      std::cin.clear();
      std::cin.ignore(10000, '\n');
      continue;
    }

    if (choice == 1) {
      std::cout << "Введите имя (без пробелов), кг еды, инв.номер, доброту(0-10) и здоровье(1-здоров, 0-болен):\n";
      std::string name; int f, n, k; bool h;
      std::cin >> name >> f >> n >> k >> h;
      zoo->addAnimal(std::make_shared<Monkey>(name, f, n, k, h));
    }
    else if (choice == 2) {
      std::cout << "Введите имя (без пробелов), кг еды, инв.номер, доброту(0-10) и здоровье(1-здоров, 0-болен):\n";
      std::string name; int f, n, k; bool h;
      std::cin >> name >> f >> n >> k >> h;
      zoo->addAnimal(std::make_shared<Rabbit>(name, f, n, k, h));
    }
    else if (choice == 3) {
      std::cout << "Введите имя (без пробелов), кг еды, инв.номер и здоровье(1-здоров, 0-болен):\n";
      std::string name; int f, n; bool h;
      std::cin >> name >> f >> n >> h;
      zoo->addAnimal(std::make_shared<Tiger>(name, f, n, h));
    }
    else if (choice == 4) {
      std::cout << "Введите инвентарный номер стола: ";
      int n; std::cin >> n;
      zoo->addThing(std::make_shared<Table>(n));
    }
    else if (choice == 5) {
      std::cout << "Введите инвентарный номер компьютера: ";
      int n; std::cin >> n;
      zoo->addThing(std::make_shared<Computer>(n));
    }
    else if (choice == 6) {
      std::cout << "\nВсего еды требуется в день: " << zoo->getTotalFood() << " кг\n";
    }
    else if (choice == 7) {
      zoo->printContactZoo();
    }
    else if (choice == 8) {
      zoo->printInventory();
    }
  }

  return 0;
}