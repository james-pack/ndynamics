#include "math/multivector_utils.h"

#include <cmath>

#include "gtest/gtest.h"
#include "math/multivector.h"

namespace ndyn::math {

TEST(MagnitudeTest, CanComputeSquareMagnitude) {
  using ScalarType = float;
  using MultivectorT = VgaMultivector<ScalarType>;

  static constexpr MultivectorT x = MultivectorT::template e<0>();
  static constexpr MultivectorT y = MultivectorT::template e<1>();
  static constexpr MultivectorT z = MultivectorT::template e<2>();

  // The square magnitude of a scalar is just that value squared.
  EXPECT_EQ(1, square_magnitude(MultivectorT{1.f}));
  EXPECT_EQ(1, square_magnitude(MultivectorT{-1.f}));
  EXPECT_EQ(9, square_magnitude(MultivectorT{3.f}));
  EXPECT_EQ(9, square_magnitude(MultivectorT{-3.f}));

  // Vectors give the length of the vector squared.
  EXPECT_EQ(2, square_magnitude(x + y));
  EXPECT_EQ(3, square_magnitude(x + y + z));
  EXPECT_EQ(5, square_magnitude(x + 2.f * y));
  EXPECT_EQ(30, square_magnitude(x + 2.f * y + 5.f * z));

  // Bivectors and trivectors in the VGA give negative square magnitudes
  EXPECT_EQ(-9, square_magnitude(3.f * x * z));
  EXPECT_EQ(-25, square_magnitude(5.f * x * y * z));
  EXPECT_EQ(-4, square_magnitude(x + 2.f * y + 3.f * x * z));
  EXPECT_EQ(-4, square_magnitude(x + 2.f * y + 3.f * x * y * z));
  EXPECT_EQ(-1.f * (-3.f * -3.f), square_magnitude(-3.f * x * z));
  EXPECT_EQ(-1.f * (-5.f * -5.f), square_magnitude(-5.f * x * y * z));
}

TEST(MagnitudeTest, CanComputeAbs) {
  using std::sqrt;

  using ScalarType = float;
  using MultivectorT = VgaMultivector<ScalarType>;

  static constexpr ScalarType EPSILON{1e-6};

  static constexpr MultivectorT x = MultivectorT::template e<0>();
  static constexpr MultivectorT y = MultivectorT::template e<1>();
  static constexpr MultivectorT z = MultivectorT::template e<2>();

  // Scalars
  EXPECT_NEAR(1, abs(MultivectorT{1.f}), EPSILON);
  EXPECT_NEAR(1, abs(MultivectorT{-1.f}), EPSILON);
  EXPECT_NEAR(3, abs(MultivectorT{3.f}), EPSILON);
  EXPECT_NEAR(3, abs(MultivectorT{-3.f}), EPSILON);

  // Vectors give the length of the vector.
  EXPECT_NEAR(sqrt(2.f), abs(x + y), EPSILON);
  EXPECT_NEAR(sqrt(3.f), abs(x + y + z), EPSILON);
  EXPECT_NEAR(sqrt(5.f), abs(x + 2.f * y), EPSILON);
  EXPECT_NEAR(sqrt(30.f), abs(x + 2.f * y + 5.f * z), EPSILON);

  // Bivectors and trivectors in the VGA do not have defined abs() values.
}

TEST(DecomposeTest, CanDecomposeTrivialGrade1Multivector) {
  using ScalarType = float;
  using MultivectorT = VgaMultivector<ScalarType>;

  static constexpr MultivectorT x = MultivectorT::template e<0>();
  static constexpr MultivectorT y = MultivectorT::template e<1>();

  static constexpr MultivectorT value = x + y;

  static constexpr auto decomposed = decompose(value, x);

  EXPECT_EQ(y, decomposed.first);
  EXPECT_EQ(x, decomposed.second);
  EXPECT_EQ(value, decomposed.first + decomposed.second);
}

TEST(DecomposeTest, CanDecomposeGrade1MultivectorOverIndependentAxis) {
  using ScalarType = float;
  using MultivectorT = VgaMultivector<ScalarType>;

  static constexpr MultivectorT x = MultivectorT::template e<0>();
  static constexpr MultivectorT y = MultivectorT::template e<1>();
  static constexpr MultivectorT z = MultivectorT::template e<2>();

  static constexpr MultivectorT value = x + y;

  static constexpr auto decomposed = decompose(value, z);

  EXPECT_EQ(value, decomposed.first);
  EXPECT_EQ(MultivectorT{}, decomposed.second);
}

TEST(DecomposeTest, CanDecomposeSimpleGrade1Multivector) {
  using ScalarType = float;
  using MultivectorT = VgaMultivector<ScalarType>;

  static constexpr MultivectorT x = MultivectorT::template e<0>();
  static constexpr MultivectorT y = MultivectorT::template e<1>();

  static constexpr MultivectorT value = x + 2.f * y;

  static constexpr auto decomposed = decompose(value, x);

  EXPECT_EQ(2.f * y, decomposed.first);
  EXPECT_EQ(x, decomposed.second);
  EXPECT_EQ(value, decomposed.first + decomposed.second);
}

TEST(DecomposeTest, CanDecomposeGrade1Multivector) {
  using ScalarType = float;
  using MultivectorT = VgaMultivector<ScalarType>;

  static constexpr MultivectorT x = MultivectorT::template e<0>();
  static constexpr MultivectorT y = MultivectorT::template e<1>();
  static constexpr MultivectorT z = MultivectorT::template e<2>();

  static constexpr MultivectorT value = x + 2.f * y + 3.f * z;

  static constexpr auto decomposed = decompose(value, x);

  EXPECT_EQ(2.f * y + 3.f * z, decomposed.first);
  EXPECT_EQ(x, decomposed.second);
  EXPECT_EQ(value, decomposed.first + decomposed.second);
}

TEST(DecomposeTest, CanDecomposeGrade1MultivectorOverNonUnitVectorAxis) {
  using ScalarType = float;
  using MultivectorT = VgaMultivector<ScalarType>;

  static constexpr MultivectorT x = MultivectorT::template e<0>();
  static constexpr MultivectorT y = MultivectorT::template e<1>();
  static constexpr MultivectorT z = MultivectorT::template e<2>();

  static constexpr MultivectorT value = x + 2.f * y + 3.f * z;

  static constexpr auto decomposed = decompose(value, 5.f * x);

  EXPECT_EQ(2.f * y + 3.f * z, decomposed.first);
  EXPECT_EQ(x, decomposed.second);
  EXPECT_EQ(value, decomposed.first + decomposed.second);
}

TEST(DecomposeTest, DecompositionDoneInConstexprContext) {
  using ScalarType = float;
  using MultivectorT = VgaMultivector<ScalarType>;

  static constexpr MultivectorT x = MultivectorT::template e<0>();
  static constexpr MultivectorT y = MultivectorT::template e<1>();
  static constexpr MultivectorT z = MultivectorT::template e<2>();

  static constexpr MultivectorT value = x + 2.f * y + 3.f * z;

  static constexpr auto decomposed = decompose(value, 5.f * x);

  static_assert(2.f * y + 3.f * z == decomposed.first, "Perpendicular decomposed value incorrect");
  static_assert(x == decomposed.second, "Parallel decomposed value incorrect");

  EXPECT_TRUE(true) << "Test in static_asserts";
}

}  // namespace ndyn::math
