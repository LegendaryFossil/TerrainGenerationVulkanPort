#include "vulkanSwapChain.h"

#include "windowDefs.h"
#include <algorithm>
#include <iostream>
#include <vector>

const std::vector<const char *> kDeviceExtensions = {VK_KHR_SWAPCHAIN_EXTENSION_NAME};

namespace {
VkSurfaceFormatKHR
chooseSwapChainSurfaceFormat(const std::vector<VkSurfaceFormatKHR> &availableSwapChainSurfaceFormats) {
  for (const auto &availableSwapChainSurfaceFormat : availableSwapChainSurfaceFormats) {
    if (availableSwapChainSurfaceFormat.format == VK_FORMAT_B8G8R8A8_SRGB &&
        availableSwapChainSurfaceFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
      return availableSwapChainSurfaceFormat;
    }
  }

  return availableSwapChainSurfaceFormats.front();
}

VkPresentModeKHR
chooseSwapChainPresentMode(const std::vector<VkPresentModeKHR> &availableSwapChainPresentModes) {
  for (const auto &availableSwapChainPresentMode : availableSwapChainPresentModes) {
    if (availableSwapChainPresentMode == VK_PRESENT_MODE_MAILBOX_KHR) {
      return availableSwapChainPresentMode;
    }
  }

  return VK_PRESENT_MODE_FIFO_KHR;
}

VkExtent2D chooseSwapChainExtent(const VkSurfaceCapabilitiesKHR &surfaceCapabilities,
                                 GLFWwindow *glfwWindow) {
  if (surfaceCapabilities.currentExtent.width != std::numeric_limits<uint32_t>::max()) {
    return surfaceCapabilities.currentExtent;
  } else {
    int frameBufferWidth, frameBufferHeight;
    glfwGetFramebufferSize(glfwWindow, &frameBufferWidth, &frameBufferHeight);

    VkExtent2D actualExtent = {uint32_t(frameBufferWidth), uint32_t(frameBufferHeight)};
    actualExtent.width = std::clamp(actualExtent.width, surfaceCapabilities.minImageExtent.width,
                                    surfaceCapabilities.maxImageExtent.width);
    actualExtent.height = std::clamp(actualExtent.height, surfaceCapabilities.minImageExtent.height,
                                     surfaceCapabilities.maxImageExtent.height);
    return actualExtent;
  }
}

void createImageViews(VulkanSetupData *vulkanSetupData) {
  vulkanSetupData->swapChainData.swapChainImageViews.resize(
      vulkanSetupData->swapChainData.swapChainImages.size());

  for (size_t i = 0; i < vulkanSetupData->swapChainData.swapChainImages.size(); ++i) {
    VkImageViewCreateInfo imageViewCreateInfo{};
    imageViewCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    imageViewCreateInfo.image = vulkanSetupData->swapChainData.swapChainImages[i];
    imageViewCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
    imageViewCreateInfo.format = vulkanSetupData->swapChainData.swapChainImageFormat;

    imageViewCreateInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
    imageViewCreateInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
    imageViewCreateInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
    imageViewCreateInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;

    imageViewCreateInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    imageViewCreateInfo.subresourceRange.baseMipLevel = 0;
    imageViewCreateInfo.subresourceRange.levelCount = 1;
    imageViewCreateInfo.subresourceRange.baseArrayLayer = 0;
    imageViewCreateInfo.subresourceRange.layerCount = 1;

    if (vkCreateImageView(vulkanSetupData->device, &imageViewCreateInfo, nullptr,
                          &vulkanSetupData->swapChainData.swapChainImageViews[i]) != VK_SUCCESS) {
      throw std::runtime_error("Failed to create image views!");
    }
  }
}
} // namespace

void createSwapChain(VulkanSetupData *vulkanSetupData, GLFWwindow *window) {
  const auto swapChainSupportDetails =
      querySwapChainSupport(vulkanSetupData->physicalDevice, vulkanSetupData->surface);

  VkSurfaceFormatKHR surfaceFormat = chooseSwapChainSurfaceFormat(swapChainSupportDetails.surfaceFormats);
  VkPresentModeKHR presentMode = chooseSwapChainPresentMode(swapChainSupportDetails.presentModes);
  VkExtent2D extent = chooseSwapChainExtent(swapChainSupportDetails.surfaceCapabilities, window);

  // Recommended to have at least one more than the minimum image account for swap chain
  auto imageCount = swapChainSupportDetails.surfaceCapabilities.minImageCount + 1;
  imageCount = std::clamp(imageCount, imageCount, swapChainSupportDetails.surfaceCapabilities.maxImageCount);

  VkSwapchainCreateInfoKHR swapChainCreateInfo = {};
  swapChainCreateInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
  swapChainCreateInfo.surface = vulkanSetupData->surface;
  swapChainCreateInfo.minImageCount = imageCount;
  swapChainCreateInfo.imageFormat = surfaceFormat.format;
  swapChainCreateInfo.imageColorSpace = surfaceFormat.colorSpace;
  swapChainCreateInfo.imageExtent = extent;
  swapChainCreateInfo.imageArrayLayers = 1;
  swapChainCreateInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

  const auto queueFamilyindices =
      findQueueFamilies(vulkanSetupData->physicalDevice, vulkanSetupData->surface);
  uint32_t queueFamilyIndices[] = {queueFamilyindices.graphicsFamily.value(),
                                   queueFamilyindices.presentFamily.value()};

  if (queueFamilyindices.graphicsFamily != queueFamilyindices.presentFamily) {
    swapChainCreateInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
    swapChainCreateInfo.queueFamilyIndexCount = 2;
    swapChainCreateInfo.pQueueFamilyIndices = queueFamilyIndices;
  } else {
    swapChainCreateInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
    swapChainCreateInfo.queueFamilyIndexCount = 0;     // Optional
    swapChainCreateInfo.pQueueFamilyIndices = nullptr; // Optional
  }

  swapChainCreateInfo.preTransform = swapChainSupportDetails.surfaceCapabilities.currentTransform;
  swapChainCreateInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
  swapChainCreateInfo.presentMode = presentMode;
  swapChainCreateInfo.clipped = VK_TRUE;
  swapChainCreateInfo.oldSwapchain = VK_NULL_HANDLE;

  if (vkCreateSwapchainKHR(vulkanSetupData->device, &swapChainCreateInfo, nullptr,
                           &vulkanSetupData->swapChainData.swapChain) != VK_SUCCESS) {
    throw std::runtime_error("failed to create swap chain!");
  }

  vkGetSwapchainImagesKHR(vulkanSetupData->device, vulkanSetupData->swapChainData.swapChain, &imageCount,
                          nullptr);
  vulkanSetupData->swapChainData.swapChainImages.resize(imageCount);
  vkGetSwapchainImagesKHR(vulkanSetupData->device, vulkanSetupData->swapChainData.swapChain, &imageCount,
                          vulkanSetupData->swapChainData.swapChainImages.data());

  vulkanSetupData->swapChainData.swapChainImageFormat = surfaceFormat.format;
  vulkanSetupData->swapChainData.swapChainExtent = extent;

  createImageViews(vulkanSetupData);
}
