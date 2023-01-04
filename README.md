# AnyaRenderer
- ```AnyaRenderer``` 是一个迷你的软渲染器，目前实现了光栅化的功能，基于 C++20 开发.
- 实现了矩阵向量库，线性插值，z-buffe 深度检测， MSAA 抗锯齿等功能.

## 编译环境
- MinGW w64 9.0
- Windows

## 构建系统
- CMake

## 第三方库
- GLFW
- nlohmann
- stb_image

## DEMO
### 光栅化
![](https://anya-1308928365.cos.ap-nanjing.myqcloud.com/blog/QQ图片20221221002002.png)
<p align="center">
<strong>
深度检测
</strong>
</p>

![](https://anya-1308928365.cos.ap-nanjing.myqcloud.com/blog/Cache_-2ff3c56be51f4525..jpg)
<p align="center">
<strong>
MSAA效果对比
</strong>
</p>