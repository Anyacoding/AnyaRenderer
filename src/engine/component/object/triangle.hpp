//
// Created by Anya on 2022/12/6.
//

#ifndef ANYA_ENGINE_TRIANGLE_HPP
#define ANYA_ENGINE_TRIANGLE_HPP

#include "tool/matrix.hpp"
#include "interface/object.hpp"
#include <array>

namespace anya {

class Triangle: public Object {
public:
    std::array<Vector4, 3> vertexes{};        // 顶点集合
    std::array<Vector4, 3> normals{};         // 法线集合
    std::array<Vector3, 3> colors{};          // 颜色集合
    std::array<Vector<2>, 3> uvs{};           // 纹理坐标集合
    std::array<Vector<2>, 3> stCoordinates{}; // 表面属性

public:
#pragma region whitted_style api
    [[nodiscard]] Vector3
    evalDiffuseColor(const Vector2& st) const override {
        numberType scale = 5;
        numberType pattern = (std::fmod(st.x() * scale, 1) > 0.5) ^ (std::fmod(st.y() * scale, 1) > 0.5);
        return MathUtils::lerp(pattern, Vector3{0.815, 0.235, 0.031}, Vector3{0.937, 0.937, 0.231});
    }
#pragma endregion

public:
    // MT算法判断三角形与光线是否相交, 并返回相交信息
    [[nodiscard]] std::optional<HitData>
    getIntersect(const Ray& ray) override {
        std::optional<HitData> hitData{};
        const auto& v0 = a();
        const auto& v1 = b();
        const auto& v2 = c();
        numberType u = 0.0;
        numberType v = 0.0;
        Vector3 E1 = (v1 - v0).to<3>();
        Vector3 E2 = (v2 - v0).to<3>();
        Vector3 S = ray.pos - v0.to<3>();
        Vector3 S1 = ray.dir.cross(E2);
        Vector3 S2 = S.cross(E1);
        Vector3 normal = E1.cross(E2).normalize();
        numberType det = S1.dot(E1);
        numberType tNear = S2.dot(E2) / det;
        u = S1.dot(S) / det;
        v = S2.dot(ray.dir) / det;
        if (u >= 0 && u <= 1 && v >= 0 && v <= 1 && 1 - u - v >= 0 && 1 - u - v <= 1 && tNear >= epsilon) {
            hitData.emplace();
            hitData->tNear = tNear;
            hitData->hitObject = shared_from_this();
            hitData->hitPoint = ray.at(tNear);
            hitData->normal = normal;
            hitData->uv = Vector2{u, v};

            // 此处的uv不是纹理坐标，而是三角形重心坐标的beta和gamma
            const Vector2& st0 = stCoordinates[0];
            const Vector2& st1 = stCoordinates[1];
            const Vector2& st2 = stCoordinates[2];
            auto alpha = 1 - u - v;
            auto beta = u;
            auto gamma = v;
            auto st = MathUtils::interpolate(alpha, beta, gamma, st0, st1, st2);
            hitData->st = st;
        }
        return hitData;
    }

    [[nodiscard]] AABB
    getBoundingBox() const override {
        auto ret = AABB::merge(AABB(this->a().to<3>(), this->b().to<3>()), this->c().to<3>());
        return ret;
    }

    numberType
    getArea() const override {
        const auto& v0 = a();
        const auto& v1 = b();
        const auto& v2 = c();
        Vector3 E1 = (v1 - v0).to<3>();
        Vector3 E2 = (v2 - v0).to<3>();
        numberType area = E1.cross(E2).norm2() * 0.5;
        return area;
    }

    std::pair<HitData, numberType>
    sample() const override {
        const auto& v0 = a().to<3>();
        const auto& v1 = b().to<3>();
        const auto& v2 = c().to<3>();
        Vector3 E1 = v1 - v0;
        Vector3 E2 = v2 - v0;
        auto x = std::sqrt(MathUtils::getRandNum());
        auto y = MathUtils::getRandNum();
        HitData hitData{};
        hitData.hitPoint = v0 * (1.0 - x) + v1 * (x * (1.0 - y)) + v2 * (x * y);
        hitData.normal = E1.cross(E2).normalize();
        numberType pdf = 1.0 / getArea();
        return { hitData, pdf };
    }

public:
#pragma region 三角形特化方法
    [[nodiscard]] Vector4 a() const { return vertexes[0]; }
    [[nodiscard]] Vector4 b() const { return vertexes[1]; }
    [[nodiscard]] Vector4 c() const { return vertexes[2]; }

    // 设置顶点
    void
    setVertex(int index, const Vector4& vertex) {
        vertexes[index] = vertex;
    }

    // 设置法线
    void
    setNormal(int index, const Vector4& normal) {
        normals[index] = normal;
    }

    // 设置颜色
    void
    setColor(int index, const Vector3& col) {
        colors[index] = col;
    }

    void
    setColor(int index, double r, double g, double b) {
        if ((r < 0.0) || (r > 255.) || (g < 0.0) || (g > 255.) || (b < 0.0) ||
            (b > 255.)) {
            throw std::runtime_error("Invalid color values");
        }
        colors[index] = make_Vec(r / 255.0, g / 255.0, b / 255.0);
    }

    // 设置纹理坐标
    void
    setUV(int index, const Vector<2>& uv) {
        uvs[index] = uv;
    }

    void
    setUV(int index, numberType u, numberType v) {
        uvs[index] = { u, v };
    }

    // 设置表面属性
    void
    setST(int index, const Vector<2>& st) {
        stCoordinates[index] = st;
    }

#pragma endregion

};

}

#endif //ANYA_ENGINE_TRIANGLE_HPP
