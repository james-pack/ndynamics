#pragma once

#include <utility>

#include "math/magnitude.h"
#include "math/multivector.h"

namespace ndyn::math {

// Utility functions for Multivectors, especially overloads of functions that might be applicable to
// scalars and built-in types as well.

template <typename T, size_t POSITIVE_BASES, size_t NEGATIVE_BASES, size_t ZERO_BASES,
          InnerProduct INNER_PRODUCT_STYLE>
constexpr T square_magnitude(
    const Multivector<T, POSITIVE_BASES, NEGATIVE_BASES, ZERO_BASES, INNER_PRODUCT_STYLE>& value) {
  return (value * value).scalar();
}

template <typename T, size_t POSITIVE_BASES, size_t NEGATIVE_BASES, size_t ZERO_BASES,
          InnerProduct INNER_PRODUCT_STYLE>
constexpr T abs(
    const Multivector<T, POSITIVE_BASES, NEGATIVE_BASES, ZERO_BASES, INNER_PRODUCT_STYLE>& value) {
  using std::sqrt;
  return sqrt(square_magnitude(value));
}

/**
 * Decompose a Multivector into a Multivector that is perpendicular to the given axis and a
 * Multivector that is parallel to that axis. Returns a std::pair whose first element is the
 * perpendicular Multivector, the second is the  parallel  Multivector. The sum of the two will
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
  result.first = axis * axis.outer(value) / square_magnitude(axis);
  result.second = value - result.first;
  return result;
}

}  // namespace ndyn::math
