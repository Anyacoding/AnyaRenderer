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
    Vector3
    BXDF(const Vector3& wi, const Vector3& wo, const Vector3& normal) override {
        // auto x1 = MathUtils::getRandNum();
        // auto x2 = MathUtils::getRandNum();
        // auto z = std::fabs(1 - 2 * x1);
        // auto r = std::sqrt(1 - z * z);
        // auto phi = 2 * pi * x2;
        // Vector3 localRay{ r * std::cos(phi), r * std::sin(phi), z };
        // return toWorld(localRay, normal);
        auto cosalpha = normal.dot(wo);
        if (cosalpha > 0.0) {
            return Kd / pi;
        }
        else {
            return {};
        }
    }
};

}

#endif //ANYA_RENDERER_DIFFUSE_HPP
