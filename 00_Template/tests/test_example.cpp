#include "example.hpp"
#include <cassert>

void test_example() {
    Example example;
    example.printMessage();
    
    assert(example.add(2, 3) == 5);
    assert(example.add(-1, 1) == 0);
    assert(example.add(10, 20) == 30);
}

int main() {
    test_example();
    std::cout << "All tests passed!" << std::endl;
    return 0;
}
