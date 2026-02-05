#include "gfx/vulkan_renderer.h"

#include <GLFW/glfw3.h>
#include <vulkan/vulkan.h>

#include <array>
#include <cstring>
#include <stdexcept>
#include <string_view>
#include <vector>

#include "base/resource_loader.h"

namespace ndyn::gfx {

std::vector<uint32_t> load_spirv(std::string_view path) {
  const auto bytes{ResourceLoader::instance().load(path)};
  if (bytes.size() % sizeof(uint32_t) != 0) {
    throw std::runtime_error("SPIR-V size not multiple of uint32_t");
  }

  std::vector<uint32_t> words(bytes.size() / sizeof(uint32_t));
  std::memcpy(words.data(), bytes.data(), bytes.size());
  return words;
}

VkShaderModule create_shader_module(VkDevice device, const std::vector<uint32_t>& shader_binary) {
  VkShaderModuleCreateInfo create_info{};
  create_info.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
  create_info.codeSize = shader_binary.size() * sizeof(uint32_t);
  create_info.pCode = shader_binary.data();

  VkShaderModule shader_module;
  if (vkCreateShaderModule(device, &create_info, nullptr, &shader_module) != VK_SUCCESS) {
    throw std::runtime_error("failed to create shader module");
  }
  return shader_module;
}

VulkanRenderer::VulkanRenderer() {
  // Initialize GLFW and create a window
  glfwInit();
  glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);  // No OpenGL context
  window_ = glfwCreateWindow(800, 600, "Vulkan Window", nullptr, nullptr);

  VkApplicationInfo app_info{};
  app_info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
  app_info.pApplicationName = "Minimal Vulkan Triangle";
  app_info.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
  app_info.pEngineName = "ndyn";
  app_info.engineVersion = VK_MAKE_VERSION(1, 0, 0);
  app_info.apiVersion = VK_API_VERSION_1_2;

  uint32_t ext_count = 0;
  const char** required_exts = glfwGetRequiredInstanceExtensions(&ext_count);

  VkInstanceCreateInfo instance_info{};
  instance_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
  instance_info.pApplicationInfo = &app_info;
  instance_info.enabledExtensionCount = ext_count;
  instance_info.ppEnabledExtensionNames = required_exts;

  if (vkCreateInstance(&instance_info, nullptr, &instance_) != VK_SUCCESS)
    throw std::runtime_error("vkCreateInstance failed");

  if (glfwCreateWindowSurface(instance_, window_, nullptr, &surface_) != VK_SUCCESS) {
    throw std::runtime_error("Failed to create Vulkan surface");
  }

  uint32_t device_count = 0;
  vkEnumeratePhysicalDevices(instance_, &device_count, nullptr);
  if (device_count == 0) throw std::runtime_error("No Vulkan devices found");
  std::vector<VkPhysicalDevice> devices(device_count);
  vkEnumeratePhysicalDevices(instance_, &device_count, devices.data());
  physical_device_ = devices[0];

  uint32_t queue_family_count = 0;
  vkGetPhysicalDeviceQueueFamilyProperties(physical_device_, &queue_family_count, nullptr);
  std::vector<VkQueueFamilyProperties> queue_family_properties(queue_family_count);
  vkGetPhysicalDeviceQueueFamilyProperties(physical_device_, &queue_family_count,
                                           queue_family_properties.data());

  uint32_t graphics_family = UINT32_MAX;
  uint32_t present_family = UINT32_MAX;
  for (uint32_t i = 0; i < queue_family_count; ++i) {
    if (queue_family_properties[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) {
      graphics_family = i;
    }

    VkBool32 present_support = VK_FALSE;
    vkGetPhysicalDeviceSurfaceSupportKHR(physical_device_, i, surface_, &present_support);

    if (present_support) {
      present_family = i;
    }
  }

  const std::vector<const char*> device_extensions = {
      VK_KHR_SWAPCHAIN_EXTENSION_NAME  // "VK_KHR_swapchain"
  };
  float priority = 1.f;
  if (graphics_family == present_family) {
    VkDeviceQueueCreateInfo queue_info{};
    queue_info.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    queue_info.queueFamilyIndex = graphics_family;
    queue_info.queueCount = 1;
    queue_info.pQueuePriorities = &priority;

    VkDeviceCreateInfo device_info{};
    device_info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    device_info.queueCreateInfoCount = 1;
    device_info.pQueueCreateInfos = &queue_info;
    device_info.enabledExtensionCount = static_cast<uint32_t>(device_extensions.size());
    device_info.ppEnabledExtensionNames = device_extensions.data();

    vkCreateDevice(physical_device_, &device_info, nullptr, &device_);

    vkGetDeviceQueue(device_, graphics_family, 0, &graphics_queue_);
    present_queue_ = graphics_queue_;
  } else {
    std::array<VkDeviceQueueCreateInfo, 2> queue_info{};
    queue_info[0].sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    queue_info[0].queueFamilyIndex = graphics_family;
    queue_info[0].queueCount = 1;
    queue_info[0].pQueuePriorities = &priority;

    queue_info[1].sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    queue_info[1].queueFamilyIndex = present_family;
    queue_info[1].queueCount = 1;
    queue_info[1].pQueuePriorities = &priority;

    VkDeviceCreateInfo device_info{};
    device_info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    device_info.queueCreateInfoCount = 2;
    device_info.pQueueCreateInfos = queue_info.data();
    device_info.enabledExtensionCount = static_cast<uint32_t>(device_extensions.size());
    device_info.ppEnabledExtensionNames = device_extensions.data();

    if (vkCreateDevice(physical_device_, &device_info, nullptr, &device_) != VK_SUCCESS)
      throw std::runtime_error("vkCreateDevice failed");

    vkGetDeviceQueue(device_, graphics_family, 0, &graphics_queue_);
    vkGetDeviceQueue(device_, present_family, 0, &present_queue_);
  }

  VkCommandPoolCreateInfo pool_info{};
  pool_info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
  pool_info.queueFamilyIndex = graphics_family;
  pool_info.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;

  if (vkCreateCommandPool(device_, &pool_info, nullptr, &command_pool_) != VK_SUCCESS)
    throw std::runtime_error("vkCreateCommandPool failed");

  VkCommandBufferAllocateInfo alloc_info{};
  alloc_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
  alloc_info.commandPool = command_pool_;
  alloc_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
  alloc_info.commandBufferCount = 1;
  if (vkAllocateCommandBuffers(device_, &alloc_info, &command_buffer_) != VK_SUCCESS)
    throw std::runtime_error("vkAllocateCommandBuffers failed");

  // Query surface capabilities
  VkSurfaceCapabilitiesKHR capabilities;
  vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physical_device_, surface_, &capabilities);

  swapchain_extent_ = capabilities.currentExtent;

  // Choose format
  uint32_t format_count = 0;
  vkGetPhysicalDeviceSurfaceFormatsKHR(physical_device_, surface_, &format_count, nullptr);
  std::vector<VkSurfaceFormatKHR> formats(format_count);
  vkGetPhysicalDeviceSurfaceFormatsKHR(physical_device_, surface_, &format_count, formats.data());
  VkFormat swapchain_image_format{formats[0].format};

  // Swapchain creation
  VkSwapchainCreateInfoKHR sc_info{};
  sc_info.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
  sc_info.surface = surface_;
  sc_info.minImageCount = capabilities.minImageCount + 1;
  sc_info.imageFormat = swapchain_image_format;
  sc_info.imageColorSpace = formats[0].colorSpace;
  sc_info.imageExtent = swapchain_extent_;
  sc_info.imageArrayLayers = 1;
  sc_info.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
  sc_info.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
  sc_info.preTransform = capabilities.currentTransform;
  sc_info.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
  sc_info.presentMode = VK_PRESENT_MODE_FIFO_KHR;
  sc_info.clipped = VK_TRUE;

  if (vkCreateSwapchainKHR(device_, &sc_info, nullptr, &swapchain_) != VK_SUCCESS) {
    throw std::runtime_error("failed to create swapchain");
  }

  VkAttachmentDescription color_attachment{};
  color_attachment.format = swapchain_image_format;
  color_attachment.samples = VK_SAMPLE_COUNT_1_BIT;
  color_attachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
  color_attachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
  color_attachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
  color_attachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

  VkAttachmentReference color_ref{};
  color_ref.attachment = 0;
  color_ref.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

  VkSubpassDescription subpass{};
  subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
  subpass.colorAttachmentCount = 1;
  subpass.pColorAttachments = &color_ref;

  VkRenderPassCreateInfo rp_info{};
  rp_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
  rp_info.attachmentCount = 1;
  rp_info.pAttachments = &color_attachment;
  rp_info.subpassCount = 1;
  rp_info.pSubpasses = &subpass;

  if (vkCreateRenderPass(device_, &rp_info, nullptr, &render_pass_) != VK_SUCCESS)
    throw std::runtime_error("vkCreateRenderPass failed");

  // Get swapchain images
  uint32_t image_count;
  vkGetSwapchainImagesKHR(device_, swapchain_, &image_count, nullptr);
  swapchain_images_.resize(image_count);
  vkGetSwapchainImagesKHR(device_, swapchain_, &image_count, swapchain_images_.data());

  VkPipelineLayoutCreateInfo layout_info{};
  layout_info.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
  if (vkCreatePipelineLayout(device_, &layout_info, nullptr, &pipeline_layout_) != VK_SUCCESS)
    throw std::runtime_error("vkCreatePipelineLayout failed");

  swapchain_image_views_.resize(swapchain_images_.size());
  for (size_t i = 0; i < swapchain_images_.size(); ++i) {
    VkImageViewCreateInfo view_info{};
    view_info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    view_info.image = swapchain_images_[i];
    view_info.viewType = VK_IMAGE_VIEW_TYPE_2D;
    view_info.format = swapchain_image_format;
    view_info.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
    view_info.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
    view_info.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
    view_info.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
    view_info.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    view_info.subresourceRange.baseMipLevel = 0;
    view_info.subresourceRange.levelCount = 1;
    view_info.subresourceRange.baseArrayLayer = 0;
    view_info.subresourceRange.layerCount = 1;

    if (vkCreateImageView(device_, &view_info, nullptr, &swapchain_image_views_[i]) != VK_SUCCESS) {
      throw std::runtime_error("failed to create swapchain image view");
    }
  }

  framebuffers_.resize(swapchain_image_views_.size());
  for (size_t i = 0; i < swapchain_image_views_.size(); ++i) {
    VkFramebufferCreateInfo fb_info{};
    fb_info.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
    fb_info.renderPass = render_pass_;
    fb_info.attachmentCount = 1;
    fb_info.pAttachments = &swapchain_image_views_[i];
    fb_info.width = swapchain_extent_.width;
    fb_info.height = swapchain_extent_.height;
    fb_info.layers = 1;

    if (vkCreateFramebuffer(device_, &fb_info, nullptr, &framebuffers_[i]) != VK_SUCCESS) {
      throw std::runtime_error("failed to create framebuffer");
    }
  }

  VkSemaphoreCreateInfo sem_info{};
  sem_info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
  if (vkCreateSemaphore(device_, &sem_info, nullptr, &image_available_) != VK_SUCCESS) {
    throw std::runtime_error("failed to create image_available semaphore");
  }

  if (vkCreateSemaphore(device_, &sem_info, nullptr, &render_finished_) != VK_SUCCESS) {
    throw std::runtime_error("failed to create render_finished semaphore");
  }

  const auto vertex_shader_spv{load_spirv("gfx/triangle.vert.spv")};
  const auto fragment_shader_spv{load_spirv("gfx/render_red.frag.spv")};
  VkShaderModule vert_shader = create_shader_module(device_, vertex_shader_spv);
  VkShaderModule frag_shader = create_shader_module(device_, fragment_shader_spv);

  VkPipelineShaderStageCreateInfo vert_stage{};
  vert_stage.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
  vert_stage.stage = VK_SHADER_STAGE_VERTEX_BIT;
  vert_stage.module = vert_shader;
  vert_stage.pName = "main";

  VkPipelineShaderStageCreateInfo frag_stage{};
  frag_stage.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
  frag_stage.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
  frag_stage.module = frag_shader;
  frag_stage.pName = "main";

  VkPipelineShaderStageCreateInfo shader_stages[] = {vert_stage, frag_stage};

  VkPipelineVertexInputStateCreateInfo vertex_input{};
  vertex_input.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
  vertex_input.vertexBindingDescriptionCount = 0;
  vertex_input.vertexAttributeDescriptionCount = 0;

  VkPipelineInputAssemblyStateCreateInfo input_assembly{};
  input_assembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
  input_assembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
  input_assembly.primitiveRestartEnable = VK_FALSE;

  VkViewport viewport{};
  viewport.x = 0.f;
  viewport.y = 0.f;
  viewport.width = static_cast<float>(swapchain_extent_.width);
  viewport.height = static_cast<float>(swapchain_extent_.height);
  viewport.minDepth = 0.f;
  viewport.maxDepth = 1.f;

  VkRect2D scissor{};
  scissor.offset = {0, 0};
  scissor.extent = swapchain_extent_;

  VkPipelineViewportStateCreateInfo viewport_state{};
  viewport_state.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
  viewport_state.viewportCount = 1;
  viewport_state.pViewports = &viewport;
  viewport_state.scissorCount = 1;
  viewport_state.pScissors = &scissor;

  // 6. Rasterizer
  VkPipelineRasterizationStateCreateInfo rasterizer{};
  rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
  rasterizer.depthClampEnable = VK_FALSE;
  rasterizer.rasterizerDiscardEnable = VK_FALSE;
  rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
  rasterizer.lineWidth = 1.f;
  rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
  rasterizer.frontFace = VK_FRONT_FACE_CLOCKWISE;
  rasterizer.depthBiasEnable = VK_FALSE;

  VkPipelineMultisampleStateCreateInfo multisampling{};
  multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
  multisampling.sampleShadingEnable = VK_FALSE;
  multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

  VkPipelineColorBlendAttachmentState color_blend_attachment{};
  color_blend_attachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT |
                                          VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
  color_blend_attachment.blendEnable = VK_FALSE;

  VkPipelineColorBlendStateCreateInfo color_blending{};
  color_blending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
  color_blending.logicOpEnable = VK_FALSE;
  color_blending.attachmentCount = 1;
  color_blending.pAttachments = &color_blend_attachment;

  VkGraphicsPipelineCreateInfo pipeline_info{};
  pipeline_info.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
  pipeline_info.stageCount = 2;
  pipeline_info.pStages = shader_stages;
  pipeline_info.pVertexInputState = &vertex_input;
  pipeline_info.pInputAssemblyState = &input_assembly;
  pipeline_info.pViewportState = &viewport_state;
  pipeline_info.pRasterizationState = &rasterizer;
  pipeline_info.pMultisampleState = &multisampling;
  pipeline_info.pColorBlendState = &color_blending;
  pipeline_info.layout = pipeline_layout_;
  pipeline_info.renderPass = render_pass_;
  pipeline_info.subpass = 0;

  if (vkCreateGraphicsPipelines(device_, VK_NULL_HANDLE, 1, &pipeline_info, nullptr,
                                &graphics_pipeline_) != VK_SUCCESS) {
    throw std::runtime_error("failed to create graphics pipeline");
  }

  vkDestroyShaderModule(device_, vert_shader, nullptr);
  vkDestroyShaderModule(device_, frag_shader, nullptr);
}

VulkanRenderer::~VulkanRenderer() {
  vkDeviceWaitIdle(device_);
  if (image_available_) vkDestroySemaphore(device_, image_available_, nullptr);
  if (render_finished_) vkDestroySemaphore(device_, render_finished_, nullptr);
  if (graphics_pipeline_) vkDestroyPipeline(device_, graphics_pipeline_, nullptr);
  if (pipeline_layout_) vkDestroyPipelineLayout(device_, pipeline_layout_, nullptr);
  if (render_pass_) vkDestroyRenderPass(device_, render_pass_, nullptr);
  if (command_pool_) vkDestroyCommandPool(device_, command_pool_, nullptr);
  if (device_) vkDestroyDevice(device_, nullptr);
  if (instance_) vkDestroyInstance(instance_, nullptr);

  if (window_) {
    glfwDestroyWindow(window_);
  }

  glfwTerminate();
}

void VulkanRenderer::render_frame() {
  uint32_t image_index;
  if (vkAcquireNextImageKHR(device_, swapchain_, UINT64_MAX, image_available_, VK_NULL_HANDLE,
                            &image_index) != VK_SUCCESS) {
    throw std::runtime_error("Failed to acquire swapchain image.");
  }

  vkResetCommandBuffer(command_buffer_, 0);
  VkCommandBufferBeginInfo begin_info{};
  begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
  begin_info.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
  vkBeginCommandBuffer(command_buffer_, &begin_info);

  VkClearValue clear{};
  clear.color = {{0.f, 0.f, 0.f, 1.f}};
  VkRenderPassBeginInfo rp_begin{};
  rp_begin.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
  rp_begin.renderPass = render_pass_;
  rp_begin.framebuffer = framebuffers_[image_index];
  rp_begin.renderArea.offset = {0, 0};
  rp_begin.renderArea.extent = swapchain_extent_;
  rp_begin.clearValueCount = 1;
  rp_begin.pClearValues = &clear;

  vkCmdBeginRenderPass(command_buffer_, &rp_begin, VK_SUBPASS_CONTENTS_INLINE);
  vkCmdBindPipeline(command_buffer_, VK_PIPELINE_BIND_POINT_GRAPHICS, graphics_pipeline_);

  // Draw a single triangle.
  vkCmdDraw(command_buffer_, 3, 1, 0, 0);

  vkCmdEndRenderPass(command_buffer_);
  vkEndCommandBuffer(command_buffer_);

  VkPipelineStageFlags wait_stages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
  VkSubmitInfo submit_info{};
  submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
  submit_info.waitSemaphoreCount = 1;               // wait on image_available
  submit_info.pWaitSemaphores = &image_available_;  // the semaphore we just acquired
  submit_info.pWaitDstStageMask = wait_stages;      // stage to wait at
  submit_info.commandBufferCount = 1;
  submit_info.pCommandBuffers = &command_buffer_;
  submit_info.signalSemaphoreCount = 1;  // signal when rendering is done
  submit_info.pSignalSemaphores = &render_finished_;

  if (vkQueueSubmit(graphics_queue_, 1, &submit_info, VK_NULL_HANDLE) != VK_SUCCESS) {
    throw std::runtime_error("Failed to submit draw command buffer.");
  }

  VkPresentInfoKHR present_info{};
  present_info.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
  present_info.waitSemaphoreCount = 1;
  present_info.pWaitSemaphores = &render_finished_;
  present_info.swapchainCount = 1;
  present_info.pSwapchains = &swapchain_;
  present_info.pImageIndices = &image_index;

  if (vkQueuePresentKHR(present_queue_, &present_info) != VK_SUCCESS) {
    throw std::runtime_error("Failed to present swapchain image.");
  }
}

}  // namespace ndyn::gfx
