//
// Created by Anya on 2022/12/5.
//

#ifndef ANYA_ENGINE_RASTERIZER_HPP
#define ANYA_ENGINE_RASTERIZER_HPP

#include "interface/renderer.hpp"
#include "tool/utils.hpp"
#include "accelerator/clip.hpp"

// 本模块实现最基本的光栅化成像渲染器

namespace anya {

class Rasterizer: public Renderer {
private:
    Matrix44 MVP;          // MVP变换矩阵
    Matrix44 viewPortMat;  // 视窗变换矩阵
    Matrix44 invMat;       // 作用于法线的矩阵

    std::vector<Vector3> frame_buf;  // 帧缓存
    std::vector<numberType> z_buf;   // 深度缓存
    std::vector<Vector3> frame_msaa; // MSAA 4倍采样
    std::vector<numberType> z_msaa;

    GLdouble view_width = 0.0, view_height = 0.0;  // 视窗
    numberType fixed = 1.0;                        // 重心坐标修正系数

public:
#pragma region renderer方法
    void
    render() override {
        std::tie(view_width, view_height) = scene.camera->getWH();
        // 初始化buffer的大小   绿幕: Vector3{92, 121.0, 92.0} / 255   灰慕: Vector3{38.25, 38.25, 38.25} / 255
        frame_buf.assign(static_cast<long long>(view_width * view_height), this->background);
        z_buf.assign(static_cast<long long>(view_width * view_height), inf);
        frame_msaa.assign(static_cast<long long>(view_width * view_height * 4), this->background / 4);
        z_msaa.assign(static_cast<long long>(view_width * view_height * 4), inf);
        outPutImage->clearWith(this->background);

        // 获取MVP矩阵
        auto viewMat = scene.camera->getViewMat();
        auto projectionMat = scene.camera->getProjectionMat();
        viewPortMat = scene.camera->getViewPortMat();

        // 缓存深度信息修正参数
        auto[f1, f2] = scene.camera->getFixedArgs();

        // 渲染每个model
        for (auto& model : scene.models) {
            // 获取每个model的modelMat
            auto modelMat = model.modelMat;
            MVP =  projectionMat * viewMat * modelMat;
            invMat = (viewMat * modelMat).inverse().transpose();

            for (auto triangle : model.TriangleList) {
                // viewSpace顶点集合
                std::vector<Vector4> viewSpace{};
                for (auto& vertex : triangle.vertexes) {
                    viewSpace.push_back(viewMat * modelMat * vertex);
                    vertex = viewPortMat * MVP * vertex;
                    // 修正深度信息，方便后续插值
                    auto w = vertex.w();
                    vertex /= w;
                    vertex.w() = w;
                    vertex.z() = vertex.z() * f1 + f2;
                }

                // 剔除优化
                if (ClipUtils::back_face_culling(triangle)) continue;

                for (auto& normal : triangle.normals) {
                    // 对法线进行变换
                    normal = invMat * normal;
                }

            #ifndef Z_BUFFER_TEST
                triangle.setColor(0, 148, 121.0, 92.0);
                triangle.setColor(1, 148, 121.0, 92.0);
                triangle.setColor(2, 148, 121.0, 92.0);
            #endif
                drawTriangleWithMSAA(triangle, viewSpace, model.fragmentShader);
            }
        }
    }

    // 获取像素信息
    [[nodiscard]] Vector3
    getPixel(int x, int y) const override {
        return frame_buf[getIndex(x, y)];
    }

#pragma endregion

private:
#pragma region 采样
    void
    drawTriangle(const Triangle& triangle, const std::vector<Vector4>& viewSpace, FragmentShader& fragmentShader) {
        // 缓存三角形的三个顶点
        auto a = triangle.a();
        auto b = triangle.b();
        auto c = triangle.c();
        // 计算包围盒
        auto[left, right, floor, top] = getBoundingBox(a, b, c);
        // z-buffer算法
        for (int j = floor; j <= top; ++j) {
            for (int i = left; i <= right; ++i) {
                if (insideTriangle(i + 0.5, j + 0.5, triangle.vertexes)) {
                    // 获取插值深度
                    auto[alpha, beta, gamma] = computeBarycentric2DWithFixed(i + 0.5, j + 0.5, triangle);
                    numberType z_lerp = MathUtils::interpolate(alpha, beta, gamma, triangle.vertexes[0].z(), triangle.vertexes[1].z(), triangle.vertexes[2].z(), fixed);
                    // 深度测试
                    if (z_lerp < z_buf[getIndex(i, j)]) {
                        z_buf[getIndex(i, j)] = z_lerp;
                        auto normal_lerp = MathUtils::interpolate(alpha, beta, gamma, triangle.normals[0], triangle.normals[1], triangle.normals[2], fixed);
                        auto color_lerp = MathUtils::interpolate(alpha, beta, gamma, triangle.colors[0], triangle.colors[1], triangle.colors[2], fixed);
                        auto uv_lerp = MathUtils::interpolate(alpha, beta, gamma, triangle.uvs[0], triangle.uvs[1], triangle.uvs[2], fixed);
                        auto shadingcoords_lerp = MathUtils::interpolate(alpha, beta, gamma, viewSpace[0], viewSpace[1], viewSpace[2], fixed);

                        // 法向量要记得单位化!!
                        fragmentShader.init(shadingcoords_lerp, color_lerp, normal_lerp.to<3>().normalize().to4(0), uv_lerp);
                        auto pixel_color = fragmentShader.process(fragmentShader);
                        frame_buf[getIndex(i, j)] = pixel_color;
                        outPutImage->setPixel(i, j, pixel_color);
                    }
                }
            }
        }
    }

    void
    drawTriangleWithMSAA(const Triangle& triangle, const std::vector<Vector4>& viewSpace, FragmentShader& fragmentShader) {
        // 缓存三角形的三个顶点
        auto a = triangle.a();
        auto b = triangle.b();
        auto c = triangle.c();
        // 计算包围盒
        auto[left, right, floor, top] = getBoundingBox(a, b, c);
        // MSAA准备
        int pid = 0;               // 当前像素在frame_msaa中的位置
        const std::array<numberType, 4> dx = { 0.25, 0.25, 0.75, 0.75 };
        const std::array<numberType, 4> dy = { 0.25, 0.75, 0.25, 0.75 };
        // z-buffer算法
        for (int j = floor; j <= top; ++j) {
            for (int i = left; i <= right; ++i) {
                pid = getIndex(i, j) * 4;
                // 4倍采样进行模糊
                for (int k = 0; k < 4; ++k) {
                    if (insideTriangle(i + dx[k], j + dy[k], triangle.vertexes)) {
                        // 获取插值深度
                        auto[alpha, beta, gamma] = computeBarycentric2DWithFixed(i + dx[k], j + dy[k], triangle);
                        numberType z_lerp = MathUtils::interpolate(alpha, beta, gamma, triangle.vertexes[0].z(), triangle.vertexes[1].z(), triangle.vertexes[2].z(), fixed);
                        // 深度测试
                        if (z_lerp < z_msaa[pid + k]) {
                            z_msaa[pid + k] = z_lerp;
                            auto normal_lerp = MathUtils::interpolate(alpha, beta, gamma, triangle.normals[0], triangle.normals[1], triangle.normals[2], fixed);
                            auto color_lerp = MathUtils::interpolate(alpha, beta, gamma, triangle.colors[0], triangle.colors[1], triangle.colors[2], fixed);
                            auto uv_lerp = MathUtils::interpolate(alpha, beta, gamma, triangle.uvs[0], triangle.uvs[1], triangle.uvs[2], fixed);
                            auto shadingcoords_lerp = MathUtils::interpolate(alpha, beta, gamma, viewSpace[0], viewSpace[1], viewSpace[2], fixed);

                            // 法向量要记得单位化!!
                            fragmentShader.init(shadingcoords_lerp, color_lerp, normal_lerp.to<3>().normalize().to4(0), uv_lerp);
                            auto pixel_color = fragmentShader.process(fragmentShader);
                            frame_msaa[pid + k] = pixel_color / 4;
                        }
                    }
                }
                frame_buf[getIndex(i, j)] = frame_msaa[pid] + frame_msaa[pid + 1] + frame_msaa[pid + 2] + frame_msaa[pid + 3];
                outPutImage->setPixel(i, j, frame_buf[getIndex(i, j)]);
            }
        }
    }
#pragma endregion

private:
#pragma region 辅助函数
    // 叉积判断点是否在三角形内
    bool
    insideTriangle(numberType x, numberType y, const std::array<Vector4, 3>& vertexes) {
        if (out_range(x, y)) {
            return false;
        }
        Vector3 q{x, y, vertexes[0].to<3>()[2]};
        Vector3 ab = vertexes[1].to<3>() - vertexes[0].to<3>(), bc = vertexes[2].to<3>() - vertexes[1].to<3>(), ca = vertexes[0].to<3>() - vertexes[2].to<3>();
        Vector3 aq = q - vertexes[0].to<3>(), bq = q - vertexes[1].to<3>(), cq = q - vertexes[2].to<3>();
        return ab.cross(aq).dot(bc.cross(bq)) > 0 && ab.cross(aq).dot(ca.cross(cq)) > 0 && bc.cross(bq).dot(ca.cross(cq)) > 0;
    }

    // 获取buffer的下标
    [[nodiscard]] int
    getIndex(int x, int y) const {
        if (out_range(x, y))
            throw std::out_of_range("Rasterizer::getIndex(int x, int y)");
        return (static_cast<int>(view_height) - 1 - y) * static_cast<int>(view_width) + x;
    }

    // 获取重心坐标
    std::tuple<numberType, numberType, numberType>
    computeBarycentric2DWithFixed(numberType x, numberType y, const Triangle& triangle) {
        if (out_range(x, y))
            throw std::out_of_range("Rasterizer::computeBarycentric2D()");
        const auto& vertexes = triangle.vertexes;
        numberType alpha = (x*(vertexes[1].y() - vertexes[2].y()) + (vertexes[2].x() - vertexes[1].x())*y + vertexes[1].x()* vertexes[2].y() - vertexes[2].x()* vertexes[1].y()) / (vertexes[0].x()*(vertexes[1].y() - vertexes[2].y()) + (vertexes[2].x() - vertexes[1].x())* vertexes[0].y() + vertexes[1].x()* vertexes[2].y() - vertexes[2].x()* vertexes[1].y());
        numberType beta = (x*(vertexes[2].y() - vertexes[0].y()) + (vertexes[0].x() - vertexes[2].x())*y + vertexes[2].x()* vertexes[0].y() - vertexes[0].x()* vertexes[2].y()) / (vertexes[1].x()*(vertexes[2].y() - vertexes[0].y()) + (vertexes[0].x() - vertexes[2].x())* vertexes[1].y() + vertexes[2].x()* vertexes[0].y() - vertexes[0].x()* vertexes[2].y());
        numberType gamma = (x*(vertexes[0].y() - vertexes[1].y()) + (vertexes[1].x() - vertexes[0].x())*y + vertexes[0].x()* vertexes[1].y() - vertexes[1].x()* vertexes[0].y()) / (vertexes[2].x()*(vertexes[0].y() - vertexes[1].y()) + (vertexes[1].x() - vertexes[0].x())* vertexes[2].y() + vertexes[0].x()* vertexes[1].y() - vertexes[1].x()* vertexes[0].y());

        fixed = 1.0 / (alpha / vertexes[0].w() + beta / vertexes[1].w() + gamma / vertexes[2].w());
        alpha  = alpha / vertexes[0].w();
        beta  = beta / vertexes[1].w();
        gamma  = gamma / vertexes[2].w();
        return { alpha, beta, gamma };
    }

    // 计算包围盒
    [[nodiscard]] std::tuple<int, int, int, int>
    getBoundingBox(Vector4 a, Vector4 b, Vector4 c) const {
        int left = static_cast<int>(std::min(a.x(), std::min(b.x(), c.x())));
        int right = static_cast<int>(std::max(a.x(), std::max(b.x(), c.x())));
        int floor = static_cast<int>(std::min(a.y(), std::min(b.y(), c.y())));
        int top = static_cast<int>(std::max(a.y(), std::max(b.y(), c.y())));
        return {left >= 0 ? left : 0, right < view_width ? right : view_width - 1, floor >= 0 ? floor : 0, top < view_height ? top : view_height - 1};
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
