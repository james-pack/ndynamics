#pragma once

#include <string>

#include "GLFW/glfw3.h"
#include "imgui.h"

/// Macro to disable console on Windows
#if defined(_WIN32) && defined(APP_NO_CONSOLE)
#pragma comment(linker, "/subsystem:windows /ENTRY:mainCRTStartup")
#endif

namespace ndyn::ui {

// Barebones Application Framework
class App {
 private:
  GLFWwindow* window_{nullptr};  // GLFW window handle
  ImVec4 clear_color_{};         // background clear color
  bool is_paused_{false};
  bool close_requested_{false};

 protected:
  // Called at top of run
  virtual void start() {}

  // Update, called once per frame. Not called when paused.
  virtual void update() = 0;

  // Called when app is first paused.
  virtual void handle_pause() {}

  // Called when app is first unpaused.
  virtual void handle_unpause() {}

  bool is_paused() const { return is_paused_; }

  void invert_pause() {
    const bool was_paused{is_paused_};
    is_paused_ = not is_paused_;
    if (was_paused) {
      handle_unpause();
    } else {
      handle_pause();
    }
  }

  void pause() {
    if (!is_paused_) {
      invert_pause();
    }
  }

  void unpause() {
    if (is_paused_) {
      invert_pause();
    }
  }

  void request_close() { close_requested_ = true; }
  bool close_requested() const { return close_requested_; }

 public:
  App(std::string title, size_t width = 0, size_t height = 0);
  virtual ~App();

  // Runs the app.
  void run();
};

}  // namespace ndyn::ui
