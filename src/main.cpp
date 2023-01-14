#include <iostream>
#include "ui/gui.hpp"
#include "component/camera.hpp"
#include "load/context.hpp"
#include "renderer/raytracer.hpp"
#include "tool/vec.hpp"

using namespace anya;

void show(const std::shared_ptr<Renderer>& renderer) {
    auto [w, h] = renderer->scene.camera->getWH();
    GUI gui("AnyaRenderer", w, h, renderer);
    gui.run();
}

void runTask(const std::string& path) {
    Context context;
    context.loadFromJson(JsonUtils::load(path));
    show(context._renderer);
}

int main() {
    // std::cout << "Hello, MnZn!" << std::endl;
    // runTask("../art/context/elfgirl.json");

    std::shared_ptr<Renderer> renderer = std::make_shared<RayTracer>();
    numberType view_width = 1280;
    numberType view_height = 960;
    Vector3 background = {0.235294, 0.67451, 0.843137};
    renderer->background = background;
    renderer->scene.camera = std::make_shared<Camera>(Vector3{0, 0, 0}, Vector3{0, 0, 0}, view_width, view_height, 90);
    renderer->outPutImage = std::make_shared<Texture>(view_width, view_height, background);
    renderer->savePathName = "../art/images/RayTracerTest.png";

    auto sph1 = std::make_shared<Sphere>(Vector3{-1, 0, -12}, 2);
    sph1->materialType = DIFFUSE_AND_GLOSSY;
    sph1->diffuseColor = Vector3{0.6, 0.7, 0.8};

    auto sph2 = std::make_shared<Sphere>(Vector3{0.5, -0.5, -8}, 1.5);
    sph2->ior = 1.5;
    sph2->materialType = REFLECTION_AND_REFRACTION;

    renderer->scene.addObject(sph1);
    renderer->scene.addObject(sph2);

    auto tri1 = std::make_shared<Triangle>();
    tri1->setVertex(0, Vector4{-5, -3, -6, 1});
    tri1->setVertex(1, Vector4{5, -3, -6, 1});
    tri1->setVertex(2, Vector4{-5,-3,-16, 1});
    tri1->setST(0, {0, 0});
    tri1->setST(1, {1, 0});
    tri1->setST(2, {0, 1});
    tri1->materialType = DIFFUSE_AND_GLOSSY;

    auto tri2 = std::make_shared<Triangle>();
    tri2->setVertex(0, Vector4{5, -3, -6, 1});
    tri2->setVertex(1, Vector4{5, -3, -16, 1});
    tri2->setVertex(2, Vector4{-5, -3, -16, 1});
    tri2->setST(0, {1, 0});
    tri2->setST(1, {1, 1});
    tri2->setST(2, {0, 1});
    tri2->materialType = DIFFUSE_AND_GLOSSY;

    renderer->scene.addObject(tri1);
    renderer->scene.addObject(tri2);

    Light light1 = {Vector3{-20, 70, 20}, {0.5, 0.5, 0.5}};
    Light light2 = {Vector3{30, 50, -12}, {0.5, 0.5, 0.5}};
    renderer->scene.addLight(light1);
    renderer->scene.addLight(light2);

    show(renderer);
    return 0;
}

// 目前可选模型:
// 1.assassin.json
// 2.azura.json
// 3.boggie.json
// 4.diablo3_pose.json
// 5.elfgirl.json
// 6.kgirl.json
// 7.marry.json
// 8.robot.json
// 9.spot_triangulated_good.json

