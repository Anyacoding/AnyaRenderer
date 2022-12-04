#include <iostream>
#include "test/test.h"
#include "math/vec.hpp"
#include "math/matrix.hpp"
#include <glad/glad.h>
#include <GLFW/glfw3.h>

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    // make sure the viewport matches the new window dimensions; note that width and
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}

void processInput(GLFWwindow *window) {
    if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

int main() {
    std::cout << "Hello, MnZn!" << std::endl;
    // glfw: initialize and configure
    // ------------------------------
    if (glfwInit() == false) {
        std::cerr << "ERROR: could not start GLFW3\n";
        return -1;
    }
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // glfw window creation
    // --------------------
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", nullptr, nullptr);
    if (window == nullptr) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // tell GL to only draw onto a pixel if the shape is closer to the viewer
    glEnable(GL_DEPTH_TEST); // enable depth-testing
    glDepthFunc(GL_LESS);    // depth-testing interprets a smaller value as "closer"

    // 交换间隔设置为1，以免交换频繁造成屏幕撕裂
    glfwSwapInterval(1);

    // 三角形三个顶点
    anya::Matrix33 triangle{};
    triangle << 0.0, 0.5, 0.0,
                0.5, -0.5, 0.0,
                -0.5, -0.5, 0.0;
    double points[9]{};
    triangle.saveToArray(points);
    for (auto i : points) std::cout << i << " ";


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
    // -----------
    while (!glfwWindowShouldClose(window)) {
        // input
        // -----
        processInput(window);

        // render
        // ------
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

//        glUseProgram(shader_programme);
        glBindVertexArray(vao);

        glDrawArrays(GL_TRIANGLES, 0, 3);
        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwPollEvents();
        glfwSwapBuffers(window);  // 双缓冲区交换

        glBindVertexArray(0);
    }

    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glfwTerminate();
    return 0;
}
