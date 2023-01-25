cmake_minimum_required(VERSION 3.20)

project(AnyaRenderer)

# C++标准
set(CMAKE_CXX_STANDARD 20)

## 添加OpenMP选项
FIND_PACKAGE(OpenMP REQUIRED)
if (OPENMP_FOUND)
    message("OPENMP FOUND")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${OpenMP_EXE_LINKER_FLAGS}")
endif ()

# 头文件目录
include_directories(src/engine dependent/include)

# 第三方库目录
link_directories(dependent/lib)

# 创建目标
add_executable(main)

# 递归搜索文件并自动更新
file(GLOB_RECURSE source CONFIGURE_DEPENDS src/*.cpp src/*.c src/*.hpp dependent/src/*.cpp dependent/src/*.c)

# 添加源文件
target_sources(main PRIVATE ${source})

# 链接第三方库
target_link_libraries(main opengl32 glfw3)

# 使用Release版本
SET(CMAKE_BUILD_TYPE "Release")

