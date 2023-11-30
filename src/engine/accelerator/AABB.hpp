//
// Created by Anya on 2023/1/27.
//

#ifndef ANYA_RENDERER_AABB_HPP
#define ANYA_RENDERER_AABB_HPP



namespace anya {

// 经典的轴对齐包围盒
class AABB {
public:
    Vector3 pMin, pMax;  // 用两个对角点即可描述一个包围盒

public:
    AABB() {
        pMin = Vector3{KMAX, KMAX, KMAX};
        pMax = Vector3{KMIN, KMIN, KMIN};
    }

    AABB(const Vector3& p1, const Vector3& p2) {
        pMin = mergeMinVec3(p1, p2);
        pMax = mergeMaxVec3(p1, p2);
    }

public:
#pragma region 判断相交
    // 判断光线是否与包围盒相交
    [[nodiscard]] bool
    intersect(const Ray& ray) const {
        const auto& origin = ray.pos;
        const auto& dir = ray.dir;
        numberType tEnter = -inf;
        numberType tExit = inf;
        for (int i = 0; i < 3; ++i) {
            numberType min = (pMin[i] - origin[i]) / dir[i];
            numberType max = (pMax[i] - origin[i]) / dir[i];
            // 这里的光线方向都是位于原点的方向向量
            // 如果坐标是负的，说明光线方向是反的，所以最大和最小的两个角刚好是反过来的
            if (dir[i] < 0) {
                std::swap(min, max);
            }
            tEnter = std::max(min, tEnter);
            tExit = std::min(max, tExit);
        }
        return tEnter <= tExit && tExit >= 0.0;
    }
#pragma endregion

public:
#pragma region 合并包围盒
    static AABB
    merge(const AABB& aabb, const Vector3& p) {
        AABB ret;
        ret.pMin = mergeMinVec3(aabb.pMin, p);
        ret.pMax = mergeMaxVec3(aabb.pMax, p);
        return ret;
    }

    static AABB
    merge(const Vector3& p, const AABB& aabb) {
        return merge(aabb, p);
    }

    static AABB
    merge(const AABB& lhs, const AABB& rhs) {
        AABB ret;
        ret.pMin = mergeMinVec3(lhs.pMin, rhs.pMin);
        ret.pMax = mergeMaxVec3(lhs.pMax, rhs.pMax);
        return ret;
    }
#pragma endregion

public:
#pragma region 包围盒属性
    // 返回重心坐标
    [[nodiscard]] Vector3
    centroid() const { return 0.5 * pMin + 0.5 * pMax; }

    // 返回对角线向量
    [[nodiscard]] Vector3
    diagonal() const { return pMax - pMin; }

    [[nodiscard]] int
    maxExtent() const {
        Vector3 d = diagonal();
        if (d.x() > d.y() && d.x() > d.z()) return 0;
        else if (d.y() > d.z()) return 1;
        else return 2;
    }
#pragma endregion

private:
#pragma region 辅助函数
    static Vector3
    mergeMinVec3(const Vector3& lhs, const Vector3& rhs) {
        return Vector3{std::fmin(lhs.x(), rhs.x()), std::fmin(lhs.y(), rhs.y()), std::fmin(lhs.z(), rhs.z())};
    }

    static Vector3
    mergeMaxVec3(const Vector3& lhs, const Vector3& rhs) {
        return Vector3{std::fmax(lhs.x(), rhs.x()), std::fmax(lhs.y(), rhs.y()), std::fmax(lhs.z(), rhs.z())};
    }
#pragma endregion

};

}

#endif //ANYA_RENDERER_AABB_HPP
