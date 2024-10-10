#include "math/cartesian_vector.h"

#include <cmath>

#include "gtest/gtest.h"
#include "math/coordinates.h"
#include "math/unit_set.h"
#include "units.h"

namespace ndyn::math {

using FloatT = float;
using Vec2 = Vector<Coordinates::CARTESIAN, FloatT, 2,
                    UnitSet<units::length::meter_t, units::length::meter_t>>;
using Vec3 = Vector<Coordinates::CARTESIAN, FloatT, 3, UnitSet<units::length::meter_t>>;

TEST(Cartesian2DVectorTest, CanEmptyListInitialize) {
  const Vec2 v{};
  EXPECT_EQ(0, v.element<0>());
}

TEST(Cartesian2DVectorTest, CanListInitialize) {
  const Vec2 v{1, 2};
  EXPECT_EQ(1, v.element<0>());
  EXPECT_EQ(2, v.element<1>());
}

TEST(Cartesian2DVectorTest, CanCompareVectors) {
  const Vec2 v1{1, 2};
  const Vec2 v2{3, 4};
  const Vec2 v3{1, 2};
  EXPECT_NE(v1, v2);
  EXPECT_EQ(v1, v3);
}

TEST(Cartesian2DVectorTest, CanMultiplyByScalar) {
  Vec2 v{1, 2};
  v = v * 2;
  EXPECT_EQ(2, v.element<0>());
  EXPECT_EQ(4, v.element<1>());
  v = static_cast<FloatT>(2) * v;
  EXPECT_EQ(4, v.element<0>());
  EXPECT_EQ(8, v.element<1>());
}

TEST(Cartesian2DVectorTest, CanAccessWithRuntimeIndex) {
  Vec2 v{1, 2};
  v = v * 2;
  EXPECT_EQ(2, v.element(0));
  EXPECT_EQ(4, v.element(1));
  v = static_cast<FloatT>(2) * v;
  EXPECT_EQ(4, v.element(0));
  EXPECT_EQ(8, v.element(1));
}

TEST(Cartesian2DVectorTest, CanSetValueWithRuntimeIndex) {
  Vec2 v{};
  for (size_t i = 0; i < v.size(); ++i) {
    v.set_element(i, 2 * i);
  }
  EXPECT_EQ(0, v.element(0));
  EXPECT_EQ(2, v.element(1));
}

TEST(Cartesian2DVectorTest, CanDivideByScalar) {
  Vec2 v{4, 8};
  v = v / 2;
  EXPECT_EQ(2, v.element<0>());
  EXPECT_EQ(4, v.element<1>());
}

TEST(Cartesian2DVectorTest, CanAddVectors) {
  const Vec2 v1{1, 2};
  const Vec2 v2{4, 8};
  auto v3 = v1 + v2;
  EXPECT_EQ(5, v3.element<0>());
  EXPECT_EQ(10, v3.element<1>());
}

TEST(Cartesian2DVectorTest, CanSubtractVectors) {
  const Vec2 v1{1, 2};
  const Vec2 v2{4, 8};
  auto v3 = v2 - v1;
  EXPECT_EQ(3, v3.element<0>());
  EXPECT_EQ(6, v3.element<1>());
}

TEST(Cartesian2DVectorTest, CanComputeMagnitude) {
  using std::sqrt;
  const Vec2 v{1, 2};
  EXPECT_EQ(5, v.square_magnitude());
  EXPECT_NEAR(sqrt(5), v.abs(), 0.00001);
}

TEST(Cartesian2DVectorTest, CanComputeInnerProduct) {
  const Vec2 v1{1, 2};
  EXPECT_EQ(v1.square_magnitude(), v1.inner(v1));

  const Vec2 v2{3, 7};
  EXPECT_EQ(17, v1.inner(v2));

  EXPECT_EQ(v1.inner(v2), v2.inner(v1));
}

TEST(Cartesian2DVectorTest, CanDecomposeParallelToAxis) {
  using std::sqrt;

  const Vec2 v1{1, 2};

  const Vec2 axis1{1, 0};
  const Vec2 axis2{0, 1};

  const Vec2 expected1{1, 0};
  const Vec2 expected2{0, 2};
  EXPECT_EQ(expected1, v1.parallel(axis1));
  EXPECT_EQ(expected2, v1.parallel(axis2));

  const Vec2 axis3{1, 1};
  const Vec2 expected3{static_cast<FloatT>(3) / 2, static_cast<FloatT>(3) / 2};
  EXPECT_EQ(expected3, v1.parallel(axis3));
}

TEST(Cartesian2DVectorTest, CanDecomposeOrthogonalToAxis) {
  using std::sqrt;

  const Vec2 v1{1, 2};

  const Vec2 axis1{1, 0};
  const Vec2 axis2{0, 1};

  const Vec2 expected1{0, 2};
  const Vec2 expected2{1, 0};
  EXPECT_EQ(expected1, v1.orthogonal(axis1));
  EXPECT_EQ(expected2, v1.orthogonal(axis2));

  const Vec2 axis3{1, 1};
  const Vec2 expected3{static_cast<FloatT>(-1) / 2, static_cast<FloatT>(1) / 2};
  EXPECT_EQ(expected3, v1.orthogonal(axis3));
}

// A few simple tests to verify that we can work with dimensions > 2.
TEST(Cartesian3DVectorTest, CanEmptyListInitialize) {
  Vec3 v{};
  EXPECT_EQ(0, v.element<0>());
}

TEST(Cartesian3DVectorTest, CanListInitialize) {
  Vec3 v{1, 2, 3};
  EXPECT_EQ(1, v.element<0>());
  EXPECT_EQ(2, v.element<1>());
  EXPECT_EQ(3, v.element<2>());
}

TEST(Cartesian3DVectorTest, CanAddVectors) {
  const Vec3 v1{1, 2, 7};
  const Vec3 v2{4, 8, 11};
  auto v3 = v1 + v2;
  EXPECT_EQ(5, v3.element<0>());
  EXPECT_EQ(10, v3.element<1>());
  EXPECT_EQ(18, v3.element<2>());
}

}  // namespace ndyn::math
