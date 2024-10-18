#pragma once

#include <memory>

#include "GLFW/glfw3.h"
#include "graphics/box.h"
#include "graphics/model.h"
#include "graphics/shader_program.h"
#include "imgui.h"
#include "ui/keyboard_shortcuts.h"
#include "ui/scene.h"

namespace ndyn::simulation {

static inline float aspect_ratio(GLFWwindow& window) {
  float result{};
  int width{};
  int height{};
  glfwGetWindowSize(&window, &width, &height);
  result = static_cast<float>(width) / height;
  return result;
}

class CubeScene final : public ui::Scene {
 private:
  using TimeType = float;
  using ScalarType = float;
  graphics::Model<ScalarType> model_;

 public:
  CubeScene(GLFWwindow& window) : Scene("3D Cube"), model_(aspect_ratio(window)) {
    model_.add_element(std::make_unique<graphics::Box<ScalarType>>());
  }

  void handle_loading() override {
    ui::bind_key(ImGuiKey_O, "Toggle between perspective and orthographic projections",
                 [this](ImGuiKeyChord) {
                   /*
                   use_perspective_projection_ = not use_perspective_projection_;
                   if (use_perspective_projection_) {
                     // Projection matrix: 50° Field of View, aspect ratio from the window, display
                     // range: 0.1
                     // unit <-> 100 units.
                     projection_ = glm::perspective(glm::radians(50.f), aspect_ratio_, 0.1f, 100.f);
                   } else {
                     // Orthographic camera.
                     projection_ = glm::ortho(-10.0f * aspect_ratio_, 10.0f * aspect_ratio_, -10.0f,
                                              10.0f, 0.0f,
                                              100.0f);  // In world coordinates
                   }
                   mvp_dirty_ = true;
                   */
                 });
  }

  void handle_unloading() override { ui::unbind_key(ImGuiKey_O); }

  void update_models() override {}
  void update_ui() override {}
  void update_direct_render_elements() override {
    const TimeType current_time{static_cast<TimeType>(ImGui::GetTime())};
    model_.update(current_time);
  }
};

}  // namespace ndyn::simulation
