//
// Created by Anya on 2022/12/22.
//

#ifndef ANYA_ENGINE_CONTEXT_HPP
#define ANYA_ENGINE_CONTEXT_HPP

#include "component/object/sphere.hpp"
#include "component/object/mesh.hpp"
#include "material/diffuse.hpp"
#include "material/glass.hpp"
#include <memory>

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
        this->_renderer->background = toVector3(renderer["background"]) / 255;

        // 加载camera字段
        json camera = config["camera"];
        Vector3 eye_pos = toVector3(camera["eye_pos"]);
        Vector3 obj_pos = toVector3(camera["obj_pos"]);
        numberType view_width = camera["view_width"];
        numberType view_height = camera["view_height"];
        numberType fovY = camera["fovY"];
        this->_renderer->scene.camera = std::make_shared<Camera>(eye_pos, obj_pos, view_width, view_height, fovY);

        // 加载image字段
        json image = config["image"];
        _renderer->savePathName = buildSavePath(image["name"], image["suffix"]);
        this->_renderer->outPutImage = std::make_shared<Texture>(view_width, view_height, this->_renderer->background);

        if (renderer["type"] == "Rasterizer") {
            // 加载models字段
            json models = config["models"];
            for (const auto& item : models) {
                this->_renderer->scene.addModel(toModel(item));
            }
        }
        else if (renderer["type"] == "RayTracer") {
            // 加载objects字段
            json objects = config["objects"];
            for (const auto& item : objects) {
                this->_renderer->scene.addObject(toObject(item));
            }
            // 加载lights字段
            json lights = config["lights"];
            for (const auto& item : lights) {
                this->_renderer->scene.addLight(toLight(item));
            }
        }
    }

private:
    static Vector3
    toVector3(const json& obj) {
        if (obj.is_array() && obj.size() == 3) {
            return { obj[0], obj[1], obj[2] };
        }
        else {
            throw std::runtime_error("json obj can not be transform by toVector3()");
        }
    }

    static Vector2
    toVector2(const json& obj) {
        if (obj.is_array() && obj.size() == 2) {
            return { obj[0], obj[1] };
        }
        else {
            throw std::runtime_error("json obj can not be transform by toVector3()");
        }
    }

    static Model
    toModel(const json& item) {
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
        return model;
    }

    static std::shared_ptr<Object>
    toObject(const json& item) {
        std::string type = item["type"];
        if (type == "sphere") {
            return toSphere(item);
        }
        else if (type == "triangle") {
            return toTriangle(item);
        }
        else if (type == "mesh") {
            return toMesh(item);
        }
        else {
            throw std::runtime_error("object type error");
        }
    }

    // TODO: 优化材质导入
    static std::shared_ptr<Object>
    toSphere(const json& obj) {
        auto center = toVector3(obj["center"]);
        numberType radius = obj["radius"];
        std::shared_ptr<Sphere> sphere = std::make_shared<Sphere>(center, radius);

        json material = obj["material"];
        std::string MaterialType = material["type"];
        if (MaterialType == "REFLECTION_AND_REFRACTION") {
            sphere->material = std::make_shared<GlassMaterial>();
            sphere->material->type = REFLECTION_AND_REFRACTION;
            sphere->material->ior = material["ior"];
        }
        else if (MaterialType == "DIFFUSE_AND_GLOSSY") {
            sphere->material = std::make_shared<DiffuseMaterial>();
            sphere->material->type = DIFFUSE_AND_GLOSSY;
            sphere->material->diffuseColor = toVector3(material["diffuseColor"]);
        }
        else {
            throw std::runtime_error("material type error");
        }

        return sphere;
    }

    static std::shared_ptr<Object>
    toTriangle(const json& obj) {
        std::shared_ptr<Triangle> triangle = std::make_shared<Triangle>();
        triangle->material = std::make_shared<DiffuseMaterial>();
        json vertexes = obj["vertexes"];
        int index = 0;
        for (const auto& vertex : vertexes) {
            triangle->setVertex(index, toVector3(vertex).to4());
            ++index;
        }
        json stCoordinates = obj["stCoordinates"];
        index = 0;
        for (const auto& st : stCoordinates) {
            triangle->setST(index, toVector2(st));
            ++index;
        }
        return triangle;
    }

    static std::shared_ptr<Object>
    toMesh(const json& obj) {
        auto material = std::make_shared<DiffuseMaterial>();
        material->kd = 0.6;
        material->ks = 0.0;
        material->specularExponent = 0.0;
        // 加载mesh的obj
        std::shared_ptr<Mesh> mesh = std::make_shared<Mesh>(obj["meshPath"], material);
        return mesh;
    }

    static Light
    toLight(const json& obj) {
        return Light{ toVector3(obj["position"]), toVector3(obj["intensity"]) };
    }

    static std::shared_ptr<Renderer>
    makeRenderer(const std::string& type) {
        if (type == "Rasterizer") {
            return std::make_shared<Rasterizer>();
        }
        else if (type == "RayTracer") {
            return std::make_shared<RayTracer>();
        }
        else {
            throw std::runtime_error("renderer type error");
        }
    }

    static std::string
    buildSavePath(const std::string& name, const std::string& suffix) {
        // path = "../art/images/${imageName}.${fileSuffix}"
        std::string path;
        path += "../art/images/" + name + "." + suffix;
        return path;
    }
};

}

#endif //ANYA_ENGINE_CONTEXT_HPP
