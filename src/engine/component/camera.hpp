//
// Created by Anya on 2022/12/5.
//

#ifndef ANYA_ENGINE_CAMERA_HPP
#define ANYA_ENGINE_CAMERA_HPP

#include "math/matrix.hpp"
#include "math/utils.hpp"
#include <cmath>
#include <glad/glad.h>
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
    friend class Rasterizer;  // 设置友类，方便光栅化器访问

    // 通过这三个属性定义一个具体的相机
    // 注意后两个属性都是在相对坐标系下得到的
    Vector3 eye_pos;                  // 摄像机位置
    Vector3 gaze_pos;                 // 观察方向
    Vector3 view_up;                  // 视点的正上方向

    GLdouble view_width, view_height; // 视窗大小
    GLdouble fovY;                    // 视野角度
    GLdouble zNear, zFar;             // 视锥近远平面距离
public:
    Camera(const Vector3& eye_pos,    // 摄像机位置
           const Vector3& obj_pos,    // 物体的位置
           GLdouble zNear,            // 视锥近平面
           GLdouble zFar,             // 视锥远平面
           GLdouble angle,            // 视野角度，需要转为弧度
           GLdouble aspect_ratio      // 宽高比
           ): eye_pos(eye_pos), fovY(MathUtils::angle2rad(angle)), zNear(zNear), zFar(zFar) {
        // 视窗大小
        view_height = 2 * std::fabs(zNear) * std::tan(fovY / 2);
        view_width = view_height * aspect_ratio;
        // 观察方向
        gaze_pos = (eye_pos - obj_pos).normalize();
        // TODO: 视点的正上方向(还不会求我去)

    }
public:
    // TODO: 优化视图变换
    // 将相机坐标系转换为世界坐标系，即视图变换
    Matrix44 getViewMat() const {
        // 先将相机移动到世界坐标系原点
        Matrix44 translate{};
        translate << 1, 0, 0, -eye_pos[0],
                     0, 1, 0, -eye_pos[1],
                     0, 0, 1, -eye_pos[2],
                     0, 0, 0, 1;
        // 再将相机坐标系摆正到世界坐标系
        Matrix44 view{};
        // 这里的x y z是相机坐标系的坐标轴
        Vector3 z = -gaze_pos;
        Vector3 x = view_up.cross(z);
        Vector3 y = view_up;
        Matrix33 temp{};
        temp.setColVec(0, x);
        temp.setColVec(1, y);
        temp.setColVec(2, z);
        view = temp.transpose().to44();
        return view * translate;
    }

    // TODO: 优化投影变换
    Matrix44 getProjectionMat() const {
        // 透视投影一共有两步，第一步首先是将透视投影转化为正交投影的形式
        GLdouble n = zNear, f = zFar;
        GLdouble aspect_ratio = view_width / view_height;
        Matrix44 Mpersp_ortho{};
        Mpersp_ortho << n, 0, 0, 0,
                        0, n, 0, 0,
                        0, 0, n+f, -n*f,
                        0, 0, 1, 0;
        // 再对其做正交投影
        GLdouble t = -n * std::tan(fovY / 2);
        GLdouble b = -t;
        GLdouble r = aspect_ratio * t;
        GLdouble l = -r;
        Matrix44 Mortho{}, Mtrans{}, Mscale{};
        Mtrans << 1, 0, 0, -(r + l) / 2,
                  0, 1, 0, -(t + b) / 2,
                  0, 0, 1, -(n + f) / 2,
                  0, 0, 0, 1;
        Mscale << 2 / (r - l), 0, 0, 0,
                  0, 2 / (t - b), 0, 0,
                  0, 0, 2 / (n - f), 0,
                  0, 0, 0, 1;
        Mortho = Mscale * Mtrans;
        return Mortho * Mpersp_ortho;
    }
};

}

#endif //ANYA_ENGINE_CAMERA_HPP
