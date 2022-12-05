#include <iostream>
#include "test/test.h"
#include "math/vec.hpp"
#include "math/matrix.hpp"
#include "ui/gui.hpp"

int main() {
    std::cout << "Hello, MnZn!" << std::endl;
    anya::GUI gui("Anya", 800, 600);
    // gui.run();
    matrixTest();
    return 0;
}

