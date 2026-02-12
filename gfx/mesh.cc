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

Mesh create_cube(float side_length) {
  const float h{0.5f * side_length};

  Mesh mesh;
  mesh.vertices.reserve(24);
  mesh.indices.reserve(36);

  auto add_face = [&](float nx, float ny, float nz, Vertex v0, Vertex v1, Vertex v2, Vertex v3) {
    const std::uint32_t base = static_cast<std::uint32_t>(mesh.vertices.size());

    // Assign normals.
    v0.nx = nx;
    v0.ny = ny;
    v0.nz = nz;

    v1.nx = nx;
    v1.ny = ny;
    v1.nz = nz;

    v2.nx = nx;
    v2.ny = ny;
    v2.nz = nz;

    v3.nx = nx;
    v3.ny = ny;
    v3.nz = nz;

    mesh.vertices.push_back(v0);
    mesh.vertices.push_back(v1);
    mesh.vertices.push_back(v2);
    mesh.vertices.push_back(v3);

    // Two triangles per face.
    mesh.indices.insert(mesh.indices.end(),
                        {base + 0, base + 1, base + 2, base + 0, base + 2, base + 3});
  };

  // +Z (front)
  add_face(0, 0, 1,  //
           {-h, -h, +h}, {+h, -h, +h}, {+h, +h, +h}, {-h, +h, +h});

  // -Z (back)
  add_face(0, 0, -1,  //
           {+h, -h, -h}, {-h, -h, -h}, {-h, +h, -h}, {+h, +h, -h});

  // +X (right)
  add_face(1, 0, 0,  //
           {+h, -h, +h}, {+h, -h, -h}, {+h, +h, -h}, {+h, +h, +h});

  // -X (left)
  add_face(-1, 0, 0,  //
           {-h, -h, -h}, {-h, -h, +h}, {-h, +h, +h}, {-h, +h, -h});

  // +Y (top)
  add_face(0, 1, 0,  //
           {-h, +h, +h}, {+h, +h, +h}, {+h, +h, -h}, {-h, +h, -h});

  // -Y (bottom)
  add_face(0, -1, 0,  //
           {-h, -h, -h}, {+h, -h, -h}, {+h, -h, +h}, {-h, -h, +h});

  return mesh;
}

Mesh create_cylinder(float radius, float height, unsigned int segments) {
  Mesh mesh{};
  const float half_height = height * 0.5f;

  // Generate vertices
  for (unsigned int i = 0; i <= segments; ++i) {
    float theta = 2.0f * M_PI * float(i) / float(segments);
    float cos_theta = std::cos(theta);
    float sin_theta = std::sin(theta);

    // Top circle
    mesh.vertices.push_back(
        {radius * cos_theta, half_height, radius * sin_theta, cos_theta, 0.f, sin_theta});
    // Bottom circle
    mesh.vertices.push_back(
        {radius * cos_theta, -half_height, radius * sin_theta, cos_theta, 0.f, sin_theta});
  }

  // Top and bottom center vertices
  auto top_center_index = mesh.vertices.size();
  mesh.vertices.push_back({0.0f, half_height, 0.0f, 0.0f, 1.0f, 0.0f});

  auto bottom_center_index = mesh.vertices.size();
  mesh.vertices.push_back({0.0f, -half_height, 0.0f, 0.0f, -1.0f, 0.0f});

  // Generate side indices
  for (unsigned int i = 0; i < segments; ++i) {
    unsigned int top1 = 2 * i;
    unsigned int bottom1 = 2 * i + 1;
    unsigned int top2 = 2 * (i + 1);
    unsigned int bottom2 = 2 * (i + 1) + 1;

    // First triangle of quad
    mesh.indices.push_back(top1);
    mesh.indices.push_back(bottom1);
    mesh.indices.push_back(top2);

    // Second triangle of quad
    mesh.indices.push_back(top2);
    mesh.indices.push_back(bottom1);
    mesh.indices.push_back(bottom2);
  }

  // Top cap
  for (unsigned int i = 0; i < segments; ++i) {
    unsigned int top = 2 * i;
    unsigned int top_next = 2 * (i + 1);
    mesh.indices.push_back(top_center_index);
    mesh.indices.push_back(top);
    mesh.indices.push_back(top_next);
  }

  // Bottom cap
  for (unsigned int i = 0; i < segments; ++i) {
    unsigned int bottom = 2 * i + 1;
    unsigned int bottom_next = 2 * (i + 1) + 1;
    mesh.indices.push_back(bottom_center_index);
    mesh.indices.push_back(bottom_next);
    mesh.indices.push_back(bottom);
  }

  return mesh;
}

}  // namespace ndyn::gfx
