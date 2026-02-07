#pragma once

#include <vulkan/vulkan.h>

#include <algorithm>
#include <cstring>
#include <limits>
#include <stdexcept>
#include <vector>

#include "gfx/alignment.h"

namespace ndyn::gfx {

template <typename Object>
class Updater;

template <typename Object>
class SsboBuffer final {
 public:
  static_assert(GpuLayoutCheck<Object>::value,
                "SsboBuffer Objects must conform to the required GPU layout.");

 private:
  VkDevice device_;
  VkPhysicalDevice physical_device_;
  VkBuffer buffer_{VK_NULL_HANDLE};
  VkDeviceMemory memory_{VK_NULL_HANDLE};
  char* mapped_memory_{nullptr};
  size_t capacity_{0};

  void allocate_buffer(size_t capacity) {
    capacity_ = capacity;
    const VkDeviceSize buffer_size{capacity_ * sizeof(Object)};

    VkBufferCreateInfo buffer_info{};
    buffer_info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    buffer_info.size = buffer_size;
    buffer_info.usage = VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT |
                        VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
    buffer_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    if (vkCreateBuffer(device_, &buffer_info, nullptr, &buffer_) != VK_SUCCESS) {
      throw std::runtime_error("Failed to create SSBO buffer");
    }

    VkMemoryRequirements mem_reqs;
    vkGetBufferMemoryRequirements(device_, buffer_, &mem_reqs);

    VkMemoryAllocateInfo alloc_info{};
    alloc_info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    alloc_info.allocationSize = mem_reqs.size;
    alloc_info.memoryTypeIndex =
        find_memory_type(mem_reqs.memoryTypeBits, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT);

    if (vkAllocateMemory(device_, &alloc_info, nullptr, &memory_) != VK_SUCCESS) {
      throw std::runtime_error("Failed to allocate SSBO memory");
    }

    vkBindBufferMemory(device_, buffer_, memory_, 0);

    vkMapMemory(device_, memory_, 0, buffer_size, 0, reinterpret_cast<void**>(&mapped_memory_));
  }

  void reallocate(size_t new_capacity) {
    VkBuffer old_buffer{buffer_};
    VkDeviceMemory old_memory{memory_};
    const char* const old_mapped{mapped_memory_};
    const size_t old_capacity{capacity_};

    allocate_buffer(new_capacity);
    std::memcpy(mapped_memory_, old_mapped, old_capacity * sizeof(Object));

    vkUnmapMemory(device_, old_memory);
    vkDestroyBuffer(device_, old_buffer, nullptr);
    vkFreeMemory(device_, old_memory, nullptr);
  }

  uint32_t find_memory_type(uint32_t type_filter, VkMemoryPropertyFlags properties) const {
    VkPhysicalDeviceMemoryProperties mem_properties;
    vkGetPhysicalDeviceMemoryProperties(physical_device_, &mem_properties);

    for (uint32_t i = 0; i < mem_properties.memoryTypeCount; ++i) {
      if ((type_filter & (1 << i)) &&
          (mem_properties.memoryTypes[i].propertyFlags & properties) == properties) {
        return i;
      }
    }
    throw std::runtime_error("Failed to find suitable memory type");
  }

  template <typename Iter>
  void update(size_t pos, Iter first, Iter last) {
    size_t index{pos};
    for (Iter it = first; it != last; ++it) {
      if (index >= capacity_) {
        throw std::out_of_range("Invalid index");
      }
      std::memcpy(mapped_memory_ + index, &(*it), sizeof(Object));
      ++index;
    }
  }

  void update(size_t pos, const Object& obj) {
    std::memcpy(mapped_memory_ + pos, &obj, sizeof(Object));
  }

  void flush() {
    VkMappedMemoryRange range{};
    range.sType = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE;
    range.memory = memory_;
    range.offset = 0;
    range.size = capacity_ * sizeof(Object);
    vkFlushMappedMemoryRanges(device_, 1, &range);
  }

  void reserve(size_t new_capacity) {
    if (new_capacity > capacity_) {
      reallocate(new_capacity);
    }
  }

  friend class Updater<Object>;

 public:
  SsboBuffer(VkDevice device, VkPhysicalDevice physical_device, size_t initial_capacity = 1024)
      : device_(device), physical_device_(physical_device) {
    allocate_buffer(initial_capacity);
  }

  ~SsboBuffer() {
    if (mapped_memory_) {
      vkUnmapMemory(device_, memory_);
    }
    if (buffer_ != VK_NULL_HANDLE) {
      vkDestroyBuffer(device_, buffer_, nullptr);
    }
    if (memory_ != VK_NULL_HANDLE) {
      vkFreeMemory(device_, memory_, nullptr);
    }
  }

  const Object& operator[](size_t index) const {
    if (index >= capacity_) {
      throw std::out_of_range("Invalid index");
    }

    return *static_cast<const Object*>(mapped_memory_ + index * sizeof(Object));
  }

  Updater<Object> begin_updates() {
    // Invalidate any caches of this buffer's data on the host.
    VkMappedMemoryRange range{};
    range.sType = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE;
    range.memory = memory_;
    range.offset = 0;
    range.size = capacity_ * sizeof(Object);
    vkInvalidateMappedMemoryRanges(device_, 1, &range);

    return Updater<Object>(*this);
  }

  size_t capacity() const { return capacity_; }

  VkBuffer buffer() const { return buffer_; }
};

/**
 * RAII class for managing updates to an SSBO buffer.
 */
template <typename Object>
class Updater final {
 private:
  SsboBuffer<Object>* buffer_;

  Updater(SsboBuffer<Object>& buffer) : buffer_(&buffer) {}

  friend class SsboBuffer<Object>;

 public:
  ~Updater() { buffer_->flush(); }

  void reserve(size_t new_capacity) { buffer_->reserve(new_capacity); }

  void update(size_t pos, const Object& obj) { buffer_->update(pos, obj); }

  template <typename Iter>
  void update(Iter first, Iter last) {
    buffer_->update(0, first, last);
  }

  template <typename Iter>
  void update(size_t pos, Iter first, Iter last) {
    buffer_->update(pos, first, last);
  }
};

}  // namespace ndyn::gfx
