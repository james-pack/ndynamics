#include "gfx/vulkan/vulkan_shader_module.h"

#include <stdexcept>

namespace ndyn::gfx::vulkan {

VulkanShaderModule::VulkanShaderModule(VkDevice device, const void* code, std::size_t size)
    : device_(device) {
  VkShaderModuleCreateInfo create_info{};
  create_info.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
  create_info.codeSize = size;
  create_info.pCode = reinterpret_cast<const uint32_t*>(code);

  if (vkCreateShaderModule(device_, &create_info, nullptr, &module_) != VK_SUCCESS) {
    throw std::runtime_error("failed to create Vulkan shader module");
  }
}

VulkanShaderModule::~VulkanShaderModule() {
  if (module_ != VK_NULL_HANDLE) {
    vkDestroyShaderModule(device_, module_, nullptr);
  }
}

}  // namespace ndyn::gfx::vulkan
