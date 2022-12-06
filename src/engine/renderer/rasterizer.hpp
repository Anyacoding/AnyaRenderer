//
// Created by Anya on 2022/12/5.
//

#ifndef ANYA_ENGINE_RASTERIZER_HPP
#define ANYA_ENGINE_RASTERIZER_HPP

#include "interface/renderer.hpp"
#include "math/matrix.hpp"
#include "object/triangle.hpp"

// 本模块实现最基本的光栅化成像渲染器

namespace anya {

class Rasterizer: public Renderer {
private:
    Matrix44 MVP;          // MVP变换矩阵

    GLdouble view_width = 0.0, view_height = 0.0;  // 视窗
public:
    void
    render() override {
        std::tie(view_width, view_height) = scene.camera->getWH();

        auto modelMat = Matrix44::Identity();   // 先不对模型做任何变换，故为单位矩阵
        auto viewMat = scene.camera->getViewMat();
        auto projectionMat = scene.camera->getProjectionMat();
        MVP =  projectionMat * viewMat * modelMat;

        // 渲染每个model
        for (auto& model : scene.models) {
            Triangle  triangle{};
            std::vector<anya::Vector4> worldPositions;
            for (const auto& local : model.localPositions) {
                worldPositions.push_back(MVP * local.to4());
            }
            for (auto& worldPos : worldPositions) {
                worldPos /= worldPos.w();
                model.vertexes.push_back(worldPos.to<3>());
            }
        }
    }
};

}

#endif //ANYA_ENGINE_RASTERIZER_HPP
