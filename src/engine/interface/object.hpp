//
// Created by Anya on 2023/1/12.
//

#ifndef ANYA_RENDERER_OBJECT_HPP
#define ANYA_RENDERER_OBJECT_HPP

namespace anya {

enum MaterialType {
    DIFFUSE_AND_GLOSSY,
    REFLECTION_AND_REFRACTION,
    REFLECTION
};

class Object {
public:
    // 材质类型
    MaterialType materialType = DIFFUSE_AND_GLOSSY;
    // 折射率
    numberType ior = 1.3;
    // 漫反射和高光系数
    numberType kd = 0.8, ks = 0.2;
    // 反射颜色
    Vector3 diffuseColor = {0.2, 0.2, 0.2};
    // 镜面指数
    numberType specularExponent = 25;

public:
    // 判断光线与图元是否相交
    [[nodiscard]] virtual bool
    intersect(const Ray& ray, numberType& tNear, Vector2& uv) const = 0;

    // 获取图元表面属性
    virtual void
    getSurfaceProperties(const Vector3& hitPoint, const Vector2& uv,
                         Vector3& normal, Vector2& st) const = 0;

    // 计算反射颜色
    [[nodiscard]] virtual Vector3
    evalDiffuseColor(const Vector2&) const {
        return diffuseColor;
    }
};

}

#endif //ANYA_RENDERER_OBJECT_HPP
