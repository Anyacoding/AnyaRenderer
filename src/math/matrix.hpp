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
            mat.data[cnt / N][cnt % N] = val;
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
#pragma region 操作容器
    constexpr numberType&
    operator()(int i, int j) {
        if (out_range(i, j))
            throw std::out_of_range("Matrix::operator()");
        return data[i][j];
    }

    constexpr numberType
    operator()(int i, int j) const {
        if (out_range(i, j))
            throw std::out_of_range("Matrix::operator()");
        return data[i][j];
    }

    [[nodiscard]] constexpr int
    row() const noexcept { return M; }

    [[nodiscard]] constexpr int
    column() const noexcept { return N; }
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
    // 底层数据存储
    numberType data[M][N];
};

}

#endif //ANYA_ENGINE_MATRIX_HPP
