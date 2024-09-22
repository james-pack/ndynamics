// clang-format off
#include "glad/gl.h"
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
      -1.0f, -1.0f, 0.0f, 1.0f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
  };

  GLuint program_id_{};

  GLuint vertex_buffer_{};

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

    program_id_ = initialize_shaders("simulation/trivial_vertex_shader.vertexshader",
                                     "simulation/red_fragment_shader.fragmentshader");

    if (program_id_ != 0) {
      glUseProgram(program_id_);
    }
  }

  void update_frame() override {
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
    // Draw the triangle !
    glDrawArrays(GL_TRIANGLES, 0, 3);  // Starting from vertex 0; 3 vertices total -> 1 triangle
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
