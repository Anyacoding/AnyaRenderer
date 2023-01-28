//
// Created by Anya on 2022/12/5.
//

#ifndef ANYA_ENGINE_CAMERA_HPP
#define ANYA_ENGINE_CAMERA_HPP

#include "tool/matrix.hpp"
#include "tool/utils.hpp"
#include "component/ray.hpp"
#include <cmath>
#include <GLFW/glfw3.h>

namespace anya{

/*
 右手坐标系的规约:
 x的正方向为从左到右
 y的正方向为从下到上
 z的正方形为从里到外
 (0, 0, 0)为屏幕正中心
 */

class Camera {
private:
    Vector3 eye_pos;                     // 摄像机位置
    Vector3 obj_pos;                     // 物体位置
    GLdouble view_width, view_height;    // 视窗大小
    GLdouble fovY;                       // 视野角度
    GLdouble zNear = -0.1, zFar = -50.0; // 视锥近远平面距离
    GLdouble aspect_ratio;               // 宽高比

    // 摄像机坐标系:
    Vector3 w;     // 观察方向
    Vector3 u;     // 摄像机右轴
    Vector3 v;     // 摄像机正上方向

public:
    bool isLock = false;  // 锁定摄像机，不允许移动

public:
    Camera(const Vector3& eye_pos,    // 摄像机位置
           const Vector3& obj_pos,    // 物体的位置
           GLdouble view_width,       // 视窗宽度
           GLdouble view_height,      // 视窗高度
           GLdouble angle             // 视角——角度制，需要转换
           ): eye_pos(eye_pos), obj_pos(obj_pos), view_width(view_width),
              view_height(view_height), fovY(MathUtils::angle2rad(angle)) {
        // 宽高比
        aspect_ratio = view_width / view_height;
        // 摄像机坐标系
        w = (-obj_pos).normalize();
        u = Vector3{0, 1, 0}.cross(w).normalize();
        v = w.cross(u);
    }

public:
    // 获取长宽
    [[nodiscard]] std::pair<GLdouble, GLdouble>
    getWH() const {
        return { view_width, view_height };
    }

    // 获取视野角度
    [[nodiscard]] numberType
    getFovY() const noexcept { return fovY; }

    // 获取相机位置
    [[nodiscard]] const Vector3&
    getEyePos() const { return eye_pos; }

    // 获取物体原始位置
    [[nodiscard]] const Vector3&
    getObjPos() const { return obj_pos; }

    // 更新相机坐标系
    void
    lookAt(Vector3 pos, Vector3 target) {
        eye_pos = pos;
        obj_pos = target;
        w = (pos - target).normalize();
        u = Vector3{0, 1, 0}.cross(w).normalize();
        v = w.cross(u);
    }

    // 深度信息修正参数
    [[nodiscard]] std::pair<numberType, numberType>
    getFixedArgs() const {
        return {(zNear - zFar) / 2, (zNear + zFar) / 2};
    }

    // 设置宽高比
    void
    setFovY(numberType fov) {
        fovY = MathUtils::angle2rad(fov);
    }
public:
    // 从世界空间转换为观察空间，即视图变换，此过程中观察的对象会跟着摄像机一起运动
    [[nodiscard]] Matrix44 getViewMat() const {
        // 先将相机移动到世界坐标系原点
        Matrix44 translate{};
        translate << 1, 0, 0, -eye_pos[0],
                     0, 1, 0, -eye_pos[1],
                     0, 0, 1, -eye_pos[2],
                     0, 0, 0, 1;
        Matrix44 view = Matrix44::Identity();
        view << u.x(), u.y(), u.z(), 0,
                v.x(), v.y(), v.z(), 0,
                w.x(), w.y(), w.z(), 0,
                0, 0, 0, 1;
        return view * translate;
    }

    // 投影变换
    [[nodiscard]] Matrix44 getProjectionMat() const {
        GLdouble n = zNear, f = zFar;
        GLdouble t = std::fabs(n) * std::tan(fovY / 2);
        GLdouble b = -t;
        GLdouble r = aspect_ratio * t;
        GLdouble l = -r;

        // 这里有修正符号 -2 * f * n / (f - n)，没有这个符号就是深度就是反的，主要原因是我们采用的是右手系，而opengl是左手系
        Matrix44 Mprojection{};
        Mprojection << 2 * n / (r - l), 0, (l + r) / (l - r), 0,
                       0, 2 * n / (t - b), (b + t) / (b - t), 0,
                       0, 0, (f + n) / (n - f), -2 * f * n / (f - n),
                       0, 0, 1, 0;
        return Mprojection;
    }

    // 视口变换
    [[nodiscard]] Matrix44 getViewPortMat() const {
        Matrix44 viewPortMat{};
        viewPortMat << view_width / 2, 0, 0, view_width / 2,
                       0, view_height / 2, 0, view_height / 2,
                       0, 0, 1, 0,
                       0, 0, 0, 1;
        return viewPortMat;
    }

public:
    // 发出光线，用于光线追踪
    [[nodiscard]] Ray
    biuRay(int i, int j) const {
        Ray ray{};
        numberType scale = std::tan(fovY / 2);
        numberType x = (2 * ((i + 0.5) / view_width) - 1) * scale * aspect_ratio;
        numberType y = (1 - 2 * ((j + 0.5) / view_height)) * scale;
        ray.dir = Vector3{x, y, -1}.normalize();
        ray.pos = eye_pos;
        return ray;
    }

};

}

#endif //ANYA_ENGINE_CAMERA_HPP
