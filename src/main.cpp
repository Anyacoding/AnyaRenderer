#include <iostream>
#include "test/test.h"
#include "math/vec.hpp"
#include "math/matrix.hpp"
#include "ui/gui.hpp"
#include "renderer/rasterizer.hpp"
#include "load/model.hpp"
#include "component/camera.hpp"

using namespace anya;

void show(const std::shared_ptr<Renderer>& renderer) {
    auto [w, h] = renderer->scene.camera->getWH();
    GUI gui("AnyaEngine", w, h, renderer);
    gui.run();
}

void runTask() {
    std::shared_ptr<Renderer> renderer = std::make_shared<Rasterizer>();
    anya::Model model{};
    model.addLocalPosition({ 2, 0, -2 });
    model.addLocalPosition({ 0, 2, -2 });
    model.addLocalPosition({ -2, 0, -2 });
    renderer->scene.models.push_back(model);
    renderer->scene.camera = std::make_shared<Camera>(Vector3{0, 0, 5}, Vector3{0, 0, -4}, 800, 600, 45);
    show(renderer);
}

int main() {
    std::cout << "Hello, MnZn!" << std::endl;
    runTask();
    return 0;
}

