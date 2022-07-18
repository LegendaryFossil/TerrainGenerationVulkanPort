#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include "vulkanUtils.h"
#include "windowDefs.h"
#include <iostream>

#ifndef NDEBUG
#include "vulkanDebugUtils.h"
#endif

WindowData windowData = {};
VulkanSetupData vulkanSetupData = {};

static std::vector<const char *> getRequiredExtensions() {
  uint32_t glfwExtensionCount = 0;
  const char **glfwRequiredInstanceExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
  std::vector<const char *> requiredExtensions(glfwRequiredInstanceExtensions,
                                               glfwRequiredInstanceExtensions + glfwExtensionCount);

#ifndef NDEBUG
  const auto debugExtensions = getDebugExtensions();
  for (const auto debugExtension : debugExtensions) {
    requiredExtensions.push_back(debugExtension);
  }
#endif

  return requiredExtensions;
}

void initWindow() {
  glfwInit();

  glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
  glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

  windowData.window =
      GLFWwindowUniquePtr(glfwCreateWindow(kWindowWidth, kWindowHeight, "Vulkan Project", nullptr, nullptr));
  windowData.width = kWindowWidth;
  windowData.height = kWindowHeight;
  windowData.center = glm::vec2(kWindowWidth * 0.5f, kWindowHeight * 0.5f);
}

void mainLoop() {
  while (!glfwWindowShouldClose(windowData.window.get())) {
    glfwPollEvents();
  }
}

void cleanup() {
  cleanupVulkan(&vulkanSetupData);
  glfwDestroyWindow(windowData.window.get());
  glfwTerminate();
}

void runApplication() {
  initWindow();

  vulkanSetupData.extensions = getRequiredExtensions();

  initVulkan(&vulkanSetupData);

  mainLoop();
  cleanup();
}

int main() {
  try {
    runApplication();
  } catch (const std::exception &e) {
    std::cerr << e.what() << std::endl;
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}