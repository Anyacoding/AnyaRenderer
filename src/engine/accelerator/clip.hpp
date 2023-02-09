//
// Created by Anya on 2023/2/9.
//

#ifndef ANYA_RENDERER_CLIP_HPP
#define ANYA_RENDERER_CLIP_HPP

#include "component/object/triangle.hpp"

namespace anya {

// 剔除优化
struct ClipUtils {
    // 背面剔除
    static bool
    back_face_culling(const Triangle& triangle) {
        auto a = triangle.a().to<3>();
        auto b = triangle.b().to<3>();
        auto c = triangle.c().to<3>();
        Vector3 ab = b - a, ac = c - a;
        return ab.cross(ac).z() < 0;
    }
};

}

#endif //ANYA_RENDERER_CLIP_HPP
