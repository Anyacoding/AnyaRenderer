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
    // 初始摄像机状态
    const Camera defaultCamera;
    
private:
#pragma region 摄像机控制相关
    /*********状态变量***********/
    static bool isSaved;
    static bool isReset;
    static bool updateCamera;

    /*********相机移动相关***********/
    // 相机速度
    GLdouble cameraSpeed = 1;
    // 相机位置
    Vector3 cameraPos{};
    // 相机朝向
    static Vector3 cameraFront;
    // 相机上方向
    Vector3 cameraUp{};
    // key状态映射
    static std::array<bool, 1024> keys;

    /*********鼠标移动相关***********/
    // 鼠标上一帧的位置
    static GLdouble lastX, lastY;
    // 第一次鼠标回调标志
    static bool firstMouse;
    // 俯仰角
    static GLdouble pitch;
    // 偏航角
    static GLdouble yaw;

    /*********滚轮移动相关***********/
    // 视野
    static GLdouble fov;
#pragma endregion

public:
    GUI(std::string_view title, GLdouble width, GLdouble height, std::shared_ptr<Renderer> _renderer)
        :title(title), width(width), height(height),
        renderer(std::move(_renderer)), defaultCamera(*renderer->scene.camera)
    {}

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

        // 设置输入模式，这里是配置鼠标
        setInputMode();

        // 设置回调函数, eg: 按esc退出
        setCallBack();

        // 设置视口大小
        setViewport();

        // 双缓冲交换间隔设置为1，以免交换频繁造成屏幕撕裂
        glfwSwapInterval(1);

        // 配置相机系统变量
        configure();

        // TODO: 将渲染逻辑丢到另一个线程
        renderer->render();

        // render loop
        while (!glfwWindowShouldClose(window)) {
            // 非阻塞处理IO事件
            glfwPollEvents();
            // 清除颜色缓存
            clearWith();
            // 检查是否恢复默认状态
            if (isReset) reset();
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

    // 更新逻辑
    void
    update() {
        ModeGuard guard(GL_POINTS);
        if (updateCamera) {
            do_movement();
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

    // 相机移动函数
    void
    do_movement() {
        if(keys[GLFW_KEY_W]) {
            cameraPos += cameraSpeed * cameraFront;
        }
        if(keys[GLFW_KEY_S]) {
            cameraPos -= cameraSpeed * cameraFront;
        }
        if(keys[GLFW_KEY_A]) {
            cameraPos -= cameraFront.cross(cameraUp).normalize() * cameraSpeed;
        }
        if(keys[GLFW_KEY_D]) {
            cameraPos += cameraFront.cross(cameraUp).normalize() * cameraSpeed;
        }
        renderer->scene.camera->lookAt(cameraPos, cameraPos + cameraFront);
        renderer->scene.camera->setFovY(fov);
        renderer->render();
        updateCamera = false;
    }

    // 恢复相机系统状态
    void
    reset() {
        renderer->scene.camera = std::make_shared<Camera>(defaultCamera);
        configure();
        renderer->render();
        isReset = false;
    }

    // 配置相机系统状态
    void
    configure() {
        // 相机状态
        cameraPos = renderer->scene.camera->getEyePos();
        cameraFront = renderer->scene.camera->getObjPos();
        cameraUp = {0, 1, 0};
        cameraSpeed = 0.5;

        // 鼠标状态
        lastX = width / 2;
        lastY = height / 2;
        yaw = -90;
        pitch = 0;

        // 滚轮状态
        fov = 45;

        // 状态变量
        isSaved = false;
        updateCamera = false;
        firstMouse = true;
        keys = {};
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

    GLFWwindow*
    createHandle() {
        // make sure the viewport matches the new window dimensions; note that width and
        // height will be significantly larger than specified on retina displays.
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
        glfwSetCursorPosCallback(window, mouse_callback);
        glfwSetScrollCallback(window, scroll_callback);
    }

    bool
    setViewport() {
        int w, h;
        glfwGetFramebufferSize(window, &w, &h);
        glViewport(0, 0, w, h);
        return true;
    }

    void setInputMode() {
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
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
        else if (key == GLFW_KEY_ENTER && action == GLFW_PRESS) {
            isSaved = true;
        }
        else if (key == GLFW_KEY_SPACE && action == GLFW_PRESS) {
            isReset = true;
        }

        if (action == GLFW_PRESS) {
            updateCamera = true;
            keys[key] = true;
        }
        else if (action == GLFW_RELEASE) {
            keys[key] = false;
        }
    }

    static void
    mouse_callback(GLFWwindow* window, double xpos, double ypos) {
        updateCamera = true;
        if (firstMouse) {
            lastX = xpos;
            lastY = ypos;
            firstMouse = false;
        }

        GLdouble xoffset = xpos - lastX;
        GLdouble yoffset = ypos - lastY;
        lastX = xpos;
        lastY = ypos;

        GLdouble sensitivity = 0.02;
        xoffset *= sensitivity;
        yoffset *= sensitivity;

        yaw += xoffset;
        pitch -= yoffset;

        // 防万向锁
        pitch = pitch > 89.0 ? 89.0 : pitch;
        pitch = pitch < -89.0 ? -89.0 : pitch;

        Vector3 front{};
        front.x() = std::cos(MathUtils::angle2rad(yaw)) * std::cos(MathUtils::angle2rad(pitch));
        front.y() = std::sin(MathUtils::angle2rad(pitch));
        front.z() = std::sin(MathUtils::angle2rad(yaw)) * std::cos(MathUtils::angle2rad(pitch));
        cameraFront = front.normalize();
    }

    static void
    scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
        updateCamera = true;
        fov = fov >= 1.0 && fov <= 45.0 ? fov - yoffset : fov;
        fov = fov <= 1.0 ? 1.0 : fov;
        fov = fov >= 45.0 ? 45.0 : fov;
    }
#pragma endregion

};

// 静态数据成员初始化
bool GUI::isSaved = false;
bool GUI::isReset = false;
bool GUI::updateCamera = false;
bool GUI::firstMouse = true;
std::array<bool, 1024> GUI::keys = {};

GLdouble GUI::lastX, GUI::lastY, GUI::pitch, GUI::yaw, GUI::fov;
Vector3 GUI::cameraFront;

}

#endif //ANYA_ENGINE_GUI_HPP
