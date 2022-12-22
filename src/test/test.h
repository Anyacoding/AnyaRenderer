//
// Created by Anya on 2022/12/3.
//

#ifndef ANYA_ENGINE_TEST_H
#define ANYA_ENGINE_TEST_H

#include "tool/vec.hpp"
#include "tool/matrix.hpp"
#include <GLFW/glfw3.h>

void vecTest();
void matrixTest();
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow *window);
int testGlfw();

#endif //ANYA_ENGINE_TEST_H
