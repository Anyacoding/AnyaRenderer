# AnyaRenderer
- ```AnyaRenderer``` 是一个迷你的软渲染器，目前实现了光栅化和 Whitted Style 风格的光线追踪的功能，基于 C++20 开发。
- 实现了矩阵向量库，线性插值，z-buffer 深度检测， MSAA 抗锯齿等功能。

## Development Environment
- Clion + MinGW w64 9.0
- Windows

## Build System
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
### Models
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

### Environments
| Scene                                                | Description     |
|------------------------------------------------------|-----------------|
| <img src="art/images/whitted_style.png" width="600"> | `Whitted Style` |


## Feature
- [x] 数学
    - [x] 向量运算
    - [x] 矩阵运算
    - [x] 工具函数
- [x] 图元
    - [x] 球体
    - [ ] 矩形
    - [x] 三角形
    - [x] 模型
- [x] 纹理
    - [x] 单色纹理
    - [x] 图片纹理
    - [x] 凹凸纹理
    - [x] Bilinear双线性插值
- [x] 渲染
    - [x] 光栅化
    - [x] 光线追踪
    - [x] 着色器
    - [x] MSAA抗锯齿
- [ ] 加速结构
    - [ ] AABB包围盒
    - [ ] 层次包围盒

## Modules
```
AnyaRenderer
├── art             // 美术资源
│   ├── context     // 上下文配置文件
│   ├── models      // 模型文件 
│   └── images      // 输出图片
│
├── dependent       // 第三方库
│   ├── include     // 引入的头文件
│   ├── lib         // 需要链接的静态库和动态库文件
│   └── src         // 引入的源码
│
├── src             // 项目源码
├── .gitattributes  // git项目属性
├── .gitignore      // git忽略文件
├── .clang-format   // 格式控制文件
├── CMakeLists.txt  // 项目构建配置
└── README.md       // 介绍文件
    
```
### Source Code Module
```
src
├── engine
│   ├── component               // 场景组件
│   │   ├── camera.hpp          // 摄像机
│   │   ├── light.hpp           // 光源
│   │   ├── object              // 图元
│   │   │   └── triangle.hpp    // 三角形
│   │   └── scene.hpp           // 场景类
│   │
│   ├── interface               // 接口
│   │   └── renderer.hpp        // 渲染器接口
│   │ 
│   ├── load                    // 资源载入
│   │   ├── context.hpp         // 上下文载入
│   │   ├── model.hpp           // 模型载入
│   │   └── texture.hpp         // 图像纹理载入
│   │
│   ├── renderer                // 渲染器
│   │   └── rasterizer.hpp      // 光栅化器
│   │
│   ├── shader                  // 着色器
│   │   ├── fragment_shader.hpp // 片元着色器
│   │   ├── vertex_shader.hpp   // 顶点着色器
│   │   └── methods.hpp         // 着色方法
│   │
│   └── tool                    // 工具
│       ├── matrix.hpp          // 矩阵类
│       ├── vec.hpp             // 向量类
│       └── utils.hpp           // 常用工具函数
│ 
├── ui                          // 图形界面
│   └── gui.hpp                 // 负责展示实时渲染效果和实现交互
├── test                        // 测试
│   ├── test.cpp                // 测试单元
│   └── test.h                  // 测试头文件
└── main.cpp                    // 入口文件
```

