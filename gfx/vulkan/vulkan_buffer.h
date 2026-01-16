#pragma once

#include <vulkan/vulkan.h>

#include <cstddef>
#include <stdexcept>

#include "gfx/buffer.h"

namespace ndyn::gfx::vulkan {

/**
 * VulkanBuffer wraps a VkBuffer and its associated device memory.
 * RAII ensures that the buffer and memory are freed on destruction.
 */
class VulkanBuffer final : public Buffer {
 private:
  VkBuffer buffer_{VK_NULL_HANDLE};
  std::size_t size_;
  BufferUsage usage_;
  VkDevice device_;
  VkDeviceMemory memory_{VK_NULL_HANDLE};

 public:
  VulkanBuffer(VkDevice device, std::size_t size_bytes, BufferUsage usage);
  ~VulkanBuffer() override;

  /**
   * Returns the underlying Vulkan buffer handle.
   */
  VkBuffer get_buffer() const { return buffer_; }

  /**
   * Returns the size of the buffer in bytes.
   */
  std::size_t size() const override { return size_; }

  /**
   * Returns the buffer usage.
   */
  BufferUsage usage() const override { return usage_; }

  /**
   * Uploads data into the buffer using a staging buffer or host-visible memory.
   * This method blocks until the copy is complete.
   */
  void upload(const void* data, std::size_t bytes, std::size_t offset = 0) override;
};

}  // namespace ndyn::gfx::vulkan
