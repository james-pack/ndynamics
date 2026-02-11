#pragma once

#include <GLFW/glfw3.h>
#include <vulkan/vulkan.h>

#include <array>
#include <memory>
#include <vector>

#include "gfx/alignment.h"
#include "gfx/instance.h"
#include "gfx/material.h"
#include "gfx/math.h"
#include "gfx/mesh.h"
#include "gfx/ssbo_buffer.h"

namespace ndyn::gfx {

struct GpuMesh final {
  VkBuffer vertex_buffer{VK_NULL_HANDLE};
  VkBuffer index_buffer{VK_NULL_HANDLE};
  VkDeviceMemory memory{VK_NULL_HANDLE};
  uint32_t index_count{};
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

  VkFence in_flight_fence_{VK_NULL_HANDLE};
  VkSemaphore image_available_{VK_NULL_HANDLE};
  VkSemaphore render_finished_{VK_NULL_HANDLE};

  std::array<VkDescriptorSetLayout, 2> descriptor_set_layouts_{VK_NULL_HANDLE, VK_NULL_HANDLE};
  VkDescriptorPool instance_descriptor_pool_{VK_NULL_HANDLE};
  VkDescriptorPool material_descriptor_pool_{VK_NULL_HANDLE};
  std::array<VkDescriptorSet, 2> descriptor_sets_{VK_NULL_HANDLE, VK_NULL_HANDLE};

  std::vector<GpuMesh> meshes_{};
  std::vector<Instance> instances_{};
  std::unique_ptr<SsboBuffer<Instance>> gpu_instances_{};
  std::unique_ptr<SsboBuffer<Material>> gpu_materials_{};
  MaterialId num_materials_{0};

  uint32_t find_memory_type(uint32_t type_filter, VkMemoryPropertyFlags properties) const;

 public:
  VulkanRenderer();
  ~VulkanRenderer();

  void render_frame();

  MeshId add_mesh(const Mesh& mesh);
  InstanceId add_instance(const Instance& instance);
  void update_position(InstanceId id, const Mat4& position) {
    instances_.at(id).position = position;
  }

  MaterialId add_material(const Material& material);
};

}  // namespace ndyn::gfx
