//
// Created by Anya on 2023/1/27.
//

#ifndef ANYA_RENDERER_BVH_HPP
#define ANYA_RENDERER_BVH_HPP

#include "accelerator/AABB.hpp"
#include <chrono>
#include <algorithm>
#include <optional>

namespace anya {

// 层次包围盒
class BVH {
private:
    // BVH树节点
    struct BVHNode {
        AABB box{};
        std::shared_ptr<BVHNode> left = nullptr;
        std::shared_ptr<BVHNode> right = nullptr;
        std::shared_ptr<Object> object = nullptr;
    };

public:
    std::vector<std::shared_ptr<Object>> rawData;  // 原始的对象
    std::shared_ptr<BVHNode> root;                 // BVH树根节点

public:
    explicit BVH(std::vector<std::shared_ptr<Object>>& objs): rawData(objs) {
        auto start = std::chrono::steady_clock::now();

        if (rawData.empty()) return;
        root = buildTree(rawData);

        auto end = std::chrono::steady_clock::now();
        auto time_diff = end - start;
        auto hours = std::chrono::duration_cast<std::chrono::hours>(time_diff);
        auto minutes = std::chrono::duration_cast<std::chrono::minutes>(time_diff - hours);
        auto seconds = std::chrono::duration_cast<std::chrono::seconds>(time_diff - hours - minutes);

        std::cout << "\rBVH Generation Complete! \nTime Taken: " <<  hours.count() << " hours, " << minutes.count() << " minutes, " << seconds.count() << " seconds\n\n";
    }

    static std::shared_ptr<BVHNode>
    buildTree(std::vector<std::shared_ptr<Object>> objects) {
        auto node = std::make_shared<BVHNode>();
        AABB box{};
        for (const auto& object : objects) {
            box = AABB::merge(box, object->getBoundingBox());
        }
        if (objects.size() == 1) {
            node->box = objects[0]->getBoundingBox();
            node->object = objects[0];
            node->left = nullptr;
            node->right = nullptr;
            return node;
        }
        else if (objects.size() == 2) {
            node->left = buildTree(std::vector{objects[0]});
            node->right = buildTree(std::vector{objects[1]});
            node->box = AABB::merge(node->left->box, node->right->box);
            return node;
        }
        else {
            AABB centroidBox{};
            for (const auto& object : objects) {
                centroidBox = AABB::merge(centroidBox, object->getBoundingBox().centroid());
            }

            int dim = centroidBox.maxExtent();

            switch (dim) {
                case 0: {
                    auto cmp = [](const auto& a, const auto& b) {
                        return a->getBoundingBox().centroid().x() <
                               b->getBoundingBox().centroid().x();
                    };
                    std::sort(objects.begin(), objects.end(), cmp);
                    break;
                }
                case 1: {
                    auto cmp = [](const auto& a, const auto& b) {
                        return a->getBoundingBox().centroid().y() <
                               b->getBoundingBox().centroid().y();
                    };
                    std::sort(objects.begin(), objects.end(), cmp);
                    break;
                }
                case 2: {
                    auto cmp = [](const auto& a, const auto& b) {
                        return a->getBoundingBox().centroid().z() <
                               b->getBoundingBox().centroid().z();
                    };
                    std::sort(objects.begin(), objects.end(), cmp);
                    break;
                }
                default:
                    throw std::runtime_error("Should not get here!");
            }

            auto begin = objects.begin();
            auto end = objects.end();
            auto mid = begin + (objects.size() / 2);

            auto leftBox = std::vector<std::shared_ptr<Object>>(begin, mid);
            auto rightBox = std::vector<std::shared_ptr<Object>>(mid, end);

            if (objects.size() != leftBox.size() + rightBox.size())
                throw std::runtime_error("Error when build BVH tree!");

            node->left = buildTree(leftBox);
            node->right = buildTree(rightBox);
            node->box = AABB::merge(node->left->box, node->right->box);
        }
        return node;
    }

public:
    [[nodiscard]] std::optional<HitData>
    intersect(const Ray& ray) const {
        std::optional<HitData> hitData;
        hitData = getIntersect(root, ray);
        return hitData;
    }

private:
    [[nodiscard]] std::optional<HitData>
    getIntersect(const std::shared_ptr<BVHNode>& node, const Ray& ray) const {
        // 没有交点，直接返回
        if (!node->box.intersect(ray)) {
            return {};
        }
        // 叶子结点且有交点 直接返回
        if (node->left == nullptr && node->right == nullptr) {
            return node->object->getIntersect(ray);
        }

        auto lchild = getIntersect(node->left, ray);
        auto rchild = getIntersect(node->right, ray);

        auto ldistance = lchild.has_value() ? lchild->distance : KMAX;
        auto rdistance = rchild.has_value() ? rchild->distance : KMAX;
        return ldistance < rdistance ? lchild : rchild;
    }
};

}

#endif //ANYA_RENDERER_BVH_HPP
