#pragma once

#include <vulkan/vulkan.h>

#include <memory>
#include <vector>

#include "gfx/pipeline_layout.h"

namespace ndyn::gfx::vulkan {

/**
 * VulkanPipelineLayout implements the PipelineLayout interface for Vulkan 1.2.
 * It manages Vulkan descriptor set layouts and push constant ranges,
 * encapsulating the VkPipelineLayout object. RAII is used for automatic cleanup.
 */
class VulkanPipelineLayout final : public PipelineLayout {
 private:
  VkDevice device_;
  VkPipelineLayout pipeline_layout_{VK_NULL_HANDLE};
  std::vector<VkDescriptorSetLayout> descriptor_set_layouts_;
  std::vector<VkPushConstantRange> push_constant_ranges_;

 public:
  VulkanPipelineLayout(VkDevice device,
                       const std::vector<VkDescriptorSetLayout>& descriptor_set_layouts = {},
                       const std::vector<VkPushConstantRange>& push_constant_ranges = {});
  ~VulkanPipelineLayout() override;

  /**
   * Returns the underlying Vulkan pipeline layout handle.
   */
  VkPipelineLayout get_layout() const { return pipeline_layout_; }
};

}  // namespace ndyn::gfx::vulkan
