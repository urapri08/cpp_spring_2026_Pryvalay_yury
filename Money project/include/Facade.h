#pragma once
#include "Patterns.h"

class FinanceFacade {
private:
  std::shared_ptr<IAccountRepo> accounts;
  std::vector<std::shared_ptr<Operation>> operations;
  int opCounter = 1;
public:
  FinanceFacade() {
    accounts = std::make_shared<CachedAccountRepoProxy>();
  }

  void executeCommand(std::shared_ptr<ICommand> cmd) {
    auto timedCmd = std::make_shared<TimingDecorator>(cmd);
    timedCmd->execute();
  }

  void createAccount(int id, const std::string& name, double balance) {
    auto cmd = std::make_shared<AddAccountCommand>(accounts, id, name, balance);
    executeCommand(cmd);
  }

  void doOperation(int accId, OperationType type, double amount, const std::string& date) {
    try {
      auto acc = accounts->get(accId);
      auto op = FinanceFactory::createOperation(opCounter++, type, acc, 0, amount, date);
      operations.push_back(op);
      std::cout << "Операция успешна! Баланс обновлен.\n";
    }
    catch (const std::exception& e) {
      std::cout << "Ошибка: " << e.what() << "\n";
    }
  }

  void printBalances() {
    BalanceReport report(accounts);
    report.generate();
  }

  void exportToCsv() {
    std::cout << "\n--- Экспорт данных ---\n";
    CsvExportVisitor exporter;
    for (auto& acc : accounts->getAll()) acc->accept(exporter);
    for (auto& op : operations) op->accept(exporter);
  }
};