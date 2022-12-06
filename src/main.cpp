#include <iostream>
#include "test/test.h"
#include "math/vec.hpp"
#include "math/matrix.hpp"
#include "ui/gui.hpp"
#include "renderer/rasterizer.hpp"
#include "load/model.hpp"
#include "component/camera.hpp"

using namespace anya;

int main() {
    std::cout << "Hello, MnZn!" << std::endl;
    std::shared_ptr<anya::Rasterizer> render = std::make_shared<anya::Rasterizer>();
    anya::Model model{};
    model.addLocalPosition({ 2, 0, -2 });
    model.addLocalPosition({ 0, 2, -2 });
    model.addLocalPosition({ -2, 0, -2 });
    render->scene.models.push_back(model);
    render->scene.camera = std::make_shared<anya::Camera>(Vector3{0, 0, 5}, Vector3{0, 0, -4}, 700, 700, 45);
    anya::GUI gui("Anya", 800, 600, render);
    gui.run();
    return 0;
}

