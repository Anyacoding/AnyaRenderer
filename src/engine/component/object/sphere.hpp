//
// Created by Anya on 2023/1/13.
//

#ifndef ANYA_RENDERER_SPHERE_HPP
#define ANYA_RENDERER_SPHERE_HPP

#include "interface/object.hpp"

namespace anya {

class Sphere: public Object {
private:
    Vector3 center{};
    numberType radius = 0.0;

public:
    Sphere(Vector3 point, numberType r): center(point), radius(r)
    {}

public:
#pragma region whitted_style api
    // 解析法求球面相交
    [[nodiscard]] bool
    intersect(const Ray& ray, numberType& tNear, Vector2&) const override {
        numberType a = ray.dir.dot(ray.dir);
        numberType b = 2 * ray.dir.dot(ray.pos - center);
        numberType c = (ray.pos - center).dot(ray.pos - center) - std::pow(radius, 2);
        auto solve = MathUtils::solveQuadratic(a, b, c);
        if (solve.has_value()) {
            auto[t0, t1] = solve.value();
            if (t0 < 0)
                t0 = t1;
            if (t0 < 0)
                return false;
            tNear = t0;
        }
        else {
            return false;
        }
        return true;
    }

    void
    getSurfaceProperties(const Vector3& hitPoint, const Vector2&,
                         Vector3& normal, Vector2&) const override {
        normal = (hitPoint - center).normalize();
    }
#pragma endregion

public:
    [[nodiscard]] std::optional<HitData>
    getIntersect(const Ray& ray) override {
        return {};
    }

    [[nodiscard]] AABB
    getBoundingBox() const override {
        return box;
    }
};


}

#endif //ANYA_RENDERER_SPHERE_HPP
