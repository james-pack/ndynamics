#pragma once

#include <vulkan/vulkan.h>

#include "gfx/pipeline_layout.h"
#include "gfx/render_pass.h"
#include "gfx/render_pipeline.h"
#include "gfx/shader_module.h"

namespace ndyn::gfx::vulkan {

class VulkanRenderPipeline final : public RenderPipeline {
 public:
  VulkanRenderPipeline(VkDevice device, PipelineLayout& layout, RenderPass& render_pass, ShaderModule& vertex_shader,
                       ShaderModule* fragment_shader = nullptr);

  ~VulkanRenderPipeline() override;

  VkPipeline get_pipeline() const { return pipeline_; }

 private:
  VkDevice device_;
  VkPipeline pipeline_{VK_NULL_HANDLE};
};

}  // namespace ndyn::gfx::vulkan
