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
    anya::Matrix<3, 4> mat{};
    std::cout << "测试逗号初始化和访问:" << std::endl;
    mat << 1, 2, 3, 5,
           4, 5, 6, 9,
           11, 12, 13, 18;
    std::cout << mat;
    std::cout << mat(2, 3) << std::endl;
    std::cout << mat.rowVec(2) << std::endl;
    std::cout << mat.colVec(2) << std::endl << std::endl;

    std::cout << "测试矩阵乘法:" << std::endl;
    anya::Matrix<2, 2> lhs{};
    lhs << 1, 1,
           2, 0;
    anya::Matrix<2, 3> rhs{};
    rhs << 0, 2, 3,
           1, 1, 2;
    std::cout << lhs * rhs << std::endl;

    anya::Matrix<1, 3> lhs2{};
    lhs2 << 8, 8, 6;
    anya::Matrix<3, 2> rhs2{};
    rhs2 << 5, 2,
           1, 3,
           6, 5;
    std::cout << lhs2 * rhs2 << std::endl;

    anya::Matrix<4, 3> A{};
    A << 2, 3, 1,
         3, 8, -2,
         4, -1, 9,
         1, -2, 4;
    anya::Vector3 x{-1, 2, 0};
    std::cout << A * x;
}

int main() {
    std::cout << "Hello, MnZn!" << std::endl;
//    vecTest();
    matrixTest();
    return 0;
}
