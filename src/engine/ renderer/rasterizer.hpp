//
// Created by Anya on 2022/12/5.
//

#ifndef ANYA_ENGINE_RASTERIZER_HPP
#define ANYA_ENGINE_RASTERIZER_HPP

#include "interface/renderer.hpp"
#include "math/matrix.hpp"

// 本模块实现最基本的光栅化成像渲染器

namespace anya {

class Rasterizer: public Renderer {
private:
    Matrix44 MVP;       // MVP变换矩阵
    Matrix44 viewPort;  // 视口变换矩阵
public:
    void render() override {

    }
};

}

#endif //ANYA_ENGINE_RASTERIZER_HPP
