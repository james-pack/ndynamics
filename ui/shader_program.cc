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

GLuint initialize_vertex_shader(std::filesystem::path vertex_file_path) {
  GLuint vertex_shader_id = glCreateShader(GL_VERTEX_SHADER);

  GLint result{GL_FALSE};
  int info_log_length{0};

  const std::string vertex_shader_code{io::read_file(vertex_file_path)};
  VLOG(2) << "vertex shader:\n" << vertex_shader_code << "\n";
  const char *vertex_source_pointer = vertex_shader_code.c_str();
  glShaderSource(vertex_shader_id, 1, &vertex_source_pointer, NULL);
  glCompileShader(vertex_shader_id);

  glGetShaderiv(vertex_shader_id, GL_COMPILE_STATUS, &result);
  if (result == GL_FALSE) {
    glGetShaderiv(vertex_shader_id, GL_INFO_LOG_LENGTH, &info_log_length);
    if (info_log_length > 0) {
      LOG(INFO) << "result: " << result << ", info_log_length: " << info_log_length;
      std::vector<GLchar> error_message(info_log_length);
      glGetShaderInfoLog(vertex_shader_id, info_log_length, &info_log_length, &error_message[0]);
      LOG(FATAL) << "Could not compile vertex shader: " << &error_message[0];
    }
  }

  return vertex_shader_id;
}

GLuint initialize_fragment_shader(std::filesystem::path fragment_file_path) {
  GLuint fragment_shader_id = glCreateShader(GL_FRAGMENT_SHADER);

  GLint result{GL_FALSE};
  int info_log_length{0};

  const std::string fragment_shader_code{io::read_file(fragment_file_path)};
  VLOG(2) << "fragment shader:\n" << fragment_shader_code << "\n";
  const char *fragment_source_pointer = fragment_shader_code.c_str();
  glShaderSource(fragment_shader_id, 1, &fragment_source_pointer, NULL);
  glCompileShader(fragment_shader_id);

  glGetShaderiv(fragment_shader_id, GL_COMPILE_STATUS, &result);
  if (result == GL_FALSE) {
    glGetShaderiv(fragment_shader_id, GL_INFO_LOG_LENGTH, &info_log_length);
    if (info_log_length > 0) {
      LOG(INFO) << "result: " << result << ", info_log_length: " << info_log_length;
      std::vector<GLchar> error_message(info_log_length);
      glGetShaderInfoLog(fragment_shader_id, info_log_length, &info_log_length, &error_message[0]);
      LOG(FATAL) << "Could not compile fragment shader: " << &error_message[0];
    }
  }

  return fragment_shader_id;
}

ShaderProgram ShaderProgramBuilder::build() {
  std::vector<GLuint> shader_ids{};
  shader_ids.reserve(vertex_shader_paths_.size() + fragment_shader_paths_.size());

  for (const auto path : vertex_shader_paths_) {
    shader_ids.push_back(initialize_vertex_shader(path));
  }

  for (const auto path : fragment_shader_paths_) {
    shader_ids.push_back(initialize_fragment_shader(path));
  }

  // Link the program
  LOG(INFO) << "Linking shader program.";
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
