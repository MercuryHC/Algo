#ifndef INCLUDE_SINGLETON_H
#define INCLUDE_SINGLETON_H
#include <iostream>

// 饿汉式
// 类初始化时创建全局唯一实例, 在main之前完成初始化
class DatabaseManager_1 {
private:
  static DatabaseManager_1 INSTANCE;
  std::string connectionInfo;

  // NOTE: 私有构造函数，定义在private中，无法在外部创建实例
  DatabaseManager_1() {
    connectionInfo = "12334567";
    std::cout << "Connection established: " << connectionInfo << std::endl;
  }

public:
  // 禁止拷贝构造函数和复制操作，防止复制单例
  DatabaseManager_1(const DatabaseManager_1 &) = delete;
  DatabaseManager_1 &operator=(const DatabaseManager_1 &) = delete;

  // 提供外部访问站点
  static DatabaseManager_1 &getInstance() { return INSTANCE; };

  // 业务方法
  void excute(const std::string &sql);
};

// 懒汉式
// 在第一次使用是创建单例实例
class DatabaseManager_2 {
private:
  std::string connectionInfo;

  // NOTE: 私有构造函数，定义在private中，无法在外部创建实例
  DatabaseManager_2() {
    connectionInfo = "7654321";
    std::cout << "Connection established: " << connectionInfo << std::endl;
  }

public:
  // 禁止拷贝构造函数和复制操作，防止复制单例
  DatabaseManager_2(const DatabaseManager_2 &) = delete;
  DatabaseManager_2 &operator=(const DatabaseManager_2 &) = delete;

  // 提供外部访问站点
  static DatabaseManager_2 &getInstance() {
    // NOTE:  C++11 起，静态局部变量在首次调用时初始化，且保证线程安全
    //  即在调用时创建单例实例
    static DatabaseManager_2 instance;
    return instance;
  }

  // 业务方法
  void excute(const std::string &sql);
};
#endif // !INCLUDE_SINGLETON_H
