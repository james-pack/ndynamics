#pragma once

#include <cstdint>
#include <vector>

#include "gfx/math.h"

namespace ndyn::gfx {

struct Mesh final {
  Transform position{};
  std::vector<Vertex> vertices;
  std::vector<std::uint32_t> indices;
};

Mesh create_cube(float size);
Mesh create_cylinder(float radius, float height, unsigned int segments);

}  // namespace ndyn::gfx
