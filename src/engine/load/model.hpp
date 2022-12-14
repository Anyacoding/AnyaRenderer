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
    Vector3 color{};
public:
    // 载入初始坐标
    void
    addLocalPosition(const Vector3& pos) {
        localPositions.push_back(pos);
    }

    // 绕z轴旋转
    void
    RotateAroundZ(numberType angle) {
        modelMat = Transform::RotateAroundZ(angle);
    }

    // 转为齐次坐标顶点
    [[nodiscard]] std::array<Vector4, 3>
    toVector4() const {
        std::array<Vector4, 3> res;
        std::transform(vertexes.begin(), vertexes.end(), res.begin(), [](auto& vec3) {
            return vec3.to4();
        });
        return res;
    }

    // 设置颜色
    void
    setColor(Vector3 col) { color = col; }

    void
    setColor(numberType r, numberType g, numberType b) {
        color = {r / 255.0, g / 255.0, b / 255.0};
    }
};

}

#endif //ANYA_ENGINE_MODEL_HPP
