#include "../include/DIContainer.h"
#include "../include/Services.h"
#include "../include/Domain.h"
#include <iostream>
#include <cstdlib>
#include <stdexcept>

void hard_crash_test() {
  SimpleDI crash_di;

  crash_di.registerSingleton<IVetClinic>("VetClinic", [](SimpleDI&) {
    return new VetClinic();
    });

  crash_di.registerSingleton<IZoo>("Zoo", [](SimpleDI& c) {
    return new Zoo(c.resolve<IVetClinic>("VetClinic"));
    });

  auto zoo_srv = crash_di.resolve<IZoo>("Zoo");

  zoo_srv->addAnimal(nullptr);
  zoo_srv->addThing(nullptr);

  try {
    auto fake_srv = crash_di.resolve<IVetClinic>("FakeService");
    std::cout << "Ошибка: Контейнер не выбросил исключение!\n";
  }
  catch (const std::runtime_error& e) {
    std::cout << "Контейнер див " << e.what() << "\n";
  }

  for (int i = 0; i < 10000; ++i) {
    zoo_srv->addAnimal(new Wolf("Клон Нитаньяху", 5, 10000 + i, false));
    zoo_srv->addAnimal(new Rabbit("Клон Багирова", 2, 20000 + i, 10, true));
    zoo_srv->addThing(new Table(30000 + i));
  }

  std::cout << "Стресс-тест пройден\n";
}

int main() {
  system("chcp 65001 > nul");

  hard_crash_test();

  SimpleDI container;

  container.registerSingleton<IVetClinic>("VetClinic", [](SimpleDI&) {
    return new VetClinic();
    });

  container.registerSingleton<IZoo>("Zoo", [](SimpleDI& c) {
    return new Zoo(c.resolve<IVetClinic>("VetClinic"));
    });

  auto zoo_service = container.resolve<IZoo>("Zoo");

  zoo_service->addAnimal(new Monkey("Обезьяна Бама", 3, 1001, 8, true));
  zoo_service->addAnimal(new Rabbit("Кролик Джефри", 1, 1002, 3, true));
  zoo_service->addAnimal(new Tiger("Тигр Лунтик ", 10, 1003, true));
  zoo_service->addAnimal(new Wolf("Волк Вульф", 5, 1004, false));
  zoo_service->addAnimal(new Rabbit("Кролик кролик", 2, 1005, 10, true));
  zoo_service->addThing(new Table(2001));
  zoo_service->addThing(new Computer(2002));
  std::cout << "\nВсего еды требуется в день: " << zoo_service->getTotalFood() << " кг\n";
  zoo_service->printContactZoo();
  zoo_service->printInventory();
  return 0;
}