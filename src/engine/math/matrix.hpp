//
// Created by Anya on 2022/11/29.
//

#ifndef ANYA_ENGINE_MATRIX_HPP
#define ANYA_ENGINE_MATRIX_HPP

#include "vec.hpp"
#include <exception>

namespace anya {

template<int M, int N> requires(M >= 1 && N >= 1)
class Matrix {
#pragma region 辅助类
private:
    struct Loader {
        constexpr Loader(Matrix& m, int i): mat(m), cnt(i) {}

        // 重载逗号运算发实现链式调用 eg: mat << 1, 2, 3;
        constexpr Loader
        operator,(numberType val) {
            // 多余的数据会被直接忽略掉
            if (cnt / N >= M) return Loader(mat, cnt + 1);
            mat(cnt / N, cnt % N) = val;
            return Loader(mat, cnt + 1);
        }
        Matrix& mat;
        int cnt = 0;
    };
#pragma endregion

public:
#pragma region 构造与初始化
    constexpr Matrix() = default;

    // 实现逗号初始化 eg: mat << 1, 2, 3;
    constexpr Loader
    operator<<(numberType val) {
        data[0][0] = val;
        return Loader(*this, 1);
    }
#pragma endregion

public:
#pragma region 访问
    constexpr numberType&
    operator()(int i, int j) {
        if (out_range(i, j))
            throw std::out_of_range("Matrix::operator()");
        return data[j][i];
    }

    constexpr numberType
    operator()(int i, int j) const {
        if (out_range(i, j))
            throw std::out_of_range("Matrix::operator()");
        return data[j][i];
    }

    // 行数
    [[nodiscard]] constexpr int
    row() const noexcept { return M; }

    // 列数
    [[nodiscard]] constexpr int
    column() const noexcept { return N; }

    // 返回第 row 行的行向量
    [[nodiscard]] constexpr Vector<N>
    rowVec(int row) const {
        if (out_range(row, 0))
            throw std::out_of_range("Matrix::rowVec");
        Vector<N> ret{};
        for (int j = 0; j < N; ++j) ret[j] = (*this)(row, j);
        return ret;
    }

    // 返回第 col 列的列向量
    [[nodiscard]] constexpr Vector<M>
    colVec(int col) const {
        if (out_range(0, col))
            throw std::out_of_range("Matrix::rowVec");
        return data[col];
    }

    // 将数据存入传入的数组中，为了适配opengl的api
    constexpr void
    saveToArray(numberType array[M * N]) const {
        for (int i = 0; i < M; ++i) {
            void* dst = array + i * N;
            size_t length = N * sizeof(numberType);
            memcpy(dst, this->rowVec(i).toRawArray(), length);
        }
    }
#pragma endregion

public:
#pragma region 矩阵运算
    // 转置
    [[nodiscard]] constexpr Matrix<N, M> transpose() const {
        Matrix<N, M> ret{};
        for (int i = 0; i < M; ++i) {
            for (int j = 0; j < N; ++j) {
                ret(j, i) = (*this)(i, j);
            }
        }
        return ret;
    }

    // 行列式
    [[nodiscard]] constexpr numberType det() const requires(M == N) {
        if constexpr (M == 1) {
            return (*this)(0, 0);
        }
        else {
            numberType ans = 0;
            for (int i = 0; i < M; ++i) {
                ans += (*this)(0, i) * minor_det(0, i);
            }
            return ans;
        }
    }

    // 求逆
    [[nodiscard]] constexpr Matrix<M, N> inverse() const requires(M == N) {
        numberType det = this->det();
        if (det == 0.0) {
            std::cerr << "The determinant is zero, this matrix has no inverse!" << std::endl;
            return {};
        }
        Matrix<M, N> ret{};
        for (int i = 0; i < M; ++i) {
            for (int j = 0; j < N; ++j) {
                ret(i, j) = minor_det(i, j);
            }
        }
        ret /= det;
        return ret.transpose();
    }

    // 转换为其他维数的矩阵
    template<int Q, int W>
    constexpr Matrix<Q, W> to(numberType fill = 0.0) const {
        Matrix<Q, W> ret{};
        for (int i = 0; i < Q; ++i) {
            for (int j = 0; j < W; ++j) {
                ret(i, j) = out_range(i, j) ? fill : (*this)(i, j);
            }
        }
        return ret;
    }

    // 从3*3矩阵转变为齐次坐标下的4*4矩阵
    [[nodiscard]] constexpr Matrix<4, 4>
    to44() const requires(M == 3 && N == 3) {
        Matrix<4, 4> ret = to<4, 4>();
        ret(3, 3) = 1;
        return ret;
    }
#pragma endregion

public:
#pragma region 矩阵运算涉及的运算符重载
    // 矩阵 * 矩阵
    template<int C>
    constexpr friend Matrix<M, C>
    operator*(const Matrix<M, N>& lhs, const Matrix<N, C>& rhs) noexcept {
        Matrix<M, C> ret{};
        for (int i = 0; i < M; ++i) {
            for (int j = 0; j < C; ++j) {
                for (int k = 0; k < N; ++k) {
                    ret(i, j) += lhs(i, k) * rhs(k, j);
                }
            }
        }
        return ret;
    }

    // 列向量左乘矩阵
    constexpr friend Vector<M>
    operator*(const Matrix<M, N>& lhs, const Vector<N>& rhs) noexcept {
        Vector<M> ret{};
        for (int i = 0; i < M; ++i) ret[i] = lhs.rowVec(i).dot(rhs);
        return ret;
    }

    // TODO: 暂时不打算提供向量右乘矩阵，因为这里默认向量都是列向量了，右乘应该是用不上的

    // 矩阵加法
    constexpr Matrix&
    operator+=(const Matrix& rhs) noexcept {
        for (int i = 0; i < N; ++i) {
            data[i] += rhs.data[i];
        }
        return *this;
    }

    // 矩阵减法
    constexpr Matrix&
    operator-=(const Matrix& rhs) noexcept {
        for (int i = 0; i < N; ++i) {
            data[i] -= rhs.data[i];
        }
        return *this;
    }

    // 矩阵乘k
    constexpr Matrix&
    operator*=(numberType k) noexcept {
        for (int i = 0; i < N; ++i) {
            data[i] *= k;
        }
        return *this;
    }

    // 矩阵除k
    constexpr Matrix&
    operator/=(numberType k) {
        for (int i = 0; i < N; ++i) {
            data[i] /= k;
        }
        return *this;
    }

    constexpr friend Matrix
    operator+(const Matrix& lhs, const Matrix& rhs) noexcept { return Matrix{lhs} += rhs; }

    constexpr friend Matrix
    operator-(const Matrix& lhs, const Matrix& rhs) noexcept { return Matrix{lhs} -= rhs; }

    constexpr friend Matrix
    operator*(const Matrix& lhs, numberType k) noexcept { return Matrix{lhs} *= k; }

    constexpr friend Matrix
    operator*(numberType k, const Matrix& rhs) noexcept { return Matrix{rhs} *= k; }

    constexpr friend Matrix
    operator/(const Matrix& lhs, numberType k) { return Matrix{lhs} /= k; }

#pragma endregion

public:
#pragma region IO
    friend std::ostream&
    operator<<(std::ostream& out, const Matrix& mat) {
        for (int i = 0; i < mat.row(); ++i) {
            for (int j = 0; j < mat.column(); ++j) {
                out << mat(i, j) << " ";
            }
            if (i != mat.row()) out << std::endl;
        }
        return out;
    }
#pragma endregion

public:
#pragma region 辅助函数
    constexpr static bool
    out_range(int i, int j) {
        return i < 0 || i >= M || j < 0 || j >= N;
    }

    // matrix去掉x行和y列后得到的余子式
    // fixed: 返回值必须用auto, 否则 Matrix<1 - 1, 2 - 1> 时无法满足约束
    [[nodiscard]] constexpr auto minor(int x, int y) const requires(M == N) {
        Matrix<M - 1, N - 1> ret{};
        for (int i = 0, row = 0; i < M; ++i) {
            if (i == x) continue ;
            for (int j = 0, column = 0; j < N; ++j) {
                if (j == y) continue ;
                ret(row, column) = (*this)(i, j);
                ++column;
            }
            ++row;
        }
        return ret;
    }

    // 代数余子式的值
    [[nodiscard]] constexpr numberType
    minor_det(int x, int y) const requires(M == N) {
        return minor(x, y).det() * ((x + y) % 2 ? -1 : 1);
    }
#pragma endregion

private:
    // 底层数据存储, 列向量形式存储
    Vector<M> data[N];
};

// 各常用矩阵别名
using Matrix33 = Matrix<3, 3>;
using Matrix34 = Matrix<3, 4>;
using Matrix43 = Matrix<4, 3>;
using Matrix44 = Matrix<4, 4>;

}

#endif //ANYA_ENGINE_MATRIX_HPP
