//
// Created by Anya on 2022/12/5.
//

#ifndef ANYA_ENGINE_RENDERER_HPP
#define ANYA_ENGINE_RENDERER_HPP

#include <memory>
#include "component/camera.hpp"

namespace anya {

class Renderer {
public:
    std::shared_ptr<Camera> camera;  // 摄像机
public:
    virtual void render() = 0;
};

}

#endif //ANYA_ENGINE_RENDERER_HPP
