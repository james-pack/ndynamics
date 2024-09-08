#pragma once

#include <ostream>
#include <string>

#include "GLFW/glfw3.h"
#include "imgui.h"

/// Macro to disable console on Windows
#if defined(_WIN32) && defined(APP_NO_CONSOLE)
#pragma comment(linker, "/subsystem:windows /ENTRY:mainCRTStartup")
#endif

std::string to_string(const ImVec2&);
std::ostream& operator<<(std::ostream&, const ImVec2&);

// Barebones Application Framework
class App {
 protected:
  ImVec4 clear_color_{};         // background clear color
  GLFWwindow* window_{nullptr};  // GLFW window handle

 public:
  App(std::string title, size_t width = 0, size_t height = 0);
  virtual ~App();

  // Called at top of run
  virtual void Start() {}

  // Update, called once per frame.
  virtual void Update() = 0;

  // Runs the app.
  void Run();

  // Get window size
  ImVec2 GetWindowSize() const;
};
