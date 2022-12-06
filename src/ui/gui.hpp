//
// Created by DELL on 2022/12/5.
//

#ifndef ANYA_ENGINE_GUI_HPP
#define ANYA_ENGINE_GUI_HPP

#include <string>
#include <iostream>
#include <memory>
#include <utility>
#include "math/vec.hpp"
#include "math/matrix.hpp"
#include <GLFW/glfw3.h>
#include "math/utils.hpp"
#include "interface/renderer.hpp"

namespace anya{

class GUI {
    // 自动控制绘图模式作用域
    struct ModeGuard {
        // 控制类型:   GL_TRIANGLES, GL_LINES, GL_POINTS, GL_POLYGON
        // 分别表示:   三角形,        线段,      像素,       多边形
        explicit ModeGuard(GLenum mode) { glBegin(mode); }

        ~ModeGuard() { glEnd(); }
    };

private:
    // 标题
    std::string_view title;
    // 宽高
    GLint width, height;
    // 窗口handle
    GLFWwindow* window = nullptr;
    // 渲染器，使用指针方便将来使用多态
    std::shared_ptr<Renderer> renderer;

public:
    GUI(std::string_view title, GLint width, GLint height, std::shared_ptr<Renderer> renderer)
        :title(title), width(width), height(height), renderer(std::move(renderer)) {}

    ~GUI() {
        glfwTerminate();   // 真正终止并释放glfw资源
    }

    void run() {
        // 初始化glfw
        init();

        // glfw创建窗口
        if ((window = createHandle()) == nullptr) return;

        // glfw构造OpenGl上下文
        glfwMakeContextCurrent(window);

        // 设置窗口回调函数, eg: 按esc退出
        setCallBack();

        setViewport();

        // 双缓冲交换间隔设置为1，以免交换频繁造成屏幕撕裂
        glfwSwapInterval(1);

        // render loop
        while (!glfwWindowShouldClose(window)) {
            clearWith();              // 清除颜色缓存
            glfwPollEvents();         // 非阻塞处理IO事件
            update();
            glfwSwapBuffers(window);  // 双缓冲区交换
        }
    }
private:
#pragma region 画面更新逻辑
    void update() {
        ModeGuard guard(GL_LINES);
        // TODO: 将渲染逻辑丢到另一个线程
        renderer->render();
        for (const auto& model : renderer->scene.models) {
            for (const auto& vertex : model.vertexes) {
                glColor3d(0.5, 0.3, 1.0);
                glVertex2d(vertex[0], vertex[1]);
            }
        }
    }

#pragma endregion

private:
#pragma region 封装OpenGl的常用函数
    static void init() {
        if (glfwInit() == false) {
            std::cerr << "ERROR: could not start GLFW3\n";
            return;
        }
    }

    GLFWwindow* createHandle() {
        window = glfwCreateWindow(width, height, title.data(), nullptr, nullptr);
        if (window == nullptr) {
            std::cerr << "Failed to create GLFW window" << std::endl;
            return nullptr;
        }
        // make sure the viewport matches the new window dimensions; note that width and
        // height will be significantly larger than specified on retina displays.
        return window;
    }

    void setCallBack() {
        glfwSetKeyCallback(window, key_callback);
    }

    bool setViewport() {
        int w, h;
        glfwGetFramebufferSize(window, &w, &h);
        glViewport(0, 0, w, h);
        return true;
    }

    static void clearWith() {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }

    static void
    key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
        if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
            glfwSetWindowShouldClose(window, GLFW_TRUE);
    }
#pragma endregion

};

}

#endif //ANYA_ENGINE_GUI_HPP
