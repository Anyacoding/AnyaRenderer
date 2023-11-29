//
// Created by Anya on 2023/1/20.
//

#ifndef ANYA_RENDERER_DIFFUSE_HPP
#define ANYA_RENDERER_DIFFUSE_HPP

#include "renderer/raytracer.hpp"

namespace anya {

class DiffuseMaterial: public Material {
public:
    DiffuseMaterial() {
        this->type = DIFFUSE_AND_GLOSSY;
    }

public:
    [[nodiscard]] Vector3
    BXDF(const Vector3& wi, const Vector3& wo, const Vector3& normal) const override {
        // return Kd / pi;
        auto cosalpha = normal.dot(wo);
        if (cosalpha > epsilon) {
            return Kd / pi;
        }
        else {
            return {};
        }
    }

    [[nodiscard]] Vector3
    sample(const Vector3& wi, const Vector3& normal) const override {
        auto x1 = MathUtils::getRandNum();
        auto x2 = MathUtils::getRandNum();
        auto z = std::fabs(1.0 - 2.0 * x1);
        auto r = std::sqrt(1.0 - z * z);
        auto phi = 2.0 * pi * x2;
        Vector3 localRay{ r * std::cos(phi), r * std::sin(phi), z };
        return toWorld(localRay, normal);
    }

    [[nodiscard]] numberType
    pdf(const Vector3& wi, const Vector3& wo, const Vector3& normal) const override {
        // return wo.dot(normal) / pi;
        if (wo.dot(normal) > epsilon) {
            return 0.5 / pi;
        }
        else {
            return 0.0;
        }
    }
};

}

#endif //ANYA_RENDERER_DIFFUSE_HPP
