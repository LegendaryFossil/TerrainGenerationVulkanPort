#pragma once

#include <vector>
#include <vulkan/vulkan.h>

VkDebugUtilsMessengerCreateInfoEXT defaultVkDebugUtilsMessengerCreateInfoEXT();
void setupValidationLayers(VkInstanceCreateInfo *vkInstanceCreateInfo);
void setupDebugMessenger(VkInstance *vkInstance);
void cleanupDebugMessenger(VkInstance *vkInstance);
std::vector<const char *> getDebugExtensions();