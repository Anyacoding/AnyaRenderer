//
// Created by Anya on 2022/12/6.
//

#ifndef ANYA_ENGINE_MODEL_HPP
#define ANYA_ENGINE_MODEL_HPP

#include <vector>
#include "math/vec.hpp"

namespace anya {

enum class Primitive{
    Line,
    Triangle
};

class Model {
public:
    std::vector<anya::Vector3> localPositions;  // model未变换前的相对坐标
    std::vector<anya::Vector3> vertexes;        // 经过变换后的顶点坐标
public:
    void
    addLocalPosition(const Vector3& pos) {
        localPositions.push_back(pos);
    }
};

}

#endif //ANYA_ENGINE_MODEL_HPP
