#pragma once

#include <GLFW/glfw3.h>
#include <vulkan/vulkan.h>

#include <vector>

#include "gfx/math.h"
#include "gfx/mesh.h"

namespace ndyn::gfx {

struct GpuMesh final {
  VkBuffer vertex_buffer{VK_NULL_HANDLE};
  VkBuffer index_buffer{VK_NULL_HANDLE};
  std::uint32_t index_count{0};
  Transform position;
};

class VulkanRenderer final {
 private:
  GLFWwindow* window_{nullptr};
  VkExtent2D swapchain_extent_{};
  VkInstance instance_{VK_NULL_HANDLE};
  VkSurfaceKHR surface_{VK_NULL_HANDLE};
  VkPhysicalDevice physical_device_{VK_NULL_HANDLE};
  VkDevice device_{VK_NULL_HANDLE};
  VkQueue graphics_queue_{VK_NULL_HANDLE};
  VkQueue present_queue_{VK_NULL_HANDLE};
  VkCommandPool command_pool_{VK_NULL_HANDLE};
  VkCommandBuffer command_buffer_{VK_NULL_HANDLE};
  VkRenderPass render_pass_{VK_NULL_HANDLE};
  VkPipelineLayout pipeline_layout_{VK_NULL_HANDLE};
  VkPipeline graphics_pipeline_{VK_NULL_HANDLE};

  VkSwapchainKHR swapchain_{VK_NULL_HANDLE};
  std::vector<VkImage> swapchain_images_{};
  std::vector<VkImageView> swapchain_image_views_{};
  std::vector<VkFramebuffer> framebuffers_{};

  VkSemaphore image_available_{VK_NULL_HANDLE};
  VkSemaphore render_finished_{VK_NULL_HANDLE};

 public:
  VulkanRenderer();
  ~VulkanRenderer();

  void render_frame();
};

}  // namespace ndyn::gfx
