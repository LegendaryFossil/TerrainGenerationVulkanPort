#pragma once

#include <vector>
#include <vulkan/vulkan.h>

VkDebugUtilsMessengerCreateInfoEXT defaultVkDebugUtilsMessengerCreateInfoEXT();
void setupValidationLayers(VkInstanceCreateInfo *vkInstanceCreateInfo);
void setupDebugMessenger(VkInstance *instance);
void cleanupDebugMessenger(VkInstance *instance);
std::vector<const char *> getDebugExtensions();