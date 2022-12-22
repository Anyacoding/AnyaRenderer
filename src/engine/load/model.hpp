//
// Created by Anya on 2022/12/6.
//

#ifndef ANYA_ENGINE_MODEL_HPP
#define ANYA_ENGINE_MODEL_HPP

#include <vector>
#include <fstream>
#include <sstream>
#include "tool/matrix.hpp"
#include "tool/utils.hpp"
#include "component/object/triangle.hpp"

namespace anya {

enum class Primitive{
    Line,
    Triangle
};

class Model {
public:
    std::vector<anya::Triangle> TriangleList;   // model的三角形集合
    Matrix44 modelMat = Matrix44::Identity();   // 模型变换矩阵
public:
    explicit Model(const std::string& modelPath) { loadFromDisk(modelPath); }

    // 从本地加载obj文件的数据
    void
    loadFromDisk(const std::string& modelPath) {
        std::ifstream ifs(modelPath);
        if (!ifs.is_open()) {
            std::cerr << "can not find the " + modelPath << std::endl;
            exit(-1);
        }
        std::vector<Vector3> vertexes{};  // 从obj读入的所有顶点集合
        Vector3 vertex{};                 // 单个顶点
        int index{};                      // face对vertexes的索引
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
                Triangle triangle{};
                for (int i = 0; i < 3; ++i) {
                    iss >> index;
                    triangle.setVertex(i, vertexes[index - 1].to4());
                }
                TriangleList.push_back(triangle);
            }
        }
        ifs.close();
        std::cout << "vertex: " << vertexes.size() << ", face: " << TriangleList.size() << std::endl;
    }
public:
    // 绕z轴旋转
    void
    RotateAroundZ(numberType angle) {
        modelMat = Transform::RotateAroundZ(angle);
    }
    // 绕任意轴轴旋转
    void
    RotateAroundN(numberType angle, Vector3 axis) {
        modelMat = Transform::RotateAroundN(angle, axis);
    }
};

}

#endif //ANYA_ENGINE_MODEL_HPP
