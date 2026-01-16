#pragma once

#include <vulkan/vulkan.h>

#include "gfx/render_pass.h"

namespace ndyn::gfx::vulkan {

/**
 * VulkanRenderPass wraps a VkRenderPass and its attachment metadata.
 * It provides a backend-specific implementation of the RenderPass abstraction.
 */
class VulkanRenderPass final : public RenderPass {
 public:
  /**
   * Creates a VulkanRenderPass with the specified color and optional depth attachment formats.
   * The render pass consists of a single subpass.
   *
   * @param device The Vulkan logical device.
   * @param color_format Format of the color attachment.
   * @param depth_format Optional format of the depth attachment.
   */
  VulkanRenderPass(VkDevice device, VkFormat color_format,
                   VkFormat depth_format = VK_FORMAT_UNDEFINED);

  ~VulkanRenderPass() override;

  VkRenderPass get_render_pass() const { return render_pass_; }

 private:
  VkDevice device_;
  VkRenderPass render_pass_{VK_NULL_HANDLE};
};

}  // namespace ndyn::gfx::vulkan
