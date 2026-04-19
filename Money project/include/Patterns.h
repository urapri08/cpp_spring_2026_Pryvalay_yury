#pragma once
#include "Domain.h"
#include <iostream>
#include <vector>
#include <memory>
#include <chrono>

class IVisitor {
public:
  virtual void visit(BankAccount& acc) = 0;
  virtual void visit(Category& cat) = 0;
  virtual void visit(Operation& op) = 0;
  virtual ~IVisitor() = default;
};

class CsvExportVisitor : public IVisitor {
public:
  void visit(BankAccount& acc) override {
    std::cout << "CSV_ACC: " << acc.getId() << "," << acc.getName() << "," << acc.getBalance() << "\n";
  }
  void visit(Category& cat) override {
    std::string t = (cat.getType() == OperationType::INCOME) ? "INC" : "EXP";
    std::cout << "CSV_CAT: " << cat.getId() << "," << t << "," << cat.getName() << "\n";
  }
  void visit(Operation& op) override {
    std::cout << "CSV_OP: " << op.getId() << "," << op.getAccountId() << "," << op.getAmount() << "," << op.getDate() << "\n";
  }
};

inline void BankAccount::accept(IVisitor& visitor) { visitor.visit(*this); }
inline void Category::accept(IVisitor& visitor) { visitor.visit(*this); }
inline void Operation::accept(IVisitor& visitor) { visitor.visit(*this); }

class FinanceFactory {
public:
  static std::shared_ptr<Operation> createOperation(int id, OperationType type, std::shared_ptr<BankAccount> acc, int catId, double amount, const std::string& date) {
    if (type == OperationType::EXPENSE) {
      acc->withdraw(amount);
    }
    else {
      acc->deposit(amount);
    }
    return std::make_shared<Operation>(id, type, acc->getId(), catId, amount, date);
  }
};

class IAccountRepo {
public:
  virtual void add(std::shared_ptr<BankAccount> acc) = 0;
  virtual std::shared_ptr<BankAccount> get(int id) = 0;
  virtual std::vector<std::shared_ptr<BankAccount>> getAll() = 0;
  virtual ~IAccountRepo() = default;
};

class RealAccountRepo : public IAccountRepo {
private:
  std::vector<std::shared_ptr<BankAccount>> db;
public:
  void add(std::shared_ptr<BankAccount> acc) override { db.push_back(acc); }
  std::shared_ptr<BankAccount> get(int id) override {
    for (auto& a : db) if (a->getId() == id) return a;
    throw std::runtime_error("Счет не найден");
  }
  std::vector<std::shared_ptr<BankAccount>> getAll() override { return db; }
};

class CachedAccountRepoProxy : public IAccountRepo {
private:
  std::shared_ptr<RealAccountRepo> realRepo;
  std::shared_ptr<BankAccount> cache;
public:
  CachedAccountRepoProxy() { realRepo = std::make_shared<RealAccountRepo>(); }
  void add(std::shared_ptr<BankAccount> acc) override {
    realRepo->add(acc);
    cache = acc;
  }
  std::shared_ptr<BankAccount> get(int id) override {
    if (cache && cache->getId() == id) return cache;
    cache = realRepo->get(id);
    return cache;
  }
  std::vector<std::shared_ptr<BankAccount>> getAll() override { return realRepo->getAll(); }
};

class ReportTemplate {
protected:
  virtual void printHeader() = 0;
  virtual void printData() = 0;
  virtual void printFooter() { std::cout << "--- Конец отчета ---\n"; }
public:
  void generate() {
    printHeader();
    printData();
    printFooter();
  }
  virtual ~ReportTemplate() = default;
};

class BalanceReport : public ReportTemplate {
private:
  std::shared_ptr<IAccountRepo> repo;
public:
  BalanceReport(std::shared_ptr<IAccountRepo> r) : repo(r) {}
protected:
  void printHeader() override { std::cout << "\n=== Отчет по балансам ===\n"; }
  void printData() override {
    for (auto& acc : repo->getAll()) {
      std::cout << "Счет: " << acc->getName() << " | Баланс: " << acc->getBalance() << "\n";
    }
  }
};

class ICommand {
public:
  virtual void execute() = 0;
  virtual ~ICommand() = default;
};

class AddAccountCommand : public ICommand {
private:
  std::shared_ptr<IAccountRepo> repo;
  int id;
  std::string name;
  double bal;
public:
  AddAccountCommand(std::shared_ptr<IAccountRepo> r, int i, std::string n, double b) : repo(r), id(i), name(n), bal(b) {}
  void execute() override {
    repo->add(std::make_shared<BankAccount>(id, name, bal));
    std::cout << "Счет '" << name << "' добавлен.\n";
  }
};

class TimingDecorator : public ICommand {
private:
  std::shared_ptr<ICommand> wrapped;
public:
  TimingDecorator(std::shared_ptr<ICommand> cmd) : wrapped(cmd) {}
  void execute() override {
    auto start = std::chrono::high_resolution_clock::now();
    wrapped->execute();
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::milli> ms = end - start;
    std::cout << "[SYSTEM] Время выполнения: " << ms.count() << " ms\n";
  }
};