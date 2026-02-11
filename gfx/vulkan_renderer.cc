#include "gfx/vulkan_renderer.h"

#include <GLFW/glfw3.h>
#include <vulkan/vulkan.h>

#include <array>
#include <cstring>
#include <memory>
#include <stdexcept>
#include <string_view>
#include <vector>

#include "base/resource_loader.h"
#include "gfx/ssbo_buffer.h"
#include "glog/logging.h"

namespace ndyn::gfx {

static constexpr const char VERTEX_SHADER[] = "gfx/pass_through.vert.spv";
static constexpr const char FRAGMENT_SHADER[] = "gfx/pass_through.frag.spv";
// static constexpr const char FRAGMENT_SHADER[] = "gfx/render_red.frag.spv";

static constexpr Material DEBUG_RED{
    .diffuse_color = {1.f, 0.f, 0.f, 1.f},
    .specular_color = {1.f, 1.f, 1.f, 1.f},
    .shininess = 0.f,
    .opacity = 0.f,
    .texture_index = 0,
};

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

VKAPI_ATTR VkBool32 VKAPI_CALL debug_callback(
    VkDebugUtilsMessageSeverityFlagBitsEXT severity, VkDebugUtilsMessageTypeFlagsEXT type,
    const VkDebugUtilsMessengerCallbackDataEXT* callback_data, void*) {
  LOG(ERROR) << "Vulkan validation: " << callback_data->pMessage;
  return VK_FALSE;
}

VulkanRenderer::VulkanRenderer() {
  // Initialize GLFW and create a window
  glfwInit();
  glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);  // No OpenGL context
  window_ = glfwCreateWindow(1200, 1000, "Vulkan Window", nullptr, nullptr);

  VkApplicationInfo app_info{};
  app_info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
  app_info.pApplicationName = "Minimal Vulkan Triangle";
  app_info.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
  app_info.pEngineName = "ndyn";
  app_info.engineVersion = VK_MAKE_VERSION(1, 0, 0);
  app_info.apiVersion = VK_API_VERSION_1_2;

  uint32_t ext_count = 0;
  const char** glfw_exts = glfwGetRequiredInstanceExtensions(&ext_count);
  std::vector<const char*> required_exts(glfw_exts, glfw_exts + ext_count);
  required_exts.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);

  VkDebugUtilsMessengerCreateInfoEXT debug_info{};
  debug_info.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
  debug_info.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT |
                               VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT;
  debug_info.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
                           VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
                           VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
  debug_info.pfnUserCallback = debug_callback;

  static constexpr const char* layers[] = {"VK_LAYER_KHRONOS_validation"};
  VkInstanceCreateInfo instance_info{};
  instance_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
  instance_info.pApplicationInfo = &app_info;
  instance_info.enabledExtensionCount = static_cast<uint32_t>(required_exts.size());
  instance_info.ppEnabledExtensionNames = required_exts.data();
  instance_info.enabledLayerCount = 1;
  instance_info.ppEnabledLayerNames = layers;
  instance_info.pNext = static_cast<const void*>(&debug_info);

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
      VK_KHR_SWAPCHAIN_EXTENSION_NAME,  // "VK_KHR_swapchain"
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

    if (vkCreateDevice(physical_device_, &device_info, nullptr, &device_) != VK_SUCCESS) {
      throw std::runtime_error("vkCreateDevice (graphics_family == present_family) failed");
    }

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

    if (vkCreateDevice(physical_device_, &device_info, nullptr, &device_) != VK_SUCCESS) {
      throw std::runtime_error("vkCreateDevice (graphics_family != present_family) failed");
    }

    vkGetDeviceQueue(device_, graphics_family, 0, &graphics_queue_);
    vkGetDeviceQueue(device_, present_family, 0, &present_queue_);
  }

  VkCommandPoolCreateInfo command_pool_info{};
  command_pool_info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
  command_pool_info.queueFamilyIndex = graphics_family;
  command_pool_info.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;

  if (vkCreateCommandPool(device_, &command_pool_info, nullptr, &command_pool_) != VK_SUCCESS) {
    throw std::runtime_error("vkCreateCommandPool failed");
  }

  VkCommandBufferAllocateInfo command_buffer_alloc_info{};
  command_buffer_alloc_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
  command_buffer_alloc_info.commandPool = command_pool_;
  command_buffer_alloc_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
  command_buffer_alloc_info.commandBufferCount = 1;
  if (vkAllocateCommandBuffers(device_, &command_buffer_alloc_info, &command_buffer_) !=
      VK_SUCCESS) {
    throw std::runtime_error("vkAllocateCommandBuffers failed");
  }

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

  std::array<VkDescriptorSetLayoutBinding, 2> bindings{};
  // Instances.
  bindings[0].binding = 0;
  bindings[0].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
  bindings[0].descriptorCount = 1;
  bindings[0].stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
  bindings[0].pImmutableSamplers = nullptr;

  // Materials.
  bindings[1].binding = 1;
  bindings[1].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
  bindings[1].descriptorCount = 1;
  bindings[1].stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
  bindings[1].pImmutableSamplers = nullptr;

  VkDescriptorSetLayoutCreateInfo layout_info{};
  layout_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
  layout_info.bindingCount = bindings.size();
  layout_info.pBindings = bindings.data();

  if (vkCreateDescriptorSetLayout(device_, &layout_info, nullptr, &instance_set_layout_) !=
      VK_SUCCESS) {
    throw std::runtime_error("Could not create descriptor set layout");
  }

  VkPipelineLayoutCreateInfo pipeline_layout_info{};
  pipeline_layout_info.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
  pipeline_layout_info.setLayoutCount = 1;
  pipeline_layout_info.pSetLayouts = &instance_set_layout_;
  pipeline_layout_info.pushConstantRangeCount = 0;

  if (vkCreatePipelineLayout(device_, &pipeline_layout_info, nullptr, &pipeline_layout_) !=
      VK_SUCCESS) {
    throw std::runtime_error("vkCreatePipelineLayout failed");
  }

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

  const auto vertex_shader_spv{load_spirv(VERTEX_SHADER)};
  const auto fragment_shader_spv{load_spirv(FRAGMENT_SHADER)};
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

  VkDescriptorPoolSize descriptor_pool_size{};
  descriptor_pool_size.type = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
  descriptor_pool_size.descriptorCount = 2;

  VkDescriptorPoolCreateInfo descriptor_pool_info{};
  descriptor_pool_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
  descriptor_pool_info.maxSets = 1;
  descriptor_pool_info.poolSizeCount = 1;
  descriptor_pool_info.pPoolSizes = &descriptor_pool_size;
  if (vkCreateDescriptorPool(device_, &descriptor_pool_info, nullptr, &descriptor_pool_) !=
      VK_SUCCESS) {
    throw std::runtime_error("failed to create descriptor pool");
  }

  VkDescriptorSetAllocateInfo descriptor_set_alloc_info{};
  descriptor_set_alloc_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
  descriptor_set_alloc_info.descriptorPool = descriptor_pool_;
  descriptor_set_alloc_info.descriptorSetCount = 1;
  descriptor_set_alloc_info.pSetLayouts = &instance_set_layout_;

  if (vkAllocateDescriptorSets(device_, &descriptor_set_alloc_info, &instance_descriptor_set_) !=
      VK_SUCCESS) {
    throw std::runtime_error("failed to allocate descriptor sets");
  }

  VkVertexInputBindingDescription binding{};
  binding.binding = 0;
  binding.stride = sizeof(float) * 6;
  binding.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

  std::array<VkVertexInputAttributeDescription, 2> attributes{};

  attributes[0].location = 0;  // in_pos
  attributes[0].binding = 0;
  attributes[0].format = VK_FORMAT_R32G32B32_SFLOAT;
  attributes[0].offset = 0;

  attributes[1].location = 1;  // in_normal
  attributes[1].binding = 0;
  attributes[1].format = VK_FORMAT_R32G32B32_SFLOAT;
  attributes[1].offset = sizeof(float) * 3;

  VkPipelineVertexInputStateCreateInfo vertex_input{};
  vertex_input.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
  vertex_input.vertexBindingDescriptionCount = 1;
  vertex_input.pVertexBindingDescriptions = &binding;
  vertex_input.vertexAttributeDescriptionCount = static_cast<uint32_t>(attributes.size());
  vertex_input.pVertexAttributeDescriptions = attributes.data();

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
  // rasterizer.cullMode = VK_CULL_MODE_NONE;  // Turn off culling for debugging.
  rasterizer.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
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

  VkFenceCreateInfo fence_info{};
  fence_info.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
  fence_info.flags = VK_FENCE_CREATE_SIGNALED_BIT;

  if (vkCreateFence(device_, &fence_info, nullptr, &in_flight_fence_) != VK_SUCCESS) {
    throw std::runtime_error("failed to create in-flight fence");
  }

  gpu_instances_ = std::make_unique<SsboBuffer<Instance>>(device_, physical_device_);
  gpu_materials_ = std::make_unique<SsboBuffer<Material>>(device_, physical_device_);

  std::array<VkDescriptorBufferInfo, 2> buffer_info{};
  buffer_info[0].buffer = gpu_instances_->buffer();
  buffer_info[0].offset = 0;
  buffer_info[0].range = VK_WHOLE_SIZE;

  buffer_info[1].buffer = gpu_materials_->buffer();
  buffer_info[1].offset = 0;
  buffer_info[1].range = VK_WHOLE_SIZE;

  for (size_t i = 0; i < buffer_info.size(); ++i) {
    VkWriteDescriptorSet write{};
    write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    write.dstSet = instance_descriptor_set_;
    write.dstBinding = i;
    write.dstArrayElement = 0;
    write.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
    write.descriptorCount = 1;
    write.pBufferInfo = &buffer_info[i];

    vkUpdateDescriptorSets(device_, 1, &write, 0, nullptr);
  }

  vkDestroyShaderModule(device_, vert_shader, nullptr);
  vkDestroyShaderModule(device_, frag_shader, nullptr);

  add_material(DEBUG_RED);
}

VulkanRenderer::~VulkanRenderer() {
  vkDeviceWaitIdle(device_);
  if (in_flight_fence_) vkDestroyFence(device_, in_flight_fence_, nullptr);
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

MaterialId VulkanRenderer::add_material(const Material& material) {
  MaterialId id{num_materials_};
  ++num_materials_;
  auto updater{gpu_materials_->begin_updates()};
  updater.reserve(num_materials_);
  updater.update(id, material);
  return id;
}

void VulkanRenderer::render_frame() {
  vkWaitForFences(device_, 1, &in_flight_fence_, VK_TRUE, UINT64_MAX);
  vkResetCommandBuffer(command_buffer_, 0);

  uint32_t image_index;
  if (vkAcquireNextImageKHR(device_, swapchain_, UINT64_MAX, image_available_, VK_NULL_HANDLE,
                            &image_index) != VK_SUCCESS) {
    throw std::runtime_error("Failed to acquire swapchain image.");
  }

  {
    auto updater{gpu_instances_->begin_updates()};
    updater.reserve(instances_.size());
    for (size_t i = 0; i < instances_.size(); ++i) {
      updater.update(i, instances_[i]);
    }
    // Let the updater fall out of scope and be destroyed to trigger a flush.
  }

  // {
  //   auto updater{gpu_materials_->begin_updates()};
  //   updater.reserve(num_materials_);
  //   // Let the updater fall out of scope and be destroyed to trigger a flush.
  // }

  VkCommandBufferBeginInfo begin_info{};
  begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
  begin_info.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
  vkBeginCommandBuffer(command_buffer_, &begin_info);

  std::array<VkBufferMemoryBarrier, 2> barriers{};
  barriers[0].sType = VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER;
  barriers[0].srcAccessMask = VK_ACCESS_HOST_WRITE_BIT;
  barriers[0].dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
  barriers[0].srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
  barriers[0].dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
  barriers[0].buffer = gpu_instances_->buffer();
  barriers[0].offset = 0;
  barriers[0].size = VK_WHOLE_SIZE;

  barriers[1].sType = VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER;
  barriers[1].srcAccessMask = VK_ACCESS_HOST_WRITE_BIT;
  barriers[1].dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
  barriers[1].srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
  barriers[1].dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
  barriers[1].buffer = gpu_materials_->buffer();
  barriers[1].offset = 0;
  barriers[1].size = VK_WHOLE_SIZE;

  vkCmdPipelineBarrier(command_buffer_, VK_PIPELINE_STAGE_HOST_BIT,
                       VK_PIPELINE_STAGE_VERTEX_SHADER_BIT | VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
                       0, 0, nullptr, barriers.size(), barriers.data(), 0, nullptr);

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

  vkCmdBindDescriptorSets(command_buffer_, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline_layout_,
                          0,  // firstSet
                          1, &instance_descriptor_set_, 0, nullptr);

  DLOG(INFO) << "VulkanRenderer::render_frame() -- instances_.size(): " << instances_.size();

  const GpuMesh& mesh = meshes_[0];
  VkDeviceSize offsets[] = {0};
  vkCmdBindVertexBuffers(command_buffer_, 0, 1, &mesh.vertex_buffer, offsets);
  vkCmdBindIndexBuffer(command_buffer_, mesh.index_buffer, 0, VK_INDEX_TYPE_UINT32);
  vkCmdDrawIndexed(command_buffer_, mesh.index_count, instances_.size(), 0, 0,
                   /* offset of first instance of this mesh */ 0);

  vkCmdEndRenderPass(command_buffer_);
  vkEndCommandBuffer(command_buffer_);

  vkResetFences(device_, 1, &in_flight_fence_);

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

  if (vkQueueSubmit(graphics_queue_, 1, &submit_info, in_flight_fence_) != VK_SUCCESS) {
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

uint32_t VulkanRenderer::find_memory_type(uint32_t type_filter,
                                          VkMemoryPropertyFlags properties) const {
  VkPhysicalDeviceMemoryProperties mem_props{};
  vkGetPhysicalDeviceMemoryProperties(physical_device_, &mem_props);

  for (uint32_t i = 0; i < mem_props.memoryTypeCount; ++i) {
    const bool type_supported = type_filter & (1u << i);
    const bool has_properties = (mem_props.memoryTypes[i].propertyFlags & properties) == properties;

    if (type_supported && has_properties) {
      return i;
    }
  }

  throw std::runtime_error("Failed to find suitable Vulkan memory type.");
}

MeshId VulkanRenderer::add_mesh(const Mesh& mesh) {
  GpuMesh gpu_mesh{};
  gpu_mesh.index_count = static_cast<uint32_t>(mesh.indices.size());

  const VkDeviceSize vertex_size = mesh.vertices.size() * sizeof(mesh.vertices[0]);
  const VkDeviceSize index_size = mesh.indices.size() * sizeof(mesh.indices[0]);

  // ---------------------------------------------------------------------------
  // 1. Create staging buffers
  // ---------------------------------------------------------------------------

  VkBuffer staging_vertex_buffer;
  VkBuffer staging_index_buffer;
  VkDeviceMemory staging_memory;

  VkBufferCreateInfo staging_info{};
  staging_info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
  staging_info.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
  staging_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

  staging_info.size = vertex_size;
  vkCreateBuffer(device_, &staging_info, nullptr, &staging_vertex_buffer);

  staging_info.size = index_size;
  vkCreateBuffer(device_, &staging_info, nullptr, &staging_index_buffer);

  VkMemoryRequirements vreq, ireq;
  vkGetBufferMemoryRequirements(device_, staging_vertex_buffer, &vreq);
  vkGetBufferMemoryRequirements(device_, staging_index_buffer, &ireq);

  const VkDeviceSize index_offset = (vreq.size + ireq.alignment - 1) & ~(ireq.alignment - 1);
  const VkDeviceSize staging_total_size = index_offset + ireq.size;

  VkMemoryAllocateInfo staging_alloc{};
  staging_alloc.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
  staging_alloc.allocationSize = staging_total_size;
  staging_alloc.memoryTypeIndex =
      find_memory_type(vreq.memoryTypeBits & ireq.memoryTypeBits,
                       VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

  vkAllocateMemory(device_, &staging_alloc, nullptr, &staging_memory);

  vkBindBufferMemory(device_, staging_vertex_buffer, staging_memory, 0);
  vkBindBufferMemory(device_, staging_index_buffer, staging_memory, index_offset);

  // Upload CPU data
  char* mapped;
  vkMapMemory(device_, staging_memory, 0, staging_total_size, 0, reinterpret_cast<void**>(&mapped));
  std::memcpy(mapped, mesh.vertices.data(), vertex_size);
  std::memcpy(mapped + index_offset, mesh.indices.data(), index_size);
  vkUnmapMemory(device_, staging_memory);

  // ---------------------------------------------------------------------------
  // 2. Create device-local buffers
  // ---------------------------------------------------------------------------

  VkBufferCreateInfo buffer_info{};
  buffer_info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
  buffer_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

  buffer_info.size = vertex_size;
  buffer_info.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT;
  vkCreateBuffer(device_, &buffer_info, nullptr, &gpu_mesh.vertex_buffer);

  buffer_info.size = index_size;
  buffer_info.usage = VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT;
  vkCreateBuffer(device_, &buffer_info, nullptr, &gpu_mesh.index_buffer);

  vkGetBufferMemoryRequirements(device_, gpu_mesh.vertex_buffer, &vreq);
  vkGetBufferMemoryRequirements(device_, gpu_mesh.index_buffer, &ireq);

  const VkDeviceSize device_index_offset = (vreq.size + ireq.alignment - 1) & ~(ireq.alignment - 1);
  const VkDeviceSize device_total_size = device_index_offset + ireq.size;

  VkMemoryAllocateInfo device_alloc{};
  device_alloc.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
  device_alloc.allocationSize = device_total_size;
  device_alloc.memoryTypeIndex = find_memory_type(vreq.memoryTypeBits & ireq.memoryTypeBits,
                                                  VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

  vkAllocateMemory(device_, &device_alloc, nullptr, &gpu_mesh.memory);

  vkBindBufferMemory(device_, gpu_mesh.vertex_buffer, gpu_mesh.memory, 0);
  vkBindBufferMemory(device_, gpu_mesh.index_buffer, gpu_mesh.memory, device_index_offset);

  // ---------------------------------------------------------------------------
  // 3. Copy staging → device-local
  // ---------------------------------------------------------------------------

  VkCommandBufferAllocateInfo cmd_alloc{};
  cmd_alloc.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
  cmd_alloc.commandPool = command_pool_;
  cmd_alloc.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
  cmd_alloc.commandBufferCount = 1;

  VkCommandBuffer cmd;
  vkAllocateCommandBuffers(device_, &cmd_alloc, &cmd);

  VkCommandBufferBeginInfo begin{};
  begin.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
  begin.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
  vkBeginCommandBuffer(cmd, &begin);

  VkBufferCopy copy{};
  copy.size = vertex_size;
  vkCmdCopyBuffer(cmd, staging_vertex_buffer, gpu_mesh.vertex_buffer, 1, &copy);

  copy.size = index_size;
  vkCmdCopyBuffer(cmd, staging_index_buffer, gpu_mesh.index_buffer, 1, &copy);

  vkEndCommandBuffer(cmd);

  VkSubmitInfo submit{};
  submit.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
  submit.commandBufferCount = 1;
  submit.pCommandBuffers = &cmd;

  vkQueueSubmit(graphics_queue_, 1, &submit, VK_NULL_HANDLE);
  vkQueueWaitIdle(graphics_queue_);

  vkFreeCommandBuffers(device_, command_pool_, 1, &cmd);

  // ---------------------------------------------------------------------------
  // 4. Cleanup staging
  // ---------------------------------------------------------------------------

  vkDestroyBuffer(device_, staging_vertex_buffer, nullptr);
  vkDestroyBuffer(device_, staging_index_buffer, nullptr);
  vkFreeMemory(device_, staging_memory, nullptr);

  meshes_.emplace_back(gpu_mesh);
  return static_cast<MeshId>(meshes_.size() - 1);
}

InstanceId VulkanRenderer::add_instance(const Instance& instance) {
  instances_.emplace_back(instance);
  return static_cast<InstanceId>(instances_.size() - 1);
}

}  // namespace ndyn::gfx
