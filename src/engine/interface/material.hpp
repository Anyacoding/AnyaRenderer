//
// Created by Anya on 2023/1/20.
//

#ifndef ANYA_RENDERER_MATERIAL_HPP
#define ANYA_RENDERER_MATERIAL_HPP

namespace anya {

// 前置声明
struct HitData;
class RayTracer;

enum MaterialType {
    DIFFUSE_AND_GLOSSY,
    REFLECTION_AND_REFRACTION,
    MIRROR
};

class Material {
public:
    // 材质类型
    MaterialType type = DIFFUSE_AND_GLOSSY;
    // 折射率
    numberType ior = 1.3;
    // 漫反射和高光系数
    numberType kd = 0.8, ks = 0.2;
    // 反射颜色
    Vector3 diffuseColor = {0.2, 0.2, 0.2};
    // 镜面指数
    numberType specularExponent = 25;

    // 光源标志位
    bool isLight = false;
    // 光源辐射量
    Vector3 emission = {};
    // path_tracing专用Kd
    Vector3 Kd{};

public:
    [[nodiscard]] virtual Vector3
    BXDF(const Vector3& wi, const Vector3& wo, const Vector3& normal) const = 0;

    [[nodiscard]] virtual Vector3
    sample(const Vector3& wi, const Vector3& normal) const = 0;

    [[nodiscard]] virtual numberType
    pdf(const Vector3& wi, const Vector3& wo, const Vector3& normal) const = 0;

protected:
    static Vector3
    toWorld(const Vector3& ray, const Vector3& normal) {
        Vector3 B, C;
        if (std::fabs(normal.x()) > std::fabs(normal.y())) {
            auto len = std::sqrt(normal.x() * normal.x() + normal.z() * normal.z());
            C = Vector3{ normal.z() / len, 0, -normal.x() / len };
        }
        else {
            auto len = std::sqrt(normal.y() * normal.y() + normal.z() * normal.z());
            C = Vector3{ 0, normal.z() / len, -normal.y() / len };
        }
        B = C.cross(normal);
        return ray.x() * B + ray.y() * C + ray.z() * normal;
    }
};

}

#endif //ANYA_RENDERER_MATERIAL_HPP
