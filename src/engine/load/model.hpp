//
// Created by Anya on 2022/12/6.
//

#ifndef ANYA_ENGINE_MODEL_HPP
#define ANYA_ENGINE_MODEL_HPP

#include <vector>
#include "math/matrix.hpp"
#include "math/utils.hpp"
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
    Vector3 color{};                            // 颜色信息
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
