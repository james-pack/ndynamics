#pragma once

#include "graphics/gpu_element.h"
#include "math/geometry.h"

namespace ndyn::graphics {

/**
 * Element that provides a positional offset for its subelements.
 */
template <typename GeometryT>
class Offset final : public GpuElement<GeometryT> {
 public:
  using GeometryType = GeometryT;
  using AlgebraType = typename GeometryType::AlgebraType;
  using ScalarType = typename AlgebraType::ScalarType;
  using VectorType = typename AlgebraType::VectorType;

 private:
  ScalarType distance_{0};
  VectorType direction_{GeometryType::y_axis};

 public:
  Offset(const ScalarType& distance, const VectorType& direction)
      : distance_(distance), direction_(direction) {}
  Offset() = default;

  VectorType compose_transform(ScalarType t, const VectorType& transform) override {
    return transform * GeometryType::translate(direction_, distance_);
  }

  void set_distance(const ScalarType& distance) { distance_ = distance; }
  const ScalarType& distance() const { return distance_; }

  const VectorType& direction() const { return direction_; }
  void set_direction(const VectorType& direction) { direction_ = direction; }
};

}  // namespace ndyn::graphics
