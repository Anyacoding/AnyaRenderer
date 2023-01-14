//
// Created by Anya on 2023/1/13.
//

#ifndef ANYA_RENDERER_RAY_HPP
#define ANYA_RENDERER_RAY_HPP

#include "tool/vec.hpp"

namespace anya {

class Ray {
public:
    Vector3 pos{};
    Vector3 dir{};
public:
    [[nodiscard]] Vector3
    at(numberType tNear) const { return pos + tNear * dir; }
};

}

#endif //ANYA_RENDERER_RAY_HPP
