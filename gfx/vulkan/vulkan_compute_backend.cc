#include "gfx/vulkan/vulkan_compute_backend.h"

#include <memory>

#include "gfx/buffer.h"
#include "gfx/command_buffer.h"
#include "gfx/compute_pipeline.h"
#include "gfx/fence.h"
#include "gfx/pipeline_layout.h"
#include "gfx/shader_module.h"
#include "gfx/vulkan/vulkan_buffer.h"
#include "gfx/vulkan/vulkan_command_buffer.h"
#include "gfx/vulkan/vulkan_compute_pipeline.h"
#include "gfx/vulkan/vulkan_fence.h"
#include "gfx/vulkan/vulkan_shader_module.h"

namespace ndyn::gfx::vulkan {

VulkanComputeBackend::VulkanComputeBackend() {}

VulkanComputeBackend::~VulkanComputeBackend() {
  if (command_pool_ != VK_NULL_HANDLE) {
    vkDestroyCommandPool(device_, command_pool_, nullptr);
  }
}

std::unique_ptr<Buffer> VulkanComputeBackend::create_buffer(std::size_t size_bytes,
                                                            BufferUsage usage) {
  return std::make_unique<VulkanBuffer>(device_, size_bytes, usage);
}

std::unique_ptr<ShaderModule> VulkanComputeBackend::create_compute_shader(const void* bytecode,
                                                                          std::size_t size) {
  return std::make_unique<VulkanShaderModule>(device_, bytecode, size);
}

std::unique_ptr<ComputePipeline> VulkanComputeBackend::create_compute_pipeline(
    ShaderModule& shader, PipelineLayout& layout) {
  return std::make_unique<VulkanComputePipeline>(device_, shader, layout);
}

std::unique_ptr<CommandBuffer> VulkanComputeBackend::create_command_buffer() {
  if (command_pool_ == VK_NULL_HANDLE) {
    VkCommandPoolCreateInfo pool_info{};
    pool_info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    pool_info.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
    pool_info.queueFamilyIndex = compute_queue_family_index_;

    if (vkCreateCommandPool(device_, &pool_info, nullptr, &command_pool_) != VK_SUCCESS) {
      throw std::runtime_error("Failed to create Vulkan command pool");
    }
  }

  return std::make_unique<VulkanCommandBuffer>(device_, command_pool_);
}

void VulkanComputeBackend::dispatch(CommandBuffer& cmd, ComputePipeline& pipeline, std::uint32_t x,
                                    std::uint32_t y, std::uint32_t z) {
  VkCommandBuffer vk_cmd = dynamic_cast<VulkanCommandBuffer&>(cmd).get_command_buffer();
  VkPipeline vk_pipeline = dynamic_cast<VulkanComputePipeline&>(pipeline).get_pipeline();
  vkCmdBindPipeline(vk_cmd, VK_PIPELINE_BIND_POINT_COMPUTE, vk_pipeline);
  vkCmdDispatch(vk_cmd, x, y, z);
}

std::unique_ptr<Fence> VulkanComputeBackend::submit(CommandBuffer& cmd) {
  VkCommandBuffer vk_cmd = dynamic_cast<VulkanCommandBuffer&>(cmd).get_command_buffer();

  VkSubmitInfo submit_info{};
  submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
  submit_info.commandBufferCount = 1;
  submit_info.pCommandBuffers = &vk_cmd;

  auto fence = std::make_unique<VulkanFence>(device_);
  vkQueueSubmit(compute_queue_, 1, &submit_info, dynamic_cast<VulkanFence&>(*fence).get_handle());
  return fence;
}

}  // namespace ndyn::gfx::vulkan
