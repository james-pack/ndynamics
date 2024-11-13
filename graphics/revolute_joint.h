#pragma once

#include "graphics/gpu_element.h"
#include "math/geometry.h"

namespace ndyn::graphics {

/**
 * Joint that provides an API to manage subelements at a distance and angle from this element.
 */
template <typename GeometryT>
class RevoluteJoint final : public GpuElement<GeometryT> {
 public:
  using GeometryType = GeometryT;
  using AlgebraType = typename GeometryType::AlgebraType;
  using ScalarType = typename AlgebraType::ScalarType;
  using VectorType = typename AlgebraType::VectorType;

 private:
  ScalarType offset_{0};
  VectorType offset_direction_{GeometryType::x_axis};
  ScalarType rotation_angle_{0};
  VectorType rotation_axis_{GeometryType::z_axis};

 public:
  RevoluteJoint(const VectorType& rotation_axis, const ScalarType& rotation_angle)
      : rotation_axis_(rotation_axis), rotation_angle_(rotation_angle) {}
  RevoluteJoint(const VectorType& rotation_axis) : RevoluteJoint(rotation_axis, 0) {}
  RevoluteJoint() = default;

  VectorType compose_transform(ScalarType t, const VectorType& transform) override {
    return transform * GeometryType::rotate(rotation_axis_, rotation_angle_) *
           GeometryType::translate(offset_direction_, offset_);
  }

  void set_rotation_angle(const ScalarType& rotation_angle) { rotation_angle_ = rotation_angle; }
  const ScalarType& rotation_angle() const { return rotation_angle_; }

  const VectorType& rotation_axis() const { return rotation_axis_; }
  void set_rotation_axis(const VectorType& rotation_axis) { rotation_axis_ = rotation_axis; }

  void set_offset(const ScalarType& offset) { offset_ = offset; }
  const ScalarType& offset() const { return offset_; }

  const VectorType& offset_direction() const { return offset_direction_; }
  void set_offset_direction(const VectorType& offset_direction) {
    offset_direction_ = offset_direction;
  }
};

}  // namespace ndyn::graphics
