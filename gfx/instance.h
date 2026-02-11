#pragma once

#include "gfx/alignment.h"
#include "gfx/material.h"
#include "gfx/math.h"
#include "gfx/mesh.h"

namespace ndyn::gfx {

struct alignas(16) Instance final {
  Mat4 position{Mat4::identity()};
  Vec4 color{0.f, 1.f, 0.f, 1.f};
  MeshId mesh;
  MaterialId material{0};
};
using InstanceId = uint32_t;

template <>
inline constexpr bool SsboLayoutCheck<Instance>::is_valid() {
  static_assert(offsetof(Instance, position) == 0);
  static_assert(offsetof(Instance, color) == 64);
  static_assert(offsetof(Instance, mesh) == 80);
  static_assert(offsetof(Instance, material) == 84);
  static_assert(alignof(Instance) == 16);
  static_assert(sizeof(Instance) == 96);
  static_assert(SsboLayoutCheck<Mat4>::value);
  static_assert(SsboLayoutCheck<Vec4>::value);
  return true;
};

}  // namespace ndyn::gfx
