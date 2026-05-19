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
 * These tests validate both the basic structural recognition and the defining
 * algebraic characteristics of a Versor: it must preserve the structural grade
 * of a vector under the sandwich product transformation (V * v * V^-1 = v').
 */
template <HasVersor G>
class HasVersorTest : public ::testing::Test {
 protected:
  using Scalar = typename G::Scalar;
  using Multivector = typename G::Multivector;
};

TYPED_TEST_SUITE_P(HasVersorTest);

// The identity element under geometric multiplication is inherently a versor of
// grade 0 (an outervector/scalar variant) across all geometric models.
TYPED_TEST_P(HasVersorTest, IdentityIsVersor) {
  using Multivector = TypeParam::Multivector;
  using Scalar = TypeParam::Scalar;
  EXPECT_TRUE(TypeParam::is_versor(Multivector{Scalar{1}}));
}

// The defining axiom of a versor V is that the sandwich product transformation
// of a direction vector 'd' preserves its vector nature (grade-1 retention).
TYPED_TEST_P(HasVersorTest, SandwichProductPreservesDirectionGrade) {
  using Multivector = TypeParam::Multivector;
  using Scalar = typename TypeParam::Scalar;
  constexpr std::size_t N{TypeParam::NUM_PHYSICAL_DIMENSIONS};

  // We fall back to the algebraic identity which is a trivial grade-0 versor,
  // ensuring this test can run for any basic model satisfying HasVersor.
  constexpr auto v{Multivector{Scalar{1}}};
  ASSERT_TRUE(TypeParam::is_versor(v));

  constexpr std::array<Scalar, N> coords{
      as_scalars<Scalar, int>(std::make_integer_sequence<int, N>())};
  constexpr auto d{TypeParam::make_direction(coords.cbegin(), coords.cend())};

  const auto transformed_d{v * d * v.inverse()};

  EXPECT_TRUE(TypeParam::is_direction(transformed_d));
}

REGISTER_TYPED_TEST_SUITE_P(HasVersorTest,     //
                            IdentityIsVersor,  //
                            SandwichProductPreservesDirectionGrade);

}  // namespace ndyn::math
