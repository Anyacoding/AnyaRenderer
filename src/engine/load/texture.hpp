//
// Created by Anya on 2022/12/26.
//

#ifndef ANYA_ENGINE_TEXTURE_HPP
#define ANYA_ENGINE_TEXTURE_HPP

#define STB_IMAGE_IMPLEMENTATION	// include之前必须定义
#include "STB/stb_image.h"
#undef STB_IMAGE_IMPLEMENTATION

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "STB/stb_image_write.h"
#undef STB_IMAGE_WRITE_IMPLEMENTATION

#include "tool/utils.hpp"

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
            std::cout << "image width: " << width << " " << "image height: " << height << std::endl << std::endl;
            loadFromRawData(data);
            stbi_image_free(data);
        }
    }

    explicit Texture(int w, int h, Vector3 bg): width(w), height(h), colors(w * h, bg)
    {}

    Texture() = default;

public:
    void
    loadFromRawData(const unsigned char* data) {
        colors.resize(width * height);
        for (int i = height - 1, k = 0; i >= 0; --i) {
            for (int j = 0; j < width; ++j, ++k) {
                numberType r = data[k * bpp];
                numberType g = data[k * bpp + 1];
                numberType b = data[k * bpp + 2];
                colors[j + i * width] = Vector3 {r, g, b};
            }
        }
    }

    [[nodiscard]] std::vector<stbi_uc>
    generateBuffer() const {
        std::vector<stbi_uc> buffer(width * height * bpp);
        for (int i = height - 1, k = 0; i >= 0; --i) {
            for (int j = 0; j < width; ++j, ++k) {
                auto color = getPixel(j, i) * 255;
                auto r = MathUtils::clamp(0, 255, color[0]);
                auto g = MathUtils::clamp(0, 255, color[1]);
                auto b = MathUtils::clamp(0, 255, color[2]);
                buffer[k * bpp]     = stbi_uc(r);
                buffer[k * bpp + 1] = stbi_uc(g);
                buffer[k * bpp + 2] = stbi_uc(b);
            }
        }
        return buffer;
    }

    void
    saveToDisk(const std::string& path) const {
        auto it = path.find_last_of('.');
        if (it == std::string::npos) {
            std::cerr << "An illegal path!" << std::endl;
            return;
        }
        std::string ext = path.substr(it + 1);
        auto buffer = generateBuffer();

        if (ext == "bmp") {
            stbi_write_bmp(path.c_str(), width, height, bpp, buffer.data());
        }
        else if (ext == "png") {
            stbi_write_png(path.c_str(), width, height, bpp, buffer.data(), 0);
        }
        else if (ext == "jpg") {
            stbi_write_jpg(path.c_str(), width, height, bpp, buffer.data(), 100);
        }
    }

public:
    // Nearst
    [[nodiscard]] Vector3
    getColor(numberType u, numberType v) const {
        u = std::fmin(1, std::fmax(u, 0));
        v = std::fmin(1, std::fmax(v, 0));

        auto u_img = int(u * (width - 1));
        auto v_img = int(v * (height - 1));

        if (out_range(u_img, v_img)) {
            throw std::out_of_range("Texture::getColor");
        }
        return colors[u_img + v_img * width];
    }

    // Bilinear 双线性插值
    [[nodiscard]] Vector3
    getColorBilinear(numberType u, numberType v) const {
        u = std::fmin(1, std::fmax(u, 0));
        v = std::fmin(1, std::fmax(v, 0));

        auto u_img = u * (width - 1);
        auto v_img = v * (height - 1);
        numberType u0 = std::fmax(0, std::floor(u_img - 0.5));
        numberType u1 = std::fmin(width - 1, std::floor(u_img + 0.5));
        numberType v0 = std::fmax(0, std::floor(v_img - 0.5));
        numberType v1 = std::fmin(height - 1, std::floor(v_img + 0.5));
        numberType s = (u_img - u0) / (u1 - u0);
        numberType t = (v_img - v0) / (v1 - v0);

        if (out_range(u0, v0) || out_range(u0, v1) || out_range(u1, v0) || out_range(u1, v1)) {
            throw std::out_of_range("Texture::getColor");
        }

        auto c00 = colors[int(u0 + v0 * width)];
        auto c01 = colors[int(u0 + v1 * width)];
        auto c10 = colors[int(u1 + v0 * width)];
        auto c11 = colors[int(u1 + v1 * width)];

        auto c0 = MathUtils::lerp(s, c00, c10);
        auto c1 = MathUtils::lerp(s, c01, c11);
        auto color = MathUtils::lerp(t, c0, c1);

        return color;
    }

public:
    [[nodiscard]] int
    getWidth() const noexcept { return width; }

    [[nodiscard]] int
    getHeight() const noexcept { return height; }

    void
    setPixel(int x, int y, Vector3 color) {
        if (out_range(x, y))
            throw std::out_of_range("Texture::setPixel(int x, int y)");
        colors[x + y * width] = color;
    }

    [[nodiscard]] Vector3
    getPixel(int x, int y) const {
        if (out_range(x, y))
            throw std::out_of_range("Texture::setPixel(int x, int y)");
        return colors[x + y * width];
    }

    void
    clearWith(Vector3 bg = {0, 0, 0}) { colors.assign(width * height, bg); }

    [[nodiscard]] constexpr bool
    out_range(numberType u, numberType v) const {
        return (int)u < 0 || (int)u >= width || (int)v < 0 || (int)v >= height;
    }
};

}

#endif //ANYA_ENGINE_TEXTURE_HPP
