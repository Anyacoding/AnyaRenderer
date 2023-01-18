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
    // ����
    std::string_view title;
    // ���
    GLdouble width, height;
    // ����handle
    GLFWwindow* window = nullptr;
    // ��Ⱦ����ʹ��ָ�뷽�㽫��ʹ�ö�̬
    std::shared_ptr<Renderer> renderer;
    // ��ʼ�����״̬
    const Camera defaultCamera;
    
private:
#pragma region ������������
    /*********״̬����***********/
    static bool isSaved;
    static bool isReset;
    static bool updateCamera;

    /*********����ƶ����***********/
    // ����ٶ�
    GLdouble cameraSpeed = 1;
    // ���λ��
    Vector3 cameraPos{};
    // �������
    static Vector3 cameraFront;
    // ����Ϸ���
    Vector3 cameraUp{};
    // key״̬ӳ��
    static std::array<bool, 1024> keys;

    /*********����ƶ����***********/
    // �����һ֡��λ��
    static GLdouble lastX, lastY;
    // ��һ�����ص���־
    static bool firstMouse;
    // ������
    static GLdouble pitch;
    // ƫ����
    static GLdouble yaw;

    /*********�����ƶ����***********/
    // ��Ұ
    static GLdouble fov;
#pragma endregion

public:
    GUI(std::string_view title, GLdouble width, GLdouble height, std::shared_ptr<Renderer> _renderer)
        :title(title), width(width), height(height),
        renderer(std::move(_renderer)), defaultCamera(*renderer->scene.camera)
    {}

    ~GUI() {
        glfwTerminate();   // ������ֹ���ͷ�glfw��Դ
    }

    void
    run() {
        // ��ʼ��glfw
        if (!init()) return;

        // glfw��������
        if ((window = createHandle()) == nullptr) return;

        // glfw����OpenGl������
        glfwMakeContextCurrent(window);

        // ��������ģʽ���������������
        setInputMode();

        // ���ûص�����, eg: ��esc�˳�
        setCallBack();

        // �����ӿڴ�С
        setViewport();

        // ˫���彻���������Ϊ1�����⽻��Ƶ�������Ļ˺��
        glfwSwapInterval(1);

        // �������ϵͳ����
        configure();

        // TODO: ����Ⱦ�߼�������һ���߳�
        renderer->render();

        // render loop
        while (!glfwWindowShouldClose(window)) {
            // ����������IO�¼�
            glfwPollEvents();
            // �����ɫ����
            clearWith();
            // ����Ƿ�ָ�Ĭ��״̬
            if (isReset) reset();
            // ���»���
            update();
            // ˫����������
            glfwSwapBuffers(window);
        }
    }
private:
#pragma region ��������߼�
    // �Զ����ƻ�ͼģʽ������
    struct ModeGuard {
        // ��������:   GL_TRIANGLES, GL_LINES, GL_POINTS, GL_POLYGON
        // �ֱ��ʾ:   ������,        �߶�,      ����,       �����
        explicit ModeGuard(GLenum mode) { glBegin(mode); }

        ~ModeGuard() { glEnd(); }
    };

    // �����߼�
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

    // ����ƶ�����
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

    // �ָ����ϵͳ״̬
    void
    reset() {
        renderer->scene.camera = std::make_shared<Camera>(defaultCamera);
        configure();
        renderer->render();
        isReset = false;
    }

    // �������ϵͳ״̬
    void
    configure() {
        // ���״̬
        cameraPos = renderer->scene.camera->getEyePos();
        cameraFront = renderer->scene.camera->getObjPos();
        cameraUp = {0, 1, 0};
        cameraSpeed = 0.5;

        // ���״̬
        lastX = width / 2;
        lastY = height / 2;
        yaw = -90;
        pitch = 0;

        // ����״̬
        fov = 45;

        // ״̬����
        isSaved = false;
        updateCamera = false;
        firstMouse = true;
        keys = {};
    }

#pragma endregion

private:
#pragma region ��װOpenGl�ĳ��ú���
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

        // ��������
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

// ��̬���ݳ�Ա��ʼ��
bool GUI::isSaved = false;
bool GUI::isReset = false;
bool GUI::updateCamera = false;
bool GUI::firstMouse = true;
std::array<bool, 1024> GUI::keys = {};

GLdouble GUI::lastX, GUI::lastY, GUI::pitch, GUI::yaw, GUI::fov;
Vector3 GUI::cameraFront;

}

#endif //ANYA_ENGINE_GUI_HPP
