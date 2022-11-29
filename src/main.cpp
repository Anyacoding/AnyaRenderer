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
    std::cout << vec2.angle({0, 1}) << std::endl << std::endl;

    std::cout << "测试叉积:" << std::endl;
    anya::Vec3 a{1, 0, 0};
    anya::Vec3 b{0, 1, 0};
    std::cout << a.cross(b) << std::endl;
    a = {1, 1, 0};
    b = {-1, 1, 1};
    std::cout << a.cross(b) << std::endl << std::endl;

    std::cout << "测试四元数归一化:" << std::endl;
    anya::Vec4 vec4{1.0, 2.0, 3.0, 2.0};
    std::cout << vec4.trim() << std::endl;
    return 0;
}
