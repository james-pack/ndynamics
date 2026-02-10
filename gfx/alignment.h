#pragma once

#include <type_traits>

namespace ndyn::gfx {

template <typename T>
struct GpuBasicAlignmentCheck final {
 private:
  static constexpr bool is_valid_object_for_gpu_buffers() {
    static_assert(std::is_trivially_copyable_v<T>,
                  "Any types that are shared between host and GPU must be trivially copyable, as "
                  "they will be serialized using memcpy to and from the GPU.");
    static_assert(std::is_standard_layout_v<T>,
                  "Any types that are shared between host and GPU must conform to the C++ standard "
                  "layout where a pointer to the struct T also points to its first data member.");
    static_assert(alignof(T) % 16 == 0,
                  "Any types that are shared between host and GPU must be 16-byte aligned.");
    static_assert(sizeof(T) % 16 == 0,
                  "Any types that are shared between host and GPU must have a size that is a "
                  "multiple of 16-bytes.");
    return true;
  }

 public:
  static constexpr bool value{is_valid_object_for_gpu_buffers()};
};

/**
 * Structure to validate the layout of a struct for use in an SSBO. This struct must be specialized
 * for each type that will be used in an SSBO.
 */
template <typename T>
struct SsboLayoutCheck final {
 private:
  static constexpr bool is_valid() { return false; }

 public:
  static constexpr bool value{is_valid()};
};

}  // namespace ndyn::gfx
