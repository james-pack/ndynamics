#pragma once

#include <cmath>

#include "math/vector.h"

namespace ndyn::math {

template <typename VectorType>
typename VectorType::ScalarType compute_angle(const VectorType& v1, const VectorType& v2) {
  using ScalarType = typename VectorType::ScalarType;
  const ScalarType projected_length{v1.inner(v2) / v2.abs()};
  const ScalarType length{v1.abs()};
  ScalarType result{std::acos(projected_length / length)};
  return result;
}

}  // namespace ndyn::math
