#pragma once

#include <cstdint>
#include <vector>

#include "gfx/math.h"

namespace ndyn::gfx {

struct Mesh final {
  std::vector<Vertex> vertices;
  std::vector<std::uint32_t> indices;
};
using MeshId = uint32_t;

}  // namespace ndyn::gfx
