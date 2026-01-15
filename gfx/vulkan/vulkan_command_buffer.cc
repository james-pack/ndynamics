#include "gfx/vulkan/vulkan_command_buffer.h"

#include <vulkan/vulkan.h>

#include <stdexcept>

#include "gfx/command_buffer.h"

namespace ndyn::gfx::vulkan {

VulkanCommandBuffer::VulkanCommandBuffer(VkDevice device, VkCommandPool pool)
    : device_(device), pool_(pool) {
  VkCommandBufferAllocateInfo alloc_info{};
  alloc_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
  alloc_info.commandPool = pool_;
  alloc_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
  alloc_info.commandBufferCount = 1;

  if (vkAllocateCommandBuffers(device_, &alloc_info, &command_buffer_) != VK_SUCCESS) {
    throw std::runtime_error("Failed to allocate Vulkan command buffer");
  }
}

VulkanCommandBuffer::~VulkanCommandBuffer() {
  if (command_buffer_ != VK_NULL_HANDLE) {
    vkFreeCommandBuffers(device_, pool_, 1, &command_buffer_);
  }
}

void VulkanCommandBuffer::begin() {
  VkCommandBufferBeginInfo begin_info{};
  begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
  begin_info.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
  begin_info.pInheritanceInfo = nullptr;

  if (vkBeginCommandBuffer(command_buffer_, &begin_info) != VK_SUCCESS) {
    throw std::runtime_error("Failed to begin Vulkan command buffer recording");
  }
}

void VulkanCommandBuffer::end() {
  if (vkEndCommandBuffer(command_buffer_) != VK_SUCCESS) {
    throw std::runtime_error("Failed to end Vulkan command buffer recording");
  }
}

}  // namespace ndyn::gfx::vulkan
