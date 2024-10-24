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

  void update(ScalarType time, const ShaderProgram& program,
              const math::Transform<ScalarType>& transform) {
    // The transform argument in the local coordinates of this element.
    const math::Transform<ScalarType> local_transform{compose_transform(time, transform)};

    glUseProgram(program.id());
    const GLuint element_matrix_id{glGetUniformLocation(program.id(), "element_matrix")};
    const glm::mat4 element_matrix{transform_to_glm_mat4(local_transform)};
    glUniformMatrix4fv(element_matrix_id, 1, GL_FALSE, &element_matrix[0][0]);

    draw(time);
  }

  virtual math::Transform<ScalarType> compose_transform(
      ScalarType /*time*/, const math::Transform<ScalarType>& transform) {
    // By default, we just use the transform passed down from our parent. Subclasses, especially
    // the joints, will override this to show motion.
    return transform;
  }

  virtual void draw(ScalarType t) {}
};

}  // namespace ndyn::graphics
