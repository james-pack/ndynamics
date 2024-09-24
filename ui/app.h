#pragma once

#include <filesystem>
#include <string>
#include <vector>

#include "GLFW/glfw3.h"
#include "imgui.h"
#include "ui/direct_render_element.h"
#include "ui/ui_elements.h"
#include "ui/ui_model.h"

namespace ndyn::ui {

// Barebones Application Framework
class App final {
 private:
  GLFWwindow* window_{nullptr};  // GLFW window handle
  ImVec4 clear_color_{};         // background clear color
  bool is_paused_{false};
  bool close_requested_{false};

  std::vector<UiModel*> models_{};
  UiElement* root_element_{nullptr};
  std::vector<DirectRenderElement*> directs_{};

  // Called when app is first paused.
  void handle_pause() {
    // TODO(james): Add eventing system, or figure out how to extend ImGui's, to notify UI elements
    // and direct renderers that the UI is paused.
  }

  // Called when app is first unpaused.
  void handle_unpause() {
    // TODO(james): Add eventing system, or figure out how to extend ImGui's, to notify UI elements
    // and direct renderers that the UI is unpaused.
  }

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
  ~App();

  void add_model(UiModel& model) { models_.push_back(&model); }
  void add_direct_render_element(DirectRenderElement& element) { directs_.push_back(&element); }
  void set_root_ui_element(Window& element) { root_element_ = &element; }

  void run();

  GLFWwindow& gl_window() { return *window_; }
};

}  // namespace ndyn::ui
