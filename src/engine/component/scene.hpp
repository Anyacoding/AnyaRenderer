//
// Created by Anya on 2022/12/6.
//

#ifndef ANYA_ENGINE_SCENE_HPP
#define ANYA_ENGINE_SCENE_HPP

#include <vector>
#include "load/model.hpp"

namespace anya {

// 场景类负责存储要渲染的对象集合
class Scene {
public:
    std::vector<Model> models{};      // 要渲染的模型集合(光栅化使用)
    std::shared_ptr<Camera> camera;   // 摄像机
public:
    void
    addModel(const Model& model) {
        models.push_back(model);
    }
};

}

#endif //ANYA_ENGINE_SCENE_HPP
