//#define GLFW_INCLUDE_NONE
#include "GLFW/glfw3.h"
#include "gtest/gtest.h"

static void error_callback(int error, const char* description) {
  ADD_FAILURE() << "Error: " << description;
}

TEST(GlfwTest, CanInitializeGlfwWindow) {
  GLFWwindow* window;
  int width, height;

  glfwSetErrorCallback(error_callback);

  glfwInitHint(GLFW_COCOA_MENUBAR, GLFW_FALSE);

  const int init_result{glfwInit()};
  ASSERT_NE(0, init_result);

  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
  glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);

  window = glfwCreateWindow(640, 480, "Test Window Creation", NULL, NULL);
  if (!window) {
    glfwTerminate();
  }
  ASSERT_NE(nullptr, window);

  glfwMakeContextCurrent(window);

  glfwGetFramebufferSize(window, &width, &height);
  EXPECT_EQ(640, width);
  EXPECT_EQ(480, height);

  glfwDestroyWindow(window);

  glfwTerminate();
}
