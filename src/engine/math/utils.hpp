//
// Created by Anya on 2022/12/5.
//

#ifndef ANYA_ENGINE_UTILS_HPP
#define ANYA_ENGINE_UTILS_HPP

#include "matrix.hpp"

namespace anya {

// 常用数学常数
constexpr numberType pi  = std::numbers::pi_v<numberType>;

// 通用工具函数
struct MathUtils {
    // 角度转弧度
    static constexpr numberType
    angle2rad(numberType angle) {
        return angle / 180 * pi;
    }
};

}

#endif //ANYA_ENGINE_UTILS_HPP
