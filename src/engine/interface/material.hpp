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
    REFLECTION
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
public:
    // 材质方法
    virtual Vector3
    process(const Ray& ray, const HitData& hitdata, RayTracer& renderer) = 0;
};

}

#endif //ANYA_RENDERER_MATERIAL_HPP
