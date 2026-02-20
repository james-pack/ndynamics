#pragma once

#include <unordered_map>
#include <utility>
#include <vector>

#include "gfx/mesh.h"

namespace ndyn::gfx {

Mesh create_triangle(float side_length);
Mesh create_cube(float size);
Mesh create_cylinder(float radius, float height, unsigned int segments);

namespace internal {

struct EdgeKey final {
  uint32_t a;
  uint32_t b;

  EdgeKey(uint32_t v0, uint32_t v1) : a(v0), b(v1) {
    if (v0 > v1) {
      std::swap(a, b);
    }
  }

  bool operator==(const EdgeKey& rhs) const { return a == rhs.a && b == rhs.b; }
};

struct EdgeHasher final {
  size_t operator()(const EdgeKey& k) const noexcept {
    return (static_cast<size_t>(k.a) << 32) ^ k.b;
  }
};

}  // namespace internal

template <uint16_t SUBDIVISIONS>
Mesh create_icosphere(float radius) {
  using namespace internal;

  Mesh mesh{
      .indices = {0, 11, 5,  0, 5,  1, 0, 1, 7, 0, 7,  10, 0, 10, 11, 1, 5, 9, 5, 11,
                  4, 11, 10, 2, 10, 7, 6, 7, 1, 8, 3,  9,  4, 3,  4,  2, 3, 2, 6, 3,
                  6, 8,  3,  8, 9,  4, 9, 5, 2, 4, 11, 6,  2, 10, 8,  6, 7, 9, 8, 1},
  };

  // Preallocate capacity for the vertices of an icosphere with SUBDIVISIONS levels.
  // Vertex count:  V(s) = 10 * 4^s + 2
  // 4^s is computed as (1u << (2 * SUBDIVISIONS)).
  mesh.vertices.reserve(10 * (1u << (2 * SUBDIVISIONS)) + 2);

  constexpr float t = (1.0f + std::sqrt(5.0f)) * 0.5f;

  auto add_initial = [&](float x, float y, float z) {
    Vec3 v{x, y, z};
    mesh.vertices.push_back(Vertex::from_direction(v, radius));
  };

  // 12 icosahedron vertices
  add_initial(-1, t, 0);
  add_initial(1, t, 0);
  add_initial(-1, -t, 0);
  add_initial(1, -t, 0);
  add_initial(0, -1, t);
  add_initial(0, 1, t);
  add_initial(0, -1, -t);
  add_initial(0, 1, -t);
  add_initial(t, 0, -1);
  add_initial(t, 0, 1);
  add_initial(-t, 0, -1);
  add_initial(-t, 0, 1);

  for (uint16_t s = 0; s < SUBDIVISIONS; ++s) {
    std::unordered_map<EdgeKey, uint32_t, EdgeHasher> midpoint_cache;
    std::vector<uint32_t> new_indices;

    // Preallocate capacity for the indices of an icosphere with s subdivision levels.
    // Index count:   I(s) = 60 * 4^s   (3 indices per triangle, 20 * 4^s triangles)
    // Each level needs 4x the indices of the previous level.
    new_indices.reserve(4 * mesh.indices.size());

    auto midpoint = [&](uint32_t i0, uint32_t i1) -> uint32_t {
      EdgeKey key{i0, i1};

      auto it = midpoint_cache.find(key);
      if (it != midpoint_cache.end()) return it->second;

      const Vertex& v0 = mesh.vertices[i0];
      const Vertex& v1 = mesh.vertices[i1];

      Vec3 mid{(v0.nx + v1.nx) * 0.5f, (v0.ny + v1.ny) * 0.5f, (v0.nz + v1.nz) * 0.5f};

      uint32_t index{static_cast<uint32_t>(mesh.vertices.size())};
      mesh.vertices.push_back(Vertex::from_direction(mid, radius));
      midpoint_cache.emplace(key, index);

      return index;
    };

    for (size_t i = 0; i < mesh.indices.size(); i += 3) {
      uint32_t v0 = mesh.indices[i + 0];
      uint32_t v1 = mesh.indices[i + 1];
      uint32_t v2 = mesh.indices[i + 2];

      uint32_t m01 = midpoint(v0, v1);
      uint32_t m12 = midpoint(v1, v2);
      uint32_t m20 = midpoint(v2, v0);

      new_indices.insert(new_indices.end(),
                         {v0, m01, m20, v1, m12, m01, v2, m20, m12, m01, m12, m20});
    }

    mesh.indices.swap(new_indices);
  }

  return mesh;
}

}  // namespace ndyn::gfx
