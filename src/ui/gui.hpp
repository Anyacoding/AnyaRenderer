//
// Created by DELL on 2022/12/5.
//

#ifndef ANYA_ENGINE_GUI_HPP
#define ANYA_ENGINE_GUI_HPP

#include <string>
#include <iostream>
#include "math/vec.hpp"
#include "math/matrix.hpp"
#include <glad/glad.h>
#include <GLFW/glfw3.h>

namespace anya{

class GUI {
private:
    // 标题
    std::string_view title;
    // 宽高
    GLint width, height;
    // 窗口handle
    GLFWwindow* window = nullptr;
public:
    GUI(std::string_view title, GLint width, GLint height)
        :title(title), width(width), height(height) {}

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

        // glad: load all OpenGL function pointers
        if (!setGlad()) return;

        // 双缓冲交换间隔设置为1，以免交换频繁造成屏幕撕裂
        glfwSwapInterval(1);


        // OOpenGl自己的渲染管线，将来用自己的代替掉
        // 三角形三个顶点
        anya::Matrix33 triangle{};
        triangle << 0.0, 0.5, 0.0,
            0.5, -0.5, 0.0,
            -0.5, -0.5, 0.0;
        double points[9]{};
        triangle.saveToArray(points);

        // vertex buffer object (VBO) —— 顶点缓冲区对象的索引
        GLuint vbo = 0;
        // 创建一个空的vbo
        glGenBuffers(1, &vbo);

        // vertex array object (VAO) —— 顶点顶点数组对象的索引
        GLuint vao = 0;
        // 创建一个空的vao
        glGenVertexArrays(1, &vao);
        // 绑定vao对象至上下文
        glBindVertexArray(vao);

        // 绑定vbo对象至上下文的缓冲区
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        // 将点复制到当前绑定的vbo缓冲区
        glBufferData(GL_ARRAY_BUFFER, 9 * sizeof(anya::numberType), points, GL_STATIC_DRAW);
        // 0代表属性号为0的定义布局，3代表每个变量是由缓冲区中的每3个浮点(GL_FLOAT)组成的vec3
        glVertexAttribPointer(0, 3, GL_DOUBLE, GL_FALSE, 0, nullptr);
        // 启用属性0
        glEnableVertexAttribArray(0);
        // 好习惯，解除绑定
        glBindVertexArray(0);

        const char* vertex_shader =
            "#version 400\n"
            "in vec3 vp;"
            "void main() {"
            "  gl_Position = vec4(vp, 1.0);"
            "}";

        const char* fragment_shader =
            "#version 400\n"
            "out vec4 frag_colour;"
            "void main() {"
            "  frag_colour = vec4(0.5, 0.0, 0.5, 1.0);"
            "}";

        GLuint vs = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vs, 1, &vertex_shader, nullptr);
        glCompileShader(vs);
        GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(fs, 1, &fragment_shader, nullptr);
        glCompileShader(fs);

        GLuint shader_programme = glCreateProgram();
        glAttachShader(shader_programme, fs);
        glAttachShader(shader_programme, vs);
        glLinkProgram(shader_programme);

        // render loop
        while (!glfwWindowShouldClose(window)) {
            clearWith();              // 清除颜色缓存
            glfwPollEvents();         // 非阻塞处理IO事件

            // render
            glUseProgram(shader_programme);
            glBindVertexArray(vao);
            glDrawArrays(GL_TRIANGLES, 0, 3);

            glfwSwapBuffers(window);  // 双缓冲区交换
            glBindVertexArray(0);
        }
    }

private:
#pragma region 封装OpenGl的常用函数
    static void init() {
        if (glfwInit() == false) {
            std::cerr << "ERROR: could not start GLFW3\n";
            return;
        }
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
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

    bool setGlad() {
        // glad: load all OpenGL function pointers
        if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
            std::cerr << "Failed to initialize GLAD" << std::endl;
            return false;
        }
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
