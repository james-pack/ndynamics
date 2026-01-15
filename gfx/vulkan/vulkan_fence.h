#pragma once

#include <vulkan/vulkan.h>

#include "gfx/fence.h"

namespace ndyn::gfx::vulkan {

/**
 * VulkanFence wraps a VkFence for CPU-visible GPU work synchronization.
 */
class VulkanFence : public Fence {
 public:
  VulkanFence(VkDevice device);
  ~VulkanFence() override;

  void wait() override;
  bool is_complete() override;

  VkFence get_handle() const { return fence_; }

 private:
  VkDevice device_;
  VkFence fence_ = VK_NULL_HANDLE;
};

}  // namespace ndyn::gfx::vulkan
