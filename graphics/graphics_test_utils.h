#pragma once

#include "glog/logging.h"
#include "gtest/gtest.h"

namespace ndyn::graphics {

static void glfw_error_callback(int error, const char *description) {
  LOG(ERROR) << "GLFW Error " << error << ": " << description;
}

class GraphicsTest : public ::testing::Test {
 public:
  GLFWwindow *window_{nullptr};  // GLFW window handle

  void SetUp() override {
    // Setup window
    glfwSetErrorCallback(glfw_error_callback);
    if (!glfwInit()) abort();

    // GL 3.0 + GLSL 130
    const char *glsl_version = "#version 130";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // 3.2+ only
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE);          // 3.0+ only

    GLFWmonitor *monitor = glfwGetPrimaryMonitor();
    const GLFWvidmode *mode = glfwGetVideoMode(monitor);
    glfwWindowHint(GLFW_RED_BITS, mode->redBits);
    glfwWindowHint(GLFW_GREEN_BITS, mode->greenBits);
    glfwWindowHint(GLFW_BLUE_BITS, mode->blueBits);
    glfwWindowHint(GLFW_REFRESH_RATE, mode->refreshRate);

    int width = mode->width;
    int height = mode->height;

    // Scaling the window to the monitor helps with readability on high resolution monitors.
    // Otherwise, the text and other renderings can be too small on 4K and higher resolution
    // monitors.
    glfwWindowHint(GLFW_SCALE_TO_MONITOR, GL_TRUE);

    // Create window with graphics context
    window_ = glfwCreateWindow(width, height, "Test", monitor, nullptr);
    if (window_ == nullptr) {
      LOG(FATAL) << "Failed to initialize GLFW window!";
    }

    glfwMakeContextCurrent(window_);
    glfwSwapInterval(0);

    // Initialize OpenGL loader
    int version = gladLoadGL(glfwGetProcAddress);
    LOG(INFO) << "glfw version: " << version;
    if (version == 0) {
      LOG(FATAL) << "Failed to initialize OpenGL loader!";
    }
  }
};

}  // namespace ndyn::graphics
