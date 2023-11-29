//
// Created by Anya on 2023/1/20.
//

#ifndef ANYA_RENDERER_MIRROR_HPP
#define ANYA_RENDERER_MIRROR_HPP

#include "renderer/raytracer.hpp"

namespace anya {

class MirrorMaterial: public Material {
public:
    MirrorMaterial() {
        this->type = MIRROR;
    }

public:
    [[nodiscard]] Vector3
    BXDF(const Vector3& wi, const Vector3& wo, const Vector3& normal) const override {
        auto cosalpha = normal.dot(wo);
        if (cosalpha > epsilon) {
            return Kd / cosalpha;
        }
        else {
            return {};
        }
    }

    [[nodiscard]] Vector3
    sample(const Vector3& wi, const Vector3& normal) const override {
        return wi - (2 * (wi.dot(normal))) * normal;
    }

    [[nodiscard]] numberType
    pdf(const Vector3& wi, const Vector3& wo, const Vector3& normal) const override {
        if (wo.dot(normal) > epsilon) {
            return 1 / pi;
        }
        else {
            return 0.0;
        }
    }
};

}

#endif //ANYA_RENDERER_MIRROR_HPP
