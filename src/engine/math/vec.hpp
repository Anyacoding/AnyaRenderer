//
// Created by Anya on 2022/11/23.
//

#ifndef ANYA_ENGINE_VEC_HPP
#define ANYA_ENGINE_VEC_HPP

#include <cmath>
#include <algorithm>
#include <iostream>
#include <concepts>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

namespace anya {

using numberType = GLdouble;

template<int N> requires(N >= 1)
class Vector {
public:
#pragma region 构造相关
    constexpr Vector() = default;
    // 超过向量长度的数据将被丢弃
    constexpr Vector(const std::initializer_list<numberType>& st) {
        auto it = st.begin();
        for (int i = 0; it < st.end() && i < N; ++i, ++it)
            data[i] = *it;
    }
#pragma endregion

public:
#pragma region 访问
    // 下标访问，并进行越界检查，支持[]访问和()访问，两者等价
    constexpr numberType&
    operator[](const int index) {
        if (index >= N)
            throw std::out_of_range("Vector::operator[]");
        return data[index];
    }
    constexpr numberType
    operator[](const int index) const {
        if (index >= N)
            throw std::out_of_range("Vector::operator[]");
        return data[index];
    }

    constexpr numberType&
    operator()(const int index) {
        if (index >= N)
            throw std::out_of_range("Vector::operator[]");
        return data[index];
    }
    constexpr numberType
    operator()(const int index) const {
        if (index >= N)
            throw std::out_of_range("Vector::operator[]");
        return data[index];
    }

    // 向量的维数
    [[nodiscard]] constexpr int
    dimension() const noexcept { return N; }

    [[nodiscard]] constexpr int
    size() const noexcept { return N; }

    // 返回底层数组
    constexpr numberType* toRawArray() noexcept {
        return this->data;
    }


#pragma endregion

public:
#pragma region 向量运算
    // 点乘，隐含了维数一致的约束，即维数都为N
    constexpr numberType
    dot(const Vector& rhs) const noexcept {
        numberType ret = {};
        for (int i = 0; i < N; ++i)
            ret += (*this)[i] * rhs[i];
        return ret;
    }

    // 叉乘，其中i j k为基向量
    //                                |i v1 w1|
    // [v1 v2 v3]T X [w1 w2 w3]T = det|j v2 w2|
    //                                |k v3 w3|
    constexpr Vector
    cross(const Vector& rhs) const noexcept requires(N == 3) {
        const Vector& lhs = *this;
        return { lhs[1] * rhs[2] - rhs[1] * lhs[2],
                -lhs[0] * rhs[2] + rhs[0] * lhs[2],
                 lhs[0] * rhs[1] - rhs[0] * lhs[1]
        };
    }

    // 向量的L2范数，也就是向量的标量的大小
    [[nodiscard]] constexpr numberType
    norm2() const { return std::sqrt(dot(*this)); }

    // 将向量归一化为单位向量
    constexpr Vector
    normalize() const { return *this / norm2(); }

    // 向量夹角 [0, pi]
    constexpr numberType
    angle(const Vector& rhs) const {
        return std::acos( this->dot(rhs) / (this->norm2() * rhs.norm2()) );
    }


#pragma endregion

public:
#pragma region 向量运算涉及的运算符重载
    // 向量加法
    constexpr Vector&
    operator+=(const Vector& rhs) noexcept {
        Vector& lhs = *this;
        for (int i = 0; i < N; ++i)
            lhs[i] += rhs[i];
        return lhs;
    }

    constexpr friend Vector
    operator+(const Vector& lhs, const Vector& rhs) noexcept { return Vector{lhs} += rhs; }

    // 向量减法
    constexpr Vector&
    operator-=(const Vector& rhs) noexcept {
        Vector& lhs = *this;
        for (int i = 0; i < N; ++i)
            lhs[i] -= rhs[i];
        return lhs;
    }

    constexpr friend Vector
    operator-(const Vector& lhs, const Vector& rhs) noexcept { return Vector{lhs} -= rhs; };

    // 向量取负
    constexpr Vector
    operator-() const noexcept { return *this * (-1); };

    // 向量伸缩
    constexpr Vector&
    operator*=(numberType k) noexcept {
        for (auto& i : this->data) i *= k;
        return *this;
    }

    constexpr friend Vector
    operator*(const Vector& lhs, numberType k) noexcept { return Vector{lhs} *= k; }

    constexpr friend Vector
    operator*(numberType k, const Vector& rhs) noexcept { return Vector{rhs} *= k; }

    constexpr Vector&
    operator/=(numberType k) {
        for (auto& i : this->data) i /= k;
        return *this;
    }

    constexpr friend Vector
    operator/(const Vector& lhs, numberType k) { return Vector{lhs} /= k; }

#pragma endregion

public:
#pragma region IO
    // 按列向量的形式输出，eg: [1, 2]T
    friend std::ostream&
    operator<<(std::ostream& out, const Vector& vec) {
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
    x() const noexcept requires(N >= 1) { return this->data[0]; }

    [[nodiscard]] constexpr numberType
    y() const noexcept requires(N >= 2) { return this->data[1]; }

    [[nodiscard]] constexpr numberType
    z() const noexcept requires(N >= 3) { return this->data[2]; }

    [[nodiscard]] constexpr numberType
    w() const noexcept requires(N >= 4) { return this->data[3]; }

    // w != 0 时，该齐次坐标代表一个点，将该点标准化表示
    constexpr Vector
    trim() const noexcept requires(N >= 4) {
        auto w = this->w();
        if (fabs(w) > 1e-8) return *this / w;
        return *this;
    }

#pragma endregion

private:
    // 底层数据存储
    numberType data[N]{};
};

// 平面坐标/二维向量
using Vector2 = Vector<2>;
// 空间坐标坐标/三维向量
using Vector3 = Vector<3>;
// 四元数/四维向量
using Vector4 = Vector<4>;

// 便捷创建向量
template<typename... Args>
constexpr auto make_Vec(Args&&... args) requires((std::convertible_to<Args, numberType> && ...)) {
    return Vector<sizeof...(args)>{static_cast<numberType>(std::forward<Args>(args))...};
}

}

#endif //ANYA_ENGINE_VEC_HPP
