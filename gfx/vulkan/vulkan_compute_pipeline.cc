#include "gfx/vulkan/vulkan_compute_pipeline.h"

#include <vulkan/vulkan.h>

#include <stdexcept>

#include "gfx/pipeline_layout.h"
#include "gfx/shader_module.h"
#include "gfx/vulkan/vulkan_pipeline_layout.h"
#include "gfx/vulkan/vulkan_shader_module.h"

namespace ndyn::gfx::vulkan {

VulkanComputePipeline::VulkanComputePipeline(VkDevice device, ShaderModule& shader,
                                             PipelineLayout& layout)
    : device_(device), pipeline_layout_(&dynamic_cast<VulkanPipelineLayout&>(layout)) {
  VkPipelineShaderStageCreateInfo stage_info{};
  stage_info.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
  stage_info.stage = VK_SHADER_STAGE_COMPUTE_BIT;
  stage_info.module = dynamic_cast<VulkanShaderModule&>(shader).get_module();
  stage_info.pName = "main";

  VkComputePipelineCreateInfo pipeline_info{};
  pipeline_info.sType = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO;
  pipeline_info.stage = stage_info;
  pipeline_info.layout = pipeline_layout_->get_layout();

  if (vkCreateComputePipelines(device_, VK_NULL_HANDLE, 1, &pipeline_info, nullptr, &pipeline_) !=
      VK_SUCCESS) {
    throw std::runtime_error("failed to create Vulkan compute pipeline");
  }
}

VulkanComputePipeline::~VulkanComputePipeline() {
  if (pipeline_ != VK_NULL_HANDLE) {
    vkDestroyPipeline(device_, pipeline_, nullptr);
  }
}

PipelineLayout& VulkanComputePipeline::layout() const { return *pipeline_layout_; }

}  // namespace ndyn::gfx::vulkan
