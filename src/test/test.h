//
// Created by Anya on 2022/12/3.
//

#ifndef ANYAENGINE_TEST_H
#define ANYAENGINE_TEST_H

#include "math/vec.hpp"
#include "math/matrix.hpp"
#include <glad/glad.h>
#include <GLFW/glfw3.h>

void vecTest();
void matrixTest();
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow *window);
int testGlfw();

#endif //ANYAENGINE_TEST_H
