//
// Created by Anya on 2023/1/20.
//

#ifndef ANYA_RENDERER_MIRROR_HPP
#define ANYA_RENDERER_MIRROR_HPP

#include "renderer/raytracer.hpp"

namespace anya {

class GlassMaterial: public Material {
public:
    GlassMaterial() {
        this->type = REFLECTION_AND_REFRACTION;
    }

public:
    [[nodiscard]] Vector3
    BXDF(const Vector3& wi, const Vector3& wo, const Vector3& normal) const override {
        throw std::runtime_error("GlassMaterial::BXDF() has not implemented!");
    }

    [[nodiscard]] Vector3
    sample(const Vector3& wi, const Vector3& normal) const override {
        throw std::runtime_error("GlassMaterial::sample() has not implemented!");
    }

    [[nodiscard]] numberType
    pdf(const Vector3& wi, const Vector3& wo, const Vector3& normal) const override {
        throw std::runtime_error("GlassMaterial::pdf() has not implemented!");
    }
};

}

#endif //ANYA_RENDERER_MIRROR_HPP
