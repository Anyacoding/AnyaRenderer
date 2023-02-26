//
// Created by Anya on 2023/1/26.
//

#ifndef ANYA_RENDERER_MESH_HPP
#define ANYA_RENDERER_MESH_HPP

#include "interface/object.hpp"
#include "accelerator/BVH.hpp"

namespace anya {

class Mesh: public Object {
private:
    // BVH树
    std::shared_ptr<BVH> bvh = nullptr;
    // 网格的面积
    numberType area = 0.0;

public:
    explicit Mesh(const std::string& meshPath, const std::shared_ptr<Material>& m) {
        this->material = m;
        loadFromDisk(meshPath);
        bvh = std::make_shared<BVH>(childs);
    }

    void
    loadFromDisk(const std::string& meshPath) {
        std::ifstream ifs(meshPath);
        if (!ifs.is_open()) {
            std::cerr << "can not find the " + meshPath << std::endl;
            exit(-1);
        }

        std::vector<Vector3> vertexes{};      // 从obj读入的所有顶点集合
        Vector3 vertex{};                     // 顶点
        this->area = 0.0;                     // 重新计算面积

        // 每次读入一行，并判断该行的类型
        std::string line, type;
        while (std::getline(ifs, line)) {
            std::istringstream iss(line);
            iss >> type;
            if (type == "v") {
                iss >> vertex.x() >> vertex.y() >> vertex.z();
                vertexes.push_back(vertex);
                this->box = AABB::merge(this->box, vertex);
            }
            else if (type == "f") {
                auto triangle = std::make_shared<Triangle>();
                for (int i = 0; i < 3; ++i) {
                    int index = 0;
                    iss >> index;
                    triangle->setVertex(i, vertexes[index - 1].to4());
                }
                triangle->material = this->material;
                childs.push_back(triangle);
                area += triangle->getArea();
            }
        }
        ifs.close();
        std::cout << "vertex: " << vertexes.size() << ", face: " << childs.size() << std::endl << std::endl;
    }

public:
#pragma region whitted_style api
    [[nodiscard]] Vector3
    evalDiffuseColor(const Vector2& st) const override {
        std::cerr << "Should not get here on Mesh::evalDiffuseColor" << std::endl;
        return {};
    }

#pragma endregion

public:
    [[nodiscard]] std::optional<HitData>
    getIntersect(const Ray& ray) override {
        std::optional<HitData> hitData{};
        if (bvh) hitData = bvh->intersect(ray);
        return hitData;
    }

    [[nodiscard]] AABB
    getBoundingBox() const override {
        return box;
    }

    numberType
    getArea() const override {
        return area;
    }

    std::pair<HitData, numberType>
    sample() const override {
        auto [pos, pdf] = bvh->sample();
        pos.radiance = this->material->emission;
        return { pos, pdf };
    }
};


}

#endif //ANYA_RENDERER_MESH_HPP
