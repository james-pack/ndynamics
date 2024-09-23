// clang-format off
#include "glad/gl.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
// clang-format on

#include "base/initializer.h"
#include "gflags/gflags.h"
#include "GLFW/glfw3.h"
#include "glog/logging.h"
#include "ui/app.h"

namespace ndyn::simulation {

class Cube final : public ui::App {
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

  GLuint program_id_{};

  GLuint vertex_buffer_{};

  glm::mat4 model_{1.f};
  glm::mat4 view_{glm::lookAt(glm::vec3(4, 3, 3),  // Camera is at (4,3,3), in World Space
                              glm::vec3(0, 0, 0),  // and looks at the origin
                              glm::vec3(0, 1, 0)   // Head is up (set to 0,-1,0 to look upside-down)
                              )};

  // Projection matrix: 50° Field of View, aspect ratio from the window, display range: 0.1 unit <-> 100 units
  // Or, for an ortho camera:
  // glm::ortho(-10.0f,10.0f,-10.0f,10.0f,0.0f,100.0f); // In world
  // coordinates
  glm::mat4 projection_{glm::perspective(glm::radians(50.f), aspect_ratio(), 0.1f, 100.f)};
  glm::mat4 mvp_{1.f};

  GLuint mvp_matrix_id_{};

  bool mvp_dirty_{true};

  void start() override {
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

    program_id_ = initialize_shaders("simulation/sample_vertex_shader.vertexshader",
                                     "simulation/sample_fragment_shader.fragmentshader");

    if (program_id_ != 0) {
      glUseProgram(program_id_);
    }

    // Get a handle for our "MVP" uniform
    // Only during the initialisation
    mvp_matrix_id_ = glGetUniformLocation(program_id_, "MVP");
  }

  void update_frame() override {
    if (mvp_dirty_) {
      mvp_ = projection_ * view_ * model_;

      // Send our transformation to the currently bound shader, in the "MVP" uniform
      // This is done in the main loop since each model will have a different MVP matrix (At least
      // for the M part)
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
    // Draw the triangles for the cube!
    glDrawArrays(GL_TRIANGLES, 0,
                 12 * 3);  // 12*3 indices starting at 0 -> 12 triangles -> 6 squares
    glDisableVertexAttribArray(0);
  }

 public:
  using ui::App::App;
};

}  // namespace ndyn::simulation

int main(int argc, char* argv[]) {
  using namespace ndyn::simulation;

  FLAGS_logtostderr = true;
  ndyn::initialize(&argc, &argv);

  Cube app{"Cube", 1920, 1080};
  app.run();
  return 0;
}
