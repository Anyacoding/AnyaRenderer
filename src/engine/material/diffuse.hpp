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
    process(const Ray& ray, const HitData& hitData, RayTracer& renderer) override {
        return {};
    }
};

}

#endif //ANYA_RENDERER_DIFFUSE_HPP
