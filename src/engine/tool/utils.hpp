//
// Created by Anya on 2022/12/5.
//

#ifndef ANYA_ENGINE_UTILS_HPP
#define ANYA_ENGINE_UTILS_HPP

#include <fstream>
#include "matrix.hpp"
#include "nlohmann/json.hpp"
#include "shader/fragment_shader.hpp"
#include "component/light.hpp"

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
        return ret;
    }

    static Vector3
    phong_fragment_shader(const FragmentShader& fs) {
        Vector3 ka = {0.005, 0.005, 0.005};    // 泛光系数
        Vector3 kd = fs.color;                 // 漫反射系数
        Vector3 ks = {0.7937, 0.7937, 0.7937}; // 高光系数

        // 多个光源
        auto light1 = Light{{20, 20, 20}, {500, 500, 500}};
        auto light2 = Light{{-20, 20, 0}, {500, 500, 500}};
        std::vector<Light> lights = {light1, light2};

        Vector3 ambient_light_intensity = {10, 10, 10};  // 环境光强度
        Vector3 eye_pos = {0, 0, 10};                    // 观察位置
        numberType p = 150.0;                            // Phong反射模型幂系数

        Vector3 color = fs.color;                              // 颜色
        Vector3 point = fs.viewSpacePosition.to<3>();          // 着色点
        Vector3 normal = fs.normal.to<3>();                    // 法线

        // Blinn-Phong整体计算公式
        Vector3 ret{0, 0, 0};
        for (auto& light : lights) {
            Vector3 l = (light.position - point).normalize();  // 入射方向l
            Vector3 v = (eye_pos - point).normalize();         // 观察方向v
            Vector3 h = (l + v).normalize();                   // 半程向量
            numberType R2 = (point - light.position).dot((point - light.position)); // 距离的平方
            ret += ka.mut(ambient_light_intensity);
            ret += kd.mut(light.intensity / R2) * std::max(0.0, normal.dot(l));
            ret += ks.mut(light.intensity / R2) * std::pow(std::max(0.0, normal.dot(h)), p);
        }

        return ret;
    }

    static Vector3
    texture_fragment_shader(const FragmentShader& fs) {
        Vector3 finalColor{};
        if (fs.texture) {
            finalColor = fs.texture->getColor(fs.uv[0], fs.uv[1]);
        }
        else {
            finalColor = fs.color;
        }
        Vector3 ka = {0.005, 0.005, 0.005};    // 泛光系数
        Vector3 kd = finalColor;                 // 漫反射系数
        Vector3 ks = {0.7937, 0.7937, 0.7937}; // 高光系数

        // 多个光源
        auto light1 = Light{{20, 20, 20}, {500, 500, 500}};
        auto light2 = Light{{-20, 20, 0}, {500, 500, 500}};
        std::vector<Light> lights = {light1, light2};

        Vector3 ambient_light_intensity = {10, 10, 10};  // 环境光强度
        Vector3 eye_pos = {0, 0, 10};                    // 观察位置
        numberType p = 150.0;                            // Phong反射模型幂系数

        Vector3 color = finalColor;                              // 颜色
        Vector3 point = fs.viewSpacePosition.to<3>();          // 着色点
        Vector3 normal = fs.normal.to<3>();                    // 法线

        // Blinn-Phong整体计算公式
        Vector3 ret{0, 0, 0};
        for (auto& light : lights) {
            Vector3 l = (light.position - point).normalize();  // 入射方向l
            Vector3 v = (eye_pos - point).normalize();         // 观察方向v
            Vector3 h = (l + v).normalize();                   // 半程向量
            numberType R2 = (point - light.position).dot((point - light.position)); // 距离的平方
            ret += ka.mut(ambient_light_intensity);
            ret += kd.mut(light.intensity / R2) * std::max(0.0, normal.dot(l));
            ret += ks.mut(light.intensity / R2) * std::pow(std::max(0.0, normal.dot(h)), p);
        }

        return ret;
    }
};

}

#endif //ANYA_ENGINE_UTILS_HPP
