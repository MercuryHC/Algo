#include "factoryMethod.h"
#include <chrono>
#include <iostream>

void ConsoleLogger::log(const std::string &message) {
  std::cout << "CONSOLE：" << message << std::endl;
}

void FileLogger::log(const std::string &message) {
  std::cout << "WRITE TO：" << filePath << ":" << message << std::endl;
}
void RemoteLogger::log(const std::string &message) {
  auto now = std::chrono::system_clock::now().time_since_epoch();
  auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(now).count();
  std::cout << "SEND TO " << remoteServer << ": "
            << "{\"message\":\"" << message << "\",\"timestamp\":\"" << ms
            << "\"}" << std::endl;
}
