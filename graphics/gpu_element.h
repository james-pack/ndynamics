#pragma once

#include <memory>
#include <string>
#include <vector>

#include "GLFW/glfw3.h"
#include "glm/glm.hpp"
#include "graphics/shader_program.h"
#include "math/geometry.h"

namespace ndyn::graphics {

std::string to_string(const glm::mat2x4& mat) {
  using std::to_string;
  std::string result{};
  result.append("[")
      .append(to_string(mat[0][0]))
      .append(", ")
      .append(to_string(mat[0][1]))
      .append(", ")
      .append(to_string(mat[0][2]))
      .append(", ")
      .append(to_string(mat[0][3]))
      .append("\n");
  result.append(to_string(mat[1][0]))
      .append(", ")
      .append(to_string(mat[1][1]))
      .append(", ")
      .append(to_string(mat[1][2]))
      .append(", ")
      .append(to_string(mat[1][3]))
      .append("]\n");
  return result;
}

template <typename ScalarType>
glm::mat2x4 transform_to_glsl_motor(const math::Transform<ScalarType>& transform) {
  const auto& vector{transform.raw_vector()};
  // #define motor     mat2x4  // [ [s, e23, e31, e12], [e01, e02, e03, e0123] ]
  // [ [s, e12, e20, e01], [e123, e203, e013, e0123] ]
  glm::mat2x4 result{vector[0],  vector[6],   -vector[5], vector[3],
                     vector[14], -vector[13], vector[11], vector[15]};
  return result;
}

template <typename ScalarType>
class GpuElement {
 private:
  std::vector<std::unique_ptr<GpuElement<ScalarType>>> children_{};

 public:
  virtual ~GpuElement() = default;

  void update(ScalarType time, const ShaderProgram& program,
              const math::Transform<ScalarType>& parent_transform) {
    // The transform argument in the local coordinates of this element.
    const math::Transform<ScalarType> local_transform{compose_transform(time, parent_transform)};

    glUseProgram(program.id());
    const GLuint element_transform_id{glGetUniformLocation(program.id(), "element_transform")};
    const glm::mat2x4 element_transform{transform_to_glsl_motor(local_transform)};
    glUniformMatrix2x4fv(element_transform_id, 1, GL_FALSE, &element_transform[0][0]);

    draw(time);

    for (auto& child : children_) {
      child->update(time, program, local_transform);
    }
  }

  virtual math::Transform<ScalarType> compose_transform(
      ScalarType /*time*/, const math::Transform<ScalarType>& transform) {
    // By default, we just use the transform passed down from our parent. Subclasses, especially
    // the joints, will compose their own transforms with this transform to show motion.
    return transform;
  }

  virtual void draw(ScalarType /*time*/) {
    // GPU elements don't draw anything by default.
    // Elements that wish to draw will be making their glDraw calls in the context of the shader
    // program specified to the update method with the model view and projection transforms /
    // matrices already bound.
  }

  void add_element(std::unique_ptr<GpuElement<ScalarType>>&& element) {
    children_.emplace_back(std::move(element));
  }

  void clear_all_elements() { children_.clear(); }
};

}  // namespace ndyn::graphics
