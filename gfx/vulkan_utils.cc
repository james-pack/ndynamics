#include "gfx/vulkan_utils.h"

#include <vulkan/vulkan.h>

#include <cstdint>
#include <stdexcept>

namespace ndyn::gfx {

uint32_t find_memory_type(VkPhysicalDevice physical_device, uint32_t type_filter,
                          VkMemoryPropertyFlags properties) {
  VkPhysicalDeviceMemoryProperties mem_props{};
  vkGetPhysicalDeviceMemoryProperties(physical_device, &mem_props);

  for (uint32_t i = 0; i < mem_props.memoryTypeCount; ++i) {
    const bool type_supported = type_filter & (1u << i);
    const bool has_properties = (mem_props.memoryTypes[i].propertyFlags & properties) == properties;

    if (type_supported && has_properties) {
      return i;
    }
  }

  throw std::runtime_error("Failed to find suitable Vulkan memory type.");
}

}  // namespace ndyn::gfx
