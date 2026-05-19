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
 * These tests probe whether a geometry's direction API is self-consistent and
 * adheres to the invariants of Geometric Algebra. Every valid direction vector
 * must be recognized by its predicates and square to a pure scalar element.
 */
template <HasDirection G>
class HasDirectionTest : public ::testing::Test {
 protected:
  using Scalar = typename G::Scalar;
};

TYPED_TEST_SUITE_P(HasDirectionTest);

// Canonical construction through make_direction with an iterator range must
// produce a multivector that the geometry recognizes as a valid direction.
TYPED_TEST_P(HasDirectionTest, DirectionFromIteratorsIsDirection) {
  using Scalar = typename TypeParam::Scalar;
  constexpr std::size_t N{TypeParam::NUM_PHYSICAL_DIMENSIONS};

  constexpr std::array<Scalar, N> coords{
      as_scalars<Scalar, int>(std::make_integer_sequence<int, N>())};

  const auto d{TypeParam::make_direction(coords.cbegin(), coords.cend())};
  EXPECT_TRUE(TypeParam::is_direction(d));
}

// Directions generated from coordinates must satisfy the fundamental axiom of
// GA: the geometric product of a vector with itself must collapse into a scalar
// grade element (v^2 = alpha).
TYPED_TEST_P(HasDirectionTest, DirectionSquaresToScalar) {
  using Scalar = typename TypeParam::Scalar;
  constexpr std::size_t N{TypeParam::NUM_PHYSICAL_DIMENSIONS};

  constexpr std::array<Scalar, N> coords{
      as_scalars<Scalar, int>(std::make_integer_sequence<int, N>())};

  constexpr auto d{TypeParam::make_direction(coords.cbegin(), coords.cend())};
  ASSERT_TRUE(TypeParam::is_direction(d));

  constexpr auto d_squared{d * d};
  EXPECT_TRUE(d_squared.is_scalar());
}

// A non-null direction vector must possess a valid, computable geometric inverse
// such that d * d_inverse equals the algebraic identity element.
TYPED_TEST_P(HasDirectionTest, NonNullDirectionHasGeometricInverse) {
  using Scalar = typename TypeParam::Scalar;
  constexpr std::size_t N{TypeParam::NUM_PHYSICAL_DIMENSIONS};

  constexpr std::array<Scalar, N> coords{
      as_scalars<Scalar, int>(std::make_integer_sequence<int, N>())};

  constexpr auto d{TypeParam::make_direction(coords.cbegin(), coords.cend())};
  constexpr auto d_squared{d * d};

  // If the model contains null vectors (e.g., Conformal GA), we skip the inverse
  // check only if the squared value is precisely zero.
  if (!d_squared.near_zero()) {
    constexpr auto identity_check{d * d.inverse()};
    EXPECT_TRUE(identity_check.near_one());
  }
}

REGISTER_TYPED_TEST_SUITE_P(HasDirectionTest,                   //
                            DirectionFromIteratorsIsDirection,  //
                            DirectionSquaresToScalar,           //
                            NonNullDirectionHasGeometricInverse);

}  // namespace ndyn::math
