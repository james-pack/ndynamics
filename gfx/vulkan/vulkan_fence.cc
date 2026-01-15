#include "gfx/vulkan/vulkan_fence.h"

#include <stdexcept>

namespace ndyn::gfx::vulkan {

VulkanFence::VulkanFence(VkDevice device) : device_(device) {
  VkFenceCreateInfo fence_info{};
  fence_info.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
  fence_info.flags = 0;

  if (vkCreateFence(device_, &fence_info, nullptr, &fence_) != VK_SUCCESS) {
    throw std::runtime_error("failed to create Vulkan fence");
  }
}

VulkanFence::~VulkanFence() {
  if (fence_ != VK_NULL_HANDLE) vkDestroyFence(device_, fence_, nullptr);
}

void VulkanFence::wait() { vkWaitForFences(device_, 1, &fence_, VK_TRUE, UINT64_MAX); }

bool VulkanFence::is_complete() {
  VkResult res = vkGetFenceStatus(device_, fence_);
  return res == VK_SUCCESS;
}

}  // namespace ndyn::gfx::vulkan
