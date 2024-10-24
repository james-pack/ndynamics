#pragma once

#include <memory>

#include "GLFW/glfw3.h"
#include "graphics/box.h"
#include "graphics/model.h"
#include "graphics/revolute_joint.h"
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
    // Uncomment to switch to revolute joint:
    // std::unique_ptr<graphics::RevoluteJoint<ScalarType>> joint{
    //     std::make_unique<graphics::RevoluteJoint<ScalarType>>()};
    // joint->add_element(std::make_unique<graphics::Box<ScalarType>>());
    // model_.add_element(std::move(joint));
    model_.add_element(std::make_unique<graphics::Box<ScalarType>>());
  }

  void handle_loading() override {
    ui::bind_key(ImGuiKey_O, "Toggle between perspective and orthographic projections",
                 [this](ImGuiKeyChord) { model_.toggle_perspective_orthographic_projections(); });
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
