//
// Created by Anya on 2022/12/22.
//

#ifndef ANYA_ENGINE_CONTEXT_HPP
#define ANYA_ENGINE_CONTEXT_HPP

#include "interface/renderer.hpp"
#include "component/scene.hpp"
#include "memory"

namespace anya {

class Context {
public:
    std::shared_ptr<Renderer> _renderer;
public:
    void
    loadFromJson(const json& config) {
        try {
            load(config);
        } catch (const std::exception& err) {
            printf("loadFromJson error: %s", err.what());
        }
    }
private:
    void
    load(const json& config) {
        // 加载renderer字段
        json renderer = config["renderer"];
        this->_renderer = makeRenderer(renderer["type"]);
        // 加载camera字段
        json camera = config["camera"];
        Vector3 eye_pos = toVector3(camera["eye_pos"]);
        Vector3 obj_pos = toVector3(camera["obj_pos"]);
        numberType view_width = camera["view_width"];
        numberType view_height = camera["view_height"];
        numberType fovY = camera["fovY"];
        this->_renderer->scene.camera = std::make_shared<Camera>(eye_pos, obj_pos, view_width, view_height, fovY);
        // 加载models字段
        json models = config["models"];
        for (auto item : models) {
            Model model(item["objPath"]);
            model.fragmentShader.setMethod(ShaderUtils::phong_fragment_shader);
            model.RotateAroundN(140, {0, 1, 0});
            this->_renderer->scene.models.push_back(model);
        }
    }
private:
    static std::shared_ptr<Renderer>
    makeRenderer(const std::string& type) {
        if (type == "Rasterizer") {
            return std::make_shared<Rasterizer>();
        }
        else {
            throw std::runtime_error("renderer type error");
        }
    }

    static Vector3
    toVector3(const json& obj) {
        if (obj.is_array() && obj.size() == 3) {
            return { obj[0], obj[1], obj[2] };
        }
        else {
            throw std::runtime_error("json obj can not be transform by toVector3()");
        }
    }
};

}

#endif //ANYA_ENGINE_CONTEXT_HPP
