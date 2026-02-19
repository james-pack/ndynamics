#pragma once

#include <GLFW/glfw3.h>
#include <vulkan/vulkan.h>

#include <array>
#include <memory>
#include <vector>

#include "gfx/alignment.h"
#include "gfx/camera.h"
#include "gfx/instance.h"
#include "gfx/light.h"
#include "gfx/material.h"
#include "gfx/math.h"
#include "gfx/mesh.h"
#include "gfx/ssbo_buffer.h"
#include "gfx/ubo_allocator.h"

namespace ndyn::gfx {

struct GpuMesh final {
  VkBuffer vertex_buffer{VK_NULL_HANDLE};
  VkBuffer index_buffer{VK_NULL_HANDLE};
  VkDeviceMemory memory{VK_NULL_HANDLE};
  uint32_t index_count{};
};

class VulkanRenderer final {
 private:
  // This documents that only one frame is allowed to be "in flight" (in the process of being
  // rendered) at a time. Just changing this value will not change the number of frames in flight.
  // Many changes would be required for that, and it is not clear that it would have any value at
  // this point.
  static constexpr size_t NUM_FRAMES_IN_FLIGHT{1};

  static constexpr size_t INSTANCE_DESCRIPTOR_SET_INDEX{0};
  static constexpr size_t MATERIAL_DESCRIPTOR_SET_INDEX{1};
  static constexpr size_t UBO_DESCRIPTOR_SET_INDEX{2};
  static constexpr size_t SSBO_DESCRIPTOR_SET_OFFSET{0};
  static constexpr size_t NUM_SSBO_DESCRIPTOR_SETS{2};
  static constexpr size_t UBO_DESCRIPTOR_SET_OFFSET{UBO_DESCRIPTOR_SET_INDEX};
  static constexpr size_t NUM_UBO_DESCRIPTOR_SETS{1};
  static constexpr size_t NUM_DESCRIPTOR_SETS{3};

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
  VkImage depth_image_{};
  VkImageView depth_image_view_{};
  VkDeviceMemory depth_image_memory_{};
  std::vector<VkFramebuffer> framebuffers_{};

  VkFence in_flight_fence_{VK_NULL_HANDLE};
  VkSemaphore image_available_{VK_NULL_HANDLE};
  VkSemaphore render_finished_{VK_NULL_HANDLE};

  std::array<VkDescriptorSetLayout, NUM_DESCRIPTOR_SETS> descriptor_set_layouts_{
      VK_NULL_HANDLE, VK_NULL_HANDLE, VK_NULL_HANDLE};
  VkDescriptorPool instance_descriptor_pool_{VK_NULL_HANDLE};
  VkDescriptorPool material_descriptor_pool_{VK_NULL_HANDLE};
  VkDescriptorPool ubo_descriptor_pool_{VK_NULL_HANDLE};
  std::array<VkDescriptorSet, NUM_DESCRIPTOR_SETS> descriptor_sets_{VK_NULL_HANDLE, VK_NULL_HANDLE,
                                                                    VK_NULL_HANDLE};

  std::vector<GpuMesh> meshes_{};
  std::vector<Instance> instances_{};
  std::unique_ptr<SsboBuffer<Instance>> gpu_instances_{};
  std::unique_ptr<SsboBuffer<Material>> gpu_materials_{};
  MaterialId num_materials_{0};

  // Note: changing this value does not automatically add more cameras. Other changes are required.
  static constexpr size_t NUM_CAMERAS{1};
  static constexpr size_t NUM_LIGHTS{16};

  static constexpr size_t NUM_UBO_BINDINGS{2};
  static constexpr size_t UBO_ALLOCATION_SIZE_PER_FRAME{sizeof(CameraState) +
                                                        NUM_LIGHTS * sizeof(Light)};
  std::unique_ptr<UboAllocator<UBO_ALLOCATION_SIZE_PER_FRAME, 1>> ubo_allocator_{};
  CameraState camera_{CameraState::default_camera()};

  std::array<Light, NUM_LIGHTS> lights_{};

 public:
  VulkanRenderer();
  ~VulkanRenderer();

  void render_frame();

  MeshId add_mesh(const Mesh& mesh);
  InstanceId add_instance(const Instance& instance);
  void update_position(InstanceId id, const Mat4& position) {
    // Note that the position matrix must be transposed as GLSL stores its matrices as column major,
    // but we use them on the host in row major.
    instances_.at(id).position = position.transpose();
  }

  MaterialId add_material(const Material& material);

  void update_camera(CameraState camera) { camera_ = std::move(camera); }
};

}  // namespace ndyn::gfx
