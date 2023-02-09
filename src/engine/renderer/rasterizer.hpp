//
// Created by Anya on 2022/12/5.
//

#ifndef ANYA_ENGINE_RASTERIZER_HPP
#define ANYA_ENGINE_RASTERIZER_HPP

#include "interface/renderer.hpp"
#include "tool/utils.hpp"
#include "accelerator/clip.hpp"

// ��ģ��ʵ��������Ĺ�դ��������Ⱦ��

namespace anya {

class Rasterizer: public Renderer {
private:
    Matrix44 MVP;          // MVP�任����
    Matrix44 viewPortMat;  // �Ӵ��任����
    Matrix44 invMat;       // �����ڷ��ߵľ���

    std::vector<Vector3> frame_buf;  // ֡����
    std::vector<numberType> z_buf;   // ��Ȼ���
    std::vector<Vector3> frame_msaa; // MSAA 4������
    std::vector<numberType> z_msaa;

    GLdouble view_width = 0.0, view_height = 0.0;  // �Ӵ�
    numberType fixed = 1.0;                        // ������������ϵ��

public:
#pragma region renderer����
    void
    render() override {
        std::tie(view_width, view_height) = scene.camera->getWH();
        // ��ʼ��buffer�Ĵ�С   ��Ļ: Vector3{92, 121.0, 92.0} / 255   ��Ľ: Vector3{38.25, 38.25, 38.25} / 255
        frame_buf.assign(static_cast<long long>(view_width * view_height), this->background);
        z_buf.assign(static_cast<long long>(view_width * view_height), inf);
        frame_msaa.assign(static_cast<long long>(view_width * view_height * 4), this->background / 4);
        z_msaa.assign(static_cast<long long>(view_width * view_height * 4), inf);
        outPutImage->clearWith(this->background);

        // ��ȡMVP����
        auto viewMat = scene.camera->getViewMat();
        auto projectionMat = scene.camera->getProjectionMat();
        viewPortMat = scene.camera->getViewPortMat();

        // ���������Ϣ��������
        auto[f1, f2] = scene.camera->getFixedArgs();

        // ��Ⱦÿ��model
        for (auto& model : scene.models) {
            // ��ȡÿ��model��modelMat
            auto modelMat = model.modelMat;
            MVP =  projectionMat * viewMat * modelMat;
            invMat = (viewMat * modelMat).inverse().transpose();

            for (auto triangle : model.TriangleList) {
                // viewSpace���㼯��
                std::vector<Vector4> viewSpace{};
                for (auto& vertex : triangle.vertexes) {
                    viewSpace.push_back(viewMat * modelMat * vertex);
                    vertex = viewPortMat * MVP * vertex;
                    // ���������Ϣ�����������ֵ
                    auto w = vertex.w();
                    vertex /= w;
                    vertex.w() = w;
                    vertex.z() = vertex.z() * f1 + f2;
                }

                // �޳��Ż�
                if (ClipUtils::back_face_culling(triangle)) continue;

                for (auto& normal : triangle.normals) {
                    // �Է��߽��б任
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

    // ��ȡ������Ϣ
    [[nodiscard]] Vector3
    getPixel(int x, int y) const override {
        return frame_buf[getIndex(x, y)];
    }

#pragma endregion

private:
#pragma region ����
    void
    drawTriangle(const Triangle& triangle, const std::vector<Vector4>& viewSpace, FragmentShader& fragmentShader) {
        // ���������ε���������
        auto a = triangle.a();
        auto b = triangle.b();
        auto c = triangle.c();
        // �����Χ��
        auto[left, right, floor, top] = getBoundingBox(a, b, c);
        // z-buffer�㷨
        for (int j = floor; j <= top; ++j) {
            for (int i = left; i <= right; ++i) {
                if (insideTriangle(i + 0.5, j + 0.5, triangle.vertexes)) {
                    // ��ȡ��ֵ���
                    auto[alpha, beta, gamma] = computeBarycentric2DWithFixed(i + 0.5, j + 0.5, triangle);
                    numberType z_lerp = MathUtils::interpolate(alpha, beta, gamma, triangle.vertexes[0].z(), triangle.vertexes[1].z(), triangle.vertexes[2].z(), fixed);
                    // ��Ȳ���
                    if (z_lerp < z_buf[getIndex(i, j)]) {
                        z_buf[getIndex(i, j)] = z_lerp;
                        auto normal_lerp = MathUtils::interpolate(alpha, beta, gamma, triangle.normals[0], triangle.normals[1], triangle.normals[2], fixed);
                        auto color_lerp = MathUtils::interpolate(alpha, beta, gamma, triangle.colors[0], triangle.colors[1], triangle.colors[2], fixed);
                        auto uv_lerp = MathUtils::interpolate(alpha, beta, gamma, triangle.uvs[0], triangle.uvs[1], triangle.uvs[2], fixed);
                        auto shadingcoords_lerp = MathUtils::interpolate(alpha, beta, gamma, viewSpace[0], viewSpace[1], viewSpace[2], fixed);

                        // ������Ҫ�ǵõ�λ��!!
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
        // ���������ε���������
        auto a = triangle.a();
        auto b = triangle.b();
        auto c = triangle.c();
        // �����Χ��
        auto[left, right, floor, top] = getBoundingBox(a, b, c);
        // MSAA׼��
        int pid = 0;               // ��ǰ������frame_msaa�е�λ��
        const std::array<numberType, 4> dx = { 0.25, 0.25, 0.75, 0.75 };
        const std::array<numberType, 4> dy = { 0.25, 0.75, 0.25, 0.75 };
        // z-buffer�㷨
        for (int j = floor; j <= top; ++j) {
            for (int i = left; i <= right; ++i) {
                pid = getIndex(i, j) * 4;
                // 4����������ģ��
                for (int k = 0; k < 4; ++k) {
                    if (insideTriangle(i + dx[k], j + dy[k], triangle.vertexes)) {
                        // ��ȡ��ֵ���
                        auto[alpha, beta, gamma] = computeBarycentric2DWithFixed(i + dx[k], j + dy[k], triangle);
                        numberType z_lerp = MathUtils::interpolate(alpha, beta, gamma, triangle.vertexes[0].z(), triangle.vertexes[1].z(), triangle.vertexes[2].z(), fixed);
                        // ��Ȳ���
                        if (z_lerp < z_msaa[pid + k]) {
                            z_msaa[pid + k] = z_lerp;
                            auto normal_lerp = MathUtils::interpolate(alpha, beta, gamma, triangle.normals[0], triangle.normals[1], triangle.normals[2], fixed);
                            auto color_lerp = MathUtils::interpolate(alpha, beta, gamma, triangle.colors[0], triangle.colors[1], triangle.colors[2], fixed);
                            auto uv_lerp = MathUtils::interpolate(alpha, beta, gamma, triangle.uvs[0], triangle.uvs[1], triangle.uvs[2], fixed);
                            auto shadingcoords_lerp = MathUtils::interpolate(alpha, beta, gamma, viewSpace[0], viewSpace[1], viewSpace[2], fixed);

                            // ������Ҫ�ǵõ�λ��!!
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
#pragma region ��������
    // ����жϵ��Ƿ�����������
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

    // ��ȡbuffer���±�
    [[nodiscard]] int
    getIndex(int x, int y) const {
        if (out_range(x, y))
            throw std::out_of_range("Rasterizer::getIndex(int x, int y)");
        return (static_cast<int>(view_height) - 1 - y) * static_cast<int>(view_width) + x;
    }

    // ��ȡ��������
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

    // �����Χ��
    [[nodiscard]] std::tuple<int, int, int, int>
    getBoundingBox(Vector4 a, Vector4 b, Vector4 c) const {
        int left = static_cast<int>(std::min(a.x(), std::min(b.x(), c.x())));
        int right = static_cast<int>(std::max(a.x(), std::max(b.x(), c.x())));
        int floor = static_cast<int>(std::min(a.y(), std::min(b.y(), c.y())));
        int top = static_cast<int>(std::max(a.y(), std::max(b.y(), c.y())));
        return {left >= 0 ? left : 0, right < view_width ? right : view_width - 1, floor >= 0 ? floor : 0, top < view_height ? top : view_height - 1};
    }



    // Խ���ж�
    [[nodiscard]] constexpr bool
    out_range(numberType x, numberType y) const {
        return x < 0 || x >= view_width || y < 0 || y >= view_height;
    }
#pragma endregion

};

}

#endif //ANYA_ENGINE_RASTERIZER_HPP
