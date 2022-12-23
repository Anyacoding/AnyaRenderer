//
// Created by Anya on 2022/12/5.
//

#ifndef ANYA_ENGINE_UTILS_HPP
#define ANYA_ENGINE_UTILS_HPP

#include <fstream>
#include "matrix.hpp"
#include "nlohmann/json.hpp"
#include "shader/fragment_shader.hpp"

using json = nlohmann::json;

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

// shader的着色方法
struct ShaderUtils {
    static Vector3
    normal_fragment_shader(const FragmentShader& fs) {
        Vector3 ret = (fs.normal.to<3>().normalize() + Vector3{1, 1, 1}) / 2;
        // std::cout << ret << std::endl;
        return ret;
    }
};

}

#endif //ANYA_ENGINE_UTILS_HPP
