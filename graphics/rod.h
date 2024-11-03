#pragma once

#include "graphics/gpu_element.h"
#include "math/geometry.h"

namespace ndyn::graphics {

template <typename ScalarType>
class Rod final : public GpuElement<ScalarType> {
 private:
  ScalarType distance_{0};
  math::Primitive<ScalarType> direction_{math::Primitive<ScalarType>::y_axis()};

 public:
  math::Transform<ScalarType> compose_transform(
      ScalarType t, const math::Transform<ScalarType>& transform) override {
    return transform.compose(math::Transform<ScalarType>::translate(direction_, distance_));
  }

  void set_distance(const ScalarType& distance) { distance_ = distance; }
  const ScalarType& distance() const { return distance_; }

  const math::Primitive<ScalarType>& direction() const { return direction_; }
  void set_direction(const math::Primitive<ScalarType>& direction) { direction_ = direction; }
};

}  // namespace ndyn::graphics
