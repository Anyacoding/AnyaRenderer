//
// Created by Anya on 2022/12/3.
//
#include "test.h"
using namespace anya;

void vecTest() {
    {
        anya::Vector2 vec2 = anya::make_Vec(1, 2);
        anya::Vector3 vec3{ 3, 4, 0 };

        std::cout << "测试向量的加减乘除:" << std::endl;
        std::cout << vec2 << std::endl;
        std::cout << (vec3 *= 3) << std::endl;
        std::cout << 3 * vec3 << std::endl;
        std::cout << (vec3 /= 3) << std::endl;
        std::cout << -vec3 << std::endl
                  << std::endl;

        std::cout << "测试向量的点乘及其特殊运算:" << std::endl;
        std::cout << vec3.dot(vec3) << std::endl;
        std::cout << vec3.norm2() << std::endl;
        std::cout << vec3.normalize().norm2() << std::endl;
        std::cout << vec2.angle({ 0, 1 }) << std::endl
                  << std::endl;
    }

    {
        std::cout << "测试叉积:" << std::endl;
        anya::Vector3 a{ 1, 0, 0 };
        anya::Vector3 b{ 0, 1, 0 };
        std::cout << a.cross(b) << std::endl;
        a = { 1, 1, 0 };
        b = { -1, 1, 1 };
        std::cout << a.cross(b) << std::endl
                  << std::endl;
    }

    {
        std::cout << "测试四元数归一化:" << std::endl;
        anya::Vector4 vec4{ 1.0, 2.0, 3.0, 2.0 };
        std::cout << vec4.trim() << std::endl << std::endl;
    }

    {
        std::cout << "测试三维向量转为齐次坐标向量:" << std::endl;
        anya::Vector3 vec3{ 1.0, 2.0, 3.0 };
        std::cout << vec3.to4() << std::endl;
        std::cout << vec3.to4(0.0) << std::endl;
    }
}

void matrixTest() {
    {
        anya::Matrix<3, 3> mat{};
        std::cout << "测试逗号初始化和访问:" << std::endl;
        mat << 0, 1, -1,
               1, -1, 0,
               1, 1, 1;
        std::cout << mat.inverse() << std::endl;
        std::cout << mat * mat.inverse() << std::endl;
    }


    {
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
        anya::Vector3 x{ -1, 2, 0 };
        std::cout << A * x << std::endl
                  << std::endl;
    }

    {
        std::cout << "测试矩阵加减法和系数乘:" << std::endl;
        anya::Matrix<4, 3> lhs{};
        lhs << 2, 3, 1,
            3, 8, -2,
            4, -1, 9,
            1, -2, 4;
        anya::Matrix<4, 3> rhs{};
        rhs << 4, 3, 3,
            3, 2, -2,
            5, -1, 3,
            1, -23, 2;
        std::cout << lhs + rhs << std::endl;
        std::cout << lhs - rhs << std::endl;
        std::cout << (lhs += rhs) << std::endl;
        std::cout << (lhs -= rhs) << std::endl;
        std::cout << lhs * 25 << std::endl;
        std::cout << 25 * lhs  << std::endl;
        std::cout << lhs / 2 << std::endl;
        std::cout << (lhs *= 25) << std::endl;
        std::cout << (lhs /= 25) << std::endl << std::endl;
    }

    {
        std::cout << "测试矩阵转置:" << std::endl;
        anya::Matrix<4, 3> lhs{};
        lhs << 2, 3, 1,
            3, 8, -2,
            4, -1, 9,
            1, -2, 4;
        anya::Matrix<4, 3> rhs{};
        rhs << 4, 3, 3,
            3, 2, -2,
            5, -1, 3,
            1, -23, 2;
        std::cout << lhs.transpose() << std::endl;
        std::cout << rhs.transpose() << std::endl;
    }

    {
        std::cout << "测试矩阵行列式:" << std::endl;
        anya::Matrix<4, 4> lhs{};
        lhs << 2, 3, 1, 6,
            3, 9, -2, 8,
            4, -1, 9, 4,
            1, -2, 4, 7;
        anya::Matrix<5, 5> rhs{};
        rhs << 1, 1, 1, 2, 1,
            3, 3, 3, 1, 1,
            0, 1, 2, 2, 3,
            1, 1, 1, 0, 0,
            1, 1, 0, 5, 7;

        std::cout << lhs.det() << std::endl;
        std::cout << rhs.det() << std::endl << std::endl;
    }

    {
        std::cout << "测试矩阵求逆:" << std::endl;
        anya::Matrix<4, 4> lhs{};
        lhs << 2, 3, 1, 6,
            3, 9, -2, 8,
            4, -1, 9, 4,
            1, -2, 4, 7;
        anya::Matrix<5, 5> rhs{};
        rhs << 1, 1, 1, 2, 1,
            3, 3, 3, 1, 1,
            0, 1, 2, 2, 3,
            1, 1, 1, 0, 0,
            1, 1, 0, 5, 7;

        std::cout << rhs.inverse() << std::endl;
        std::cout << rhs.inverse() * rhs << std::endl;
    }

    {
        std::cout << "测试3*3矩阵转换为齐次坐标:" << std::endl;
        anya::Matrix<3, 3> lhs{};
        lhs << 2, 3, 1,
            3, 8, -2,
            4, -1, 9;
        anya::Matrix<5, 5> rhs{};
        rhs << 1, 1, 1, 2, 1,
            3, 3, 3, 1, 1,
            0, 1, 2, 2, 3,
            1, 1, 1, 0, 0,
            1, 1, 0, 5, 7;
        anya::Matrix44 mat44 = lhs.to44();
        std::cout << mat44 << std::endl;
        std::cout << rhs.to<3, 3>() << std::endl;
        std::cout << rhs.colVec(3) << std::endl << std::endl;
    }

    {
        std::cout << "测试替换矩阵特定行、列:" << std::endl;
        anya::Matrix<5, 5> rhs{};
        rhs << 1, 1, 1, 2, 1,
               3, 3, 3, 1, 1,
               0, 1, 2, 2, 3,
               1, 1, 1, 0, 0,
               1, 1, 0, 5, 7;
        anya::Vector<5> vec{9, 8, 7, 6, 5};
        rhs.setRowVec(0, vec);
        std::cout << rhs << std::endl;
    }

    {
        std::cout << "测试单位矩阵:" << std::endl;
        anya::Matrix<5, 5> rhs = anya::Matrix<5, 5>::Identity();
        std::cout << rhs << std::endl;
    }

    {
        std::cout << "测试列向量和行向量相乘:" << std::endl;
        anya::Vector3 lhs{3, 2, 1};
        anya::Matrix<1, 3> rhs;
        rhs << 3, 2, 1;
        std::cout << lhs * rhs;
    }
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    // make sure the viewport matches the new window dimensions; note that width and
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}

void processInput(GLFWwindow *window) {
    if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

