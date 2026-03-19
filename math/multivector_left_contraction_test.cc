#include <cmath>

#include "glog/logging.h"
#include "gtest/gtest.h"
#include "math/canonical_basis_representation.h"
#include "math/multivector.h"

namespace ndyn::math {

/**
 * These tests were designed to isolate some particular bugs in the left contraction operation on
 * algebras with degenerate bases. For more general tests, see multivector_test.cc.
 */
TEST(MultiplyTest, Pga2dBasisOnInvertiblePseudoscalar) {
  using AlgebraType = Pga2d<float>;
  using Vector = typename AlgebraType::VectorType;

  Vector I_prime{Vector::invertible_pseudoscalar()};
  Vector e1{Vector::e<1>()};
  Vector e2{Vector::e<2>()};

  EXPECT_EQ(e2, e1.multiply(I_prime));
  EXPECT_EQ(-e1, e2.multiply(I_prime));
}

TEST(MultiplyTest, PgaBasisOnInvertiblePseudoscalar) {
  using AlgebraType = Pga<float>;
  using Vector = typename AlgebraType::VectorType;

  Vector I_prime{Vector::invertible_pseudoscalar()};
  Vector e1{Vector::e<1>()};
  Vector e2{Vector::e<2>()};
  Vector e3{Vector::e<3>()};

  EXPECT_EQ(e2 * e3, e1.multiply(I_prime));
  EXPECT_EQ(-e1 * e3, e2.multiply(I_prime));
  EXPECT_EQ(e1 * e2, e3.multiply(I_prime));
}

TEST(LeftContractionTest, Pga2dBasisOnInvertiblePseudoscalar) {
  using AlgebraType = Pga2d<float>;
  using Vector = typename AlgebraType::VectorType;

  Vector I_prime{Vector::invertible_pseudoscalar()};
  Vector e1{Vector::e<1>()};
  Vector e2{Vector::e<2>()};

  EXPECT_EQ(e2, e1.left_contraction(I_prime));
  EXPECT_EQ(-e1, e2.left_contraction(I_prime));
}

TEST(LeftContractionTest, PgaBasisOnInvertiblePseudoscalar) {
  using AlgebraType = Pga<float>;
  using Vector = typename AlgebraType::VectorType;

  Vector I_prime{Vector::invertible_pseudoscalar()};
  Vector e1{Vector::e<1>()};
  Vector e2{Vector::e<2>()};
  Vector e3{Vector::e<3>()};

  EXPECT_EQ(e2 * e3, e1.left_contraction(I_prime));
  EXPECT_EQ(-e1 * e3, e2.left_contraction(I_prime));
  EXPECT_EQ(e1 * e2, e3.left_contraction(I_prime));
}

}  // namespace ndyn::math
