#pragma once

#include <cmath>
#include <cstring>
#include <string>

#include "gtest/gtest.h"
#include "math/multivector.h"

namespace ndyn::math {

template <typename MultivectorT>
::testing::AssertionResult AreNear(const MultivectorT& lhs, const MultivectorT& rhs,
                                   typename MultivectorT::ScalarType epsilon) {
  using std::abs;
  using std::to_string;
  const auto difference{lhs - rhs};
  epsilon = abs(epsilon);
  for (size_t i = 0; i < MultivectorT::component_count(); ++i) {
    if (abs(difference.component(i)) > epsilon) {
      return ::testing::AssertionFailure()
             << "lhs: " << lhs << ", rhs: " << rhs << ", difference: " << difference
             << " (epsilon: " << to_string(epsilon) << ")";
    }
  }
  return ::testing::AssertionSuccess();
}

}  // namespace ndyn::math
