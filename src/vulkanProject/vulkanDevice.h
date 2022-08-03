#pragma once

struct VulkanSetupData;

void pickPhysicalDevice(VulkanSetupData *vulkanSetupData);
void createLogicalDevice(VulkanSetupData *vulkanSetupData);