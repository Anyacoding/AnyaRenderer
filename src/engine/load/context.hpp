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
            // 加载model的obj
            Model model(item["objPath"]);

            // 加载model的shaders
            json shader = item["shader"];
            model.setFragmentShaderMethod(shader["fragmentShader"]);

            // 加载model的rotate
            json rotate = item["rotate"];
            model.setBaseMat(Transform::RotateAroundN(rotate["angle"], toVector3(rotate["axis"])));

            // 加载model的scale
            json scale = item["scale"];
            model.setBaseMat(Transform::scale(scale["ratio"]));

            // 加载model的texture
            if (item.value("texture", json::object()) != json::object()) {
                json texture = item["texture"];
                model.fragmentShader.texture = std::make_shared<Texture>(texture["texturePath"]);
            }
        #ifdef Z_BUFFER_TEST
            model.TriangleList[0].setColor(0, 217.0, 238.0, 185.0);
            model.TriangleList[0].setColor(1, 217.0, 238.0, 185.0);
            model.TriangleList[0].setColor(2, 217.0, 238.0, 185.0);
            model.TriangleList[1].setColor(0, 185.0, 217.0, 238.0);
            model.TriangleList[1].setColor(1, 185.0, 217.0, 238.0);
            model.TriangleList[1].setColor(2, 185.0, 217.0, 238.0);
        #endif
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
