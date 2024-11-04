#pragma once

#include "graphics/gpu_element.h"
#include "math/geometry.h"

namespace ndyn::graphics {

template <typename GeometryT>
class RevoluteJoint final : public GpuElement<GeometryT> {
 public:
  using GeometryType = GeometryT;
  using AlgebraType = typename GeometryType::AlgebraType;
  using ScalarType = typename AlgebraType::ScalarType;
  using VectorType = typename AlgebraType::VectorType;

 private:
  ScalarType angle_{0};
  VectorType axis_{GeometryType::z_axis};

 public:
  VectorType compose_transform(ScalarType t, const VectorType& transform) override {
    return transform * GeometryType::rotate(axis_, angle_);
  }

  void set_angle(const ScalarType& angle) { angle_ = angle; }
  const ScalarType& angle() const { return angle_; }

  const VectorType& axis() const { return axis_; }
  void set_axis(const VectorType& axis) { axis_ = axis; }
};

}  // namespace ndyn::graphics
