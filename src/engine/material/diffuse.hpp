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
        Vector3 hitPoint = ray.at(hitData.tNear);
        Vector3 normal;
        Vector2 st;
        hitData.hitObject->getSurfaceProperties(hitPoint, hitData.uv, normal, st);

        Vector3 ambient_light = {0, 0, 0}, specularColor = {0, 0, 0};
        Vector3 shadowPointOrig = ray.dir.dot(normal) < 0
                                      ? hitPoint + normal * epsilon
                                      : hitPoint - normal * epsilon;

        for (auto& light : renderer.scene.lights) {
            Vector3 lightDir = light.position - hitPoint;
            numberType lightDistance2 = lightDir.dot(lightDir);
            lightDir = lightDir.normalize();
            numberType LdotN = std::fmax(0.0, lightDir.dot(normal));
            auto shadow_res = renderer.trace({shadowPointOrig, lightDir});
            bool inShadow = shadow_res && (shadow_res->tNear * shadow_res->tNear < lightDistance2);

            ambient_light += inShadow ? Vector3{0, 0, 0} : light.intensity * LdotN;
            Vector3 reflectionDirection = RayTracer::reflect(-lightDir, normal);
            specularColor += std::pow(std::fmax(0.0, -(reflectionDirection.dot(ray.dir))), hitData.hitObject->material->specularExponent) * light.intensity;
        }

        return ambient_light.mut(hitData.hitObject->evalDiffuseColor(st)) * hitData.hitObject->material->kd + specularColor * hitData.hitObject->material->ks;
    }
};

}

#endif //ANYA_RENDERER_DIFFUSE_HPP
