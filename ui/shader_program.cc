// clang-format off
#include "glad/gl.h"
// clang-format on

#include "ui/shader_program.h"

#include <filesystem>
#include <string>

#include "GLFW/glfw3.h"
#include "glog/logging.h"
#include "io/utils.h"

namespace ndyn::ui {

template <GLenum SHADER_TYPE>
GLuint initialize_shader(std::filesystem::path path) {
  GLuint shader_id = glCreateShader(SHADER_TYPE);

  GLint result{GL_FALSE};
  int info_log_length{0};

  const std::string shader_code{io::read_file(path)};
  VLOG(2) << "shader source:\n" << shader_code << "\n";
  const char *source_pointer = shader_code.c_str();
  glShaderSource(shader_id, 1, &source_pointer, nullptr);
  glCompileShader(shader_id);

  glGetShaderiv(shader_id, GL_COMPILE_STATUS, &result);
  if (result == GL_FALSE) {
    glGetShaderiv(shader_id, GL_INFO_LOG_LENGTH, &info_log_length);
    if (info_log_length > 0) {
      LOG(INFO) << "result: " << result << ", info_log_length: " << info_log_length;
      std::vector<GLchar> error_message(info_log_length);
      glGetShaderInfoLog(shader_id, info_log_length, &info_log_length, &error_message[0]);
      LOG(FATAL) << "Could not compile shader '" << path << "'. Error: " << &error_message[0];
    }
  }

  return shader_id;
}

ShaderProgram ShaderProgramBuilder::build() {
  std::vector<GLuint> shader_ids{};
  shader_ids.reserve(vertex_shader_paths_.size() + fragment_shader_paths_.size());

  // Initialize each of the vertex shaders, including compiling them.
  for (const auto path : vertex_shader_paths_) {
    shader_ids.push_back(initialize_shader<GL_VERTEX_SHADER>(path));
  }

  // Same for fragment shaders.
  for (const auto path : fragment_shader_paths_) {
    shader_ids.push_back(initialize_shader<GL_FRAGMENT_SHADER>(path));
  }

  // Link the program
  GLuint program_id = glCreateProgram();

  for (const auto shader_id : shader_ids) {
    glAttachShader(program_id, shader_id);
  }

  glLinkProgram(program_id);

  // Check the program
  GLint result{GL_FALSE};
  int info_log_length{0};
  glGetProgramiv(program_id, GL_LINK_STATUS, &result);
  if (result == GL_FALSE) {
    glGetProgramiv(program_id, GL_INFO_LOG_LENGTH, &info_log_length);
    if (info_log_length > 0) {
      LOG(INFO) << "result: " << result << ", info_log_length: " << info_log_length;
      std::vector<GLchar> error_message(info_log_length);
      glGetProgramInfoLog(program_id, info_log_length, &info_log_length, &error_message[0]);
      LOG(FATAL) << "Could not link shader program: " << &error_message[0];
    }
  }

  for (const auto shader_id : shader_ids) {
    glDetachShader(program_id, shader_id);
  }

  for (const auto shader_id : shader_ids) {
    glDeleteShader(shader_id);
  }

  VLOG(3) << "program_id: " << program_id;
  return ShaderProgram{program_id};
}

}  // namespace ndyn::ui
