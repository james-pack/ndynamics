#pragma once

#include <memory>
#include <string>
#include <vector>

#include "GLFW/glfw3.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "graphics/gpu_element.h"
#include "graphics/shader_program.h"
#include "math/geometry.h"

namespace ndyn::graphics {

std::string to_string(const glm::mat4& mat) {
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
  result.append(" ")
      .append(to_string(mat[1][0]))
      .append(", ")
      .append(to_string(mat[1][1]))
      .append(", ")
      .append(to_string(mat[1][2]))
      .append(", ")
      .append(to_string(mat[1][3]))
      .append("\n");
  result.append(" ")
      .append(to_string(mat[2][0]))
      .append(", ")
      .append(to_string(mat[2][1]))
      .append(", ")
      .append(to_string(mat[2][2]))
      .append(", ")
      .append(to_string(mat[2][3]))
      .append("\n");
  result.append(" ")
      .append(to_string(mat[3][0]))
      .append(", ")
      .append(to_string(mat[3][1]))
      .append(", ")
      .append(to_string(mat[3][2]))
      .append(", ")
      .append(to_string(mat[3][3]))
      .append("]\n");
  return result;
}

template <typename GeometryT>
class Model final {
 public:
  using GeometryType = GeometryT;
  using AlgebraType = typename GeometryType::AlgebraType;
  using ScalarType = typename AlgebraType::ScalarType;
  using VectorType = typename AlgebraType::VectorType;

 private:
  std::vector<std::unique_ptr<GpuElement<GeometryType>>> top_level_elements_{};

  // We expect to have different ShaderPrograms for different distinct renderings. Possibilities
  // include orthographic vs perspective projections, wireframe vs solid vs debugging colorations,
  // etc. We would switch between programs based on some configuration mechanism TBD. The active
  // program is then passed to each GpuElement on update().
  ShaderProgram default_program_;
  ShaderProgram* active_program_;

  GLfloat aspect_ratio_;

  static constexpr glm::mat3 identity_3x3_{glm::mat3{1.f}};

  glm::mat4 perspective_projection_{
      glm::perspective(glm::radians(50.f), aspect_ratio_, 0.1f, 100.f)};

  glm::mat4 orthographic_projection_{
      glm::ortho(-10.0f * aspect_ratio_, 10.0f * aspect_ratio_, -10.0f, 10.0f, 0.f,
                 100.0f)  // In world coordinates
  };

  glm::mat4* active_projection_{&perspective_projection_};

  glm::mat4 camera_{glm::lookAt(
      glm::vec3{0, 2, 10},  // Camera location in World Space
      glm::vec3{0, 0, 0},   // and looks at this location in World Space
      glm::vec3{0, 1, 1}    // Head is up (set to 0,-1,0 to look upside-down)
      )};

  bool projection_matrix_dirty_{true};
  bool camera_matrix_dirty_{true};

 public:
  explicit Model(GLfloat aspect_ratio = 1.f)
      : default_program_(ShaderProgramBuilder{}
                             .add_vertex_shader("graphics/pga_transform_vertex_shader.glsl")
                             .add_fragment_shader("graphics/default_fragment_shader.glsl")
                             .build()),
        active_program_(&default_program_),
        aspect_ratio_(aspect_ratio) {}

  void add_element(std::unique_ptr<GpuElement<GeometryType>>&& element) {
    top_level_elements_.emplace_back(std::move(element));
  }

  void clear_all_elements() { top_level_elements_.clear(); }

  void toggle_perspective_orthographic_projections() {
    if (active_projection_ == &perspective_projection_) {
      active_projection_ = &orthographic_projection_;
    } else {
      active_projection_ = &perspective_projection_;
    }
    projection_matrix_dirty_ = true;
  }

  void update(ScalarType t) {
    glUseProgram(active_program_->id());

    const GLuint scale_matrix_id{glGetUniformLocation(active_program_->id(), "scale_matrix")};
    glUniformMatrix3fv(scale_matrix_id, 1, GL_FALSE, &identity_3x3_[0][0]);

    if (projection_matrix_dirty_) {
      const GLuint projection_matrix_id{
          glGetUniformLocation(active_program_->id(), "projection_matrix")};
      glUniformMatrix4fv(projection_matrix_id, 1, GL_FALSE, &(*active_projection_)[0][0]);
      projection_matrix_dirty_ = false;
    }

    if (camera_matrix_dirty_) {
      const GLuint camera_matrix_id{glGetUniformLocation(active_program_->id(), "camera_matrix")};
      glUniformMatrix4fv(camera_matrix_id, 1, GL_FALSE, &camera_[0][0]);
      camera_matrix_dirty_ = false;
    }

    for (auto& element : top_level_elements_) {
      element->update(t, *active_program_, GeometryType::identity_transform);
    }
  }
};

}  // namespace ndyn::graphics
