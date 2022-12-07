//
// Created by Anya on 2022/12/6.
//

#ifndef ANYA_ENGINE_MODEL_HPP
#define ANYA_ENGINE_MODEL_HPP

#include <vector>
#include "math/matrix.hpp"
#include "math/utils.hpp"

namespace anya {

enum class Primitive{
    Line,
    Triangle
};

class Model {
public:
    std::vector<anya::Vector3> localPositions;  // model未变换前的相对坐标
    std::vector<anya::Vector3> vertexes;        // 经过变换后的顶点坐标
    Matrix44 modelMat = Matrix44::Identity();   // 模型变换矩阵
public:
    void
    addLocalPosition(const Vector3& pos) {
        localPositions.push_back(pos);
    }

    void
    RotateAroundZ(numberType angle) {
        modelMat = Transform::RotateAroundZ(angle);
    }
};

}

#endif //ANYA_ENGINE_MODEL_HPP
