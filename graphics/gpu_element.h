#pragma once

#include <functional>

#include "GLFW/glfw3.h"
#include "glm/glm.hpp"
#include "graphics/shader_program.h"
#include "math/geometry.h"

namespace ndyn::graphics {

template <typename ScalarType>
glm::mat4 transform_to_glm_mat4(const math::Transform<ScalarType>& transform) {
  return glm::mat4{1.f};
}

template <typename ScalarType>
class GpuElement {
 public:
  virtual ~GpuElement() = default;

  void update(ScalarType t, const ShaderProgram& program,
              const math::Transform<ScalarType>& transform) {
    // The transform argument in the local coordinates of this element.
    const math::Transform<ScalarType> local_transform{compose_transform(t, transform)};

    glUseProgram(program.id());
    const GLuint element_matrix_id{glGetUniformLocation(program.id(), "element_matrix")};
    const glm::mat4 element_matrix{transform_to_glm_mat4(local_transform)};
    glUniformMatrix4fv(element_matrix_id, 1, GL_FALSE, &element_matrix[0][0]);

    draw(t);
  }

  virtual math::Transform<ScalarType> compose_transform(
      ScalarType /*time*/, const math::Transform<ScalarType>& transform) {
    return transform;
  }

  virtual void draw(ScalarType t) {}
};

}  // namespace ndyn::graphics
