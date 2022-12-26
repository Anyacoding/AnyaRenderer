//
// Created by Anya on 2022/12/26.
//

#ifndef ANYA_ENGINE_TEXTURE_HPP
#define ANYA_ENGINE_TEXTURE_HPP

#include "tool/vec.hpp"

#define STB_IMAGE_IMPLEMENTATION	// include之前必须定义
#include "STB/stb_image.h"
#undef STB_IMAGE_IMPLEMENTATION

// stb的库像素数据都是从左到右，从上到下存储
// 我们要转为通用纹理坐标，左下角为(0,0) , 右上角为(width-1, height-1)

namespace anya {

class Texture {
private:
    std::vector<Vector3> colors;   // 纹理颜色信息
    int width = 0, height = 0;     // 图片的长宽
    int n = 0;                     // 图片自身的颜色的通道数
    static constexpr int bpp = 3;  // 自己设置的颜色通道数
public:
    explicit Texture(const std::string& path) {
        unsigned char* data = stbi_load(path.c_str(), &width, &height, &n, bpp);
        if (data == nullptr) {
            std::cerr << "ERROR IMAGE NOT FOUND" << std::endl;
        }
        else {
            std::cout << "image width: " << width << " " << "image height: " << height << std::endl;
            loadFromRawData(data);
            stbi_image_free(data);
        }
    }
public:
    void
    loadFromRawData(const unsigned char* data) {
        colors.resize(width * height);
        for (int i = height - 1, k = 0; i >= 0; --i) {
            for (int j = 0; j < width; ++j, ++k) {
                numberType r = data[k * bpp];
                numberType g = data[k * bpp + 1];
                numberType b = data[k * bpp + 2];
                colors[j + i * width] = Vector3 {r, g, b} / 255;
            }
        }
    }

    [[nodiscard]] Vector3
    getColor(numberType u, numberType v) const {
        u *= (width - 1);  v *= (height - 1);
        int modu = (int)std::fabs(u), modv = (int)std::fabs(v);
        if (out_range(modu, modv)) {
            throw std::out_of_range("Texture::getColor");
        }
        return colors[modu + modv * width];
    }
public:
    [[nodiscard]] constexpr bool
    out_range(int u, int v) const {
        return u < 0 || u >= width || v < 0 || v >= height;
    }
};

}

#endif //ANYA_ENGINE_TEXTURE_HPP
