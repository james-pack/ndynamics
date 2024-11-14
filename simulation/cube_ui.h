#pragma once

#include <cmath>
#include <memory>

#include "GLFW/glfw3.h"
#include "base/pi.h"
#include "graphics/box.h"
#include "graphics/cylinder.h"
#include "graphics/model.h"
#include "graphics/offset.h"
#include "graphics/part_builder.h"
#include "graphics/revolute_joint.h"
#include "graphics/shader_program.h"
#include "graphics/sphere.h"
#include "imgui.h"
#include "math/algebra.h"
#include "math/geometry.h"
#include "math/multivector.h"
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
  using ScalarType = float;
  using TimeType = ScalarType;
  using AlgebraType = math::Algebra<ScalarType, 3, 0, 1>;
  using GeometryType = math::Geometry3D<AlgebraType>;
  using VectorType = typename AlgebraType::VectorType;

  graphics::Model<GeometryType> model_;

  graphics::RevoluteJoint<GeometryType>* joint0_;
  graphics::RevoluteJoint<GeometryType>* joint1_;

 public:
  CubeScene(GLFWwindow& window) : Scene("3D Cube"), model_(aspect_ratio(window)) {
    graphics::PartBuilder<GeometryType> builder{};
    builder.add_revolute_joint(GeometryType::y_axis, pi)
        .bind(&joint0_)
        .down()
        .add_revolute_joint(GeometryType::z_axis)
        .bind(&joint1_)
        .down()
        .add_sphere(.25)
        .add_offset(14, GeometryType::y_axis)
        .down()
        .add_rod(0.25, 1, GeometryType::y_axis)
        .down()
        .add_offset(1, GeometryType::y_axis)
        .down()
        .add_cube(.25);
    model_.add_element(builder.build());
    LOG(INFO) << "joint0_: " << joint0_;
  }

  void handle_loading() override {
    ui::bind_key(ImGuiKey_O, "Toggle between perspective and orthographic projections",
                 [this](ImGuiKeyChord) { model_.toggle_perspective_orthographic_projections(); });
  }

  void handle_unloading() override { ui::unbind_key(ImGuiKey_O); }

  void update_models() override {
    using std::cos;
    using std::sin;
    const TimeType t{static_cast<TimeType>(ImGui::GetTime())};
    // joint0_->set_rotation_angle(pi * t / 2);
    joint0_->set_distance(pi * t / 2);
    joint1_->set_rotation_angle(pi + pi / 3 * cos(pi / 2 * t));
  }

  void update_ui() override {}
  void update_direct_render_elements() override {
    const TimeType current_time{static_cast<TimeType>(ImGui::GetTime())};
    model_.update(current_time);
  }
};

}  // namespace ndyn::simulation
