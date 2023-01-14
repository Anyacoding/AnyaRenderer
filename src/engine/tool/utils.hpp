//
// Created by Anya on 2022/12/5.
//

#ifndef ANYA_ENGINE_UTILS_HPP
#define ANYA_ENGINE_UTILS_HPP

#include <fstream>
#include "nlohmann/json.hpp"
#include "component/light.hpp"

using json = nlohmann::json;

namespace anya {

// 常用数学常数
constexpr numberType pi  = std::numbers::pi_v<numberType>;
constexpr numberType inf = std::numeric_limits<numberType>::infinity();
constexpr numberType KMAX = std::numeric_limits<numberType>::max();
constexpr numberType epsilon = 0.00001;

// 通用数学工具函数
struct MathUtils {
    // 角度转弧度
    static constexpr numberType
    angle2rad(numberType angle) {
        return angle / 180 * pi;
    }

    // 线性插值
    template<class T>
    static constexpr T
    lerp(numberType k, const T& v0, const T& v1) {
        return v0 + k * (v1 - v0);
    }

    // 重心坐标插值
    template<class T>
    static T
    interpolate(numberType alpha, numberType beta, numberType gamma, T a, T b, T c, numberType fixed = 1.0) {
        return (alpha * a + beta * b + gamma * c) * fixed;
    }

    // 约束范围
    static constexpr numberType
    clamp(numberType lower, numberType upper, numberType val) {
        return std::max(lower, std::min(upper, val));
    }

    // 解二元一次函数的根
    static std::optional<std::pair<numberType, numberType>>
    solveQuadratic(numberType a, numberType b, numberType c) {
        std::optional<std::pair<numberType, numberType>> ans;
        numberType delta = std::pow(b, 2) - 4 * a * c;
        if (delta < 0) {
            return ans;
        }
        else if (delta < 1e-8) {
            numberType t = -b / (2 * a);
            ans.emplace(std::make_pair(t, t));
        }
        else {
            numberType t0 = (-b - std::sqrt(delta)) / (2 * a);
            numberType t1 = (-b + std::sqrt(delta)) / (2 * a);
            ans.emplace(std::make_pair(t0, t1));
        }
        return ans;
    }
};

// 变换矩阵的工厂函数
struct Transform {
#pragma region 旋转
    // 绕x轴旋转
    static Matrix44
    RotateAroundX(numberType angle) {
        return RotateAroundN(angle, {1, 0, 0});
    }

    // 绕y轴旋转
    static Matrix44
    RotateAroundY(numberType angle) {
        return RotateAroundN(angle, {0, 1, 0});
    }

    // 绕z轴旋转
    static Matrix44
    RotateAroundZ(numberType angle) {
        return RotateAroundN(angle, {0, 0, 1});
    }

    // 罗德里格斯旋转，绕任意轴
    static Matrix44
    RotateAroundN(numberType angle, Vector3 axis) {
        axis = axis.normalize();  // 归一化为单位向量
        numberType rad = MathUtils::angle2rad(angle);
        Matrix33 rotation = Matrix33 ::Identity();
        Matrix33 I = Matrix33 ::Identity();
        Vector3 n = axis;
        RowVector3 nt;
        nt << axis.x(), axis.y() ,axis.z();
        Matrix33 N{};
        N << 0, -n.z(), n.y(),
             n.z(), 0, -n.x(),
             -n.y(), n.x(), 0;
        rotation = std::cos(rad) * I + (1 - std::cos(rad)) * n * nt + std::sin(rad) * N;
        return rotation.to44();
    }
#pragma endregion

#pragma region 缩放
    static Matrix44
    scale(numberType ratio) {
        Matrix44 scale = Matrix44::Identity();
        scale << ratio, 0, 0, 0,
                 0, ratio, 0, 0,
                 0, 0, ratio, 0,
                 0, 0, 0, 1;
        return scale;
    }
#pragma endregion
};

// json的便捷工具库
struct JsonUtils {
    static json
    load(const std::string& path) {
        std::ifstream ifs(path);
        json ret = json::parse(ifs);
        return ret;
    }
};

}

#endif //ANYA_ENGINE_UTILS_HPP
