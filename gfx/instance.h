#pragma once

#include "gfx/alignment.h"
#include "gfx/material.h"
#include "gfx/math.h"
#include "gfx/mesh.h"

namespace ndyn::gfx {

struct alignas(16) Instance final {
  Mat4 position{Mat4::identity()};
  MeshId mesh;
  MaterialId material{0};
};
using InstanceId = uint32_t;

template <>
inline constexpr bool SsboLayoutCheck<Instance>::is_valid() {
  static_assert(offsetof(Instance, position) == 0);
  static_assert(offsetof(Instance, mesh) == 64);
  static_assert(offsetof(Instance, material) == 68);
  static_assert(alignof(Instance) == 16);
  static_assert(sizeof(Instance) == 80);
  static_assert(SsboLayoutCheck<Mat4>::value);
  return true;
};

}  // namespace ndyn::gfx
