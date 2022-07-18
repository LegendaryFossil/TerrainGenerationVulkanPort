#pragma once

#include "vulkan/vulkan.h"
#include <vector>
#include <optional>

struct VulkanSetupData {
  VkInstance vkInstance = nullptr; // Instance to vulkan library
  VkPhysicalDevice vkPhysicalDevice = VK_NULL_HANDLE; // Graphic card
  std::vector<const char *> extensions;
};

struct QueueFamilyIndices {
  std::optional<uint32_t> graphicsFamily;
};

void initVulkan(VulkanSetupData *vulkanSetupData);
void cleanupVulkan(VulkanSetupData *vulkanSetupData);
