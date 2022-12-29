//
// Created by Anya on 2022/12/6.
//

#ifndef ANYA_ENGINE_MODEL_HPP
#define ANYA_ENGINE_MODEL_HPP

#include <vector>
#include <fstream>
#include <sstream>
#include "tool/matrix.hpp"
#include "tool/utils.hpp"
#include "component/object/triangle.hpp"
#include "shader/fragment_shader.hpp"
#include "shader/vertex_shader.hpp"
#include "load/texture.hpp"
#include "shader/methods.hpp"

namespace anya {

class Model {
public:
    std::vector<anya::Triangle> TriangleList;   // model的三角形集合
    Matrix44 modelMat = Matrix44::Identity();   // 模型变换矩阵
    FragmentShader fragmentShader{};            // 片元着色器
public:
    explicit Model(const std::string& modelPath) { loadFromDisk(modelPath); }

    // 从本地加载obj文件的数据
    void
    loadFromDisk(const std::string& modelPath) {
        std::ifstream ifs(modelPath);
        if (!ifs.is_open()) {
            std::cerr << "can not find the " + modelPath << std::endl;
            exit(-1);
        }
        std::vector<Vector3> vertexes{};  // 从obj读入的所有顶点集合
        std::vector<Vector3> normals{};   // 从obj读入的所有法线集合
        std::vector<Vector<2>> uvs{};     // 从obj读入的所有法线集合
        Vector3 vertex{};                 // 顶点
        Vector3 normal{};                 // 法线
        Vector<2> uv{};                   // 纹理坐标
        int v, t, n;                      // face对vertexes的索引
        char hole;                        // 吞掉多余的字符
        // 每次读入一行，并判断该行的类型
        std::string line, type;
        while (std::getline(ifs, line)) {
            std::istringstream iss(line);
            iss >> type;
            if (type == "v") {
                iss >> vertex.x() >> vertex.y() >> vertex.z();
                vertexes.push_back(vertex);
            }
            else if (type == "vn") {
                iss >> normal.x() >> normal.y() >> normal.z();
                normals.push_back(normal);
            }
            else if (type == "vt") {
                iss >> uv.x() >> uv.y();
                uvs.push_back(uv);
            }
            else if (type == "f") {
                Triangle triangle{};

                for (int i = 0; i < 3; ++i) {
                #ifndef Z_BUFFER_TEST
                    // f v1/vt1/vn1 v2/vt2/vn2 v3/vt3/vn3 ...
                    iss >> v >> hole >> t >> hole >> n;
                    triangle.setVertex(i, vertexes[v - 1].to4());
                    triangle.setNormal(i, normals[n - 1].to4(0.0));
                    triangle.setUV(i, uvs[t - 1]);
                #else
                    int index = 0;
                    iss >> index;
                    triangle.setVertex(i, vertexes[index - 1].to4());
                #endif
                }
                TriangleList.push_back(triangle);
            }
        }
        ifs.close();
        std::cout << "vertex: " << vertexes.size() << ", face: " << TriangleList.size() << std::endl;
    }
public:
    // 绕x轴旋转
    void
    RotateAroundX(numberType angle) {
        modelMat = Transform::RotateAroundX(angle);
    }
    // 绕y轴旋转
    void
    RotateAroundY(numberType angle) {
        modelMat = Transform::RotateAroundY(angle);
    }
    // 绕z轴旋转
    void
    RotateAroundZ(numberType angle) {
        modelMat = Transform::RotateAroundZ(angle);
    }
    // 绕任意轴轴旋转
    void
    RotateAroundN(numberType angle, Vector3 axis) {
        modelMat = Transform::RotateAroundN(angle, axis);
    }
public:
    void setFragmentShaderMethod(const std::string& method) {
        if (method == "phong_fragment_shader") {
            fragmentShader.setMethod(ShaderUtils::phong_fragment_shader);
        }
        else if (method == "normal_fragment_shader") {
            fragmentShader.setMethod(ShaderUtils::normal_fragment_shader);
        }
        else if (method == "texture_fragment_shader") {
            fragmentShader.setMethod(ShaderUtils::texture_fragment_shader);
        }
        else if (method == "bump_fragment_shader") {
            fragmentShader.setMethod(ShaderUtils::bump_fragment_shader);
        }
        else if (method == "displacement_fragment_shader") {
            fragmentShader.setMethod(ShaderUtils::displacement_fragment_shader);
        }
        else {
            throw std::runtime_error("fragment_shader_method type error");
        }
    }
};

}

#endif //ANYA_ENGINE_MODEL_HPP
