#pragma once

#include "vulkan/vulkan.h"
#include <optional>
#include <vector>

struct GLFWwindow;

struct SwapChainSupportDetails {
  VkSurfaceCapabilitiesKHR surfaceCapabilities;
  std::vector<VkSurfaceFormatKHR> surfaceFormats;
  std::vector<VkPresentModeKHR> presentModes;
};

struct QueueFamilyIndices {
  std::optional<uint32_t> graphicsFamily;
  std::optional<uint32_t> presentFamily;
};

struct VulkanSetupData {
  VkInstance instance = nullptr;                    // Instance to vulkan library
  VkPhysicalDevice physicalDevice = VK_NULL_HANDLE; // Graphic card
  VkDevice device = VK_NULL_HANDLE;                 // Logical device
  VkQueue graphicsQueue = VK_NULL_HANDLE;           // Graphics queue from graphic queue family
  VkQueue presentQueue = VK_NULL_HANDLE;
  VkSurfaceKHR surface;

  struct {
    VkSwapchainKHR swapChain;
    std::vector<VkImage> swapChainImages;
    std::vector<VkImageView> swapChainImageViews;
    VkFormat swapChainImageFormat;
    VkExtent2D swapChainExtent;
  } swapChainData;

  std::vector<const char *> extensions;
};

SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice device, const VkSurfaceKHR surface);
QueueFamilyIndices findQueueFamilies(VkPhysicalDevice physicalDevice, VkSurfaceKHR surface);
void initVulkan(VulkanSetupData *vulkanSetupData, GLFWwindow *window);
void cleanupVulkan(VulkanSetupData *vulkanSetupData);
