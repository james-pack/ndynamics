#pragma once

#include <vulkan/vulkan.h>

#include <cstddef>

#include "gfx/shader_module.h"

namespace ndyn::gfx::vulkan {

/**
 * VulkanShaderModule wraps a VkShaderModule created from SPIR-V bytecode.
 */
class VulkanShaderModule : public ShaderModule {
 private:
  VkDevice device_;
  VkShaderModule module_{VK_NULL_HANDLE};

 public:
  VulkanShaderModule(VkDevice device, const void* code, std::size_t size);
  ~VulkanShaderModule() override;

  VkShaderModule get_module() const { return module_; }
};

}  // namespace ndyn::gfx::vulkan
