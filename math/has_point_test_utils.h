#pragma once

#include <array>
#include <cmath>
#include <type_traits>
#include <utility>

#include "gtest/gtest.h"
#include "math/geometry_model.h"
#include "math/scalar_test_utils.h"

namespace ndyn::math {

/**
 * These tests probe whether a geometry's point API is self-consistent. They do
 * not assume any particular geometric model (Euclidean, conformal, projective,
 * etc.), only that the geometry satisfies HasPoint.
 */
template <HasPoint G>
class HasPointTest : public ::testing::Test {
 protected:
  using Scalar = typename G::Scalar;
};

TYPED_TEST_SUITE_P(HasPointTest);

// The origin must be recognised as a point by the geometry's own predicate.
TYPED_TEST_P(HasPointTest, OriginIsPoint) {
  const auto origin = TypeParam::origin();
  EXPECT_TRUE(TypeParam::is_point(origin));
}

// Canonical construction through make_point with an iterator range must
// produce something the geometry recognises as a point. We build a zero
// vector to cover every dimension count ≥ 1.
TYPED_TEST_P(HasPointTest, PointFromIteratorsIsPoint) {
  using Scalar = typename TypeParam::Scalar;
  constexpr std::size_t N{TypeParam::NUM_PHYSICAL_DIMENSIONS};

  constexpr std::array<Scalar, N> coords{
      as_scalars<Scalar, int>(std::make_integer_sequence<int, N>())};

  constexpr auto p{TypeParam::make_point(coords.begin(), coords.end())};
  EXPECT_TRUE(TypeParam::is_point(p));
}

REGISTER_TYPED_TEST_SUITE_P(HasPointTest,   //
                            OriginIsPoint,  //
                            PointFromIteratorsIsPoint);

}  // namespace ndyn::math
