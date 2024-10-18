#pragma once

#include <memory>
#include <vector>

#include "GLFW/glfw3.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "graphics/gpu_element.h"
#include "graphics/shader_program.h"
#include "math/geometry.h"

namespace ndyn::graphics {

template <typename ScalarType>
class Model final {
 private:
  static constexpr math::Transform<ScalarType> identity_transform_{
      math::Transform<ScalarType>::identity()};

  std::vector<std::unique_ptr<GpuElement<ScalarType>>> top_level_elements_{};

  // We expect to have different ShaderPrograms for different distinct renderings. Possibilities
  // include orthographic vs perspective projections, wireframe vs solid vs debugging colorations,
  // etc. We would switch between programs based on some configuration mechanism TBD. The active
  // program is then passed to each GpuElement on update().
  ShaderProgram default_program_;
  ShaderProgram* active_program_;

  GLfloat aspect_ratio_;

  glm::mat4 projection_{glm::perspective(glm::radians(50.f), aspect_ratio_, 0.1f, 100.f)};
  glm::mat4 camera_{glm::lookAt(
      glm::vec3{0, 2, 10},  // Camera location in World Space
      glm::vec3{0, 0, 0},   // and looks at this location in World Space
      glm::vec3{0, 1, 0}    // Head is up (set to 0,-1,0 to look upside-down)
      )};

  bool projection_matrix_dirty_{true};
  bool camera_matrix_dirty_{true};

 public:
  explicit Model(GLfloat aspect_ratio = 1.f)
      : default_program_(ShaderProgramBuilder{}
                             .add_vertex_shader("graphics/default_vertex_shader.glsl")
                             .add_fragment_shader("graphics/default_fragment_shader.glsl")
                             .build()),
        active_program_(&default_program_),
        aspect_ratio_(aspect_ratio) {}

  void add_element(std::unique_ptr<GpuElement<ScalarType>>&& element) {
    top_level_elements_.emplace_back(std::move(element));
  }

  void clear_all_elements() { top_level_elements_.clear(); }

  void update(ScalarType t) {
    glUseProgram(active_program_->id());

    if (projection_matrix_dirty_) {
      const GLuint projection_matrix_id{
          glGetUniformLocation(active_program_->id(), "projection_matrix")};
      glUniformMatrix4fv(projection_matrix_id, 1, GL_FALSE, &projection_[0][0]);
      projection_matrix_dirty_ = false;
    }

    if (camera_matrix_dirty_) {
      const GLuint camera_matrix_id{glGetUniformLocation(active_program_->id(), "camera_matrix")};
      glUniformMatrix4fv(camera_matrix_id, 1, GL_FALSE, &camera_[0][0]);
      camera_matrix_dirty_ = false;
    }

    for (auto& element : top_level_elements_) {
      element->update(t, *active_program_, identity_transform_);
    }
  }
};

}  // namespace ndyn::graphics
