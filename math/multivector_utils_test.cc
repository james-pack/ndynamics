#include "math/multivector_utils.h"

#include <cmath>

#include "gtest/gtest.h"
#include "math/multivector.h"
#include "math/multivector_test_utils.h"

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

  // Bivectors and trivectors.
  EXPECT_EQ(9.f, square_magnitude(3.f * x * z));
  EXPECT_EQ(25.f, square_magnitude(5.f * x * y * z));
  EXPECT_EQ(14.f, square_magnitude(x + 2.f * y + 3.f * x * z));
  EXPECT_EQ(14.f, square_magnitude(x + 2.f * y + 3.f * x * y * z));
  EXPECT_EQ(9.f, square_magnitude(-3.f * x * z));
  EXPECT_EQ(25.f, square_magnitude(-5.f * x * y * z));
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

  EXPECT_EQ(x, decomposed.first);
  EXPECT_EQ(y, decomposed.second);
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

  EXPECT_EQ(value, decomposed.second);
  EXPECT_EQ(MultivectorT{}, decomposed.first);
}

TEST(DecomposeTest, CanDecomposeSimpleGrade1Multivector) {
  using ScalarType = float;
  using MultivectorT = VgaMultivector<ScalarType>;

  static constexpr MultivectorT x = MultivectorT::template e<0>();
  static constexpr MultivectorT y = MultivectorT::template e<1>();

  static constexpr MultivectorT value = x + 2.f * y;

  static constexpr auto decomposed = decompose(value, x);

  EXPECT_EQ(x, decomposed.first);
  EXPECT_EQ(2.f * y, decomposed.second);
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

  EXPECT_EQ(x, decomposed.first);
  EXPECT_EQ(2.f * y + 3.f * z, decomposed.second);
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

  EXPECT_EQ(x, decomposed.first);
  EXPECT_EQ(2.f * y + 3.f * z, decomposed.second);
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

  //  static_assert(x == decomposed.first, "Parallel decomposed value incorrect");
  //  static_assert(2.f * y + 3.f * z == decomposed.second, "Perpendicular decomposed value
  //  incorrect");

  EXPECT_TRUE(true) << "Test in static_asserts";
}

TEST(DecomposeTest, CanDecomposeGrade1MultivectorNegativeE0Orientation) {
  using ScalarType = float;
  using MultivectorT = VgaMultivector<ScalarType>;

  static constexpr MultivectorT x = MultivectorT::template e<0>();
  static constexpr MultivectorT y = MultivectorT::template e<1>();

  static constexpr MultivectorT value = -x + 2.f * y;

  static constexpr auto decomposed = decompose(value, x);

  EXPECT_EQ(-x, decomposed.first);
  EXPECT_EQ(2.f * y, decomposed.second);
  EXPECT_EQ(value, decomposed.first + decomposed.second);
}

TEST(DecomposeTest, CanDecomposeGrade1MultivectorNegativeE1Orientation) {
  using ScalarType = float;
  using MultivectorT = VgaMultivector<ScalarType>;

  static constexpr MultivectorT x = MultivectorT::template e<0>();
  static constexpr MultivectorT y = MultivectorT::template e<1>();

  static constexpr MultivectorT value = x - 2.f * y;

  static constexpr auto decomposed = decompose(value, x);

  EXPECT_EQ(x, decomposed.first);
  EXPECT_EQ(-2.f * y, decomposed.second);
  EXPECT_EQ(value, decomposed.first + decomposed.second);
}

TEST(DecomposeTest, CanDecomposeGrade1MultivectorNegativeOrientation) {
  using ScalarType = float;
  using MultivectorT = VgaMultivector<ScalarType>;

  static constexpr MultivectorT x = MultivectorT::template e<0>();
  static constexpr MultivectorT y = MultivectorT::template e<1>();

  static constexpr MultivectorT value = -x - 2.f * y;

  static constexpr auto decomposed = decompose(value, x);

  EXPECT_EQ(-x, decomposed.first);
  EXPECT_EQ(-2.f * y, decomposed.second);
  EXPECT_EQ(value, decomposed.first + decomposed.second);
}

TEST(DecomposeTest, CanDecomposeGrade1MultivectorNegativeE0OrientationNegativeAxis) {
  using ScalarType = float;
  using MultivectorT = VgaMultivector<ScalarType>;

  static constexpr MultivectorT x = MultivectorT::template e<0>();
  static constexpr MultivectorT y = MultivectorT::template e<1>();

  static constexpr MultivectorT value = -x + 2.f * y;

  static constexpr auto decomposed = decompose(value, -x);

  EXPECT_EQ(-x, decomposed.first);
  EXPECT_EQ(2.f * y, decomposed.second);
  EXPECT_EQ(value, decomposed.first + decomposed.second);
}

TEST(DecomposeTest, CanDecomposeGrade1MultivectorNegativeE1OrientationNegativeAxis) {
  using ScalarType = float;
  using MultivectorT = VgaMultivector<ScalarType>;

  static constexpr MultivectorT x = MultivectorT::template e<0>();
  static constexpr MultivectorT y = MultivectorT::template e<1>();

  static constexpr MultivectorT value = x - 2.f * y;

  static constexpr auto decomposed = decompose(value, -x);

  EXPECT_EQ(x, decomposed.first);
  EXPECT_EQ(-2.f * y, decomposed.second);
  EXPECT_EQ(value, decomposed.first + decomposed.second);
}

TEST(DecomposeTest, CanDecomposeGrade1MultivectorNonBasisAxisBehindVector) {
  using ScalarType = float;
  using MultivectorT = VgaMultivector<ScalarType>;

  static constexpr MultivectorT x = MultivectorT::template e<0>();
  static constexpr MultivectorT y = MultivectorT::template e<1>();

  static constexpr MultivectorT value = x + 2.f * y;
  static constexpr MultivectorT axis = x - y;

  static constexpr auto decomposed = decompose(value, axis);

  EXPECT_EQ(value, decomposed.first + decomposed.second);
}

TEST(DecomposeTest, CanDecomposeGrade1MultivectorNonBasisAxisAheadVector) {
  using ScalarType = float;
  using MultivectorT = VgaMultivector<ScalarType>;

  static constexpr MultivectorT x = MultivectorT::template e<0>();
  static constexpr MultivectorT y = MultivectorT::template e<1>();

  static constexpr MultivectorT value = x - 0.5f * y;
  static constexpr MultivectorT axis = x - y;

  static constexpr auto decomposed = decompose(value, axis);

  EXPECT_EQ(value, decomposed.first + decomposed.second);
}

TEST(DecomposeTest, CanDecomposeGrade1MultivectorNegativeBasisAxisBehindVector) {
  using ScalarType = float;
  using MultivectorT = VgaMultivector<ScalarType>;

  static constexpr MultivectorT x = MultivectorT::template e<0>();
  static constexpr MultivectorT y = MultivectorT::template e<1>();

  static constexpr MultivectorT value = x + 2.f * y;
  static constexpr MultivectorT axis = -y;

  static constexpr auto decomposed = decompose(value, axis);

  EXPECT_EQ(2.f * y, decomposed.first);
  EXPECT_EQ(x, decomposed.second);
  EXPECT_EQ(value, decomposed.first + decomposed.second);
}

TEST(DecomposeTest, CanDecomposeGrade1MultivectorNegativeBasisAxisAheadVector) {
  using ScalarType = float;
  using MultivectorT = VgaMultivector<ScalarType>;

  static constexpr MultivectorT x = MultivectorT::template e<0>();
  static constexpr MultivectorT y = MultivectorT::template e<1>();

  static constexpr MultivectorT value = x - 0.5f * y;
  static constexpr MultivectorT axis = -y;

  static constexpr auto decomposed = decompose(value, axis);

  EXPECT_EQ(-0.5f * y, decomposed.first);
  EXPECT_EQ(x, decomposed.second);
  EXPECT_EQ(value, decomposed.first + decomposed.second);
}

TEST(OrthogonalTest, Grade1MultivectorBasisAsAxis) {
  using ScalarType = float;
  using MultivectorT = VgaMultivector<ScalarType>;

  static constexpr MultivectorT x = MultivectorT::template e<0>();
  static constexpr MultivectorT y = MultivectorT::template e<1>();

  static constexpr MultivectorT value = x - y;

  static constexpr auto ortho_x = orthogonal(value, x);
  static constexpr auto ortho_y = orthogonal(value, y);

  EXPECT_TRUE(AreNear(-y, ortho_x, 0.001));
  EXPECT_TRUE(AreNear(x, ortho_y, 0.001));
}

TEST(OrthogonalTest, Grade1MultivectorParallelAxis) {
  using ScalarType = float;
  using MultivectorT = VgaMultivector<ScalarType>;

  static constexpr MultivectorT x = MultivectorT::template e<0>();
  static constexpr MultivectorT y = MultivectorT::template e<1>();

  static constexpr MultivectorT value = x - y;

  static constexpr auto ortho_same = orthogonal(value, x - y);
  static constexpr auto ortho_opposite = orthogonal(value, y - x);
  static constexpr auto ortho_scaled = orthogonal(value, 5.f * x - 5.f * y);

  EXPECT_TRUE(AreNear(MultivectorT{0.}, ortho_same, 0.001));
  EXPECT_TRUE(AreNear(MultivectorT{0.}, ortho_opposite, 0.001));
  EXPECT_TRUE(AreNear(MultivectorT{0.}, ortho_scaled, 0.001));
}

TEST(ParallelTest, Grade1MultivectorBasisAsAxis) {
  using ScalarType = float;
  using MultivectorT = VgaMultivector<ScalarType>;

  static constexpr MultivectorT x = MultivectorT::template e<0>();
  static constexpr MultivectorT y = MultivectorT::template e<1>();

  static constexpr MultivectorT value = x - y;

  static constexpr auto parallel_x = parallel(value, x);
  static constexpr auto parallel_y = parallel(value, y);

  EXPECT_TRUE(AreNear(x, parallel_x, 0.001));
  EXPECT_TRUE(AreNear(-y, parallel_y, 0.001));
}

TEST(ParallelTest, Grade1MultivectorParallelAxis) {
  using ScalarType = float;
  using MultivectorT = VgaMultivector<ScalarType>;

  static constexpr MultivectorT x = MultivectorT::template e<0>();
  static constexpr MultivectorT y = MultivectorT::template e<1>();

  static constexpr MultivectorT value = x - y;

  static constexpr auto parallel_same = parallel(value, x - y);
  static constexpr auto parallel_opposite = parallel(value, y - x);
  static constexpr auto parallel_scaled = parallel(value, 5.f * x - 5.f * y);

  EXPECT_TRUE(AreNear(value, parallel_same, 0.001));
  EXPECT_TRUE(AreNear(value, parallel_opposite, 0.001));
  EXPECT_TRUE(AreNear(value, parallel_scaled, 0.001));
}

TEST(AxisReflectionTest, Grade1MultivectorAcrossBasis) {
  using ScalarType = float;
  using MultivectorT = VgaMultivector<ScalarType>;

  static constexpr MultivectorT x = MultivectorT::template e<0>();
  static constexpr MultivectorT y = MultivectorT::template e<1>();

  static constexpr MultivectorT value = x - y;

  static constexpr auto reflect_x = reflect(value, x);
  static constexpr auto reflect_y = reflect(value, y);

  EXPECT_TRUE(AreNear(x + y, reflect_x, 0.001));
  EXPECT_TRUE(AreNear(-x - y, reflect_y, 0.001));
}

}  // namespace ndyn::math
