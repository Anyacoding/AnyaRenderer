//
// Created by Anya on 2023/1/12.
//

#ifndef ANYA_RENDERER_RAYTRACER_HPP
#define ANYA_RENDERER_RAYTRACER_HPP

#include "interface/renderer.hpp"
#include "interface/object.hpp"
#include "tool/utils.hpp"
#include "tool/progress.hpp"
#include <functional>

namespace anya {

// 本模块实现最基本的光线追踪成像渲染器

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

        auto start = std::chrono::steady_clock::now();
        for (int j = 0; j < view_height; ++j) {
            for (int i = 0; i < view_width; ++i) {
                // 相机发出的光线
                auto ray = scene.camera->biuRay(i, j);
                // 利用光线弹射着色函数返回颜色信息
                Vector3 pixel_color{};
                for (int k = 0; k < spp; ++k) {
                    pixel_color += whitted_style(ray, 0) / spp;
                }
                // 将像素写入帧缓存
                int x = i;
                int y = static_cast<int>(view_height) - 1 - j;
                frame_buf[getIndex(x, y)] = pixel_color;
                outPutImage->setPixel(x, y, pixel_color);
            }
            progress.update(double(j) / view_height);
        }
        progress.update(1.0);
        auto end = std::chrono::steady_clock::now();

        auto time_diff = end - start;
        auto hours = std::chrono::duration_cast<std::chrono::hours>(time_diff);
        auto minutes = std::chrono::duration_cast<std::chrono::minutes>(time_diff - hours);
        auto seconds = std::chrono::duration_cast<std::chrono::seconds>(time_diff - hours - minutes);
        std::cout << "\n\n\rRendering Complete! \nTime Taken: " <<  hours.count() << " hours, " << minutes.count() << " minutes, " << seconds.count() << " seconds\n";
    }

    // 获取像素信息
    [[nodiscard]] Vector3
    getPixel(int x, int y) const override {
        return frame_buf[getIndex(x, y)];
    }

private:
    Vector3
    cast_ray (const Ray& ray, int depth) {
        switch (mode) {
            case Mode::WHITTED_STYLE: {
                return whitted_style(ray, depth);
            }
            case Mode::PATH_TRACING: {
                return path_tracing(ray);
            }
            default: {
                std::cerr << "Unknown RayTracer Mode Type!" << std::endl;
                break;
            }
        }
    }

#pragma endregion

private:
#pragma region 光追方法: whitted_style
    Vector3
    whitted_style (const Ray& ray, int depth) {
        // 到达递归最大深度，直接返回
        if (depth > maxDepth) {
            return Vector3{0, 0, 0};
        }

        Vector3 hitColor = this->background;
        auto hitData = intersect(ray);

        if (hitData.has_value()) {
            Vector3 hitPoint = hitData->hitPoint;
            Vector3 normal = hitData->normal;
            Vector2 st = hitData->st;

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

                        auto shadow_res = intersect({shadowPointOrig, lightDir});
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

                    Vector3 reflectionColor = whitted_style({ reflectionRayOrig, reflectionDirection }, depth + 1);
                    Vector3 refractionColor = whitted_style({ refractionRayOrig, refractionDirection }, depth + 1);

                    numberType kr = fresnel(ray.dir, normal, hitData->hitObject->material->ior);
                    hitColor = reflectionColor * kr + refractionColor * (1 - kr);
                    break;
                }
                default: {
                    std::cerr << "Unknown Material Type!" << std::endl;
                    break;
                }
            }
        }
        return hitColor;
    }

#pragma endregion

private:
#pragma region 光追方法: path_tracing
    Vector3
    path_tracing (const Ray& ray) {
        auto hitData = intersect(ray);
        if (!hitData.has_value()) return Vector3{};

    }

#pragma endregion

private:
#pragma region 数学物理模型
    // 相交
    [[nodiscard]] std::optional<HitData>
    intersect(const Ray &ray) const {
        return this->scene.bvh->intersect(ray);
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
