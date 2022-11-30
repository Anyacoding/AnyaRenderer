//
// Created by Anya on 2022/11/29.
//

#ifndef ANYA_ENGINE_MATRIX_HPP
#define ANYA_ENGINE_MATRIX_HPP

#include "vec.hpp"

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
    constexpr Vector<N>
    rowVec(int row) const {
        if (out_range(row, 0))
            throw std::out_of_range("Matrix::rowVec");
        Vector<N> ret{};
        for (int j = 0; j < N; ++j) ret[j] = (*this)(row, j);
        return ret;
    }

    // 返回第 col 列的列向量
    constexpr Vector<M>
    colVec(int col) const {
        if (out_range(0, col))
            throw std::out_of_range("Matrix::rowVec");
        return data[col];
    }
#pragma endregion

public:
#pragma region 矩阵运算

#pragma endregion

public:
#pragma region 矩阵运算涉及的运算符重载
    // 矩阵乘法
    template<int C>
    constexpr friend Matrix<M, C>
    operator*(const Matrix<M, N>& lhs, const Matrix<N, C>& rhs) {
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
    operator*(const Matrix<M, N>& lhs, const Vector<N>& rhs) {
        Vector<M> ret{};
        for (int i = 0; i < M; ++i) ret[i] = lhs.rowVec(i).dot(rhs);
        return ret;
    }

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
#pragma endregion

private:
    // 底层数据存储, 列向量形式存储
    Vector<M> data[N];
};

}

#endif //ANYA_ENGINE_MATRIX_HPP
