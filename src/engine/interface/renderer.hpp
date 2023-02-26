//
// Created by Anya on 2022/12/5.
//

#ifndef ANYA_ENGINE_RENDERER_HPP
#define ANYA_ENGINE_RENDERER_HPP

#include <memory>
#include "component/camera.hpp"
#include "component/scene.hpp"

namespace anya {

enum class RenderMode {
    RASTERIZER, WHITTED_STYLE, PATH_TRACING
};

class Renderer {
public:
    Scene  scene{};                               // 场景
    Vector3 background{};                         // 背景颜色
    std::shared_ptr<Texture> outPutImage{};       // 输出图片
    std::string savePathName;                     // 保存路径
    int spp = 1;                                  // 采样频率
    RenderMode mode = RenderMode::WHITTED_STYLE;  // 渲染模式
public:
    virtual void render() = 0;
    [[nodiscard]] virtual Vector3 getPixel(int x, int y) const = 0;
};

}

#endif //ANYA_ENGINE_RENDERER_HPP
