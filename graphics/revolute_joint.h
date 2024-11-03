#pragma once

#include "graphics/gpu_element.h"
#include "math/geometry.h"

namespace ndyn::graphics {

template <typename ScalarType>
class RevoluteJoint final : public GpuElement<ScalarType> {
 private:
  ScalarType angle_{0};
  math::Primitive<ScalarType> axis_{math::Primitive<ScalarType>::z_axis()};

 public:
  math::Transform<ScalarType> compose_transform(
      ScalarType t, const math::Transform<ScalarType>& transform) override {
    return transform.compose(math::Transform<ScalarType>::rotate(axis_, angle_));
  }

  void set_angle(const ScalarType& angle) { angle_ = angle; }
  const ScalarType& angle() const { return angle_; }

  const math::Primitive<ScalarType>& axis() const { return axis_; }
  void set_axis(const math::Primitive<ScalarType>& axis) { axis_ = axis; }
};

}  // namespace ndyn::graphics
