#pragma once

#include <type_traits>

namespace ndyn::gfx {

struct alignas(16) GpuStdLayout {};

template <typename T>
struct GpuLayoutCheck final {
 private:
  static constexpr bool is_valid_object_for_gpu_buffers() {
    static_assert(std::is_trivially_copyable_v<T>,
                  "Any types that are shared between host and GPU must be trivially copyable, as "
                  "they will be serialized using memcpy to and from the GPU.");
    static_assert(std::is_standard_layout_v<T>,
                  "Any types that are shared between host and GPU must conform to the C++ standard "
                  "layout where a pointer to the struct T also points to its first data member.");
    static_assert(alignof(T) % 16 == 0,
                  "Any types that are shared between host and GPU must be 16-byte aligned");
    static_assert(sizeof(T) % 16 == 0,
                  "Any types that are shared between host and GPU must have 16-byte size");
    return true;
  }

 public:
  static constexpr bool value{is_valid_object_for_gpu_buffers()};
};

// Double-check that the standard layout passes the layout check.
static_assert(GpuLayoutCheck<GpuStdLayout>::value,
              "The GpuStdLayout must pass the GpuLayoutCheck else classes using GpuStdLayout may "
              "not abide the layout rules for use in GPU buffers (SSBO, UBO)");

}  // namespace ndyn::gfx
