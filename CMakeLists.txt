cmake_minimum_required(VERSION 3.22)
project(AnyaEngine)
# C++标准
set(CMAKE_CXX_STANDARD 20)


# 创建目标
add_executable(main src/math/matrix.hpp)
# 递归搜索文件并自动更新
file(GLOB_RECURSE source CONFIGURE_DEPENDS src/*.cpp src/*.hpp)
# 添加源文件
target_sources(main PRIVATE ${source})