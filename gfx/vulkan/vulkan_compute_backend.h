#pragma once

#include <vulkan/vulkan.h>

#include <cstdint>
#include <memory>
#include <vector>

#include "gfx/buffer.h"
#include "gfx/command_buffer.h"
#include "gfx/compute_backend.h"
#include "gfx/compute_pipeline.h"
#include "gfx/fence.h"
#include "gfx/pipeline_layout.h"
#include "gfx/shader_module.h"

namespace ndyn::gfx::vulkan {

/**
 * VulkanComputeBackend implements the ComputeBackend interface using Vulkan 1.2.
 * It manages Vulkan instance, device, queues, and command buffers to execute
 * compute pipelines safely and efficiently. All resources are RAII-managed.
 */
class VulkanComputeBackend : public ComputeBackend {
 private:
  VkInstance instance_{VK_NULL_HANDLE};
  VkPhysicalDevice physical_device_{VK_NULL_HANDLE};
  VkDevice device_{VK_NULL_HANDLE};
  VkQueue compute_queue_{VK_NULL_HANDLE};
  uint32_t compute_queue_family_index_ = 0;

  std::vector<VkCommandBuffer> command_buffers_;
  VkCommandPool command_pool_{VK_NULL_HANDLE};

 public:
  VulkanComputeBackend();
  ~VulkanComputeBackend() override;

  std::unique_ptr<Buffer> create_buffer(std::size_t size_bytes, BufferUsage usage) override;

  std::unique_ptr<ShaderModule> create_compute_shader(const void* bytecode,
                                                      std::size_t size) override;

  std::unique_ptr<ComputePipeline> create_compute_pipeline(ShaderModule& shader,
                                                           PipelineLayout& layout) override;

  std::unique_ptr<CommandBuffer> create_command_buffer() override;

  void dispatch(CommandBuffer& cmd, ComputePipeline& pipeline, std::uint32_t x, std::uint32_t y,
                std::uint32_t z) override;

  std::unique_ptr<Fence> submit(CommandBuffer& cmd) override;
};

}  // namespace ndyn::gfx::vulkan
