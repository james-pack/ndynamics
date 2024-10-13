#pragma once

#include <algorithm>
#include <cmath>

#include "gtest/gtest.h"

namespace ndyn::math {

template <typename ScalarType>
ScalarType epsilon(ScalarType scale, ScalarType absolute = 0.0001) {
  using std::abs;
  return std::max(abs(scale * static_cast<ScalarType>(0.00001)), abs(absolute));
}

template <typename VectorType>
::testing::AssertionResult AreNear(const VectorType& lhs, const VectorType& rhs,
                                   typename VectorType::ScalarType epsilon) {
  using std::abs;
  using std::to_string;
  const auto difference{lhs - rhs};
  epsilon = abs(epsilon);
  for (size_t i = 0; i < VectorType::size(); ++i) {
    if (abs(difference.element(i)) > epsilon) {
      return ::testing::AssertionFailure()
             << "lhs: " << lhs << ", rhs: " << rhs << ", difference: " << difference
             << " (epsilon: " << to_string(epsilon) << ")";
    }
  }
  return ::testing::AssertionSuccess()
         << "lhs: " << lhs << ", rhs: " << rhs << ", difference: " << difference
         << " (epsilon: " << to_string(epsilon) << ")";
}

template <typename VectorType>
::testing::AssertionResult AreNear(const VectorType& lhs, const VectorType& rhs) {
  using ScalarType = typename VectorType::ScalarType;
  const ScalarType greater_mag{std::max(lhs.square_magnitude(), rhs.square_magnitude())};
  return AreNear(lhs, rhs, epsilon(greater_mag));
}

}  // namespace ndyn::math
