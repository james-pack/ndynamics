#pragma once

#include <vulkan/vulkan.h>

#include "gfx/command_buffer.h"

namespace ndyn::gfx::vulkan {

/**
 * VulkanCommandBuffer wraps a VkCommandBuffer allocated from a command pool.
 */
class VulkanCommandBuffer : public CommandBuffer {
 private:
  VkDevice device_;
  VkCommandPool pool_;
  VkCommandBuffer command_buffer_{VK_NULL_HANDLE};

 public:
  VulkanCommandBuffer(VkDevice device, VkCommandPool pool);
  ~VulkanCommandBuffer() override;

  void begin() override;
  void end() override;

  VkCommandBuffer get_command_buffer() const { return command_buffer_; }
};

}  // namespace ndyn::gfx::vulkan
