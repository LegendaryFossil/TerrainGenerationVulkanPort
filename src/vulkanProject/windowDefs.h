#pragma once

#include "glm/glm.hpp"
#include <GLFW/glfw3.h>
#include <functional>
#include <memory>

constexpr auto kWindowWidth = 1920;
constexpr auto kWindowHeight = 1280;

struct DestroyWindow {
  void operator()(GLFWwindow *ptr) { glfwDestroyWindow(ptr); }
};

typedef std::unique_ptr<GLFWwindow, DestroyWindow> GLFWwindowUniquePtr;

struct WindowData {
  GLFWwindowUniquePtr window;
  int width;
  int height;
  glm::dvec2 center;
};