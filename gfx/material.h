#pragma once

#include <cstddef>

#include "gfx/alignment.h"
#include "gfx/math.h"

namespace ndyn::gfx {

struct alignas(16) Material final {
  Vec4 diffuse_color{};
  Vec4 specular_color{1.f, 1.f, 1.f, 1.f};
  float shininess{0.f};
  float opacity{1.f};
  uint texture_index{};
};
using MaterialId = uint32_t;

template <>
inline constexpr bool SsboLayoutCheck<Material>::is_valid() {
  static_assert(offsetof(Material, diffuse_color) == 0);
  static_assert(offsetof(Material, specular_color) == 16);
  static_assert(offsetof(Material, shininess) == 32);
  static_assert(offsetof(Material, opacity) == 36);
  static_assert(offsetof(Material, texture_index) == 40);
  static_assert(alignof(Material) == 16);
  static_assert(sizeof(Material) == 48);
  static_assert(SsboLayoutCheck<Vec4>::value);
  return true;
}

}  // namespace ndyn::gfx
