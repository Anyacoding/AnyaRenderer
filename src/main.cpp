#include <iostream>
#include "math/vec.hpp"
#include "math/matrix.hpp"

void vecTest() {
    anya::Vector2 vec2 = anya::make_Vec(1, 2);
    anya::Vector3 vec3{ 3, 4, 0 };

    std::cout << "测试向量的加减乘除:" << std::endl;
    std::cout << vec2 << std::endl;
    std::cout << (vec3 *= 3) << std::endl;
    std::cout << 3 * vec3 << std::endl;
    std::cout << (vec3 /= 3) << std::endl;
    std::cout << -vec3 << std::endl << std::endl;

    std::cout << "测试向量的点乘及其特殊运算:" << std::endl;
    std::cout << vec3.dot(vec3) << std::endl;
    std::cout << vec3.norm2() << std::endl;
    std::cout << vec3.normalize().norm2() << std::endl;
    std::cout << vec2.angle({0, 1}) << std::endl << std::endl;

    std::cout << "测试叉积:" << std::endl;
    anya::Vector3 a{1, 0, 0};
    anya::Vector3 b{0, 1, 0};
    std::cout << a.cross(b) << std::endl;
    a = {1, 1, 0};
    b = {-1, 1, 1};
    std::cout << a.cross(b) << std::endl << std::endl;

    std::cout << "测试四元数归一化:" << std::endl;
    anya::Vector4 vec4{1.0, 2.0, 3.0, 2.0};
    std::cout << vec4.trim() << std::endl;
}

void matrixTest() {
    anya::Matrix<3, 3> mat{};
    mat << 1, 2, 3,
           4, 5, 6,
           7, 8, 9,
           11, 12, 13;
    std::cout << mat;
}

int main() {
    std::cout << "Hello, MnZn!" << std::endl;
//    vecTest();
    matrixTest();
    return 0;
}
