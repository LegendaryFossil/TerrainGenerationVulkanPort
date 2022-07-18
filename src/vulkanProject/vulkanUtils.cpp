#include "vulkanUtils.h"
#include <assert.h>
#include <iostream>
#include <vector>
#include <vulkan/vulkan.h>

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

void createInstance(VulkanInstanceData *vulkanInstanceData) {
  assert(vulkanInstanceData != nullptr);

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

  if (!vulkanInstanceData->extensions.empty()) {
    setupExtensions(&vkInstanceCreateInfo, vulkanInstanceData->extensions);
  }

#ifndef NDEBUG
  setupValidationLayers(&vkInstanceCreateInfo);
#endif

  vkInstanceCreateInfo.pNext =
      (VkDebugUtilsMessengerCreateInfoEXT *)&defaultVkDebugUtilsMessengerCreateInfoEXT();
  if (vkCreateInstance(&vkInstanceCreateInfo, nullptr, &vulkanInstanceData->vkInstance) != VK_SUCCESS)
    throw std::runtime_error("Failed to create VkInstance");
}
} // namespace

void initVulkan(VulkanInstanceData *vulkanInstanceData) {
  createInstance(vulkanInstanceData);

#ifndef NDEBUG
  setupDebugMessenger(&vulkanInstanceData->vkInstance);
#endif
}

void cleanupVulkan(VulkanInstanceData *vulkanInstanceData) {
#ifndef NDEBUG
  cleanupDebugMessenger(&vulkanInstanceData->vkInstance);
#endif

  vkDestroyInstance(vulkanInstanceData->vkInstance, nullptr);
}