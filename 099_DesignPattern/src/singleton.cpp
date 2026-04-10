#include "./singleton.h"

void DatabaseManager_1::excute(const std::string &sql) {
  std::cout << "Excuting: " << sql << std::endl;
}

void DatabaseManager_2::excute(const std::string &sql) {
  std::cout << "Excuting: " << sql << std::endl;
}
