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

Mesh create_triangle(float side_length);
Mesh create_cube(float size);
Mesh create_cylinder(float radius, float height, unsigned int segments);

}  // namespace ndyn::gfx
