//
// Created by Anya on 2022/11/23.
//

#ifndef ANYAENGINE_VEC_HPP
#define ANYAENGINE_VEC_HPP

#include <cmath>
#include <algorithm>
#include <iostream>
#include <concepts>

namespace anya {

using numberType = double;

template<int N> requires(N >= 1)
class Vec {
public:
#pragma region 构造相关
    constexpr Vec() = default;
    // 超过向量长度的数据将被丢弃
    constexpr Vec(std::initializer_list<numberType> st) {
        auto it = st.begin();
        for (int i = 0; it < st.end() && i < N; ++i, ++it)
            data[i] = *it;
    }
#pragma endregion

public:
#pragma region 操作容器
    // 下标访问，并进行越界检查
    constexpr numberType&
    operator[](const int index) {
        if (index >= N)
            throw std::out_of_range("Cross boundary access of vector!");
        return data[index];
    }
    constexpr numberType
    operator[](const int index) const {
        if (index >= N)
            throw std::out_of_range("Cross boundary access of vector!");
        return data[index];
    }

    // 向量的维数
    [[nodiscard]] constexpr int
    dimension() const noexcept { return N; }

    [[nodiscard]] constexpr int
    size() const noexcept { return N; }

#pragma endregion

public:
#pragma region 向量运算
    // 点乘，隐含了维数一致的约束，即维数都为N
    constexpr numberType
    dot(const Vec& rhs) const noexcept {
        numberType ret = {};
        for (int i = 0; i < N; ++i)
            ret += (*this)[i] * rhs[i];
        return ret;
    }

    // 叉乘，其中i j k为基向量
    //                                |i v1 w1|
    // [v1 v2 v3]T X [w1 w2 w3]T = det|j v2 w2|
    //                                |k v3 w3|
    constexpr Vec
    cross(const Vec& rhs) const noexcept requires(N == 3) {
        const Vec& lhs = *this;
        return { lhs[1] * rhs[2] - rhs[1] * lhs[2],
                -lhs[0] * rhs[2] + rhs[0] * lhs[2],
                 lhs[0] * rhs[1] - rhs[0] * lhs[1]
        };
    }

    // 向量的L2范数，也就是向量的标量的大小
    [[nodiscard]] constexpr numberType
    norm2() const { return std::sqrt(dot(*this)); }

    // 将向量归一化为单位向量
    constexpr Vec
    normalize() const { return *this / norm2(); }

    // 向量夹角 [0, pi]
    constexpr numberType
    angle(const Vec& rhs) const {
        return std::acos( this->dot(rhs) / (this->norm2() * rhs.norm2()) );
    }


#pragma endregion

public:
#pragma region 向量运算涉及的运算符重载
    // 向量加法
    constexpr Vec&
    operator+=(const Vec& rhs) noexcept {
        Vec& lhs = *this;
        for (int i = 0; i < N; ++i)
            lhs[i] += rhs[i];
        return lhs;
    }

    constexpr friend Vec
    operator+(const Vec& lhs, const Vec& rhs) noexcept { return Vec{lhs} += rhs; }

    // 向量减法
    constexpr Vec&
    operator-=(const Vec& rhs) noexcept {
        Vec& lhs = *this;
        for (int i = 0; i < N; ++i)
            lhs[i] -= rhs[i];
        return lhs;
    }

    constexpr friend Vec
    operator-(const Vec& lhs, const Vec& rhs) noexcept { return Vec{lhs} -= rhs; };

    // 向量取负
    constexpr Vec
    operator-() const noexcept { return *this * (-1); };

    // 向量伸缩
    constexpr Vec&
    operator*=(numberType k) noexcept {
        for (auto& i : this->data) i *= k;
        return *this;
    }

    constexpr friend Vec
    operator*(const Vec& lhs, numberType k) noexcept { return Vec{lhs} *= k; }

    constexpr friend Vec
    operator*(numberType k, const Vec& rhs) noexcept { return Vec{rhs} *= k; }

    constexpr Vec&
    operator/=(numberType k) {
        for (auto& i : this->data) i /= k;
        return *this;
    }

    constexpr friend Vec
    operator/(const Vec& lhs, numberType k) { return Vec{lhs} /= k; }
#pragma endregion

public:
#pragma region IO
    // 按列向量的形式输出，eg: [1, 2]T
    friend std::ostream&
    operator<<(std::ostream& out, const Vec& vec) {
        out << "[";
        for (int i = 0; i < vec.size(); ++i) {
            out << vec[i];
            if (i != vec.size() - 1) out << ", ";
        }
        out << "]T";
        return out;
    }
#pragma endregion

public:
#pragma region 坐标
    [[nodiscard]] constexpr numberType
    x() const noexcept requires(N >= 1) { return data[0]; }

    [[nodiscard]] constexpr numberType
    y() const noexcept requires(N >= 2) { return data[1]; }

    [[nodiscard]] constexpr numberType
    z() const noexcept requires(N >= 3) { return data[2]; }

    [[nodiscard]] constexpr numberType
    w() const noexcept requires(N >= 4) { return data[3]; }

    // w != 0 时，该齐次坐标代表一个点，将该点标准化表示
    constexpr Vec
    trim() const noexcept requires(N >= 4) {
        auto w = this->w();
        if (fabs(w) > 1e-8) return *this / w;
        return *this;
    }

#pragma endregion
private:
    // 底层数据存储
    numberType data[N] = {};
};

// 平面坐标/二维向量
using Vec2 = Vec<2>;
// 空间坐标坐标/三维向量
using Vec3 = Vec<3>;
// 四元数/四维向量
using Vec4 = Vec<4>;

// 便捷创建向量
template<typename... Args>
constexpr auto make_Vec(Args&&... args) requires((std::convertible_to<Args, numberType> && ...)) {
    return Vec<sizeof...(args)>{static_cast<numberType>(std::forward<Args>(args))...};
}

}

#endif //ANYAENGINE_VEC_HPP
