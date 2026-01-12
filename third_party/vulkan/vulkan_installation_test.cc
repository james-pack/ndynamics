#include <vulkan/vulkan.h>

#include <vector>

#include "gtest/gtest.h"

TEST(VulkanInstalledTest, IsInstalled) {
  VkApplicationInfo app_info{};
  app_info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
  app_info.pApplicationName = "VulkanHeadlessTest";
  app_info.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
  app_info.pEngineName = "None";
  app_info.engineVersion = VK_MAKE_VERSION(1, 0, 0);
  app_info.apiVersion = VK_API_VERSION_1_2;

  VkInstanceCreateInfo create_info{};
  create_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
  create_info.pApplicationInfo = &app_info;
  create_info.enabledLayerCount = 0;
  create_info.ppEnabledLayerNames = nullptr;
  create_info.enabledExtensionCount = 0;
  create_info.ppEnabledExtensionNames = nullptr;

  VkInstance instance;
  VkResult result = vkCreateInstance(&create_info, nullptr, &instance);

  // Two acceptable outcomes:
  // 1. VK_SUCCESS -> Vulkan loader and driver available
  // 2. VK_ERROR_INCOMPATIBLE_DRIVER -> headless server without GPU
  EXPECT_TRUE(result == VK_SUCCESS || result == VK_ERROR_INCOMPATIBLE_DRIVER);

  if (result == VK_SUCCESS) {
    vkDestroyInstance(instance, nullptr);
  }
}

TEST(VulkanInstalledTest, HasComputeCapableDevice) {
  VkApplicationInfo app_info{};
  app_info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
  app_info.pApplicationName = "VulkanComputeTest";
  app_info.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
  app_info.pEngineName = "None";
  app_info.engineVersion = VK_MAKE_VERSION(1, 0, 0);
  app_info.apiVersion = VK_API_VERSION_1_2;

  VkInstanceCreateInfo create_info{};
  create_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
  create_info.pApplicationInfo = &app_info;

  VkInstance instance = VK_NULL_HANDLE;
  VkResult result = vkCreateInstance(&create_info, nullptr, &instance);
  ASSERT_EQ(VK_SUCCESS, result) << "No Vulkan-compatible driver found.";

  uint32_t device_count = 0;
  result = vkEnumeratePhysicalDevices(instance, &device_count, nullptr);
  EXPECT_EQ(VK_SUCCESS, result) << "Could not enumerate physical devices.";
  EXPECT_GT(device_count, 0u) << "No devices (GPUs) found.";

  if (device_count > 0) {
    std::vector<VkPhysicalDevice> devices(device_count);
    result = vkEnumeratePhysicalDevices(instance, &device_count, devices.data());
    EXPECT_EQ(result, VK_SUCCESS);

    bool found_compute_device = false;
    for (VkPhysicalDevice device : devices) {
      uint32_t queue_family_count = 0;
      vkGetPhysicalDeviceQueueFamilyProperties(device, &queue_family_count, nullptr);
      std::vector<VkQueueFamilyProperties> queue_families(queue_family_count);
      vkGetPhysicalDeviceQueueFamilyProperties(device, &queue_family_count, queue_families.data());

      for (const auto& qf : queue_families) {
        if (qf.queueFlags & VK_QUEUE_COMPUTE_BIT) {
          found_compute_device = true;
          break;
        }
      }
      if (found_compute_device) break;
    }

    EXPECT_TRUE(found_compute_device) << "No Vulkan device with compute capability found.";
  }

  vkDestroyInstance(instance, nullptr);
}
