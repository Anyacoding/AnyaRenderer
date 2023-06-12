#include <iostream>
#include "ui/gui.hpp"
#include "component/camera.hpp"
#include "load/context.hpp"
#include "renderer/raytracer.hpp"
#include "test/test.h"

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
    runTask("../art/context/elfgirl.json");
    return 0;
}



// 目前可选光栅化模型:                   目前可选模型光追模型:
// 1.assassin.json                      1.whitted_style.json
// 2.azura.json                         2.bunny.json
// 3.boggie.json                        3.cornell_box.json
// 4.diablo3_pose.json
// 5.elfgirl.json
// 6.kgirl.json
// 7.marry.json
// 8.robot.json
// 9.spot_triangulated_good.json


