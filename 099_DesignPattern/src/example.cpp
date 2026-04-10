#include "example.hpp"

Example::Example() {
}

Example::~Example() {
}

void Example::printMessage() {
    std::cout << "Hello from Example class!" << std::endl;
}

int Example::add(int a, int b) {
    return a + b;
}