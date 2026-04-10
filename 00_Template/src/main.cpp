#include "example.hpp"

int main() {
    Example example;
    example.printMessage();
    
    int result = example.add(3, 5);
    std::cout << "3 + 5 = " << result << std::endl;
    
    return 0;
}