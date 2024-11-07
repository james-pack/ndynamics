#pragma once

#include <array>
#include <cmath>

#include "GLFW/glfw3.h"
#include "glm/glm.hpp"
#include "graphics/colors.h"
#include "graphics/gpu_element.h"

namespace ndyn::graphics {

namespace {

using VertexType = std::array<GLfloat, 3>;
using ColorType = std::array<GLfloat, 3>;

constexpr VertexType normalized_add(const VertexType& v1, const VertexType& v2) {
  VertexType result{v1[0] + v2[0], v1[1] + v2[1], v1[2] + v2[2]};
  GLfloat norm{std::sqrt(result[0] * result[0] + result[1] * result[1] + result[2] * result[2])};
  result[0] /= norm;
  result[1] /= norm;
  result[2] /= norm;
  return result;
}

// Compute the number of vertices required by the triangles of an icosahedron that is subdivided.
constexpr size_t compute_num_vertices(size_t subdivisions) {
  return 3UL * (20UL << (2 * subdivisions));
}

template <size_t NUM_VERTICES, size_t DEPTH>
struct Subdivide final {
  constexpr size_t operator()(const VertexType& v1, const std::array<GLfloat, 3>& v2,
                              const std::array<GLfloat, 3>& v3,
                              std::array<VertexType, NUM_VERTICES>& result,
                              size_t next_result_index) const {
    VertexType v12{normalized_add(v1, v2)};
    VertexType v23{normalized_add(v2, v3)};
    VertexType v31{normalized_add(v3, v1)};

    next_result_index =
        Subdivide<NUM_VERTICES, DEPTH - 1>{}(v1, v12, v31, result, next_result_index);
    next_result_index =
        Subdivide<NUM_VERTICES, DEPTH - 1>{}(v2, v23, v12, result, next_result_index);
    next_result_index =
        Subdivide<NUM_VERTICES, DEPTH - 1>{}(v3, v31, v23, result, next_result_index);
    next_result_index =
        Subdivide<NUM_VERTICES, DEPTH - 1>{}(v12, v23, v31, result, next_result_index);
    return next_result_index;
  }
};

template <size_t NUM_VERTICES>
struct Subdivide<NUM_VERTICES, 0> final {
  constexpr size_t operator()(const VertexType& v1, const std::array<GLfloat, 3>& v2,
                              const std::array<GLfloat, 3>& v3,
                              std::array<VertexType, NUM_VERTICES>& result,
                              size_t next_result_index) const {
    result[next_result_index++] = v1;
    result[next_result_index++] = v2;
    result[next_result_index++] = v3;
    return next_result_index;
  }
};

template <size_t NUM_SUBDIVISIONS>
constexpr std::array<VertexType, compute_num_vertices(NUM_SUBDIVISIONS)> generate_vertices() {
  // A convex icosahedron can be constructed by positioning 3 rectangles perpendicular to each
  // other and using the vertices of these rectangles as the vertices of the icosahedron. If those
  // rectangles have an aspect ratio of the golden ratio, the result is a regular icosahedron.
  // See https://en.wikipedia.org/wiki/Icosahedron#Cartesian_coordinates for more information,
  // though this source only gives a hint about this construction.
  constexpr double PHI{(1 + std::sqrt(5.)) / 2};
  constexpr double NORM{std::sqrt(PHI * PHI + 1.)};
  constexpr GLfloat X{1 / NORM};
  constexpr GLfloat Z{PHI / NORM};

  // The vertices of a regular icosahedron of radius 1.
  constexpr std::array<VertexType, 12> icosahedron_vertices{
      VertexType{-X, 0.0, Z},  VertexType{X, 0.0, Z},
      VertexType{-X, 0.0, -Z}, VertexType{X, 0.0, -Z},  //
      VertexType{0.0, Z, X},   VertexType{0.0, Z, -X},
      VertexType{0.0, -Z, X},  VertexType{0.0, -Z, -X},  //
      VertexType{Z, X, 0.0},   VertexType{-Z, X, 0.0},
      VertexType{Z, -X, 0.0},  VertexType{-Z, -X, 0.0}  //
  };

  using VertexIndices = std::array<size_t, 3>;
  // Indices of the icosahedron_vertices that will render them as CCW triangles.
  constexpr std::array<VertexIndices, 20> triangle_indices{
      VertexIndices{0, 4, 1},  VertexIndices{0, 9, 4},
      VertexIndices{9, 5, 4},  VertexIndices{4, 5, 8},  //
      VertexIndices{4, 8, 1},  VertexIndices{8, 10, 1},
      VertexIndices{8, 3, 10}, VertexIndices{5, 3, 8},  //
      VertexIndices{5, 2, 3},  VertexIndices{2, 7, 3},
      VertexIndices{7, 10, 3}, VertexIndices{7, 6, 10},  //
      VertexIndices{7, 11, 6}, VertexIndices{11, 0, 6},
      VertexIndices{0, 1, 6},  VertexIndices{6, 1, 10},  //
      VertexIndices{9, 0, 11}, VertexIndices{9, 11, 2},
      VertexIndices{9, 2, 5},  VertexIndices{7, 2, 11}  //
  };

  constexpr size_t NUM_VERTICES{compute_num_vertices(NUM_SUBDIVISIONS)};
  std::array<VertexType, NUM_VERTICES> result{};
  size_t next_result_index{0};
  for (const VertexIndices& triangle_index : triangle_indices) {
    next_result_index =
        Subdivide<NUM_VERTICES, NUM_SUBDIVISIONS>{}(icosahedron_vertices[triangle_index[0]],  //
                                                    icosahedron_vertices[triangle_index[1]],  //
                                                    icosahedron_vertices[triangle_index[2]],  //
                                                    result, next_result_index);
  }
  return result;
}

}  // namespace

template <typename GeometryT,
          // NUM_SUBDIVISIONS tells how good of an approximation of the sphere to generate. Zero
          // gives an icosahedron; 1 gives a mediocre approximation of a sphere but will be very
          // fast to compute and render; 2 gives a good approximation of a sphere; 4 will give an
          // extremely good approximation but may be slow to compute and render.
          size_t NUM_SUBDIVISIONS = 2>
class Sphere final : public GpuElement<GeometryT> {
 public:
  using GeometryType = GeometryT;
  using AlgebraType = typename GeometryType::AlgebraType;
  using ScalarType = typename AlgebraType::ScalarType;
  using VectorType = typename AlgebraType::VectorType;

 private:
  static constexpr size_t NUM_VERTICES{compute_num_vertices(NUM_SUBDIVISIONS)};

  // Vertices for a radius 1 subdivided regular icosahedron computed at compile-time.
  static constexpr std::array<VertexType, NUM_VERTICES> vertices_{
      generate_vertices<NUM_SUBDIVISIONS>()};

  static constexpr std::array<ColorType, NUM_VERTICES> vertex_colors_{
      rainbow_colors<NUM_VERTICES>()};

  GLuint vertex_buffer_{};
  GLuint color_buffer_{};

  const glm::mat3 scale_matrix_;

 public:
  Sphere(const ScalarType& width, const ScalarType& length, const ScalarType& height)
      : scale_matrix_(glm::mat3(width, 0, 0, 0, length, 0, 0, 0, height)) {
    GLuint vertex_array_id;
    glGenVertexArrays(1, &vertex_array_id);
    glBindVertexArray(vertex_array_id);

    // Generate 1 buffer, put the resulting identifier in vertex_buffer_
    glGenBuffers(1, &vertex_buffer_);
    // The following commands will talk about our 'vertex_buffer_' buffer
    glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_);
    // Give our vertices to OpenGL.
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices_), vertices_.data(), GL_STATIC_DRAW);

    glGenBuffers(1, &color_buffer_);
    glBindBuffer(GL_ARRAY_BUFFER, color_buffer_);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertex_colors_), vertex_colors_.data(), GL_STATIC_DRAW);
  }

  Sphere(const ScalarType& scale) : Sphere(scale, scale, scale) {}
  Sphere() : Sphere(1, 1, 1) {}

  void draw(ScalarType /*time*/, const ShaderProgram& program) override {
    const GLuint scale_matrix_id{glGetUniformLocation(program.id(), "scale_matrix")};
    glUniformMatrix3fv(scale_matrix_id, 1, GL_FALSE, &scale_matrix_[0][0]);

    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_);
    glVertexAttribPointer(
        0,  // attribute 0. No particular reason for 0, but must match the layout in the shader.
        3,  // size
        GL_FLOAT,  // type
        GL_FALSE,  // normalized?
        0,         // stride
        (void*)0   // array buffer offset
    );

    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, color_buffer_);
    glVertexAttribPointer(
        1,  // attribute 1. No particular reason for 1, but must match the layout in the shader.
        3,  // size
        GL_FLOAT,  // type
        GL_FALSE,  // normalized?
        0,         // stride
        (void*)0   // array buffer offset
    );

    // Draw the triangles for the sphere.
    glDrawArrays(GL_TRIANGLES, 0, NUM_VERTICES);
    glDisableVertexAttribArray(0);
  }

  constexpr const ScalarType& width() const { return scale_matrix_[0][0]; }

  constexpr const ScalarType& length() const { return scale_matrix_[1][1]; }

  constexpr const ScalarType& height() const { return scale_matrix_[2][2]; }
};

}  // namespace ndyn::graphics
