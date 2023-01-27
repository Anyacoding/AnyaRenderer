//
// Created by Anya on 2023/1/12.
//

#ifndef ANYA_RENDERER_RAYTRACER_HPP
#define ANYA_RENDERER_RAYTRACER_HPP

#include "interface/renderer.hpp"
#include "interface/object.hpp"
#include "tool/utils.hpp"
#include "tool/progress.hpp"

namespace anya {

// 本模块实现最基本的光线追踪成像渲染器

struct HitData {
    double tNear = 0.0;
    Vector2 uv = {};
    std::shared_ptr<Object> hitObject = nullptr;
};

class RayTracer: public Renderer {
private:
    // 帧缓存
    std::vector<Vector3> frame_buf;
    // 视窗长宽
    GLdouble view_width = 0.0, view_height = 0.0;
    // 光线追踪最大递归深度
    int maxDepth = 5;
private:
    // 导入友元
    friend class DiffuseMaterial;
    friend class GlassMaterial;

public:
#pragma region renderer方法
    void
    render() override {
        std::tie(view_width, view_height) = scene.camera->getWH();
        frame_buf.assign(static_cast<long long>(view_width * view_height), this->background);
        Progress progress;
        for (int j = 0; j < view_height; ++j) {
            for (int i = 0; i < view_width; ++i) {
                // 相机发出的光线
                auto ray = scene.camera->biuRay(i, j);
                // 利用光线弹射着色函数返回颜色信息
                auto pixel_color = cast_ray(ray, 0);
                int x = i;
                int y = static_cast<int>(view_height) - 1 - j;
                frame_buf[getIndex(x, y)] = pixel_color;
                outPutImage->setPixel(x, y, pixel_color);
            }
            progress.update(double(j) / view_height);
        }
    }

    // 获取像素信息
    [[nodiscard]] Vector3
    getPixel(int x, int y) const override {
        return frame_buf[getIndex(x, y)];
    }
#pragma endregion

private:
#pragma region 光追的主要函数
    Vector3
    cast_ray(const Ray& ray, int depth) {
        // 到达递归最大深度，直接返回
        if (depth > maxDepth) {
            return Vector3{0, 0, 0};
        }

        Vector3 hitColor = this->background;
        auto hitData = trace(ray);

        if (hitData.has_value()) {

            Vector3 hitPoint = ray.at(hitData->tNear);
            Vector3 normal;
            Vector2 st;
            hitData->hitObject->getSurfaceProperties(hitPoint, hitData->uv, normal, st);

            switch (hitData->hitObject->material->type) {
                case DIFFUSE_AND_GLOSSY: {
                    Vector3 ambient_light = {0, 0, 0}, specularColor = {0, 0, 0};
                    Vector3 shadowPointOrig = ray.dir.dot(normal) < 0
                                              ? hitPoint + normal * epsilon
                                              : hitPoint - normal * epsilon;

                    for (auto& light : scene.lights) {
                        Vector3 lightDir = light.position - hitPoint;
                        numberType lightDistance2 = lightDir.dot(lightDir);
                        lightDir = lightDir.normalize();
                        numberType LdotN = std::fmax(0.0, lightDir.dot(normal));
                        auto shadow_res = trace({shadowPointOrig, lightDir});
                        bool inShadow = shadow_res && (shadow_res->tNear * shadow_res->tNear < lightDistance2);

                        ambient_light += inShadow ? Vector3{0, 0, 0} : light.intensity * LdotN;
                        Vector3 reflectionDirection = reflect(-lightDir, normal);
                        specularColor += std::pow(std::fmax(0.0, -(reflectionDirection.dot(ray.dir))), hitData->hitObject->material->specularExponent) * light.intensity;
                    }
                    hitColor = ambient_light.mut(hitData->hitObject->evalDiffuseColor(st)) * hitData->hitObject->material->kd + specularColor * hitData->hitObject->material->ks;
                    break;
                }
                case REFLECTION_AND_REFRACTION: {
                    Vector3 reflectionDirection = reflect(ray.dir, normal).normalize();
                    Vector3 refractionDirection = refract(ray.dir, normal, hitData->hitObject->material->ior).normalize();
                    Vector3 reflectionRayOrig = (reflectionDirection.dot(normal) < 0)
                                                ? hitPoint - normal * epsilon
                                                : hitPoint + normal * epsilon;
                    Vector3 refractionRayOrig = (refractionDirection.dot(normal) < 0)
                                                ? hitPoint - normal * epsilon
                                                : hitPoint + normal * epsilon;

                    Vector3 reflectionColor = cast_ray({ reflectionRayOrig, reflectionDirection }, depth + 1);
                    Vector3 refractionColor = cast_ray({ refractionRayOrig, refractionDirection }, depth + 1);

                    numberType kr = fresnel(ray.dir, normal, hitData->hitObject->material->ior);
                    hitColor = reflectionColor * kr + refractionColor * (1 - kr);
                    break;
                }
                case REFLECTION: {

                }
                default: {
                    std::cerr << "Unknown Material Type!" << std::endl;
                    break;
                }
            }
        }

        return hitColor;
    }

    std::optional<HitData>
    trace(const Ray& ray) {
        numberType tNearK = KMAX;
        std::optional<HitData> payload;
        // 对所有obj进行相交测试，并返回距离最近的相交obj
        for (const auto& obj : scene.objects) {
            // TODO: 临时修改
            for (const auto& child : obj->childs) {
                numberType tNear = KMAX;
                Vector2 uv;
                if (child->intersect(ray, tNear, uv) && tNear < tNearK) {
                    payload.emplace();
                    payload->tNear = tNear;
                    payload->uv = uv;
                    payload->hitObject = child;
                    tNearK = tNear;
                }
            }
        }
        return payload;
    }

    // 反射
    [[nodiscard]] static Vector3
    reflect(const Vector3& wi, const Vector3& normal) {
        return wi - 2 * wi.dot(normal) * normal;
    }

    // 利用 Snell’s Law 求出折射方向
    static Vector3
    refract(const Vector3& wi, Vector3 normal, const numberType& ior) {
        numberType cosi = MathUtils::clamp(-1, 1, wi.dot(normal));
        numberType etai = 1, etat = ior;
        if (cosi < 0) {
            cosi = -cosi;
        }
        else {
            std::swap(etai, etat);
            normal = -normal;
        }
        numberType eta = etai / etat;
        numberType k = 1 - eta * eta * (1 - cosi * cosi);
        return k < 0 ? Vector3{0, 0, 0} : eta * wi + (eta * cosi - sqrt(k)) * normal;
    }

    // 菲涅尔算法
    static numberType
    fresnel(const Vector3& I, const Vector3& N, const numberType& ior) {
        numberType cosi = MathUtils::clamp(-1, 1, I.dot(N));
        numberType etai = 1, etat = ior;
        if (cosi > 0) {
            std::swap(etai, etat);
        }

        numberType sint = etai / etat * sqrt(std::max(0.0, 1 - cosi * cosi));
        if (sint >= 1) {
            return 1;
        }
        else {
            numberType cost = std::sqrt(std::max(0.0, 1 - sint * sint));
            cosi = std::fabs(cosi);
            numberType Rs = ((etat * cosi) - (etai * cost)) / ((etat * cosi) + (etai * cost));
            numberType Rp = ((etai * cosi) - (etat * cost)) / ((etai * cosi) + (etat * cost));
            return (Rs * Rs + Rp * Rp) / 2;
        }
    }

#pragma endregion

private:
#pragma region 辅助函数
    // 获取buffer的下标
    [[nodiscard]] int
    getIndex(int x, int y) const {
        if (out_range(x, y))
            throw std::out_of_range("RayTracer::getIndex(int x, int y)");
        return (static_cast<int>(view_height) - 1 - y) * static_cast<int>(view_width) + x;
    }

    // 越界判断
    [[nodiscard]] constexpr bool
    out_range(numberType x, numberType y) const {
        return x < 0 || x >= view_width || y < 0 || y >= view_height;
    }
#pragma endregion

};


}

#endif //ANYA_RENDERER_RAYTRACER_HPP
