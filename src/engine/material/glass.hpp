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
    Vector3
    process(const Ray& ray, const HitData& hitData, RayTracer& renderer) override {
        Vector3 hitPoint = ray.at(hitData.tNear);
        Vector3 normal;
        Vector2 st;
        hitData.hitObject->getSurfaceProperties(hitPoint, hitData.uv, normal, st);
        return {};
    }
};

}

#endif //ANYA_RENDERER_MIRROR_HPP
