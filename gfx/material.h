#pragma once

#include <cstddef>

#include "gfx/alignment.h"
#include "gfx/math.h"

namespace ndyn::gfx {

struct Material final : GpuStdAlign {
  Vec4 diffuse_color;
  Vec4 specular_color;
  float shininess;
  float opacity;
  uint texture_index;
};

template <>
inline constexpr bool SsboLayoutCheck<Material>::is_valid() {
  return offsetof(Material, diffuse_color) % 16 == 0 and   //
         offsetof(Material, specular_color) % 16 == 0 and  //
         offsetof(Material, shininess) % 16 == 0 and       //
         offsetof(Material, opacity) % 4 == 0 and          //
         offsetof(Material, texture_index) % 4 == 0;
}

}  // namespace ndyn::gfx
