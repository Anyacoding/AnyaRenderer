//
// Created by Anya on 2022/12/29.
//

#ifndef ANYA_ENGINE_SHADER_HPP
#define ANYA_ENGINE_SHADER_HPP

#include "tool/matrix.hpp"
#include "fragment_shader.hpp"
#include "component/light.hpp"

namespace anya {

// shader的着色方法
struct ShaderUtils {
    static Vector3
    simple_fragment_shader(const FragmentShader& fs) {
        Vector3 finalColor{};
        if (fs.texture) {
            finalColor = fs.texture->getColorBilinear(fs.uv[0], fs.uv[1]) / 255;
        }
        else {
            finalColor = fs.color;
        }
        return finalColor;
    }

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

        Vector3 color = fs.color;                            // 颜色
        Vector3 point = fs.viewSpacePosition.to<3>();          // 着色点
        Vector3 normal = fs.normal.to<3>();                    // 法线

        // Blinn-Phong整体计算公式
        Vector3 ret{};
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
            finalColor = fs.texture->getColorBilinear(fs.uv[0], fs.uv[1]) / 255;
        }
        else {
            finalColor = fs.color;
        }
        Vector3 ka = {0.239, 0.239, 0.239};    // 泛光系数
        Vector3 kd = finalColor;               // 漫反射系数
        Vector3 ks = {0.7937, 0.7937, 0.7937}; // 高光系数

        // 多个光源
        auto light1 = Light{{20, 20, 20}, {500, 500, 500}};
        auto light2 = Light{{-20, 20, 0}, {500, 500, 500}};
        std::vector<Light> lights = {light1, light2};

        Vector3 ambient_light_intensity = kd;  // 环境光强度
        Vector3 eye_pos = {0, 0, 10};                    // 观察位置
        numberType p = 150.0;                            // Phong反射模型幂系数

        Vector3 color = finalColor;                            // 颜色
        Vector3 point = fs.viewSpacePosition.to<3>();          // 着色点
        Vector3 normal = fs.normal.to<3>();                    // 法线

        // Blinn-Phong整体计算公式
        Vector3 ret = {};
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
    bump_fragment_shader(const FragmentShader& fs) {
        Vector3 normal = fs.normal.to<3>();                    // 法线

        // TODO: 不是很懂，课上也讲的不是很清楚，后面再说
        numberType kh = 0.2, kn = 0.1;

        numberType x = normal.x();
        numberType y = normal.y();
        numberType z = normal.z();

        Vector3 n = normal;
        Vector3 t = { x * y / std::sqrt(x * x + z * z), std::sqrt(x * x + z * z), z * y / std::sqrt(x * x + z * z) };
        Vector3 b = normal.cross(t);
        Matrix33 TBN;
        TBN << t.x(), b.x(), n.x(),
            t.y(), b.y(), n.y(),
            t.z(), b.z(), n.z();

        numberType u = fs.uv.x(), v = fs.uv.y();
        numberType w = fs.texture->getWidth(), h = fs.texture->getHeight();
        numberType dU = kh * kn * (fs.texture->getColorBilinear(u + 1.0 / w, v).norm2() - fs.texture->getColorBilinear(u, v).norm2());
        numberType dV = kh * kn * (fs.texture->getColorBilinear(u, v + 1.0 / h).norm2() - fs.texture->getColorBilinear(u, v).norm2());
        Vector3 ln = Vector3{-dU, -dV, 1.0};
        normal = (TBN * ln).normalize();
        Vector3 ret = normal;

        return ret;
    }

    static Vector3
    displacement_fragment_shader(const FragmentShader& fs) {
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

        Vector3 color = fs.color;                            // 颜色
        Vector3 point = fs.viewSpacePosition.to<3>();    // 着色点
        Vector3 normal = fs.normal.to<3>();              // 法线


        // TODO: 有bug，暂且不能使用
        numberType kh = 0.2, kn = 0.1;

        numberType x = normal.x();
        numberType y = normal.y();
        numberType z = normal.z();

        Vector3 n = normal;
        Vector3 t = { x * y / std::sqrt(x * x + z * z), std::sqrt(x * x + z * z), z * y / std::sqrt(x * x + z * z) };
        Vector3 b = normal.cross(t);
        Matrix33 TBN;
        TBN << t.x(), b.x(), n.x(),
            t.y(), b.y(), n.y(),
            t.z(), b.z(), n.z();
        numberType u = fs.uv.x(), v = fs.uv.y();
        numberType w = fs.texture->getWidth(), h = fs.texture->getHeight();
        numberType dU = kh * kn * (fs.texture->getColorBilinear(u + 1.0 / w, v).norm2() - fs.texture->getColorBilinear(u, v).norm2());
        numberType dV = kh * kn * (fs.texture->getColorBilinear(u, v + 1.0 / h).norm2() - fs.texture->getColorBilinear(u, v).norm2());
        Vector3 ln = Vector3{-dU, -dV, 1.0};
        normal = (TBN * ln).normalize();
        point += (kn * normal * fs.texture->getColorBilinear(u, v).norm2());

        // Blinn-Phong整体计算公式
        Vector3 ret{};
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

#endif //ANYA_ENGINE_SHADER_HPP
