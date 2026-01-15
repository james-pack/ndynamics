#pragma once

#include <vulkan/vulkan.h>

#include "gfx/compute_pipeline.h"
#include "gfx/pipeline_layout.h"
#include "gfx/shader_module.h"
#include "gfx/vulkan/vulkan_pipeline_layout.h"

namespace ndyn::gfx::vulkan {

class VulkanComputePipeline final : public ComputePipeline {
 private:
  VkDevice device_;
  VulkanPipelineLayout* pipeline_layout_;
  VkPipeline pipeline_{VK_NULL_HANDLE};

 public:
  VulkanComputePipeline(VkDevice device, ShaderModule& shader, PipelineLayout& layout);
  ~VulkanComputePipeline() override;

  PipelineLayout& layout() const override;

  VkPipeline get_pipeline() const { return pipeline_; }
};

}  // namespace ndyn::gfx::vulkan
