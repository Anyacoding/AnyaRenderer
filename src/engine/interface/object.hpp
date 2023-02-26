//
// Created by Anya on 2023/1/12.
//

#ifndef ANYA_RENDERER_OBJECT_HPP
#define ANYA_RENDERER_OBJECT_HPP

#include "interface/material.hpp"
#include "accelerator/AABB.hpp"
#include <memory>

namespace anya {

struct HitData;

class Object: public std::enable_shared_from_this<Object> {
public:
    std::shared_ptr<Material> material;            // 材质
    std::vector<std::shared_ptr<Object>> childs;   // 子对象集合
    AABB box{};                                    // 包围盒

public:
#pragma region whitted_style api
    // 计算反射颜色
    [[nodiscard]] virtual Vector3
    evalDiffuseColor(const Vector2&) const {
        return material->diffuseColor;
    }

#pragma endregion

public:
    // 获取包围盒
    [[nodiscard]] virtual AABB getBoundingBox() const = 0;

    // 返回光线与object的相交结果
    [[nodiscard]] virtual std::optional<HitData> getIntersect(const Ray& ray) = 0;

    // 返回物体面积
    virtual numberType getArea() const = 0;

    // 采样
    virtual std::pair<HitData, numberType> sample() const = 0;

    // 物体是否是光源的一部分
    bool isLight() const { return material != nullptr && material->isLight; }

    // 返回光源的辐射量
    Vector3 getEmission() const { return material->emission; }
};

struct HitData {
    numberType tNear = KMAX;    // 光源和物体的距离
    Vector3 hitPoint{};         // 相交点
    Vector3 normal{};           // 相交点处的法线
    Vector2 uv = {};            // 三角形重心坐标的beta和gamma
    Vector2 st = {};            // 三角形专用插值坐标
    Vector3 radiance = {};      // 物体的辐射率
    std::shared_ptr<Object> hitObject = nullptr;
};

}

#endif //ANYA_RENDERER_OBJECT_HPP
