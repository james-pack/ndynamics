#pragma once

#include <filesystem>
#include <vector>

#include "GLFW/glfw3.h"

namespace ndyn::graphics {

class ShaderProgramBuilder;

/**
 * Representation of a compiled and linked shader program. The primary value in this class is the
 * ShaderProgramBuilder which supports organizing, compiling, and linking the program.
 */
class ShaderProgram final {
 private:
  GLuint program_id_;
  friend class ShaderProgramBuilder;

  ShaderProgram(GLuint program_id) : program_id_(program_id) {}

 public:
  GLuint id() const { return program_id_; }
};

class ShaderProgramBuilder final {
  std::vector<std::filesystem::path> vertex_shader_paths_{};
  std::vector<std::filesystem::path> fragment_shader_paths_{};

 public:
  ShaderProgramBuilder& add_vertex_shader(std::filesystem::path path) {
    vertex_shader_paths_.push_back(path);
    return *this;
  }

  ShaderProgramBuilder& add_fragment_shader(std::filesystem::path path) {
    fragment_shader_paths_.push_back(path);
    return *this;
  }

  // Compile and link the shaders or abort the process in the event of a compilation or link error.
  ShaderProgram build();
};

}  // namespace ndyn::graphics
