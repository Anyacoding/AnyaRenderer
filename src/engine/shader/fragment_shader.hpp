//
// Created by Anya on 2022/12/22.
//

#ifndef ANYA_ENGINE_FRAGMENT_SHADER_HPP
#define ANYA_ENGINE_FRAGMENT_SHADER_HPP

#include "load/texture.hpp"

namespace anya {

class FragmentShader {
public:
    Vector4 viewSpacePosition{};                            // view-space下的坐标
    Vector3 color{};                                        // 颜色
    Vector4 normal{};                                       // 法线
    Vector<2> uv{};                                         // 纹理坐标
    std::shared_ptr<Texture> texture;                       // 纹理资源
    std::function<Vector3(const FragmentShader&)> process;  // 着色方法
public:
    void
    setMethod(std::function<Vector3(const FragmentShader&)> f) {
        process = std::move(f);
    }

    void
    init(const Vector4& v, const Vector3& c, const Vector4& n, const Vector<2>& UV) {
        viewSpacePosition = v;
        color = c;
        normal = n;
        uv = UV;
    }
};

}

#endif //ANYA_ENGINE_FRAGMENT_SHADER_HPP
