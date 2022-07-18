#include <iostream>
#include <vector>
#include <vulkan/vulkan.h>

const std::vector<const char *> kValidationLayerNames = {"VK_LAYER_KHRONOS_validation"};
VkDebugUtilsMessengerEXT debugMessenger;

namespace {
VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
                                             VkDebugUtilsMessageTypeFlagsEXT messageTypes,
                                             const VkDebugUtilsMessengerCallbackDataEXT *pCallbackData,
                                             void *pUserData) {
  // if (messageSeverity >= VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT) {
  //  // Message is important enough to show
  //  std::cerr << "validation layer: " << pCallbackData->pMessage << std::endl;
  //}

  std::cerr << "validation layer: " << pCallbackData->pMessage << std::endl;
  return VK_FALSE;
}

VkResult createDebugUtilsMessengerEXT(VkInstance instance,
                                      const VkDebugUtilsMessengerCreateInfoEXT *pCreateInfo,
                                      const VkAllocationCallbacks *pAllocator,
                                      VkDebugUtilsMessengerEXT *pDebugMessenger) {
  auto func =
      (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
  if (func != nullptr) {
    return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
  } else {
    return VK_ERROR_EXTENSION_NOT_PRESENT;
  }
}

void destroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger,
                                   const VkAllocationCallbacks *pAllocator) {
  auto func =
      (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
  if (func != nullptr) {
    func(instance, debugMessenger, pAllocator);
  }
}

bool checkValidationLayerSupport() {
  uint32_t layerCount;
  vkEnumerateInstanceLayerProperties(&layerCount, nullptr);
  std::vector<VkLayerProperties> availableLayers(layerCount);
  vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

  for (const auto validationLayerName : kValidationLayerNames) {
    auto isValidationLayerSupported = false;
    for (const auto &availableLayer : availableLayers) {
      if (strcmp(validationLayerName, availableLayer.layerName) == 0) {
        isValidationLayerSupported = true;
        break;
      }
    }

    if (!isValidationLayerSupported)
      return false;
  }

  return true;
}

} // namespace

VkDebugUtilsMessengerCreateInfoEXT defaultVkDebugUtilsMessengerCreateInfoEXT() {
  VkDebugUtilsMessengerCreateInfoEXT createInfo = {};

  createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
  createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
                               VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
                               VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
  createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
                           VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
                           VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
  createInfo.pfnUserCallback = debugCallback;

  return createInfo;
};

void setupValidationLayers(VkInstanceCreateInfo *vkInstanceCreateInfo) {
  if (!checkValidationLayerSupport()) {
    throw std::runtime_error("Validation layers requested, but not available!");
  }

  vkInstanceCreateInfo->enabledLayerCount = uint32_t(kValidationLayerNames.size());
  vkInstanceCreateInfo->ppEnabledLayerNames = kValidationLayerNames.data();
}

void setupDebugMessenger(VkInstance *vkInstance) {
  VkDebugUtilsMessengerCreateInfoEXT createInfo = {};
  if (createDebugUtilsMessengerEXT(*vkInstance, &defaultVkDebugUtilsMessengerCreateInfoEXT(), nullptr,
                                   &debugMessenger) != VK_SUCCESS) {
    throw std::runtime_error("failed to set up debug messenger!");
  }
}

void cleanupDebugMessenger(VkInstance *vkInstance) {
  destroyDebugUtilsMessengerEXT(*vkInstance, debugMessenger, nullptr);
}

std::vector<const char *> getDebugExtensions() { return {VK_EXT_DEBUG_UTILS_EXTENSION_NAME}; }