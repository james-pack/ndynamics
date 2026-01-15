#include "gfx/vulkan/vulkan_pipeline_layout.h"

#include <cstdint>
#include <stdexcept>
#include <vector>

namespace ndyn::gfx::vulkan {

VulkanPipelineLayout::VulkanPipelineLayout(
    VkDevice device, const std::vector<VkDescriptorSetLayout>& descriptor_set_layouts,
    const std::vector<VkPushConstantRange>& push_constant_ranges)
    : device_(device),
      descriptor_set_layouts_(descriptor_set_layouts),
      push_constant_ranges_(push_constant_ranges) {
  VkPipelineLayoutCreateInfo layout_info{};
  layout_info.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
  layout_info.setLayoutCount = static_cast<uint32_t>(descriptor_set_layouts_.size());
  layout_info.pSetLayouts = descriptor_set_layouts_.data();
  layout_info.pushConstantRangeCount = static_cast<uint32_t>(push_constant_ranges_.size());
  layout_info.pPushConstantRanges = push_constant_ranges_.data();

  VkResult result = vkCreatePipelineLayout(device_, &layout_info, nullptr, &pipeline_layout_);
  if (result != VK_SUCCESS) {
    throw std::runtime_error("Failed to create Vulkan pipeline layout.");
  }
}

VulkanPipelineLayout::~VulkanPipelineLayout() {
  if (pipeline_layout_ != VK_NULL_HANDLE) {
    vkDestroyPipelineLayout(device_, pipeline_layout_, nullptr);
  }
}

}  // namespace ndyn::gfx::vulkan
