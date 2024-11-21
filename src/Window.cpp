#include "Window.hpp"
#include "GLFW/glfw3.h"
#include <stdexcept>

void keyCallback(GLFWwindow* window, int32_t key, int32_t scancode,
                 int32_t action, int32_t mods);

Window::Window(void) : width(720), height(480), _title("Raycast") {
    if (glfwInit() == GLFW_FALSE)
        throw std::runtime_error("glfwInit failed");

    window = glfwCreateWindow(width, height, _title, nullptr, nullptr);
    if (window == nullptr)
        throw std::runtime_error("glfwCreateWindow failed");

    glfwMakeContextCurrent(window);
    glOrtho(0, width, height, 0, -1, 1);
    glfwSetWindowSizeLimits(window, width, height, width, height);
    glfwSetKeyCallback(window, keyCallback);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
}

Window::~Window() {
    glfwDestroyWindow(window);
    glfwTerminate();
}

bool Window::shouldClose(void) const { return glfwWindowShouldClose(window); }

void Window::swapBuffers(void) const { glfwSwapBuffers(window); }

void Window::getCursorPos(double* pos_x, double* pos_y) const {
    glfwGetCursorPos(window, pos_x, pos_y);
}

void Window::centerCursor(void) const {
    glfwSetCursorPos(window, width / 2.0, height / 2.0);
}
