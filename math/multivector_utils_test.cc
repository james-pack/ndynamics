#include "math/multivector_utils.h"

#include "gtest/gtest.h"
#include "math/multivector.h"

namespace ndyn::math {

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
