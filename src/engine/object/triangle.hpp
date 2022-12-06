//
// Created by Anya on 2022/12/6.
//

#ifndef ANYA_ENGINE_TRIANGLE_HPP
#define ANYA_ENGINE_TRIANGLE_HPP

#include "math/matrix.hpp"
#include <array>

namespace anya {

class Triangle {
public:
    std::array<Vector3, 3> vertexes;
    std::array<Vector3, 3> colors;
public:
    [[nodiscard]] Vector3 a() const { return vertexes[0]; }
    [[nodiscard]] Vector3 b() const { return vertexes[1]; }
    [[nodiscard]] Vector3 c() const { return vertexes[2]; }

    void
    setVertex(int index, Vector3 vertex) {
        vertexes[index] = vertex;
    }

    void
    setColor(int index, double r, double g, double b) {
        if ((r < 0.0) || (r > 255.) || (g < 0.0) || (g > 255.) || (b < 0.0) ||
            (b > 255.)) {
            throw std::runtime_error("Invalid color values");
        }
        colors[index] = make_Vec(r / 255.0, g / 255.0, b / 255.0);
    }
};

}

#endif //ANYA_ENGINE_TRIANGLE_HPP
