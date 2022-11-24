#include <iostream>
#include "math/vec.hpp"

int main() {
    std::cout << "Hello, MnZn!" << std::endl;

    anya::Vec2 vec2 = anya::make_Vec(1, 2);
    std::cout << vec2 << std::endl;

    anya::Vec3 vec3{ 3, 4, 0 };
    std::cout << (vec3 *= 3) << std::endl;
    std::cout << 3 * vec3 << std::endl;
    std::cout << (vec3 /= 3) << std::endl;
    std::cout << -vec3 << std::endl;
    std::cout << vec3.dot(vec3) << std::endl;
    std::cout << vec3.norm2() << std::endl;
    std::cout << vec3.normalize().norm2() << std::endl;
    std::cout << vec2.angle({0, 1}) << std::endl;
    return 0;
}
