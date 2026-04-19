#pragma once
#include <string>
#include <stdexcept>
#include <memory>

class Money {
private:
  double amount;
public:
  explicit Money(double amt = 0.0) {
    if (amt < 0) throw std::invalid_argument("Сумма не может быть отрицательной");
    amount = amt;
  }
  double getAmount() const { return amount; }
  Money add(const Money& other) const { return Money(amount + other.amount); }
  Money subtract(const Money& other) const {
    if (amount < other.amount) throw std::invalid_argument("Недостаточно средств на счете");
    return Money(amount - other.amount);
  }
};

enum class OperationType { INCOME, EXPENSE };

class IVisitor;

class BankAccount {
private:
  int id;
  std::string name;
  Money balance;
public:
  BankAccount(int i, std::string n, double start_balance = 0.0)
    : id(i), name(n), balance(start_balance) {
  }

  int getId() const { return id; }
  std::string getName() const { return name; }
  double getBalance() const { return balance.getAmount(); }

  void deposit(double amount) {
    balance = balance.add(Money(amount));
  }
  void withdraw(double amount) {
    balance = balance.subtract(Money(amount));
  }

  void accept(IVisitor& visitor);
};

class Category {
private:
  int id;
  OperationType type;
  std::string name;
public:
  Category(int i, OperationType t, std::string n) : id(i), type(t), name(n) {}
  int getId() const { return id; }
  OperationType getType() const { return type; }
  std::string getName() const { return name; }
  void accept(IVisitor& visitor);
};

class Operation {
private:
  int id;
  OperationType type;
  int accountId;
  int categoryId;
  Money amount;
  std::string date;
public:
  Operation(int i, OperationType t, int accId, int catId, double amt, std::string d)
    : id(i), type(t), accountId(accId), categoryId(catId), amount(amt), date(d) {
  }

  int getId() const { return id; }
  OperationType getType() const { return type; }
  int getAccountId() const { return accountId; }
  int getCategoryId() const { return categoryId; }
  double getAmount() const { return amount.getAmount(); }
  std::string getDate() const { return date; }

  void accept(IVisitor& visitor);
};