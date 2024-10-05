#pragma once

#include <cmath>
#include <string>

#include "gtest/gtest.h"
#include "math/multivector_test_utils.h"

namespace ndyn::math {

template <typename StateT>
::testing::AssertionResult AreStatesNear(const StateT& lhs, const StateT& rhs,
                                         typename StateT::ScalarType epsilon) {
  for (size_t element = 0; element < StateT::depth(); ++element) {
    ::testing::AssertionResult result{AreNear(lhs.element(element), rhs.element(element), epsilon)
                                      << "element: " << element};
    if (!result) {
      return result;
    }
  }

  return ::testing::AssertionSuccess();
}

}  // namespace ndyn::math
