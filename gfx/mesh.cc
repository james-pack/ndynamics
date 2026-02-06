#include "gfx/mesh.h"

#include <cmath>

namespace ndyn::gfx {

Mesh create_triangle(float s) {
  const float half{s / 2};
  Mesh m;
  m.vertices = {
      {0, -half, 0, 0, 0, 1},
      {half, half, 0, 0, 0, 1},
      {-half, half, 0, 0, 0, 1},
  };
  m.indices = {0, 1, 2};
  return m;
}

Mesh create_cube(float s) {
  const float half{s / 2};
  Mesh m;
  m.vertices = {
      {-half, -half, -half, 0, 0, -1}, {half, -half, -half, 0, 0, -1},
      {half, half, -half, 0, 0, -1},   {-half, half, -half, 0, 0, -1},
      {-half, -half, half, 0, 0, 1},   {half, -half, half, 0, 0, 1},
      {half, half, half, 0, 0, 1},     {-half, half, half, 0, 0, 1},
  };
  m.indices = {0, 1, 2, 2, 3, 0, 4, 6, 5, 6, 4, 7, 0, 4, 5, 5, 1, 0,
               1, 5, 6, 6, 2, 1, 2, 6, 7, 7, 3, 2, 3, 7, 4, 4, 0, 3};
  return m;
}

Mesh create_cylinder(float radius, float height, unsigned int segments) {
  Mesh m;
  const float half{height * 0.5f};
  for (unsigned int i = 0; i < segments; ++i) {
    float a = (2.f * M_PI * i) / segments;
    float x = radius * std::cos(a);
    float z = radius * std::sin(a);
    m.vertices.push_back({x, -half, z, x, 0, z});
    m.vertices.push_back({x, half, z, x, 0, z});
  }
  for (unsigned int i = 0; i < segments; ++i) {
    unsigned int n = (i + 1) % segments;
    m.indices.insert(m.indices.end(), {2 * i, 2 * n, 2 * i + 1, 2 * i + 1, 2 * n, 2 * n + 1});
  }
  return m;
}

}  // namespace ndyn::gfx
