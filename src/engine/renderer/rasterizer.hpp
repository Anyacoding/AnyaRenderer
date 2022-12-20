//
// Created by Anya on 2022/12/5.
//

#ifndef ANYA_ENGINE_RASTERIZER_HPP
#define ANYA_ENGINE_RASTERIZER_HPP

#include "interface/renderer.hpp"
#include "math/matrix.hpp"
#include "math/utils.hpp"

// 本模块实现最基本的光栅化成像渲染器

namespace anya {

class Rasterizer: public Renderer {
private:
    Matrix44 MVP;          // MVP变换矩阵
    Matrix44 viewPortMat;  // 视窗变换矩阵

    std::vector<Vector3> frame_buf;  // 帧缓存
    std::vector<numberType> z_buf;   // 深度缓存

    std::vector<Vector3> frame_msaa; // MSAA 4倍采样
    std::vector<numberType> z_msaa;

    GLdouble view_width = 0.0, view_height = 0.0;  // 视窗
public:
    void
    render() override {
        std::tie(view_width, view_height) = scene.camera->getWH();

        // 初始化buffer的大小
        frame_buf.assign(static_cast<long long>(view_width * view_height), {0, 0, 0});
        z_buf.assign(static_cast<long long>(view_width * view_height), inf);
        frame_msaa.assign(static_cast<long long>(view_width * view_height * 4), {0, 0, 0});
        z_msaa.assign(static_cast<long long>(view_width * view_height * 4), inf);

        auto viewMat = scene.camera->getViewMat();
        auto projectionMat = scene.camera->getProjectionMat();
        viewPortMat = scene.camera->getViewPortMat();
        // 渲染每个model
        for (auto& model : scene.models) {
            auto modelMat = model.modelMat;    // 获取每个model的modelMat
            MVP =  projectionMat * viewMat * modelMat;
            std::vector<anya::Vector4> worldPositions;
            // TODO: 这里的视口变换逻辑有问题，需要修正
            for (const auto& local : model.localPositions) {
                worldPositions.push_back(viewPortMat * MVP * local.to4());
            }
            model.vertexes.clear();
            for (auto& worldPos : worldPositions) {
                worldPos /= worldPos.w();
                model.vertexes.push_back(worldPos.to<3>());
            }
            drawTriangleWithMSAA(model);
        }
    }

    // 获取像素信息
    [[nodiscard]] Vector3
    getPixel(int x, int y) const override {
        return frame_buf[getIndex(x, y)];
    }

private:
    // 采样
    void
    drawTriangleWithMSAA(const Model& triangle) {
        // 缓存三角形的三个顶点
        auto a = triangle.vertexes[0];
        auto b = triangle.vertexes[1];
        auto c = triangle.vertexes[2];
        // 计算包围盒
        int left, right, floor, top;
        std::tie(left, right, floor, top) = getBoundingBox(a, b, c);
        // MSAA准备
        int pid = 0;               // 当前像素在frame_msaa中的位置
        numberType z_pixel = inf;  // 当前像素的深度
        const std::array<numberType, 4> dx = { 0.25, 0.25, 0.75, 0.75 };
        const std::array<numberType, 4> dy = { 0.25, 0.75, 0.25, 0.75 };
        // z-buffer算法
        for (int i = left; i <= right; ++i) {
            for (int j = floor; j <= top; ++j) {
                pid = getIndex(i, j) * 4;
                // 4倍采样进行模糊
                for (int k = 0; k < 4; ++k) {
                    if (insideTriangle(i + dx[k], j + dy[k], triangle.vertexes)) {
                        // 获取插值深度
                        numberType z_interpolated = computeBarycentric2D(i + dx[k], j + dy[k], triangle);
                        // 深度测试
                        if (z_interpolated < z_msaa[pid + k]) {
                            z_msaa[pid + k] = z_interpolated;
                            frame_msaa[pid + k] = triangle.color / 4;
                        }
                    }
                }
                frame_buf[getIndex(i, j)] = frame_msaa[pid] + frame_msaa[pid + 1] + frame_msaa[pid + 2] + frame_msaa[pid + 3];
            }
        }
    }

    void
    drawTriangle(const Model& triangle) {
        // 缓存三角形的三个顶点
        auto a = triangle.vertexes[0];
        auto b = triangle.vertexes[1];
        auto c = triangle.vertexes[2];
        // 计算包围盒
        int left, right, floor, top;
        std::tie(left, right, floor, top) = getBoundingBox(a, b, c);
        // z-buffer算法
        for (int i = left; i <= right; ++i) {
            for (int j = floor; j <= top; ++j) {
                if (insideTriangle(i + 0.5, j + 0.5, triangle.vertexes)) {
                    // 获取插值深度
                    numberType z_interpolated = computeBarycentric2D(i + 0.5, j + 0.5, triangle);
                    // 深度测试
                    if (z_interpolated < z_buf[getIndex(i, j)]) {
                        z_buf[getIndex(i, j)] = z_interpolated;
                        frame_buf[getIndex(i, j)] = triangle.color;
                    }
                }
            }
        }
    }

private:
#pragma region 辅助函数
    // 叉积判断点是否在三角形内
    bool
    insideTriangle(numberType x, numberType y, const std::vector<Vector3>& vertexes) {
        if (out_range(x, y))
            throw std::out_of_range("Rasterizer::insideTriangle()");
        Vector3 q{x, y, vertexes[0][2]};
        Vector3 ab = vertexes[1] - vertexes[0], bc = vertexes[2] - vertexes[1], ca = vertexes[0] - vertexes[2];
        Vector3 aq = q - vertexes[0], bq = q - vertexes[1], cq = q - vertexes[2];
        return ab.cross(aq).dot(bc.cross(bq)) > 0 && ab.cross(aq).dot(ca.cross(cq)) > 0 && bc.cross(bq).dot(ca.cross(cq)) > 0;
    }

    // 获取buffer的下标
    [[nodiscard]] int
    getIndex(int x, int y) const {
        if (out_range(x, y))
            throw std::out_of_range("Rasterizer::getIndex(int x, int y)");
        return (static_cast<int>(view_height) - 1 - y) * static_cast<int>(view_width) + x;
    }

    // 插值获取深度
    numberType
    computeBarycentric2D(numberType x, numberType y, const Model& triangle) {
        if (out_range(x, y))
            throw std::out_of_range("Rasterizer::computeBarycentric2D()");
        const std::vector<Vector3>& vertexes = triangle.vertexes;
        auto v = triangle.toVector4();
        numberType alpha = (x*(vertexes[1].y() - vertexes[2].y()) + (vertexes[2].x() - vertexes[1].x())*y + vertexes[1].x()* vertexes[2].y() - vertexes[2].x()* vertexes[1].y()) / (vertexes[0].x()*(vertexes[1].y() - vertexes[2].y()) + (vertexes[2].x() - vertexes[1].x())* vertexes[0].y() + vertexes[1].x()* vertexes[2].y() - vertexes[2].x()* vertexes[1].y());
        numberType beta = (x*(vertexes[2].y() - vertexes[0].y()) + (vertexes[0].x() - vertexes[2].x())*y + vertexes[2].x()* vertexes[0].y() - vertexes[0].x()* vertexes[2].y()) / (vertexes[1].x()*(vertexes[2].y() - vertexes[0].y()) + (vertexes[0].x() - vertexes[2].x())* vertexes[1].y() + vertexes[2].x()* vertexes[0].y() - vertexes[0].x()* vertexes[2].y());
        numberType gamma = (x*(vertexes[0].y() - vertexes[1].y()) + (vertexes[1].x() - vertexes[0].x())*y + vertexes[0].x()* vertexes[1].y() - vertexes[1].x()* vertexes[0].y()) / (vertexes[2].x()*(vertexes[0].y() - vertexes[1].y()) + (vertexes[1].x() - vertexes[0].x())* vertexes[2].y() + vertexes[0].x()* vertexes[1].y() - vertexes[1].x()* vertexes[0].y());
        numberType w_reciprocal = 1.0 / (alpha / v[0].w() + beta / v[1].w() + gamma / v[2].w());
        numberType z_interpolated = alpha * v[0].z() / v[0].w() + beta * v[1].z() / v[1].w() + gamma * v[2].z() / v[2].w();
        z_interpolated *= -w_reciprocal;
        return z_interpolated;
    }

    // 计算包围盒
    static std::tuple<int, int, int, int>
    getBoundingBox(Vector3 a, Vector3 b, Vector3 c) {
        int left = std::min(static_cast<int>(a.x()), std::min(static_cast<int>(b.x()), static_cast<int>(c.x())));
        int right = std::max(static_cast<int>(a.x()), std::max(static_cast<int>(b.x()), static_cast<int>(c.x()))) + 1;
        int floor = std::min(static_cast<int>(a.y()), std::min(static_cast<int>(b.y()), static_cast<int>(c.y())));
        int top = std::max(static_cast<int>(a.y()), std::max(static_cast<int>(b.y()), static_cast<int>(c.y()))) + 1;
        return {left, right, floor, top};
    }

    // 越界判断
    [[nodiscard]] constexpr bool
    out_range(numberType x, numberType y) const {
        return x < 0 || x >= view_width || y < 0 || y >= view_height;
    }
#pragma endregion

};

}

#endif //ANYA_ENGINE_RASTERIZER_HPP
