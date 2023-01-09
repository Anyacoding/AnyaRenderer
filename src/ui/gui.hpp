//
// Created by DELL on 2022/12/5.
//

#ifndef ANYA_ENGINE_GUI_HPP
#define ANYA_ENGINE_GUI_HPP

#include <GLFW/glfw3.h>
#include <string>
#include <iostream>
#include <memory>
#include <utility>
#include "tool/vec.hpp"
#include "tool/matrix.hpp"
#include "tool/utils.hpp"
#include "interface/renderer.hpp"
#include "renderer/rasterizer.hpp"

namespace anya{

class GUI {
private:
    // 标题
    std::string_view title;
    // 宽高
    GLdouble width, height;
    // 窗口handle
    GLFWwindow* window = nullptr;
    // 渲染器，使用指针方便将来使用多态
    std::shared_ptr<Renderer> renderer;
    // 绕N轴旋转角
    static numberType angleAroundN;
    static bool       updateRotate;
    // 旋转轴
    static Vector3 axis;
    // 保存截图
    static bool isSaved;
public:
    GUI(std::string_view title, GLdouble width, GLdouble height, std::shared_ptr<Renderer> renderer)
        :title(title), width(width), height(height), renderer(std::move(renderer)) {}

    ~GUI() {
        glfwTerminate();   // 真正终止并释放glfw资源
    }

    void
    run() {
        // 初始化glfw
        if (!init()) return;

        // glfw创建窗口
        if ((window = createHandle()) == nullptr) return;

        // glfw构造OpenGl上下文
        glfwMakeContextCurrent(window);

        // 设置窗口回调函数, eg: 按esc退出
        setCallBack();

        // 设置视口大小
        setViewport();

        // 双缓冲交换间隔设置为1，以免交换频繁造成屏幕撕裂
        glfwSwapInterval(1);

        // TODO: 将渲染逻辑丢到另一个线程
        renderer->render();

        // render loop
        while (!glfwWindowShouldClose(window)) {
            // 非阻塞处理IO事件
            glfwPollEvents();
            // 清除颜色缓存
            // clearWith();
            // 更新画面
            update();
            // 双缓冲区交换
            glfwSwapBuffers(window);
        }
    }
private:
#pragma region 画面更新逻辑

    // 自动控制绘图模式作用域
    struct ModeGuard {
        // 控制类型:   GL_TRIANGLES, GL_LINES, GL_POINTS, GL_POLYGON
        // 分别表示:   三角形,        线段,      像素,       多边形
        explicit ModeGuard(GLenum mode) { glBegin(mode); }

        ~ModeGuard() { glEnd(); }
    };

    void
    update() {
        ModeGuard guard(GL_POINTS);
        if (updateRotate) {
            for (auto& model : renderer->scene.models) {
                model.RotateAroundN(angleAroundN, axis);
            }
            renderer->render();
            updateRotate = false;
        }
        for (int i = 0; i < static_cast<int>(width); ++i) {
            auto x = static_cast<numberType>(i) / width * 2 - 1;
            for (int j = 0; j < static_cast<int>(height); ++j) {
                auto y = static_cast<numberType>(j) / height * 2 - 1;
                auto color = renderer->getPixel(i, j);
                glColor3d(color[0], color[1], color[2]);
                glVertex2d(x, y);
            }
        }
        if (isSaved) {
            renderer->outPutImage->saveToDisk(renderer->savePathName);
            std::cout << "Save Successfully!" << std::endl;
            isSaved = false;
        }
    }

#pragma endregion

private:
#pragma region 封装OpenGl的常用函数
    static bool
    init() {
        if (glfwInit() == false) {
            std::cerr << "ERROR: could not start GLFW3\n";
            return false;
        }
        return true;
    }

    // make sure the viewport matches the new window dimensions; note that width and
    // height will be significantly larger than specified on retina displays.
    GLFWwindow*
    createHandle() {
        window = glfwCreateWindow(static_cast<int>(width), static_cast<int>(height), title.data(), nullptr, nullptr);
        if (window == nullptr) {
            std::cerr << "Failed to create GLFW window" << std::endl;
            return nullptr;
        }
        return window;
    }

    void
    setCallBack() {
        glfwSetKeyCallback(window, key_callback);
    }

    bool
    setViewport() {
        int w, h;
        glfwGetFramebufferSize(window, &w, &h);
        glViewport(0, 0, w, h);
        return true;
    }

    static void
    clearWith(Vector4 color = {0, 0, 0, 0}) {
        glClearColor(float(color.x()), float(color.y()), float(color.z()), float(color.w()));
        glClear(GL_COLOR_BUFFER_BIT);
    }

    static void
    key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
        if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
            glfwSetWindowShouldClose(window, GLFW_TRUE);
        }
        else if (key == GLFW_KEY_A && action == GLFW_PRESS) {
            angleAroundN += 10;
            updateRotate = true;
        }
        else if (key == GLFW_KEY_D && action == GLFW_PRESS) {
            angleAroundN -= 10;
            updateRotate = true;
        }
        else if (key == GLFW_KEY_X && action == GLFW_PRESS) {
            axis = {1, 0, 0};
        }
        else if (key == GLFW_KEY_Y && action == GLFW_PRESS) {
            axis = {0, 1, 0};
        }
        else if (key == GLFW_KEY_Z && action == GLFW_PRESS) {
            axis = {0, 0, 1};
        }
        else if (key == GLFW_KEY_Z && action == GLFW_PRESS) {
            axis = {0, 0, 1};
        }
        else if (key == GLFW_KEY_ENTER && action == GLFW_PRESS) {
            isSaved = true;
        }
    }
#pragma endregion

};

// 静态数据成员初始化
numberType GUI::angleAroundN = 0.0;
bool GUI::updateRotate = false;
Vector3 GUI::axis = Vector3{0, 1, 0};
bool GUI::isSaved = false;

}

#endif //ANYA_ENGINE_GUI_HPP
