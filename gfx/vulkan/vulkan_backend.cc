#include "gfx/vulkan/vulkan_backend.h"

#include <memory>

#include "gfx/buffer.h"
#include "gfx/command_buffer.h"
#include "gfx/compute_pipeline.h"
#include "gfx/fence.h"
#include "gfx/pipeline_layout.h"
#include "gfx/shader_module.h"
#include "gfx/vulkan/vulkan_buffer.h"
#include "gfx/vulkan/vulkan_command_buffer.h"
#include "gfx/vulkan/vulkan_compute_pipeline.h"
#include "gfx/vulkan/vulkan_fence.h"
#include "gfx/vulkan/vulkan_render_pipeline.h"
#include "gfx/vulkan/vulkan_shader_module.h"

namespace ndyn::gfx::vulkan {

VulkanBackend::VulkanBackend() {
  /* ----------------------------
   * 1. Create VkInstance
   * ---------------------------- */
  VkApplicationInfo app_info{};
  app_info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
  app_info.pApplicationName = "ndyn";
  app_info.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
  app_info.pEngineName = "ndyn";
  app_info.engineVersion = VK_MAKE_VERSION(1, 0, 0);
  app_info.apiVersion = VK_API_VERSION_1_2;

  VkInstanceCreateInfo instance_info{};
  instance_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
  instance_info.pApplicationInfo = &app_info;

  if (vkCreateInstance(&instance_info, nullptr, &instance_) != VK_SUCCESS) {
    throw std::runtime_error("Failed to create VkInstance");
  }

  /* ----------------------------
   * 2. Select physical device
   * ---------------------------- */
  uint32_t physical_device_count = 0;
  vkEnumeratePhysicalDevices(instance_, &physical_device_count, nullptr);
  if (physical_device_count == 0) {
    throw std::runtime_error("No Vulkan physical devices found");
  }

  std::vector<VkPhysicalDevice> physical_devices(physical_device_count);
  vkEnumeratePhysicalDevices(instance_, &physical_device_count, physical_devices.data());

  physical_device_ = physical_devices[0];  // deterministic, simple choice

  /* ----------------------------
   * 3. Find compute-capable queue family
   * ---------------------------- */
  uint32_t queue_family_count = 0;
  vkGetPhysicalDeviceQueueFamilyProperties(physical_device_, &queue_family_count, nullptr);

  std::vector<VkQueueFamilyProperties> queue_families(queue_family_count);
  vkGetPhysicalDeviceQueueFamilyProperties(physical_device_, &queue_family_count,
                                           queue_families.data());

  bool found_compute_queue = false;
  for (uint32_t i = 0; i < queue_family_count; ++i) {
    if (queue_families[i].queueFlags & VK_QUEUE_COMPUTE_BIT) {
      compute_queue_family_index_ = i;
      found_compute_queue = true;
      break;
    }
  }

  if (!found_compute_queue) {
    throw std::runtime_error("No compute-capable queue family found");
  }

  /* ----------------------------
   * 4. Create logical device
   * ---------------------------- */
  float queue_priority = 1.0f;

  VkDeviceQueueCreateInfo queue_info{};
  queue_info.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
  queue_info.queueFamilyIndex = compute_queue_family_index_;
  queue_info.queueCount = 1;
  queue_info.pQueuePriorities = &queue_priority;

  VkDeviceCreateInfo device_info{};
  device_info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
  device_info.queueCreateInfoCount = 1;
  device_info.pQueueCreateInfos = &queue_info;

  if (vkCreateDevice(physical_device_, &device_info, nullptr, &device_) != VK_SUCCESS) {
    throw std::runtime_error("Failed to create VkDevice");
  }

  vkGetDeviceQueue(device_, compute_queue_family_index_, 0, &compute_queue_);

  /* ----------------------------
   * 5. Create command pool
   * ---------------------------- */
  VkCommandPoolCreateInfo pool_info{};
  pool_info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
  pool_info.queueFamilyIndex = compute_queue_family_index_;
  pool_info.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;

  if (vkCreateCommandPool(device_, &pool_info, nullptr, &command_pool_) != VK_SUCCESS) {
    throw std::runtime_error("Failed to create VkCommandPool");
  }
}

VulkanBackend::~VulkanBackend() {
  if (device_ != VK_NULL_HANDLE) {
    // Ensure no work is in flight before tearing down resources.
    vkDeviceWaitIdle(device_);

    if (command_pool_ != VK_NULL_HANDLE) {
      vkDestroyCommandPool(device_, command_pool_, nullptr);
    }

    vkDestroyDevice(device_, nullptr);
  }

  if (instance_ != VK_NULL_HANDLE) {
    vkDestroyInstance(instance_, nullptr);
  }
}

std::unique_ptr<Buffer> VulkanBackend::create_buffer(std::size_t size_bytes, BufferUsage usage) {
  return std::make_unique<VulkanBuffer>(device_, size_bytes, usage);
}

std::unique_ptr<ShaderModule> VulkanBackend::create_shader(ShaderUsage usage, const void* bytecode,
                                                           std::size_t size) {
  return std::make_unique<VulkanShaderModule>(device_, usage, bytecode, size);
}

std::unique_ptr<ComputePipeline> VulkanBackend::create_compute_pipeline(ShaderModule& shader,
                                                                        PipelineLayout& layout) {
  return std::make_unique<VulkanComputePipeline>(device_, shader, layout);
}

std::unique_ptr<CommandBuffer> VulkanBackend::create_command_buffer() {
  if (command_pool_ == VK_NULL_HANDLE) {
    VkCommandPoolCreateInfo pool_info{};
    pool_info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    pool_info.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
    pool_info.queueFamilyIndex = compute_queue_family_index_;

    if (vkCreateCommandPool(device_, &pool_info, nullptr, &command_pool_) != VK_SUCCESS) {
      throw std::runtime_error("Failed to create Vulkan command pool");
    }
  }

  return std::make_unique<VulkanCommandBuffer>(device_, command_pool_);
}

void VulkanBackend::dispatch(CommandBuffer& cmd, ComputePipeline& pipeline, std::uint32_t x,
                             std::uint32_t y, std::uint32_t z) {
  VkCommandBuffer vk_cmd = dynamic_cast<VulkanCommandBuffer&>(cmd).get_command_buffer();
  VkPipeline vk_pipeline = dynamic_cast<VulkanComputePipeline&>(pipeline).get_pipeline();
  vkCmdBindPipeline(vk_cmd, VK_PIPELINE_BIND_POINT_COMPUTE, vk_pipeline);
  vkCmdDispatch(vk_cmd, x, y, z);
}

void VulkanBackend::draw(CommandBuffer& cmd, RenderPipeline& pipeline, Buffer& vertex_buffer,
                         Buffer* index_buffer, std::uint32_t vertex_or_index_count) {
  auto& vk_cmd = dynamic_cast<VulkanCommandBuffer&>(cmd);
  auto& vk_pipeline = dynamic_cast<VulkanRenderPipeline&>(pipeline);
  auto& vk_vertex_buffer = dynamic_cast<VulkanBuffer&>(vertex_buffer);

  VkCommandBuffer command_buffer = vk_cmd.get_command_buffer();

  // Bind graphics pipeline
  vkCmdBindPipeline(command_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, vk_pipeline.get_pipeline());

  // Bind vertex buffer
  VkBuffer vertex_buffers[] = {vk_vertex_buffer.get_buffer()};
  VkDeviceSize offsets[] = {0};

  vkCmdBindVertexBuffers(command_buffer,
                         /*firstBinding*/ 0,
                         /*bindingCount*/ 1, vertex_buffers, offsets);

  if (index_buffer != nullptr) {
    auto& vk_index_buffer = dynamic_cast<VulkanBuffer&>(*index_buffer);

    vkCmdBindIndexBuffer(command_buffer, vk_index_buffer.get_buffer(),
                         /*offset*/ 0, VK_INDEX_TYPE_UINT32);

    vkCmdDrawIndexed(command_buffer, vertex_or_index_count,
                     /*instanceCount */ 1,
                     /*firstIndex*/ 0,
                     /*vertexOffset*/ 0,
                     /*firstInstance*/ 0);
  } else {
    vkCmdDraw(command_buffer, vertex_or_index_count,
              /*instanceCount=*/1,
              /*firstVertex=*/0,
              /*firstInstance=*/0);
  }
}

std::unique_ptr<Fence> VulkanBackend::submit(CommandBuffer& cmd) {
  VkCommandBuffer vk_cmd = dynamic_cast<VulkanCommandBuffer&>(cmd).get_command_buffer();

  VkSubmitInfo submit_info{};
  submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
  submit_info.commandBufferCount = 1;
  submit_info.pCommandBuffers = &vk_cmd;

  auto fence = std::make_unique<VulkanFence>(device_);
  vkQueueSubmit(compute_queue_, 1, &submit_info, dynamic_cast<VulkanFence&>(*fence).get_handle());
  return fence;
}

}  // namespace ndyn::gfx::vulkan
