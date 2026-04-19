#include "../include/Facade.h"
#include <iostream>
#include <string>

#ifdef _WIN32
#include <windows.h>
#endif

void printMenu() {
  std::cout << "\n=== Ф-Банк: Учет финансов ===\n";
  std::cout << "1. Открыть новый счет\n";
  std::cout << "2. Зафиксировать доход\n";
  std::cout << "3. Зафиксировать расход\n";
  std::cout << "4. Вывести балансы (Шаблонный метод)\n";
  std::cout << "5. Экспорт в CSV (Посетитель)\n";
  std::cout << "0. Выход\n";
  std::cout << "Ваш выбор: ";
}

int main() {
#ifdef _WIN32
  SetConsoleOutputCP(CP_UTF8);
  SetConsoleCP(CP_UTF8);
#else
  setlocale(LC_ALL, "");
#endif

  FinanceFacade app;

  app.createAccount(1, "Главный", 5000.0);
  app.createAccount(2, "Заначка", 1000.0);

  int choice = -1;
  while (choice != 0) {
    printMenu();
    if (!(std::cin >> choice)) {
      std::cin.clear();
      std::cin.ignore(10000, '\n');
      continue;
    }

    if (choice == 1) {
      int id; std::string name; double bal;
      std::cout << "Введите ID счета, Название (без пробелов) и Начальный баланс: ";
      std::cin >> id >> name >> bal;
      app.createAccount(id, name, bal);
    }
    else if (choice == 2) {
      int accId; double amt;
      std::cout << "Введите ID счета и сумму Дохода: ";
      std::cin >> accId >> amt;
      app.doOperation(accId, OperationType::INCOME, amt, "2026-04-09");
    }
    else if (choice == 3) {
      int accId; double amt;
      std::cout << "Введите ID счета и сумму Расхода: ";
      std::cin >> accId >> amt;
      app.doOperation(accId, OperationType::EXPENSE, amt, "2026-04-09");
    }
    else if (choice == 4) {
      app.printBalances();
    }
    else if (choice == 5) {
      app.exportToCsv();
    }
  }

  return 0;
}