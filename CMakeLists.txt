cmake_minimum_required(VERSION 3.20)

project(AnyaEngine)

# C++标准
set(CMAKE_CXX_STANDARD 20)

# 头文件目录
include_directories(src/engine depend/include)

# 第三方库目录
link_directories(depend/lib)

# 创建目标
add_executable(main)

# 递归搜索文件并自动更新
file(GLOB_RECURSE source CONFIGURE_DEPENDS src/*.cpp src/*.c)

# 添加源文件
target_sources(main PRIVATE ${source})

# 链接第三方库
target_link_libraries(main glfw3)
