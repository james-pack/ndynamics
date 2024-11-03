#pragma once

#include <cmath>
#include <utility>

#include "math/multivector.h"

namespace ndyn::math {

// Utility functions for Multivectors, especially overloads of functions that might be applicable to
// scalars and built-in types as well.

template <typename ScalarType, size_t POSITIVE_BASES, size_t NEGATIVE_BASES, size_t ZERO_BASES,
          InnerProduct INNER_PRODUCT_STYLE>
constexpr ScalarType square_magnitude(const Multivector<ScalarType, POSITIVE_BASES, NEGATIVE_BASES,
                                                        ZERO_BASES, INNER_PRODUCT_STYLE>& value) {
  return value.square_magnitude();
}

template <typename ScalarType, size_t POSITIVE_BASES, size_t NEGATIVE_BASES, size_t ZERO_BASES,
          InnerProduct INNER_PRODUCT_STYLE>
constexpr ScalarType abs(const Multivector<ScalarType, POSITIVE_BASES, NEGATIVE_BASES, ZERO_BASES,
                                           INNER_PRODUCT_STYLE>& value) {
  using std::sqrt;
  return sqrt(square_magnitude(value));
}

/**
 * Decompose a Multivector into a Multivector that is perpendicular to the given axis.
 */
template <typename T, size_t POSITIVE_BASES, size_t NEGATIVE_BASES, size_t ZERO_BASES,
          InnerProduct INNER_PRODUCT_STYLE>
constexpr Multivector<T, POSITIVE_BASES, NEGATIVE_BASES, ZERO_BASES, INNER_PRODUCT_STYLE>
orthogonal(
    const Multivector<T, POSITIVE_BASES, NEGATIVE_BASES, ZERO_BASES, INNER_PRODUCT_STYLE>& value,
    const Multivector<T, POSITIVE_BASES, NEGATIVE_BASES, ZERO_BASES, INNER_PRODUCT_STYLE>& axis) {
  using MultivectorT =
      Multivector<T, POSITIVE_BASES, NEGATIVE_BASES, ZERO_BASES, INNER_PRODUCT_STYLE>;

  MultivectorT result{};
  result = axis * axis.outer(value) / square_magnitude(axis);
  return result;
}

/**
 * Decompose a Multivector into a Multivector that is parallel to the given axis.
 */
template <typename T, size_t POSITIVE_BASES, size_t NEGATIVE_BASES, size_t ZERO_BASES,
          InnerProduct INNER_PRODUCT_STYLE>
constexpr Multivector<T, POSITIVE_BASES, NEGATIVE_BASES, ZERO_BASES, INNER_PRODUCT_STYLE> parallel(
    const Multivector<T, POSITIVE_BASES, NEGATIVE_BASES, ZERO_BASES, INNER_PRODUCT_STYLE>& value,
    const Multivector<T, POSITIVE_BASES, NEGATIVE_BASES, ZERO_BASES, INNER_PRODUCT_STYLE>& axis) {
  using MultivectorT =
      Multivector<T, POSITIVE_BASES, NEGATIVE_BASES, ZERO_BASES, INNER_PRODUCT_STYLE>;

  MultivectorT result{};
  result = value.left_contraction(axis).scalar() / square_magnitude(axis) * axis;
  return result;
}

/**
 * Decompose a Multivector into a Multivector that is parallel to the given axis and a
 * Multivector that is perpendicular to that axis. Returns a std::pair whose first element is the
 * parallel Multivector, the second is the perpendicular Multivector. The sum of the two will
 * equal the original value.
 */
template <typename T, size_t POSITIVE_BASES, size_t NEGATIVE_BASES, size_t ZERO_BASES,
          InnerProduct INNER_PRODUCT_STYLE>
constexpr std::pair<Multivector<T, POSITIVE_BASES, NEGATIVE_BASES, ZERO_BASES, INNER_PRODUCT_STYLE>,
                    Multivector<T, POSITIVE_BASES, NEGATIVE_BASES, ZERO_BASES, INNER_PRODUCT_STYLE>>
decompose(
    const Multivector<T, POSITIVE_BASES, NEGATIVE_BASES, ZERO_BASES, INNER_PRODUCT_STYLE>& value,
    const Multivector<T, POSITIVE_BASES, NEGATIVE_BASES, ZERO_BASES, INNER_PRODUCT_STYLE>& axis) {
  using MultivectorT =
      Multivector<T, POSITIVE_BASES, NEGATIVE_BASES, ZERO_BASES, INNER_PRODUCT_STYLE>;

  std::pair<MultivectorT, MultivectorT> result{};
  result.second = orthogonal(value, axis);
  result.first = value - result.second;
  return result;
}

/**
 * Reflect a Multivector across the given axis.
 */
template <typename T, size_t POSITIVE_BASES, size_t NEGATIVE_BASES, size_t ZERO_BASES,
          InnerProduct INNER_PRODUCT_STYLE>
constexpr Multivector<T, POSITIVE_BASES, NEGATIVE_BASES, ZERO_BASES, INNER_PRODUCT_STYLE> reflect(
    const Multivector<T, POSITIVE_BASES, NEGATIVE_BASES, ZERO_BASES, INNER_PRODUCT_STYLE>& value,
    const Multivector<T, POSITIVE_BASES, NEGATIVE_BASES, ZERO_BASES, INNER_PRODUCT_STYLE>& axis) {
  return axis * value * axis / square_magnitude(axis);
}

}  // namespace ndyn::math
