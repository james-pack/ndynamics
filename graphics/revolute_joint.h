#pragma once

#include <memory>
#include <vector>

#include "GLFW/glfw3.h"
#include "base/pi.h"
#include "graphics/gpu_element.h"
#include "math/geometry.h"

namespace ndyn::graphics {

template <typename ScalarType>
class RevoluteJoint final : public GpuElement<ScalarType> {
 private:
  ScalarType angle_{0};
  math::Primitive<ScalarType> joint_axis_{math::Primitive<ScalarType>::y_axis()};
  std::vector<std::unique_ptr<GpuElement<ScalarType>>> children_{};

 public:
  math::Transform<ScalarType> compose_transform(
      ScalarType t, const math::Transform<ScalarType>& transform) override {
    angle_ = t * pi / 2 / 5;
    return transform;  // math::Transform<ScalarType>::rotate(joint_axis_, angle_);
  }

  void add_element(std::unique_ptr<GpuElement<ScalarType>>&& element) {
    children_.emplace_back(std::move(element));
  }

  void clear_all_elements() { children_.clear(); }
};

}  // namespace ndyn::graphics
