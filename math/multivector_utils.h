#pragma once

#include <cmath>
#include <utility>

#include "math/algebra.h"
#include "math/multivector.h"

namespace ndyn::math {

// Utility functions for Multivectors, especially overloads of functions that might be applicable to
// scalars and built-in types as well.

template <typename AlgebraType>
constexpr typename AlgebraType::ScalarType square_magnitude(const Multivector<AlgebraType>& value) {
  return value.square_magnitude();
}

template <typename AlgebraType>
constexpr typename AlgebraType::ScalarType abs(const Multivector<AlgebraType>& value) {
  using std::sqrt;
  return sqrt(square_magnitude(value));
}

/**
 * Decompose a Multivector into a Multivector that is perpendicular to the given axis.
 */
template <typename AlgebraType>
constexpr Multivector<AlgebraType> orthogonal(const Multivector<AlgebraType>& value,
                                              const Multivector<AlgebraType>& axis) {
  Multivector<AlgebraType> result{};
  result = axis * axis.outer(value) / square_magnitude(axis);
  return result;
}

/**
 * Decompose a Multivector into a Multivector that is parallel to the given axis.
 */
template <typename AlgebraType>
constexpr Multivector<AlgebraType> parallel(const Multivector<AlgebraType>& value,
                                            const Multivector<AlgebraType>& axis) {
  Multivector<AlgebraType> result{};
  result = value.left_contraction(axis).scalar() / square_magnitude(axis) * axis;
  return result;
}

/**
 * Decompose a Multivector into a Multivector that is parallel to the given axis and a
 * Multivector that is perpendicular to that axis. Returns a std::pair whose first element is the
 * parallel Multivector, the second is the perpendicular Multivector. The sum of the two will
 * equal the original value.
 */
template <typename AlgebraType>
constexpr std::pair<Multivector<AlgebraType>, Multivector<AlgebraType>> decompose(
    const Multivector<AlgebraType>& value, const Multivector<AlgebraType>& axis) {
  std::pair<Multivector<AlgebraType>, Multivector<AlgebraType>> result{};
  result.second = orthogonal(value, axis);
  result.first = value - result.second;
  return result;
}

/**
 * Reflect a Multivector across the given axis.
 */
template <typename AlgebraType>
constexpr Multivector<AlgebraType> reflect(const Multivector<AlgebraType>& value,
                                           const Multivector<AlgebraType>& axis) {
  return axis * value * axis / square_magnitude(axis);
}

}  // namespace ndyn::math
