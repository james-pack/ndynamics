#pragma once

#include <vulkan/vulkan.h>

#include <algorithm>
#include <cstdint>
#include <cstring>
#include <stdexcept>

#include "gfx/vulkan_utils.h"

namespace ndyn::gfx {

// Alignment helpers.
template <size_t SIZE, size_t ALIGNMENT>
constexpr size_t align_up() {
  static_assert(ALIGNMENT != 0, "ALIGNMENT must be non-zero");
  return (SIZE + ALIGNMENT - 1) & ~(ALIGNMENT - 1);
}

template <size_t ALIGNMENT>
constexpr size_t align_up(size_t size) {
  static_assert(ALIGNMENT != 0, "ALIGNMENT must be non-zero");
  return (size + ALIGNMENT - 1) & ~(ALIGNMENT - 1);
}

inline size_t align_up(size_t size, size_t alignment) {
  if (alignment == 0) return size;
  return (size + alignment - 1) & ~(alignment - 1);
}

/**
 * Allocated block with pointer to memory and an offset to use in a vkCmdBindDescriptorSets() call.
 */
struct UboAllocation final {
  void* ptr{nullptr};
  uint32_t dynamic_offset{0};
};

template <size_t PER_FRAME_SIZE, uint32_t FRAMES_IN_FLIGHT>
class UboAllocator;

template <size_t PER_FRAME_SIZE, uint32_t FRAMES_IN_FLIGHT>
class FrameScope final {
 private:
  UboAllocator<PER_FRAME_SIZE, FRAMES_IN_FLIGHT>* allocator_;

  FrameScope(UboAllocator<PER_FRAME_SIZE, FRAMES_IN_FLIGHT>& allocator, uint32_t frame_index)
      : allocator_(&allocator) {
    allocator_->begin_frame_internal(frame_index);
  }

  friend class UboAllocator<PER_FRAME_SIZE, FRAMES_IN_FLIGHT>;

 public:
  ~FrameScope() { allocator_->flush_internal(); }

  UboAllocation allocate(size_t size) { return allocator_->allocate_internal(size); }

  template <size_t SIZE>
  UboAllocation allocate() {
    return allocator_->allocate_internal(SIZE);
  }
};

/**
 * A persistently-mapped, per-frame linear allocator for dynamic UBO usage.
 *
 * Typical usage:
 *   auto frame = allocator.begin_frame(frame_index);
 *   auto a = frame.allocate<sizeof(MyUbo)>();
 *   memcpy(a.ptr, &ubo, sizeof(MyUbo));
 *   vkCmdBindDescriptorSets(..., 1, &a.dynamic_offset);
 */
template <size_t PER_FRAME_SIZE, uint32_t FRAMES_IN_FLIGHT>
class UboAllocator final {
 public:
  static_assert(FRAMES_IN_FLIGHT > 0, "FRAMES_IN_FLIGHT must be > 0");
  static_assert(PER_FRAME_SIZE > 0, "PER_FRAME_SIZE must be > 0");

 private:
  VkDevice device_;
  VkPhysicalDevice physical_device_;

  VkBuffer buffer_{VK_NULL_HANDLE};
  VkDeviceMemory memory_{VK_NULL_HANDLE};
  uint8_t* mapped_{nullptr};

  VkDeviceSize alignment_{1};
  VkDeviceSize non_coherent_atom_size_{1};
  VkDeviceSize per_frame_size_runtime_{0};

  uint32_t current_frame_{0};
  VkDeviceSize frame_base_{0};
  VkDeviceSize frame_offset_{0};

  void begin_frame_internal(uint32_t frame_index) {
    current_frame_ = frame_index % FRAMES_IN_FLIGHT;
    frame_base_ = per_frame_size_runtime_ * current_frame_;
    frame_offset_ = 0;
  }

  UboAllocation allocate_internal(VkDeviceSize size) {
    const VkDeviceSize aligned = align_up(size, alignment_);
    if (frame_offset_ + aligned > per_frame_size_runtime_) {
      throw std::runtime_error("UboAllocator: frame arena exhausted");
    }

    const VkDeviceSize offset = frame_base_ + frame_offset_;
    void* ptr = mapped_ + offset;
    frame_offset_ += aligned;

    UboAllocation a{};
    a.ptr = ptr;
    a.dynamic_offset = static_cast<uint32_t>(offset);
    return a;
  }

  void flush_internal() {
    const VkDeviceSize start = align_up(frame_base_, non_coherent_atom_size_);
    const VkDeviceSize end = align_up(frame_base_ + frame_offset_, non_coherent_atom_size_);
    if (end <= start) return;

    VkMappedMemoryRange range{};
    range.sType = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE;
    range.memory = memory_;
    range.offset = start;
    range.size = end - start;
    vkFlushMappedMemoryRanges(device_, 1, &range);
  }

  friend class FrameScope<PER_FRAME_SIZE, FRAMES_IN_FLIGHT>;

 public:
  UboAllocator(VkDevice device, VkPhysicalDevice physical_device)
      : device_(device), physical_device_(physical_device) {
    VkPhysicalDeviceProperties props{};
    vkGetPhysicalDeviceProperties(physical_device_, &props);
    alignment_ = std::max<VkDeviceSize>(props.limits.minUniformBufferOffsetAlignment, 1);
    non_coherent_atom_size_ = std::max<VkDeviceSize>(props.limits.nonCoherentAtomSize, 1);

    per_frame_size_runtime_ = align_up(PER_FRAME_SIZE, alignment_);
    const VkDeviceSize total_size = per_frame_size_runtime_ * FRAMES_IN_FLIGHT;

    VkBufferCreateInfo bci{};
    bci.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bci.size = total_size;
    bci.usage = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
    bci.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    if (vkCreateBuffer(device_, &bci, nullptr, &buffer_) != VK_SUCCESS) {
      throw std::runtime_error("UboAllocator: vkCreateBuffer failed");
    }

    VkMemoryRequirements reqs{};
    vkGetBufferMemoryRequirements(device_, buffer_, &reqs);

    const VkMemoryPropertyFlags desired =
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;

    VkMemoryAllocateInfo mai{};
    mai.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    mai.allocationSize = reqs.size;
    mai.memoryTypeIndex = find_memory_type(physical_device_, reqs.memoryTypeBits, desired);

    if (vkAllocateMemory(device_, &mai, nullptr, &memory_) != VK_SUCCESS) {
      throw std::runtime_error("UboAllocator: vkAllocateMemory failed");
    }

    if (vkBindBufferMemory(device_, buffer_, memory_, 0) != VK_SUCCESS) {
      throw std::runtime_error("UboAllocator: vkBindBufferMemory failed");
    }

    if (vkMapMemory(device_, memory_, 0, VK_WHOLE_SIZE, 0, reinterpret_cast<void**>(&mapped_)) !=
        VK_SUCCESS) {
      throw std::runtime_error("UboAllocator: vkMapMemory failed");
    }
  }

  ~UboAllocator() {
    if (mapped_) {
      vkUnmapMemory(device_, memory_);
    }
    if (buffer_ != VK_NULL_HANDLE) {
      vkDestroyBuffer(device_, buffer_, nullptr);
    }
    if (memory_ != VK_NULL_HANDLE) {
      vkFreeMemory(device_, memory_, nullptr);
    }
  }

  auto begin_frame(uint32_t frame_index) {
    return FrameScope<PER_FRAME_SIZE, FRAMES_IN_FLIGHT>(*this, frame_index);
  }

  VkBuffer buffer() const { return buffer_; }

  VkDescriptorBufferInfo descriptor_info() const {
    VkDescriptorBufferInfo info{};
    info.buffer = buffer_;
    info.offset = 0;
    info.range = per_frame_size_runtime_;
    return info;
  }
};

}  // namespace ndyn::gfx
