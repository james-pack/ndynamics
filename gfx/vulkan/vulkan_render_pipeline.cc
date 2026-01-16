#include "gfx/vulkan/vulkan_render_pipeline.h"

#include <stdexcept>

#include "gfx/pipeline_layout.h"
#include "gfx/render_pass.h"
#include "gfx/shader_module.h"
#include "gfx/vulkan/vulkan_pipeline_layout.h"
#include "gfx/vulkan/vulkan_render_pass.h"
#include "gfx/vulkan/vulkan_shader_module.h"

namespace ndyn::gfx::vulkan {

VulkanRenderPipeline::VulkanRenderPipeline(VkDevice device, PipelineLayout& layout,
                                           RenderPass& render_pass, ShaderModule& vertex_shader,
                                           ShaderModule* fragment_shader)
    : device_(device) {
  auto& vk_layout = dynamic_cast<VulkanPipelineLayout&>(layout);
  auto& vk_pass = dynamic_cast<VulkanRenderPass&>(render_pass);

  auto& vk_vertex = dynamic_cast<VulkanShaderModule&>(vertex_shader);
  VulkanShaderModule* vk_fragment{nullptr};
  if (fragment_shader) {
    vk_fragment = &dynamic_cast<VulkanShaderModule&>(*fragment_shader);
  }

  VkPipelineShaderStageCreateInfo shader_stages[2]{};
  shader_stages[0].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
  shader_stages[0].stage = VK_SHADER_STAGE_VERTEX_BIT;
  shader_stages[0].module = vk_vertex.get_module();
  shader_stages[0].pName = "main";

  uint32_t stage_count = 1;
  if (vk_fragment) {
    shader_stages[1].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    shader_stages[1].stage = VK_SHADER_STAGE_FRAGMENT_BIT;
    shader_stages[1].module = vk_fragment->get_module();
    shader_stages[1].pName = "main";
    stage_count = 2;
  }

  VkGraphicsPipelineCreateInfo pipeline_info{};
  pipeline_info.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
  pipeline_info.stageCount = stage_count;
  pipeline_info.pStages = shader_stages;
  pipeline_info.layout = vk_layout.get_layout();
  pipeline_info.renderPass = vk_pass.get_render_pass();
  pipeline_info.subpass = 0;

  // Minimal defaults for other fixed-function stages
  VkPipelineVertexInputStateCreateInfo vertex_input{};
  vertex_input.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
  pipeline_info.pVertexInputState = &vertex_input;

  VkPipelineInputAssemblyStateCreateInfo input_assembly{};
  input_assembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
  input_assembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
  pipeline_info.pInputAssemblyState = &input_assembly;

  VkViewport viewport{};
  viewport.width = 1.0f;
  viewport.height = 1.0f;
  viewport.minDepth = 0.0f;
  viewport.maxDepth = 1.0f;
  VkRect2D scissor{{0, 0}, {1, 1}};
  VkPipelineViewportStateCreateInfo viewport_state{};
  viewport_state.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
  viewport_state.viewportCount = 1;
  viewport_state.pViewports = &viewport;
  viewport_state.scissorCount = 1;
  viewport_state.pScissors = &scissor;
  pipeline_info.pViewportState = &viewport_state;

  VkPipelineRasterizationStateCreateInfo rasterizer{};
  rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
  rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
  rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
  rasterizer.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
  rasterizer.lineWidth = 1.0f;
  pipeline_info.pRasterizationState = &rasterizer;

  VkPipelineMultisampleStateCreateInfo multisample{};
  multisample.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
  multisample.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
  pipeline_info.pMultisampleState = &multisample;

  VkPipelineColorBlendAttachmentState color_blend_attachment{};
  color_blend_attachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT |
                                          VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
  VkPipelineColorBlendStateCreateInfo color_blending{};
  color_blending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
  color_blending.attachmentCount = 1;
  color_blending.pAttachments = &color_blend_attachment;
  pipeline_info.pColorBlendState = &color_blending;

  if (vkCreateGraphicsPipelines(device_, VK_NULL_HANDLE, 1, &pipeline_info, nullptr, &pipeline_) !=
      VK_SUCCESS) {
    throw std::runtime_error("failed to create Vulkan render pipeline");
  }
}

VulkanRenderPipeline::~VulkanRenderPipeline() {
  if (pipeline_ != VK_NULL_HANDLE) {
    vkDestroyPipeline(device_, pipeline_, nullptr);
  }
}

}  // namespace ndyn::gfx::vulkan
