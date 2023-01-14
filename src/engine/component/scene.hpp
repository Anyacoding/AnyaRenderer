//
// Created by Anya on 2022/12/6.
//

#ifndef ANYA_ENGINE_SCENE_HPP
#define ANYA_ENGINE_SCENE_HPP

#include <vector>
#include "load/model.hpp"
#include "interface/object.hpp"
#include "component/light.hpp"

namespace anya {

// 场景类负责存储要渲染的对象集合
class Scene {
public:
    std::vector<Model> models{};                    // 要渲染的模型集合(光栅化使用)
    std::vector<std::shared_ptr<Object>> objects{}; // 要渲染的物体集合(光线追踪使用)
    std::vector<Light> lights{};                    // 光源集合
    std::shared_ptr<Camera> camera;                 // 摄像机
public:
    void
    addModel(const Model& model) {
        models.push_back(model);
    }

    void
    addObject(const std::shared_ptr<Object>& obj) {
        objects.push_back(obj);
    }

    void
    addLight(const Light& light) {
        lights.push_back(light);
    }
};

}

#endif //ANYA_ENGINE_SCENE_HPP
