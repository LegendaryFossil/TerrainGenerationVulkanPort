#include "vulkanUtils.h"
#include <assert.h>
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
  if (vkCreateInstance(&vkInstanceCreateInfo, nullptr, &vulkanSetupData->vkInstance) != VK_SUCCESS)
    throw std::runtime_error("Failed to create VkInstance");
}

QueueFamilyIndices findQueueFamilies(VkPhysicalDevice physicalDevice) {
  QueueFamilyIndices queueFamilyIndices;

  uint32_t queueFamilyCount = 0;
  vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, nullptr);
  std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
  vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, queueFamilies.data());

  for (size_t i = 0; i < queueFamilies.size(); ++i) {
    if (queueFamilies[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) {
      queueFamilyIndices.graphicsFamily = uint32_t(i);
      break;
    }
  }

  return queueFamilyIndices;
}

bool isPhysicalDeviceSuitable(VkPhysicalDevice physicalDevice) {
  const auto queueFamilyIndices = findQueueFamilies(physicalDevice);
  return queueFamilyIndices.graphicsFamily.has_value();
}

void pickPhysicalDevice(VulkanSetupData *vulkanSetupData) {
  uint32_t physicalDeviceCount = 0;
  vkEnumeratePhysicalDevices(vulkanSetupData->vkInstance, &physicalDeviceCount, nullptr);

  if (physicalDeviceCount == 0) {
    throw std::runtime_error("Failed to find GPUs with Vulkan support!");
  }

  std::vector<VkPhysicalDevice> physicalDevices(physicalDeviceCount);
  vkEnumeratePhysicalDevices(vulkanSetupData->vkInstance, &physicalDeviceCount, physicalDevices.data());

  for (const auto &physicalDevice : physicalDevices) {
    if (isPhysicalDeviceSuitable(physicalDevice)) {
      vulkanSetupData->vkPhysicalDevice = physicalDevice;
      break;
    }
  }

  if (vulkanSetupData->vkPhysicalDevice == VK_NULL_HANDLE) {
    throw std::runtime_error("Failed to find a suitable GPU!");
  }
}

} // namespace

void initVulkan(VulkanSetupData *vulkanSetupData) {
  assert(vulkanSetupData != nullptr);

  createInstance(vulkanSetupData);

#ifndef NDEBUG
  setupDebugMessenger(&vulkanSetupData->vkInstance);
#endif

  pickPhysicalDevice(vulkanSetupData);
}

void cleanupVulkan(VulkanSetupData *vulkanSetupData) {
  assert(vulkanSetupData != nullptr);

#ifndef NDEBUG
  cleanupDebugMessenger(&vulkanSetupData->vkInstance);
#endif

  vkDestroyInstance(vulkanSetupData->vkInstance, nullptr);
}