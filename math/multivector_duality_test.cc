#include "gtest/gtest.h"
#include "math/algebra.h"
#include "math/canonical_basis_representation.h"
#include "math/multivector.h"

namespace ndyn::math {

TEST(PgaMultivectorDualityTest, DualOfTrivialGrade0) {
  using Algebra = Pga<>;
  using Multivector = Algebra::VectorType;
  using Scalar = Algebra::ScalarType;

  static constexpr Scalar FACTOR{1};
  static constexpr auto a{Multivector{FACTOR}};
  static constexpr auto expected{FACTOR * Multivector::e<0>() * Multivector::e<1>() *
                                 Multivector::e<2>() * Multivector::e<3>()};

  EXPECT_EQ(expected, a.dual());
}

TEST(PgaMultivectorDualityTest, DualOfGrade0) {
  using Algebra = Pga<>;
  using Multivector = Algebra::VectorType;
  using Scalar = Algebra::ScalarType;

  static constexpr Scalar FACTOR{2};
  static constexpr auto a{Multivector{FACTOR}};
  static constexpr auto expected{FACTOR * Multivector::e<0>() * Multivector::e<1>() *
                                 Multivector::e<2>() * Multivector::e<3>()};

  EXPECT_EQ(expected, a.dual());
}

TEST(PgaMultivectorDualityTest, DualOfTrivialGrade1) {
  using Algebra = Pga<>;
  using Multivector = Algebra::VectorType;
  using Scalar = Algebra::ScalarType;

  static constexpr Scalar FACTOR{1};
  static constexpr auto a{FACTOR * Multivector::e<1>()};
  static constexpr auto expected{-1 * FACTOR * Multivector::e<0>() * Multivector::e<2>() *
                                 Multivector::e<3>()};

  EXPECT_EQ(expected, a.dual());
}

TEST(PgaMultivectorDualityTest, DualOfGrade1) {
  using Algebra = Pga<>;
  using Multivector = Algebra::VectorType;
  using Scalar = Algebra::ScalarType;

  static constexpr Scalar FACTOR{2};
  static constexpr auto a{FACTOR * Multivector::e<1>()};
  static constexpr auto expected{-1 * FACTOR * Multivector::e<0>() * Multivector::e<2>() *
                                 Multivector::e<3>()};

  EXPECT_EQ(expected, a.dual());
}

TEST(PgaMultivectorDualityTest, DualOfTrivialDegenerateGrade1) {
  using Algebra = Pga<>;
  using Multivector = Algebra::VectorType;
  using Scalar = Algebra::ScalarType;

  static constexpr Scalar FACTOR{1};
  static constexpr auto a{FACTOR * Multivector::e<0>()};
  static constexpr auto reversed_pseudoscalar{Multivector::pseudoscalar().reverse()};
  static constexpr auto expected{FACTOR * Multivector::e<1>() * Multivector::e<2>() *
                                 Multivector::e<3>()};
  static constexpr auto a_dual{a.dual()};

  EXPECT_EQ(expected, a_dual) << "a: " << a << ", reversed_pseudoscalar: " << reversed_pseudoscalar
                              << ", a_dual: " << a_dual << ", expected: " << expected;
}

TEST(PgaMultivectorDualityTest, DualOfDegenerateGrade1) {
  using Algebra = Pga<>;
  using Multivector = Algebra::VectorType;
  using Scalar = Algebra::ScalarType;

  static constexpr Scalar FACTOR{2};
  static constexpr auto a{FACTOR * Multivector::e<0>()};
  static constexpr auto reversed_pseudoscalar{Multivector::pseudoscalar().reverse()};
  static constexpr auto expected{FACTOR * Multivector::e<1>() * Multivector::e<2>() *
                                 Multivector::e<3>()};
  static constexpr auto a_dual{a.dual()};

  EXPECT_EQ(expected, a_dual) << "a: " << a << ", reversed_pseudoscalar: " << reversed_pseudoscalar
                              << ", a_dual: " << a_dual << ", expected: " << expected;
}

}  // namespace ndyn::math
