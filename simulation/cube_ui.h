#pragma once

#include <cmath>
#include <memory>

#include "GLFW/glfw3.h"
#include "base/pi.h"
#include "graphics/box.h"
#include "graphics/model.h"
#include "graphics/revolute_joint.h"
#include "graphics/rod.h"
#include "graphics/shader_program.h"
#include "imgui.h"
#include "math/geometry.h"
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

  graphics::Rod<ScalarType>* rod1_;
  graphics::Rod<ScalarType>* rod2_;
  graphics::RevoluteJoint<ScalarType>* joint1_;
  graphics::RevoluteJoint<ScalarType>* joint2_;

 public:
  CubeScene(GLFWwindow& window) : Scene("3D Cube"), model_(aspect_ratio(window)) {
    std::unique_ptr<graphics::Box<ScalarType>> box{std::make_unique<graphics::Box<ScalarType>>()};

    std::unique_ptr<graphics::Rod<ScalarType>> rod2{std::make_unique<graphics::Rod<ScalarType>>()};
    rod2_ = rod2.get();
    rod2->set_distance(0);
    rod2->set_direction(math::Primitive<ScalarType>::x_axis());
    rod2->add_element(std::move(box));

    std::unique_ptr<graphics::RevoluteJoint<ScalarType>> joint2{
        std::make_unique<graphics::RevoluteJoint<ScalarType>>()};
    joint2_ = joint2.get();
    joint2->set_axis(math::Primitive<ScalarType>::y_axis());
    joint2->add_element(std::move(rod2));

    std::unique_ptr<graphics::Rod<ScalarType>> rod1{std::make_unique<graphics::Rod<ScalarType>>()};
    rod1_ = rod1.get();
    rod1->set_distance(0);
    rod1->set_direction(math::Primitive<ScalarType>::y_axis());
    rod1->add_element(std::move(joint2));

    std::unique_ptr<graphics::RevoluteJoint<ScalarType>> joint1{
        std::make_unique<graphics::RevoluteJoint<ScalarType>>()};
    joint1_ = joint1.get();
    joint1->set_axis(math::Primitive<ScalarType>::z_axis());
    joint1->add_element(std::move(rod1));

    model_.add_element(std::move(joint1));
  }

  void handle_loading() override {
    ui::bind_key(ImGuiKey_O, "Toggle between perspective and orthographic projections",
                 [this](ImGuiKeyChord) { model_.toggle_perspective_orthographic_projections(); });
  }

  void handle_unloading() override { ui::unbind_key(ImGuiKey_O); }

  void update_models() override {
    using std::cos;
    const TimeType t{static_cast<TimeType>(ImGui::GetTime())};
    joint1_->set_angle(pi / 2 * sin(t / 2));
    joint2_->set_angle(pi / 2 * sin(t / 3));
  }

  void update_ui() override {}
  void update_direct_render_elements() override {
    const TimeType current_time{static_cast<TimeType>(ImGui::GetTime())};
    model_.update(current_time);
  }
};

}  // namespace ndyn::simulation
