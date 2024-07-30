#include "math/geometry.h"

#include "gtest/gtest.h"

namespace ndyn::math {

TEST(GeometryTest, CanCompile) {
  static constexpr ScalarMultivector<float> a{};
  EXPECT_TRUE(true);
}

TEST(GeometryTest, CanInitializeToScalar) {
  static constexpr float SCALAR{1};
  static constexpr ScalarMultivector<float> a{SCALAR};
  EXPECT_EQ(a.scalar(), SCALAR);
}

TEST(GeometryTest, CanAddScalars) {
  static constexpr float SCALAR_1{1};
  static constexpr ScalarMultivector<float> m1{SCALAR_1};
  static constexpr float SCALAR_2{2};
  static constexpr ScalarMultivector<float> m2{SCALAR_2};
  EXPECT_EQ(m1.add(m2).scalar(), SCALAR_1 + SCALAR_2);
}

TEST(GeometryTest, CanMultiplySimpleComplexNumbers) {
  static constexpr auto i{ComplexMultivector<float>::e<0>()};
  EXPECT_EQ(-1.f, i.multiply(i).scalar());
}

TEST(GeometryTest, CanMultiplyComplexNumbers) {
  static constexpr auto u{ComplexMultivector<float>::e<0>().add(1.f)};  // 1 + i
  static constexpr auto v{u.multiply(u)};                               // (1 + i)^2 = 2i
  static constexpr auto w{v.multiply(v)};                               // (2i)^2 = -4

  static_assert(v.scalar() == 0.f,
                "Verifying that we can do multivector operations in constexpr contexts");
  static_assert(w.scalar() == -4.f,
                "Verifying that we can do multivector operations in constexpr contexts");

  ASSERT_EQ(1.f, u.scalar());
  EXPECT_EQ(0.f, v.scalar());
  EXPECT_EQ(-4.f, w.scalar());
  EXPECT_EQ(16.f, w.multiply(w).scalar());
}

}  // namespace ndyn::math
