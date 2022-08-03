#include "vulkanDevice.h"

#include "vulkanUtils.h"
#include <assert.h>
#include <iostream>
#include <set>
#include <vector>

const std::vector<const char *> kDeviceExtensions = {VK_KHR_SWAPCHAIN_EXTENSION_NAME};

namespace {
bool checkPhysicalDeviceExtensionSupport(VkPhysicalDevice physicalDevice) {
  uint32_t deviceExtensionCount;
  vkEnumerateDeviceExtensionProperties(physicalDevice, nullptr, &deviceExtensionCount, nullptr);

  std::vector<VkExtensionProperties> availableExtensions(deviceExtensionCount);
  vkEnumerateDeviceExtensionProperties(physicalDevice, nullptr, &deviceExtensionCount,
                                       availableExtensions.data());

  for (const auto deviceExtension : kDeviceExtensions) {
    auto isDeviceExtensionSupported = false;
    for (const auto &availableExtension : availableExtensions) {
      if (strcmp(deviceExtension, availableExtension.extensionName) == 0) {
        isDeviceExtensionSupported = true;
        break;
      }
    }

    if (!isDeviceExtensionSupported)
      return false;
  }

  return true;
}

bool isPhysicalDeviceSuitable(const VkPhysicalDevice physicalDevice, const VkSurfaceKHR surface) {
  const auto queueFamilyIndices = findQueueFamilies(physicalDevice, surface);

  bool extensionsSupported = checkPhysicalDeviceExtensionSupport(physicalDevice);

  auto isSwapChainSupported = false;
  if (extensionsSupported) {
    const auto swapChainSupportDetails = querySwapChainSupport(physicalDevice, surface);
    isSwapChainSupported =
        !swapChainSupportDetails.surfaceFormats.empty() && !swapChainSupportDetails.presentModes.empty();
  }

  return queueFamilyIndices.graphicsFamily.has_value() && queueFamilyIndices.presentFamily.has_value() &&
         isSwapChainSupported;
}

} // namespace

void pickPhysicalDevice(VulkanSetupData *vulkanSetupData) {
  uint32_t physicalDeviceCount = 0;
  vkEnumeratePhysicalDevices(vulkanSetupData->instance, &physicalDeviceCount, nullptr);

  if (physicalDeviceCount == 0) {
    throw std::runtime_error("Failed to find GPUs with Vulkan support!");
  }

  std::vector<VkPhysicalDevice> physicalDevices(physicalDeviceCount);
  vkEnumeratePhysicalDevices(vulkanSetupData->instance, &physicalDeviceCount, physicalDevices.data());

  for (const auto &physicalDevice : physicalDevices) {
    if (isPhysicalDeviceSuitable(physicalDevice, vulkanSetupData->surface)) {
      vulkanSetupData->physicalDevice = physicalDevice;
      break;
    }
  }

  if (vulkanSetupData->physicalDevice == VK_NULL_HANDLE) {
    throw std::runtime_error("Failed to find a suitable GPU!");
  }
}

void createLogicalDevice(VulkanSetupData *vulkanSetupData) {
  const auto queueFamilyIndices =
      findQueueFamilies(vulkanSetupData->physicalDevice, vulkanSetupData->surface);

  std::vector<VkDeviceQueueCreateInfo> vkDeviceQueueCreateInfos;
  std::set<uint32_t> uniqueQueueFamilies = {queueFamilyIndices.graphicsFamily.value(),
                                            queueFamilyIndices.presentFamily.value()};
  const auto queuePriority = 1.0f;
  for (const auto &uniqueQueueFamily : uniqueQueueFamilies) {
    VkDeviceQueueCreateInfo vkDeviceQueueCreateInfo = {};
    vkDeviceQueueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    vkDeviceQueueCreateInfo.queueFamilyIndex = uniqueQueueFamily;
    vkDeviceQueueCreateInfo.queueCount = 1;
    vkDeviceQueueCreateInfo.pQueuePriorities = &queuePriority;
    vkDeviceQueueCreateInfos.push_back(vkDeviceQueueCreateInfo);
  }

  VkPhysicalDeviceFeatures vkPhysicalDeviceFeatures = {};

  VkDeviceCreateInfo vkDeviceCreateInfo = {};
  vkDeviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
  vkDeviceCreateInfo.pQueueCreateInfos = vkDeviceQueueCreateInfos.data();
  vkDeviceCreateInfo.queueCreateInfoCount = uint32_t(vkDeviceQueueCreateInfos.size());
  vkDeviceCreateInfo.pEnabledFeatures = &vkPhysicalDeviceFeatures;
  vkDeviceCreateInfo.enabledExtensionCount = uint32_t(kDeviceExtensions.size());
  vkDeviceCreateInfo.ppEnabledExtensionNames = kDeviceExtensions.data();

  if (vkCreateDevice(vulkanSetupData->physicalDevice, &vkDeviceCreateInfo, nullptr,
                     &vulkanSetupData->device) != VK_SUCCESS) {
    throw std::runtime_error("ailed to create logical device!");
  }

  // Get queue handle to interact with it
  vkGetDeviceQueue(vulkanSetupData->device, queueFamilyIndices.graphicsFamily.value(), 0,
                   &vulkanSetupData->graphicsQueue);
  vkGetDeviceQueue(vulkanSetupData->device, queueFamilyIndices.presentFamily.value(), 0,
                   &vulkanSetupData->presentQueue);
}
