#pragma once

#include <algorithm>
#include <array>
#include <filesystem>
#include <map>
#include <string>
#include <vector>

#include "GLFW/glfw3.h"
#include "imgui.h"
#include "ui/direct_render_element.h"
#include "ui/scene.h"
#include "ui/ui_elements.h"
#include "ui/ui_model.h"

namespace ndyn::ui {

// Barebones Application Framework
class App final {
 private:
  using SceneBankType = std::vector<Scene*>;

  GLFWwindow* window_{nullptr};  // GLFW window handle
  ImVec4 clear_color_{};         // background clear color
  bool is_paused_{false};
  bool close_requested_{false};
  bool show_help_text_{false};

  Scene* current_scene_{nullptr};

  std::map<size_t, SceneBankType> scene_banks_{};
  size_t current_bank_{0};

  std::string default_window_title_{};

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

  size_t next_bank_index() const {
    // Look for the first bank after current_bank_.
    auto iter{std::upper_bound(scene_banks_.begin(), scene_banks_.end(), current_bank_,
                               [](const auto& lhs, const auto& rhs) { return lhs < rhs.first; })};
    // If we are pointing to a valid bank after the current bank, we return it.
    if (iter != scene_banks_.end()) {
      return iter->first;
    }

    // If there are no banks after the current bank, cycle around to the beginning.
    iter = scene_banks_.begin();
    if (iter != scene_banks_.end()) {
      return iter->first;
    } else {
      // There are no banks at all.
      return 0;
    }
  }

  size_t prev_bank_index() const {
    auto iter{std::lower_bound(scene_banks_.begin(), scene_banks_.end(), current_bank_,
                               [](const auto& lhs, const auto& rhs) { return lhs.first < rhs; })};
    if (iter != scene_banks_.begin()) {
      --iter;
      if (iter != scene_banks_.end()) {
        return iter->first;
      }
    }

    // If there are no banks before the current bank, cycle around to the end.
    iter = scene_banks_.end();
    --iter;
    if (iter != scene_banks_.end()) {
      return iter->first;
    } else {
      // There are no banks at all.
      return 0;
    }
  }

  // Load a scene from the current bank of scenes.
  void load_scene(size_t index) {
    // Unload the current scene, if it exists.
    if (current_scene_ != nullptr) {
      current_scene_->handle_unloading();
    }

    current_scene_ = nullptr;

    // Load the new scene, if it exists.
    auto iter{scene_banks_.find(current_bank_)};
    if (iter != scene_banks_.end()) {
      const SceneBankType& bank{iter->second};
      if (index < bank.size()) {
        current_scene_ = bank[index];
        if (current_scene_ != nullptr) {
          glfwSetWindowTitle(window_, current_scene_->description().c_str());
          current_scene_->handle_loading();
        }
      }
    }

    if (current_scene_ == nullptr) {
      glfwSetWindowTitle(window_, default_window_title_.c_str());
    }
  }

  // Switch scene banks. Note that this is a no-op if the app has a valid scene from the requested
  // scene bank.
  void goto_scene_bank(size_t bank) {
    if (bank != current_bank_ || current_scene_ == nullptr) {
      current_bank_ = bank;
      load_scene(0);
    }
  }

  // Switch scene banks. Note that this is a no-op if the app is already using the requested scene
  // bank; the scene does not change.
  void goto_prev_scene_bank() { goto_scene_bank(prev_bank_index()); }

  // Switch scene banks. Note that this is a no-op if the app is already using the requested scene
  // bank; the scene does not change.
  void goto_next_scene_bank() { goto_scene_bank(next_bank_index()); }

  void show_help_text();
  void render_missing_scene_ui();

 public:
  App(std::string title, size_t width = 0, size_t height = 0);
  ~App();

  void add_scene(size_t bank_index, Scene& scene) {
    // Note that this line implicitly creates the bank, if it did not exist before.
    SceneBankType& bank{scene_banks_[bank_index]};
    bank.push_back(&scene);
  }

  void add_scene(Scene& scene) { add_scene(0, scene); }

  void run();

  GLFWwindow& gl_window() { return *window_; }
};

}  // namespace ndyn::ui
