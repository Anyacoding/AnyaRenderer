//
// Created by Anya on 2022/12/6.
//

#ifndef ANYA_ENGINE_TRIANGLE_HPP
#define ANYA_ENGINE_TRIANGLE_HPP

#include "tool/matrix.hpp"
#include "load/texture.hpp"
#include <array>

namespace anya {

class Triangle {
public:
    std::array<Vector4, 3> vertexes;  // 顶点集合
    std::array<Vector4, 3> normals;   // 法线集合
    std::array<Vector3, 3> colors;    // 颜色集合
    std::array<Vector<2>, 3> uvs;     // 纹理坐标集合
public:
    [[nodiscard]] Vector4 a() const { return vertexes[0]; }
    [[nodiscard]] Vector4 b() const { return vertexes[1]; }
    [[nodiscard]] Vector4 c() const { return vertexes[2]; }

    // 设置顶点
    void
    setVertex(int index, Vector4 vertex) {
        vertexes[index] = vertex;
    }

    // 设置法线
    void
    setNormal(int index, Vector4 normal) {
        normals[index] = normal;
    }

    // 设置颜色
    void
    setColor(int index, Vector3 col) { colors[index] = col; }

    void
    setColor(int index, double r, double g, double b) {
        if ((r < 0.0) || (r > 255.) || (g < 0.0) || (g > 255.) || (b < 0.0) ||
            (b > 255.)) {
            throw std::runtime_error("Invalid color values");
        }
        colors[index] = make_Vec(r / 255.0, g / 255.0, b / 255.0);
    }

    // 设置纹理坐标
    void
    setUV(int index, Vector<2> uv) { uvs[index] = uv; }

    void
    setUV(int index, numberType u, numberType v) { uvs[index] = { u, v }; }
};

}

#endif //ANYA_ENGINE_TRIANGLE_HPP
