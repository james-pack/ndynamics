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

constexpr size_t compute_num_vertices(size_t sides, size_t layers) {
  size_t result{};
  // The number of vertices in the circles at the ends of the cylinder. Two circles with one
  // triangle (3 vertices) per side.
  result += 2 * 3 * sides;
  // For each layer, we will have two triangles for each side to form a quadrilateral, but we also
  // add an additional set of triangles for the space between one of the circles at the end of the
  // cylinder and the first layer.
  result += (layers + 1) * 2 * 3 * sides;
  return result;
}

template <size_t NUM_SIDES, size_t NUM_LAYERS>
constexpr std::array<VertexType, compute_num_vertices(NUM_SIDES, NUM_LAYERS)> generate_vertices() {
  using std::cos;
  using std::sin;

  std::array<VertexType, compute_num_vertices(NUM_SIDES, NUM_LAYERS)> result{};

  size_t index{};

  // Bottom circle.
  for (size_t i = 0; i < NUM_SIDES; ++i) {
    result[index++] = {0, -1, 0};
    result[index++] = {cos(2 * pi * i / NUM_SIDES), -1, sin(2 * pi * i / NUM_SIDES)};
    result[index++] = {cos(2 * pi * (i + 1) / NUM_SIDES), -1, sin(2 * pi * (i + 1) / NUM_SIDES)};
  }

  constexpr const float height{2. / (NUM_LAYERS + 1)};
  for (size_t layer = 0; layer < NUM_LAYERS + 1; ++layer) {
    for (size_t i = 0; i < NUM_SIDES; ++i) {
      result[index++] = {cos(2 * pi * i / NUM_SIDES), layer * height - 1,
                         sin(2 * pi * i / NUM_SIDES)};
      result[index++] = {cos(2 * pi * (i + 1) / NUM_SIDES), layer * height - 1,
                         sin(2 * pi * (i + 1) / NUM_SIDES)};
      result[index++] = {cos(2 * pi * i / NUM_SIDES), (layer + 1) * height - 1,
                         sin(2 * pi * i / NUM_SIDES)};

      result[index++] = {cos(2 * pi * i / NUM_SIDES), (layer + 1) * height - 1,
                         sin(2 * pi * i / NUM_SIDES)};
      result[index++] = {cos(2 * pi * (i + 1) / NUM_SIDES), layer * height - 1,
                         sin(2 * pi * (i + 1) / NUM_SIDES)};
      result[index++] = {cos(2 * pi * (i + 1) / NUM_SIDES), (layer + 1) * height - 1,
                         sin(2 * pi * (i + 1) / NUM_SIDES)};
    }
  }

  // Top circle.
  for (size_t i = 0; i < NUM_SIDES; ++i) {
    result[index++] = {0, 1, 0};
    result[index++] = {cos(2 * pi * i / NUM_SIDES), 1, sin(2 * pi * i / NUM_SIDES)};
    result[index++] = {cos(2 * pi * (i + 1) / NUM_SIDES), 1, sin(2 * pi * (i + 1) / NUM_SIDES)};
  }

  return result;
}

}  // namespace

template <typename GeometryT, size_t NUM_SIDES = 15, size_t NUM_LAYERS = 0>
class Cylinder final : public GpuElement<GeometryT> {
 public:
  using GeometryType = GeometryT;
  using AlgebraType = typename GeometryType::AlgebraType;
  using ScalarType = typename AlgebraType::ScalarType;
  using VectorType = typename AlgebraType::VectorType;

 private:
  static constexpr size_t NUM_VERTICES{compute_num_vertices(NUM_SIDES, NUM_LAYERS)};

  // Vertices for a radius 1 cylinder with the specified sides and layers computed at compile-time.
  static constexpr std::array<VertexType, NUM_VERTICES> vertices_{
      generate_vertices<NUM_SIDES, NUM_LAYERS>()};

  static constexpr std::array<ColorType, NUM_VERTICES> vertex_colors_{
      rainbow_colors<NUM_VERTICES>()};

  GLuint vertex_buffer_{};
  GLuint color_buffer_{};

  const glm::mat3 scale_matrix_;

 public:
  Cylinder(const ScalarType& width, const ScalarType& length, const ScalarType& height)
      : scale_matrix_(glm::mat3(width, 0, 0, 0, height, 0, 0, 0, length)) {
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

  Cylinder(const ScalarType& radius, const ScalarType& height) : Cylinder(radius, radius, height) {}
  Cylinder() : Cylinder(1, 1, 1) {}

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

    // Draw the triangles for the cylinder.
    glDrawArrays(GL_TRIANGLES, 0, NUM_VERTICES);
    glDisableVertexAttribArray(0);
  }

  constexpr const ScalarType& width() const { return scale_matrix_[0][0]; }

  constexpr const ScalarType& length() const { return scale_matrix_[1][1]; }

  constexpr const ScalarType& height() const { return scale_matrix_[2][2]; }
};

}  // namespace ndyn::graphics
