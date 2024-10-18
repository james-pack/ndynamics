#pragma once

#include <functional>

#include "GLFW/glfw3.h"
#include "glm/glm.hpp"
#include "graphics/shader_program.h"
#include "math/geometry.h"
#include "time/time.h"

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
    glUseProgram(program.id());
    const GLuint element_matrix_id{glGetUniformLocation(program.id(), "element_matrix")};
    const glm::mat4 element_matrix{transform_to_glm_mat4(transform)};
    glUniformMatrix4fv(element_matrix_id, 1, GL_FALSE, &element_matrix[0][0]);

    draw(t);
  }

  virtual void draw(ScalarType t) = 0;
};

}  // namespace ndyn::graphics
