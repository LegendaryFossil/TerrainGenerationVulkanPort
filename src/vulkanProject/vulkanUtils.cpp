#include "vulkanUtils.h"

#include "vulkanDevice.h"
#include "vulkanSwapChain.h"
#include "windowDefs.h"
#include <iostream>

#ifndef NDEBUG
#include "vulkanDebugUtils.h"
#endif

namespace {
bool checkExtensionsSupport(const std::vector<const char *> &requiredExtensions) {
  uint32_t supportedExtensionCount = 0;
  vkEnumerateInstanceExtensionProperties(nullptr, &supportedExtensionCount, nullptr);
  std::vector<VkExtensionProperties> supportedExtensions(supportedExtensionCount);
  vkEnumerateInstanceExtensionProperties(nullptr, &supportedExtensionCount, supportedExtensions.data());

  std::cout << "Available extensions:\n";
  for (const auto &supportedExtension : supportedExtensions) {
    std::cout << '\t' << supportedExtension.extensionName << '\n';
  }

  for (const auto requiredExtension : requiredExtensions) {
    auto isExtensionSupported = false;
    for (const auto &supportedExtension : supportedExtensions) {
      if (strcmp(requiredExtension, supportedExtension.extensionName) == 0) {
        isExtensionSupported = true;
        break;
      }
    }

    if (!isExtensionSupported)
      return false;
  }

  return true;
}

void setupExtensions(VkInstanceCreateInfo *vkInstanceCreateInfo,
                     const std::vector<const char *> &extensions) {
  vkInstanceCreateInfo->ppEnabledExtensionNames = extensions.data();
  vkInstanceCreateInfo->enabledExtensionCount = uint32_t(extensions.size());

  if (!checkExtensionsSupport(extensions)) {
    throw std::runtime_error("Failed to create VkInstance");
  }
}

void createSurface(VulkanSetupData *vulkanSetupData, GLFWwindow *window) {
  if (glfwCreateWindowSurface(vulkanSetupData->instance, window, nullptr, &vulkanSetupData->surface) !=
      VK_SUCCESS) {
    throw std::runtime_error("Failed to create window surface!");
  }
}

void createInstance(VulkanSetupData *vulkanSetupData) {
  VkApplicationInfo vkApplicationInfo = {};
  vkApplicationInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
  vkApplicationInfo.pApplicationName = "Hello Triangle";
  vkApplicationInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
  vkApplicationInfo.pEngineName = "No Engine";
  vkApplicationInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
  vkApplicationInfo.apiVersion = VK_API_VERSION_1_0;

  VkInstanceCreateInfo vkInstanceCreateInfo{};
  vkInstanceCreateInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
  vkInstanceCreateInfo.pApplicationInfo = &vkApplicationInfo;

  if (!vulkanSetupData->extensions.empty()) {
    setupExtensions(&vkInstanceCreateInfo, vulkanSetupData->extensions);
  }

#ifndef NDEBUG
  setupValidationLayers(&vkInstanceCreateInfo);
#endif

  vkInstanceCreateInfo.pNext =
      (VkDebugUtilsMessengerCreateInfoEXT *)&defaultVkDebugUtilsMessengerCreateInfoEXT();
  if (vkCreateInstance(&vkInstanceCreateInfo, nullptr, &vulkanSetupData->instance) != VK_SUCCESS)
    throw std::runtime_error("Failed to create VkInstance");
}

void createGraphicsPipeline() {

}

} // namespace

SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice device, const VkSurfaceKHR surface) {
  SwapChainSupportDetails swapChainSupportDetails = {};

  vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, surface, &swapChainSupportDetails.surfaceCapabilities);

  uint32_t formatCount;
  vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, nullptr);
  if (formatCount != 0) {
    swapChainSupportDetails.surfaceFormats.resize(formatCount);
    vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount,
                                         swapChainSupportDetails.surfaceFormats.data());
  }

  uint32_t presentModeCount;
  vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount, nullptr);
  if (presentModeCount != 0) {
    swapChainSupportDetails.presentModes.resize(presentModeCount);
    vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount,
                                              swapChainSupportDetails.presentModes.data());
  }

  return swapChainSupportDetails;
}

QueueFamilyIndices findQueueFamilies(VkPhysicalDevice physicalDevice, VkSurfaceKHR surface) {
  QueueFamilyIndices queueFamilyIndices;

  uint32_t queueFamilyCount = 0;
  vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, nullptr);
  std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
  vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, queueFamilies.data());

  for (size_t i = 0; i < queueFamilies.size(); ++i) {
    if (queueFamilies[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) {
      queueFamilyIndices.graphicsFamily = uint32_t(i);
    }

    VkBool32 presentSupport = false;
    vkGetPhysicalDeviceSurfaceSupportKHR(physicalDevice, uint32_t(i), surface, &presentSupport);

    if (presentSupport) {
      queueFamilyIndices.presentFamily = uint32_t(i);
    }

    if (queueFamilyIndices.graphicsFamily.has_value() && queueFamilyIndices.presentFamily.has_value()) {
      break;
    }
  }

  return queueFamilyIndices;
}

void initVulkan(VulkanSetupData *vulkanSetupData, GLFWwindow *window) {
  assert(vulkanSetupData != nullptr);

  createInstance(vulkanSetupData);
#ifndef NDEBUG
  setupDebugMessenger(&vulkanSetupData->instance);
#endif
  createSurface(vulkanSetupData, window);
  pickPhysicalDevice(vulkanSetupData);
  createLogicalDevice(vulkanSetupData);
  createSwapChain(vulkanSetupData, window);
}

void cleanupVulkan(VulkanSetupData *vulkanSetupData) {
  assert(vulkanSetupData != nullptr);

#ifndef NDEBUG
  cleanupDebugMessenger(&vulkanSetupData->instance);
#endif

  for (auto imageView : vulkanSetupData->swapChainData.swapChainImageViews) {
    vkDestroyImageView(vulkanSetupData->device, imageView, nullptr);
  }

  vkDestroySwapchainKHR(vulkanSetupData->device, vulkanSetupData->swapChainData.swapChain, nullptr);
  vkDestroyDevice(vulkanSetupData->device, nullptr);
  vkDestroySurfaceKHR(vulkanSetupData->instance, vulkanSetupData->surface, nullptr);
  vkDestroyInstance(vulkanSetupData->instance, nullptr);
}