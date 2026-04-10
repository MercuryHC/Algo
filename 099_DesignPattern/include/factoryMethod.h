#ifndef INCLUDE_FACTORYMETHOD_H
#define INCLUDE_FACTORYMETHOD_H
#include <ctime>
#include <memory>
#include <string>
#include <type_traits>

// 工厂方法模式：
// 定义一个用于创建对象的接口(工厂方法),让实现该接口的子类来决定到底要实例化哪一个具体的类
// 简单来说就是父类暴露了一个方法，让子类根据自己的需求实现

// 日志类
class ILogger {

public:
  // 暴露的接口，子类可实现自己的需求
  virtual ~ILogger() = default;
  virtual void log(const std::string &message) = 0;
};

// 子类通过 override实现各自的方法内容
// 子类1： 控制台日志记录器
class ConsoleLogger : public ILogger {

public:
  void log(const std::string &message) override;
};

// 子类2： 文件日志记录器
class FileLogger : public ILogger {
private:
  std::string filePath;

public:
  FileLogger(const std::string &filePath) : filePath(filePath) {}
  void log(const std::string &message) override;
};

// 子类3 ：远程日志记录器
class RemoteLogger : public ILogger {
private:
  std::string remoteServer;

public:
  RemoteLogger(const std::string &remoteServer) : remoteServer(remoteServer) {}
  void log(const std::string &message) override;
};

// 应用类
class Application {
private:
  std::unique_ptr<ILogger> logger;

public:
  Application(std::unique_ptr<ILogger> logger) : logger(std::move(logger)) {}
  virtual ~Application() = default;

  // 业务方法
  void doSomething() { logger->log("Start doing doSomething..."); }
};

// 开发环境
class DevelopmentApplication : public Application {
public:
  DevelopmentApplication() : Application(std::make_unique<ConsoleLogger>()) {}
};

// 测试环境
class TestingApplication : public Application {
public:
  TestingApplication()
      : Application(std::make_unique<FileLogger>("test.log")) {}
};
// 生产环境
class ProductionApplication : public Application {
public:
  ProductionApplication()
      : Application(
            std::make_unique<RemoteLogger>("http://remote-server.com")) {}
};

#endif // !INCLUDE_FACTORYMETHOD_H
