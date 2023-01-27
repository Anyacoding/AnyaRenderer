//
// Created by Anya on 2023/1/26.
//

#ifndef ANYA_RENDERER_MESH_HPP
#define ANYA_RENDERER_MESH_HPP

#include "interface/object.hpp"

namespace anya {

class Mesh: public Object {
public:
    explicit Mesh(const std::string& meshPath, const std::shared_ptr<Material>& m) {
        this->material = m;
        loadFromDisk(meshPath);
    }

    void
    loadFromDisk(const std::string& meshPath) {
        std::ifstream ifs(meshPath);
        if (!ifs.is_open()) {
            std::cerr << "can not find the " + meshPath << std::endl;
            exit(-1);
        }

        std::vector<Vector3> vertexes{};  // 从obj读入的所有顶点集合
        Vector3 vertex{};                 // 顶点

        // 每次读入一行，并判断该行的类型
        std::string line, type;
        while (std::getline(ifs, line)) {
            std::istringstream iss(line);
            iss >> type;
            if (type == "v") {
                iss >> vertex.x() >> vertex.y() >> vertex.z();
                vertexes.push_back(vertex);
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
            }
        }
        ifs.close();
        std::cout << "vertex: " << vertexes.size() << ", face: " << childs.size() << std::endl;
    }

public:
#pragma region whitted_style api
    [[nodiscard]] bool
    intersect(const Ray& ray, numberType& tNear, Vector2& uv) const override {
        bool intersect = false;
        std::cerr << "Should not get here on Mesh::intersect()" << std::endl;
        return intersect;
    }

    void
    getSurfaceProperties(const Vector3& hitPoint, const Vector2& uv,
                         Vector3& normal, Vector2& st) const override {
        std::cerr << "Should not get here on Mesh::getSurfaceProperties()" << std::endl;
    }

    [[nodiscard]] Vector3
    evalDiffuseColor(const Vector2& st) const override {
        std::cerr << "Should not get here on Mesh::evalDiffuseColor" << std::endl;
        return {};
    }
#pragma endregion

};


}

#endif //ANYA_RENDERER_MESH_HPP
