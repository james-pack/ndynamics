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

  GLuint program_id_{};

  GLuint vertex_buffer_;
  GLuint color_buffer_;

  glm::mat4 model_{1.f};
  glm::mat4 view_{glm::lookAt(glm::vec3(4, 3, 3),  // Camera is at (4,3,3), in World Space
                              glm::vec3(0, 0, 0),  // and looks at the origin
                              glm::vec3(0, 1, 0)   // Head is up (set to 0,-1,0 to look upside-down)
                              )};

  // Projection matrix: 50° Field of View, aspect ratio from the window, display range: 0.1 unit <->
  // 100 units Or, for an ortho camera: glm::ortho(-10.0f,10.0f,-10.0f,10.0f,0.0f,100.0f); // In
  // world coordinates
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

    glGenBuffers(1, &color_buffer_);
    glBindBuffer(GL_ARRAY_BUFFER, color_buffer_);
    glBufferData(GL_ARRAY_BUFFER, sizeof(g_color_buffer_data), g_color_buffer_data, GL_STATIC_DRAW);

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
