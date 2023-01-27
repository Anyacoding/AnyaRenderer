//
// Created by Anya on 2023/1/12.
//

#ifndef ANYA_RENDERER_OBJECT_HPP
#define ANYA_RENDERER_OBJECT_HPP

#include "interface/material.hpp"

namespace anya {

struct HitData;

class Object {
public:
    std::shared_ptr<Material> material;            // 材质
    std::vector<std::shared_ptr<Object>> childs;   // 子对象集合
public:
#pragma region whitted_style api
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
        return material->diffuseColor;
    }
#pragma endregion

};



}

#endif //ANYA_RENDERER_OBJECT_HPP
