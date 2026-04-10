#include "example.hpp"
#include "factoryMethod.h"
#include "singleton.h"
#include <iostream>

void example() {
  std::cout << "------ Example ------" << std::endl;

  Example example;
  example.printMessage();

  int result = example.add(3, 5);
  std::cout << "3 + 5 = " << result << std::endl;
  std::cout << "---------------------" << std::endl;
}

// 类外实例化，触发构造函数，在main之前完成 --饿汉式
DatabaseManager_1 DatabaseManager_1::INSTANCE;
// 单例实例1-饿汉式
void singleton_1() {
  std::cout << "------ 单例实例1： 饿汉式 ------" << std::endl;

  DatabaseManager_1 &db1 = DatabaseManager_1::getInstance();
  DatabaseManager_1 &db2 = DatabaseManager_1::getInstance();

  db1.excute("SELECT * FROM users");
  db2.excute("INSERT INTO orders VALUES (1)");

  // 验证是否是同一个实例
  std::cout << "是否是同一个实例:" << (&db1 == &db2 ? "是" : "不是")
            << std::endl;
  std::cout << "--------------------------------" << std::endl;
}

void singleton_2() {
  std::cout << "------ 单例实例2： 懒汉式 ------" << std::endl;

  DatabaseManager_2 &db1 = DatabaseManager_2::getInstance();
  DatabaseManager_2 &db2 = DatabaseManager_2::getInstance();

  db1.excute("SELECT * FROM users");
  db2.excute("INSERT INTO orders VALUES (1)");

  // 验证是否是同一个实例
  std::cout << "是否是同一个实例:" << (&db1 == &db2 ? "是" : "不是")
            << std::endl;
  std::cout << "--------------------------------" << std::endl;
}

void factoryMethod() {
  std::cout << "------ 工厂方法类 ------" << std::endl;

  // 不同的子类通过工厂方法创建不同的 Logger
  std::cout << "=== Development ===" << std::endl;
  DevelopmentApplication devApp;
  devApp.doSomething();

  std::cout << "=== Testing ===" << std::endl;
  TestingApplication testApp;
  testApp.doSomething();

  std::cout << "=== Production ===" << std::endl;
  ProductionApplication prodApp;
  prodApp.doSomething();
  std::cout << "------------------------" << std::endl;
}

int main() {

  // example();
  singleton_1();
  singleton_2();
  factoryMethod();

  return 0;
}
