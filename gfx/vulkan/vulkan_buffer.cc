#include "gfx/vulkan/vulkan_buffer.h"

#include <cstring>
#include <stdexcept>

#include "gfx/buffer.h"

namespace ndyn::gfx::vulkan {

VulkanBuffer::VulkanBuffer(VkDevice device, std::size_t size_bytes, BufferUsage usage)
    : device_(device), size_(size_bytes), usage_(usage) {
  VkBufferUsageFlags vk_usage = 0;

  switch (usage_) {
    case BufferUsage::storage:
      vk_usage = VK_BUFFER_USAGE_STORAGE_BUFFER_BIT;
      break;
    case BufferUsage::vertex:
      vk_usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
      break;
    case BufferUsage::index:
      vk_usage = VK_BUFFER_USAGE_INDEX_BUFFER_BIT;
      break;
    case BufferUsage::uniform:
      vk_usage = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
      break;
    case BufferUsage::indirect:
      vk_usage = VK_BUFFER_USAGE_INDIRECT_BUFFER_BIT;
      break;
  }

  VkBufferCreateInfo buffer_info{};
  buffer_info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
  buffer_info.size = size_;
  buffer_info.usage = vk_usage;
  buffer_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

  if (vkCreateBuffer(device_, &buffer_info, nullptr, &buffer_) != VK_SUCCESS) {
    throw std::runtime_error("Failed to create Vulkan buffer.");
  }

  VkMemoryRequirements mem_requirements;
  vkGetBufferMemoryRequirements(device_, buffer_, &mem_requirements);

  VkMemoryAllocateInfo alloc_info{};
  alloc_info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
  alloc_info.allocationSize = mem_requirements.size;
  // For simplicity, assume memory type index 0; in real code you would pick proper memory type
  alloc_info.memoryTypeIndex = 0;

  if (vkAllocateMemory(device_, &alloc_info, nullptr, &memory_) != VK_SUCCESS) {
    throw std::runtime_error("Failed to allocate Vulkan buffer memory.");
  }

  vkBindBufferMemory(device_, buffer_, memory_, 0);
}

VulkanBuffer::~VulkanBuffer() {
  if (buffer_ != VK_NULL_HANDLE) {
    vkDestroyBuffer(device_, buffer_, nullptr);
    buffer_ = VK_NULL_HANDLE;
  }
  if (memory_ != VK_NULL_HANDLE) {
    vkFreeMemory(device_, memory_, nullptr);
    memory_ = VK_NULL_HANDLE;
  }
}

void VulkanBuffer::upload(const void* data, std::size_t bytes, std::size_t offset) {
  if (offset + bytes > size_) {
    throw std::runtime_error("Buffer upload exceeds buffer size.");
  }

  void* mapped = nullptr;
  vkMapMemory(device_, memory_, offset, bytes, 0, &mapped);
  std::memcpy(mapped, data, bytes);
  vkUnmapMemory(device_, memory_);
}

}  // namespace ndyn::gfx::vulkan
