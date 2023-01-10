# AnyaRenderer
- ```AnyaRenderer``` 是一个迷你的软渲染器，目前实现了光栅化的功能，基于 C++20 开发。
- 实现了矩阵向量库，线性插值，z-buffer 深度检测， MSAA 抗锯齿等功能。

## Development environment
- Clion + MinGW w64 9.0
- Windows

## Build
- CMake VERSION 3.20

## Dependent
- GLFW
- nlohmann
- stb_image

## Controls
- Axis  : ```X``` ```Y``` ```Z``` 选择旋转轴。
- Rotate: 按下```A```逆时针旋转10°，按下```D```顺时针旋转10°。
- Save: 按下```Enter```保存当前模型姿态为图片。

## Screenshots
| Scene                                           | Description |
|-------------------------------------------------|-------------|
| <img src="art/images/triangle.png" width="600"> | `Triangle`  |
| <img src="art/images/MSAA.jpg" width="600">     | `MSAA`      |
| <img src="art/images/cow.png" width="600">      | `Cow`       |
| <img src="art/images/elfgirl.png" width="600">  | `Elfgirl`   |
| <img src="art/images/azura.png" width="600">    | `Azura`     |
| <img src="art/images/diablo3.png" width="600">  | `Diablo3`   |
| <img src="art/images/boggie.png" width="600">   | `Boggie`    |
| <img src="art/images/robot.png" width="600">    | `Robot`     |
| <img src="art/images/assassin.png" width="600"> | `Assassin`  |
| <img src="art/images/marry.png" width="600">    | `Marry`     |
| <img src="art/images/kgirl.png" width="600">    | `Kgirl`     |

## Feature
- [x] 数学
    - [x] 向量运算
    - [x] 矩阵运算
    - [x] 工具函数
- [ ] 图元
    - [ ] 球体
    - [ ] 矩形
    - [x] 三角形
    - [ ] 模型
- [x] 纹理
    - [x] 单色纹理
    - [x] 图片纹理
    - [x] 凹凸纹理
    - [x] Bilinear双线性插值
- [x] 渲染
    - [x] 光栅化
    - [ ] 光线追踪
    - [x] 着色器
    - [x] MSAA抗锯齿
- [ ] 加速结构
    - [ ] AABB包围盒
    - [ ] 层次包围盒

