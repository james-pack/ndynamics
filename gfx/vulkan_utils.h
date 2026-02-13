#pragma once

#include <vulkan/vulkan.h>

#include <cstdint>

namespace ndyn::gfx {

uint32_t find_memory_type(VkPhysicalDevice physical_device, uint32_t type_filter,
                          VkMemoryPropertyFlags properties);

}  // namespace ndyn::gfx
