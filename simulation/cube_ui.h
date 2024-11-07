#pragma once

#include <cmath>
#include <memory>

#include "GLFW/glfw3.h"
#include "base/pi.h"
#include "graphics/box.h"
#include "graphics/model.h"
#include "graphics/offset.h"
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

  graphics::RevoluteJoint<GeometryType>* joint1_;
  graphics::RevoluteJoint<GeometryType>* joint2_;

 public:
  CubeScene(GLFWwindow& window) : Scene("3D Cube"), model_(aspect_ratio(window)) {
    std::unique_ptr<graphics::Box<GeometryType>> box2{
        std::make_unique<graphics::Box<GeometryType>>(.3, .3, .3)};

    std::unique_ptr<graphics::RevoluteJoint<GeometryType>> joint2{
        std::make_unique<graphics::RevoluteJoint<GeometryType>>()};
    joint2_ = joint2.get();
    joint2->set_rotation_axis(GeometryType::z_axis);
    joint2->set_offset_direction(GeometryType::y_axis);
    joint2->set_offset(2);
    joint2->add_element(std::move(box2));

    std::unique_ptr<graphics::Box<GeometryType>> box1{
        std::make_unique<graphics::Box<GeometryType>>(.75, .2, .35)};

    std::unique_ptr<graphics::Offset<GeometryType>> offset2{
        std::make_unique<graphics::Offset<GeometryType>>()};
    offset2->set_distance(box1->height() / 2);
    offset2->set_direction(GeometryType::y_axis);
    offset2->add_element(std::move(joint2));

    std::unique_ptr<graphics::Offset<GeometryType>> offset1{
        std::make_unique<graphics::Offset<GeometryType>>()};
    offset1->set_distance(2);
    offset1->set_direction(GeometryType::y_axis);
    offset1->add_element(std::move(offset2));
    offset1->add_element(std::move(box1));

    std::unique_ptr<graphics::Sphere<GeometryType, 5>> pivot{
        std::make_unique<graphics::Sphere<GeometryType, 5>>(.2, .2, .2)};

    std::unique_ptr<graphics::RevoluteJoint<GeometryType>> joint1{
        std::make_unique<graphics::RevoluteJoint<GeometryType>>()};
    joint1_ = joint1.get();
    joint1->set_rotation_axis(GeometryType::z_axis);
    joint1->set_rotation_angle(pi);
    joint1->add_element(std::move(offset1));
    joint1->add_element(std::move(pivot));

    model_.add_element(std::move(joint1));
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
    joint1_->set_rotation_angle(pi + pi / 2 * sin(3 * t));
    joint2_->set_rotation_angle(pi / 2 * sin(2 * t));
  }

  void update_ui() override {}
  void update_direct_render_elements() override {
    const TimeType current_time{static_cast<TimeType>(ImGui::GetTime())};
    model_.update(current_time);
  }
};

}  // namespace ndyn::simulation
