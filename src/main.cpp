#include <iostream>
#include "test/test.h"
#include "tool/vec.hpp"
#include "ui/gui.hpp"
#include "renderer/rasterizer.hpp"
#include "load/model.hpp"
#include "component/camera.hpp"
#include "load/context.hpp"

using namespace anya;

void show(const std::shared_ptr<Renderer>& renderer) {
    auto [w, h] = renderer->scene.camera->getWH();
    GUI gui("AnyaEngine", w, h, renderer);
    gui.run();
}

void runTask(const std::string& path) {
    Context context;
    context.loadFromJson(JsonUtils::load(path));
    show(context._renderer);
}

int main() {
    std::cout << "Hello, MnZn!" << std::endl;
    runTask("../art/context/elfgirl.json");
    return 0;
}

