#pragma once

#include <functional>

#include "GLFW/glfw3.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "ui/direct_render_element.h"
#include "ui/keyboard_shortcuts.h"
#include "ui/shader_program.h"

namespace ndyn::simulation {

static inline float aspect_ratio(GLFWwindow& window) {
  float result{};
  int width{};
  int height{};
  glfwGetWindowSize(&window, &width, &height);
  result = static_cast<float>(width) / height;
  return result;
}

using CubePositionFn = std::function<glm::mat4()>;

class Cube final : public ui::DirectRenderElement {
 private:
  static constexpr GLfloat g_vertex_buffer_data[] = {
      -1.0f, -1.0f, -1.0f,  // triangle 1 : begin
      -1.0f, -1.0f, 1.0f,   //
      -1.0f, 1.0f,  1.0f,   // triangle 1 : end
      1.0f,  1.0f,  -1.0f,  // triangle 2 : begin
      -1.0f, -1.0f, -1.0f,  //
      -1.0f, 1.0f,  -1.0f,  // triangle 2 : end
      1.0f,  -1.0f, 1.0f,   //
      -1.0f, -1.0f, -1.0f,  //
      1.0f,  -1.0f, -1.0f,  //
      1.0f,  1.0f,  -1.0f,  //
      1.0f,  -1.0f, -1.0f,  //
      -1.0f, -1.0f, -1.0f,  //
      -1.0f, -1.0f, -1.0f,  //
      -1.0f, 1.0f,  1.0f,   //
      -1.0f, 1.0f,  -1.0f,  //
      1.0f,  -1.0f, 1.0f,   //
      -1.0f, -1.0f, 1.0f,   //
      -1.0f, -1.0f, -1.0f,  //
      -1.0f, 1.0f,  1.0f,   //
      -1.0f, -1.0f, 1.0f,   //
      1.0f,  -1.0f, 1.0f,   //
      1.0f,  1.0f,  1.0f,   //
      1.0f,  -1.0f, -1.0f,  //
      1.0f,  1.0f,  -1.0f,  //
      1.0f,  -1.0f, -1.0f,  //
      1.0f,  1.0f,  1.0f,   //
      1.0f,  -1.0f, 1.0f,   //
      1.0f,  1.0f,  1.0f,   //
      1.0f,  1.0f,  -1.0f,  //
      -1.0f, 1.0f,  -1.0f,  //
      1.0f,  1.0f,  1.0f,   //
      -1.0f, 1.0f,  -1.0f,  //
      -1.0f, 1.0f,  1.0f,   //
      1.0f,  1.0f,  1.0f,   //
      -1.0f, 1.0f,  1.0f,   //
      1.0f,  -1.0f, 1.0f    //
  };

  // One color for each vertex. They were generated randomly.
  static constexpr GLfloat g_color_buffer_data[] = {
      0.583f, 0.771f, 0.014f,  //
      0.609f, 0.115f, 0.436f,  //
      0.327f, 0.483f, 0.844f,  //
      0.822f, 0.569f, 0.201f,  //
      0.435f, 0.602f, 0.223f,  //
      0.310f, 0.747f, 0.185f,  //
      0.597f, 0.770f, 0.761f,  //
      0.559f, 0.436f, 0.730f,  //
      0.359f, 0.583f, 0.152f,  //
      0.483f, 0.596f, 0.789f,  //
      0.559f, 0.861f, 0.639f,  //
      0.195f, 0.548f, 0.859f,  //
      0.014f, 0.184f, 0.576f,  //
      0.771f, 0.328f, 0.970f,  //
      0.406f, 0.615f, 0.116f,  //
      0.676f, 0.977f, 0.133f,  //
      0.971f, 0.572f, 0.833f,  //
      0.140f, 0.616f, 0.489f,  //
      0.997f, 0.513f, 0.064f,  //
      0.945f, 0.719f, 0.592f,  //
      0.543f, 0.021f, 0.978f,  //
      0.279f, 0.317f, 0.505f,  //
      0.167f, 0.620f, 0.077f,  //
      0.347f, 0.857f, 0.137f,  //
      0.055f, 0.953f, 0.042f,  //
      0.714f, 0.505f, 0.345f,  //
      0.783f, 0.290f, 0.734f,  //
      0.722f, 0.645f, 0.174f,  //
      0.302f, 0.455f, 0.848f,  //
      0.225f, 0.587f, 0.040f,  //
      0.517f, 0.713f, 0.338f,  //
      0.053f, 0.959f, 0.120f,  //
      0.393f, 0.621f, 0.362f,  //
      0.673f, 0.211f, 0.457f,  //
      0.820f, 0.883f, 0.371f,  //
      0.982f, 0.099f, 0.879f   //
  };

  const ui::ShaderProgram program_;
  CubePositionFn position_fn_;

  GLuint vertex_buffer_{};
  GLuint color_buffer_{};

  float aspect_ratio_;

  glm::mat4 model_{1.f};
  glm::mat4 view_{glm::lookAt(glm::vec3{0, 0, 10},  // Camera location in World Space
                              glm::vec3{0, 0, 0},   // and looks at this location in World Space
                              glm::vec3{0, 1, 0}  // Head is up (set to 0,-1,0 to look upside-down)
                              )};

  glm::mat4 projection_;
  glm::mat4 mvp_{1.f};

  GLuint mvp_matrix_id_{};

  bool mvp_dirty_{true};
  bool use_perspective_projection_{true};

  void rotate_model() {
    static constexpr glm::vec3 rotation_axis{0, 1, 1};
    // Assuming that we render about 1000 fps, this gives us a rotation about every 10s.
    static constexpr float RADIANS_PER_FRAME{6.2832f / 1000 / 10};
    model_ = glm::rotate(model_, RADIANS_PER_FRAME, rotation_axis);

    mvp_dirty_ = true;
  }

  void sync_model_position() {
    model_ = position_fn_();
    mvp_dirty_ = true;
  }

  void update() override {
    if (position_fn_) {
      sync_model_position();
    } else {
      rotate_model();
    }

    glUseProgram(program_.id());

    if (mvp_dirty_) {
      mvp_ = projection_ * view_ * model_ * glm::scale(glm::mat4{1.f}, glm::vec3{0.75});
      glUniformMatrix4fv(mvp_matrix_id_, 1, GL_FALSE, &mvp_[0][0]);
      mvp_dirty_ = false;
    }

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

    // Draw the triangles for the cube!
    glDrawArrays(GL_TRIANGLES, 0,
                 12 * 3);  // 12*3 indices starting at 0 -> 12 triangles -> 6 squares
    glDisableVertexAttribArray(0);
  }

 public:
  Cube(GLFWwindow& window, const CubePositionFn& position_fn = CubePositionFn{})
      : program_(ui::ShaderProgramBuilder{}
                     .add_vertex_shader("simulation/sample_vertex_shader.vertexshader")
                     .add_fragment_shader("simulation/sample_fragment_shader.fragmentshader")
                     .build()),
        position_fn_(position_fn),
        aspect_ratio_(aspect_ratio(window)),
        projection_(glm::perspective(glm::radians(50.f), aspect_ratio_, 0.1f, 100.f)) {
    GLuint VertexArrayID;
    glGenVertexArrays(1, &VertexArrayID);
    glBindVertexArray(VertexArrayID);

    // Generate 1 buffer, put the resulting identifier in vertexbuffer
    glGenBuffers(1, &vertex_buffer_);
    // The following commands will talk about our 'vertexbuffer' buffer
    glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_);
    // Give our vertices to OpenGL.
    glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex_buffer_data), g_vertex_buffer_data,
                 GL_STATIC_DRAW);

    glGenBuffers(1, &color_buffer_);
    glBindBuffer(GL_ARRAY_BUFFER, color_buffer_);
    glBufferData(GL_ARRAY_BUFFER, sizeof(g_color_buffer_data), g_color_buffer_data, GL_STATIC_DRAW);

    glUseProgram(program_.id());
    mvp_matrix_id_ = glGetUniformLocation(program_.id(), "MVP");

    ui::bind_key(ImGuiKey_O, "Toggle between perspective and orthographic projections",
                 [this](ImGuiKeyChord) {
                   use_perspective_projection_ = not use_perspective_projection_;
                   if (use_perspective_projection_) {
                     // Projection matrix: 50° Field of View, aspect ratio from the window, display
                     // range: 0.1
                     // unit <-> 100 units.
                     projection_ = glm::perspective(glm::radians(50.f), aspect_ratio_, 0.1f, 100.f);
                   } else {
                     // Orthographic camera.
                     projection_ = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, 0.0f,
                                              100.0f);  // In world coordinates
                   }
                   mvp_dirty_ = true;
                 });
  }
};

}  // namespace ndyn::simulation
