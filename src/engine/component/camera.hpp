//
// Created by Anya on 2022/12/5.
//

#ifndef ANYA_ENGINE_CAMERA_HPP
#define ANYA_ENGINE_CAMERA_HPP

#include "tool/matrix.hpp"
#include "tool/utils.hpp"
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
    // 通过这三个属性定义一个具体的相机
    // 注意后两个属性都是在相对坐标系下得到的
    Vector3 eye_pos;                  // 摄像机位置
    Vector3 gaze_pos;                 // 观察方向
    Vector3 view_up;                  // 视点的正上方向

    GLdouble view_width, view_height;    // 视窗大小
    GLdouble fovY;                       // 视野角度
    GLdouble zNear = -0.1, zFar = -50.0; // 视锥近远平面距离
    GLdouble aspect_ratio;               // 宽高比
public:
    Camera(const Vector3& eye_pos,    // 摄像机位置
           const Vector3& obj_pos,    // 物体的位置
           GLdouble view_width,       // 视窗宽度
           GLdouble view_height,      // 视窗高度
           GLdouble angle             // 视角——角度制，需要转换
           ): eye_pos(eye_pos), view_width(view_width), view_height(view_height), fovY(MathUtils::angle2rad(angle)) {
        // 宽高比
        aspect_ratio = view_width / view_height;
        // 观察方向
        gaze_pos = (obj_pos - eye_pos).normalize();
        // TODO: 视点的正上方向(还不会求我去)
    }

    [[nodiscard]] std::pair<GLdouble, GLdouble>
    getWH() const {
        return { view_width, view_height };
    };

public:
    // TODO: 优化视图变换
    // 将相机坐标系转换为世界坐标系，即视图变换，此过程中观察的对象会跟着一起运动
    [[nodiscard]] Matrix44 getViewMat() const {
        // 先将相机移动到世界坐标系原点
        Matrix44 translate{};
        translate << 1, 0, 0, -eye_pos[0],
                     0, 1, 0, -eye_pos[1],
                     0, 0, 1, -eye_pos[2],
                     0, 0, 0, 1;
        // 再将相机坐标系摆正到世界坐标系
        // Matrix44 view{};
        // 这里的x y z是相机坐标系的坐标轴
        // Vector3 z = -gaze_pos;
        // Vector3 x = view_up.cross(z);
        // Vector3 y = view_up;
        // Matrix33 temp{};
        // temp.setColVec(0, x);
        // temp.setColVec(1, y);
        // temp.setColVec(2, z);
        // view = temp.transpose().to44();

        // TODO: 权宜之计，假设相机一开始都是摆正的
        // view = Matrix44::Identity();
        // return view * translate;
        return translate;
    }

    // 投影变换
    [[nodiscard]] Matrix44 getProjectionMat() const {
        // 透视投影一共有两步，第一步首先是将透视投影转化为正交投影的形式
        GLdouble n = zNear, f = zFar;
        // Matrix44 Mpersp_ortho{};
        // Mpersp_ortho << n, 0, 0, 0,
        //                 0, n, 0, 0,
        //                 0, 0, n+f, -n*f,
        //                 0, 0, 1, 0;
        // 再对其做正交投影
        GLdouble t = std::fabs(n) * std::tan(fovY / 2);
        GLdouble b = -t;
        GLdouble r = aspect_ratio * t;
        GLdouble l = -r;
        // Matrix44 Mortho{}, Mtrans{}, Mscale{};
        // Mtrans << 1, 0, 0, -(r + l) / 2,
        //           0, 1, 0, -(t + b) / 2,
        //           0, 0, 1, -(n + f) / 2,
        //           0, 0, 0, 1;
        // Mscale << 2 / (r - l), 0, 0, 0,
        //           0, 2 / (t - b), 0, 0,
        //           0, 0, 2 / (n - f), 0,
        //           0, 0, 0, 1;
        // Mortho = Mscale * Mtrans;

        // 这里直接一步到位填好投影矩阵
        Matrix44 Mprojection{};
        Mprojection << 2 * n / (r - l), 0, (l + r) / (l - r), 0,
                       0, 2 * n / (t - b), (b + t) / (b - t), 0,
                       0, 0, (f + n) / (n - f), 2 * f * n / (f - n),
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

    // 深度信息修正参数
    [[nodiscard]] std::pair<numberType, numberType>
    getFixedArgs() const {
        return {(zNear - zFar) / 2, (zNear + zFar) / 2};
    }
};

}

#endif //ANYA_ENGINE_CAMERA_HPP
