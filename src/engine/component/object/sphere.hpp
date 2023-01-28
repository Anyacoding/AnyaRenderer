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
    // 解析法求球面相交
    [[nodiscard]] std::optional<HitData>
    getIntersect(const Ray& ray) override {
        std::optional<HitData> hitData{};
        numberType a = ray.dir.dot(ray.dir);
        numberType b = 2 * ray.dir.dot(ray.pos - center);
        numberType c = (ray.pos - center).dot(ray.pos - center) - std::pow(radius, 2);
        numberType tNear = 0.0;
        auto solve = MathUtils::solveQuadratic(a, b, c);
        if (solve.has_value()) {
            auto[t0, t1] = solve.value();
            if (t0 < 0) t0 = t1;
            if (t0 < 0) return hitData;
            tNear = t0;
            hitData.emplace();
            hitData->hitPoint = ray.at(tNear);
            hitData->normal = (hitData->hitPoint - center).normalize();
            hitData->hitObject = std::make_shared<Sphere>(*this);
            hitData->tNear = tNear;
        }
        return hitData;
    }

    [[nodiscard]] AABB
    getBoundingBox() const override {
        return AABB(Vector3{center.x() - radius, center.y() - radius, center.z() - radius},
                    Vector3{center.x() + radius, center.y() + radius, center.z() + radius});
    }
};


}

#endif //ANYA_RENDERER_SPHERE_HPP
