#pragma once

#include <string>

#include "GLFW/glfw3.h"
#include "imgui.h"

/// Macro to disable console on Windows
#if defined(_WIN32) && defined(APP_NO_CONSOLE)
#pragma comment(linker, "/subsystem:windows /ENTRY:mainCRTStartup")
#endif

// Barebones Application Framework
class App {
 protected:
  ImVec4 clear_color_{};         // background clear color
  GLFWwindow* window_{nullptr};  // GLFW window handle

 public:
  App(std::string title);
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
