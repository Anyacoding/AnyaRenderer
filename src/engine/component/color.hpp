//
// Created by DELL on 2023/1/10.
//

#ifndef ANYA_RENDERER_COLOR_HPP
#define ANYA_RENDERER_COLOR_HPP

#include "tool/vec.hpp"

namespace anya {

class Color {
private:
    numberType red = 0.0, green = 0.0, blue = 0.0;

public:
    Color() = default;

    Color(numberType r, numberType g, numberType b): red(r), green(g), blue(b)
    {}

    Color(const std::initializer_list<numberType>& st) {
        auto it = st.begin();
        for (int i = 0; it < st.end() && i < 3; ++i, ++it) {
            if (i == 0)      red = *it;
            else if (i == 1) green = *it;
            else if (i == 2) blue = *it;
            else break;
        }
    }

    explicit Color(const Vector3& vec) {
        red = vec.x();
        green = vec.y();
        blue = vec.z();
    }
public:
    [[nodiscard]] constexpr numberType&
    r() noexcept {
        return red;
    }

    [[nodiscard]] constexpr numberType&
    g() noexcept {
        return green;
    }

    [[nodiscard]] constexpr numberType&
    b() noexcept {
        return blue;
    }

    [[nodiscard]] constexpr const numberType&
    r() const noexcept {
        return red;
    }

    [[nodiscard]] constexpr const numberType&
    g() const noexcept {
        return green;
    }

    [[nodiscard]] constexpr const numberType&
    b() const noexcept {
        return blue;
    }

public:
    // 加法
    Color&
    operator+=(const Color& rhs) {
        red += rhs.r(); green += rhs.g(); blue += rhs.b();
        return *this;
    }

    friend Color
    operator+(const Color& lhs, const Color& rhs) { return Color{lhs} += rhs; }

    // 减法
    Color&
    operator-=(const Color& rhs) {
        red -= rhs.r(); green -= rhs.g(); blue -= rhs.b();
        return *this;
    }

    friend Color
    operator-(const Color& lhs, const Color& rhs) { return Color{lhs} -= rhs; }

    // 除法
    Color&
    operator/=(numberType k) {
        red /= k; green /= k; blue /= k;
        *this = this->clamp(0, 1);
        return *this;
    }

    friend Color
    operator/(const Color& lhs, numberType k) { return Color{lhs} /= k; }

    // 乘法
    Color&
    operator*=(numberType k) {
        red *= k; green *= k; blue *= k;
        *this = this->clamp(0, 255);
        return *this;
    }

    friend Color
    operator*(numberType k, const Color& rhs) { return Color{rhs} *= k; }

    friend Color
    operator*(const Color& lhs, numberType k) { return Color{lhs} *= k; }

public:
    [[nodiscard]] Color
    clamp(numberType lower = 0.0, numberType upper = 1.0) const {
        return { MathUtils::clamp(lower, upper, red), MathUtils::clamp(lower, upper, green), MathUtils::clamp(lower, upper, blue) };
    }
};

}

#endif //ANYA_RENDERER_COLOR_HPP
