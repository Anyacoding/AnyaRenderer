//
// Created by Anya on 2022/12/5.
//

#ifndef ANYA_ENGINE_UTILS_HPP
#define ANYA_ENGINE_UTILS_HPP

#include "matrix.hpp"

namespace anya {

// 常用数学常数
constexpr numberType pi  = std::numbers::pi_v<numberType>;
constexpr numberType inf = std::numeric_limits<numberType>::infinity();

// 通用工具函数
struct MathUtils {
    // 角度转弧度
    static constexpr numberType
    angle2rad(numberType angle) {
        return angle / 180 * pi;
    }
};

// 变换矩阵的工厂函数
struct Transform {
    // 绕z轴旋转
    static Matrix44
    RotateAroundZ(numberType angle) {
        Matrix44 mat = Matrix44::Identity();
        numberType rad = MathUtils::angle2rad(angle);
        mat << std::cos(rad), -std::sin(rad), 0, 0,
               std::sin(rad), std::cos(rad), 0, 0,
               0, 0, 1, 0,
               0, 0, 0, 1;
        return mat;
    }
};

}

#endif //ANYA_ENGINE_UTILS_HPP
