#pragma once

#include "vulkan/vulkan.h"
#include <vector>

struct VulkanInstanceData {
  VkInstance vkInstance;
  std::vector<const char *> extensions;
};

void initVulkan(VulkanInstanceData *vulkanInstanceData);
void cleanupVulkan(VulkanInstanceData *vulkanInstanceData);
