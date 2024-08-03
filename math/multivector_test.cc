#include "math/multivector.h"

#include "gtest/gtest.h"

namespace ndyn::math {

TEST(MultivectorTest, CanCompile) {
  static constexpr ScalarMultivector<float> a{};
  EXPECT_TRUE(true);
}

TEST(MultivectorTest, CanInitializeToScalar) {
  static constexpr float SCALAR{1};
  static constexpr ScalarMultivector<float> a{SCALAR};
  EXPECT_EQ(a.scalar(), SCALAR);
}

TEST(MultivectorTest, CanAddScalars) {
  static constexpr float SCALAR_1{1};
  static constexpr ScalarMultivector<float> m1{SCALAR_1};
  static constexpr float SCALAR_2{2};
  static constexpr ScalarMultivector<float> m2{SCALAR_2};
  EXPECT_EQ(m1.add(m2).scalar(), SCALAR_1 + SCALAR_2);
}

TEST(MultivectorTest, CanAddScalarsWithOperator) {
  static constexpr float SCALAR_1{1};
  static constexpr ScalarMultivector<float> m1{SCALAR_1};
  static constexpr float SCALAR_2{2};
  static constexpr ScalarMultivector<float> m2{SCALAR_2};

  static constexpr ScalarMultivector<float> result{m1 + m2};

  EXPECT_EQ(result.scalar(), SCALAR_1 + SCALAR_2);
}

TEST(MultivectorTest, CanMultiplyScalarsWithOperator) {
  static constexpr float SCALAR_1{1};
  static constexpr ScalarMultivector<float> m1{SCALAR_1};
  static constexpr float SCALAR_2{2};
  static constexpr ScalarMultivector<float> m2{SCALAR_2};

  static constexpr ScalarMultivector<float> result{m1 * m2};

  EXPECT_EQ(result.scalar(), SCALAR_1 * SCALAR_2);
}

TEST(MultivectorTest, CanMultiplySimpleComplexNumbers) {
  static constexpr auto i{ComplexMultivector<float>::e<0>()};
  EXPECT_EQ(-1.f, i.multiply(i).scalar());
}

TEST(MultivectorTest, CanMultiplySimpleComplexNumbersWithOperator) {
  static constexpr auto i{ComplexMultivector<float>::e<0>()};
  EXPECT_EQ(-1.f, (i * i).scalar());
}

TEST(MultivectorTest, CanMultiplyComplexNumbers) {
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

TEST(MultivectorTest, CanMultiplyComplexNumbersWithOperator) {
  static constexpr auto u{ComplexMultivector<float>::e<0>().add(1.f)};  // 1 + i
  static constexpr auto v{u * u};                                       // (1 + i)^2 = 2i
  static constexpr auto w{v * v};                                       // (2i)^2 = -4

  static_assert(v.scalar() == 0.f,
                "Verifying that we can do multivector operations in constexpr contexts");
  static_assert(w.scalar() == -4.f,
                "Verifying that we can do multivector operations in constexpr contexts");

  ASSERT_EQ(1.f, u.scalar());
  EXPECT_EQ(0.f, v.scalar());
  EXPECT_EQ(-4.f, w.scalar());
  EXPECT_EQ(16.f, (w * w).scalar());
}

TEST(MultivectorTest, EqualComplexNumbersEvaluateAsEqual) {
  static constexpr auto i{ComplexMultivector<float>::e<0>()};  // i
  static constexpr auto u{i + 1.f};                            // 1 + i
  static constexpr ComplexMultivector<float> v{1.f};           // 1
  static constexpr ComplexMultivector<float> w{2.f * i};       // 2i

  EXPECT_EQ(u, v + i);
  EXPECT_EQ(w, u * u);
}

TEST(MultivectorTest, CanUseUnaryMinusOnComplexNumbers) {
  static constexpr auto i{ComplexMultivector<float>::e<0>()};  // i
  static constexpr auto u{i + 1.f};                            // 1 + i

  EXPECT_EQ(i * -1.f, -i);
  EXPECT_EQ(-1.f - i, -u);
}

TEST(MultivectorTest, ValidateOperatorOverloadsOnComplexNumbers) {
  static constexpr auto i{ComplexMultivector<float>::e<0>()};

  EXPECT_EQ(-1.f, i * i);
  EXPECT_EQ(i * i, -1.f);

  EXPECT_EQ(0.f, i * i + 1.f);
  EXPECT_EQ(i * i + 1.f, 0.f);

  EXPECT_EQ(i - 1.f, i * (i + 1.f));
  EXPECT_EQ(i * (i - 1.f), -1.f - i);

  EXPECT_EQ(-i * (i - 1.f), 1.f + i);
}

TEST(MultivectorTest, CantAccessNonexistentBasesInComplexNumbers) {
  const auto i{ComplexMultivector<float>::e<0>()};
  // Uncomment to generate compile error.
  // const auto does_not_exist{ComplexMultivector<float>::e<1>()};

  EXPECT_EQ(0, i.scalar());
}

TEST(MultivectorTest, ValidGradeOperatorOnTrivialComplexNumbers) {
  const auto i{ComplexMultivector<float>::e<0>()};
  const auto zero{ComplexMultivector<float>{0.f}};

  EXPECT_EQ(i, i.grade(1));
  EXPECT_EQ(zero, i.grade(0));
  EXPECT_EQ(zero, zero.grade(0));
  EXPECT_EQ(zero, zero.grade(1));
}

TEST(MultivectorTest, ValidGradeOperatorOnComplexNumbers) {
  const auto i{ComplexMultivector<float>::e<0>()};
  const auto one{ComplexMultivector<float>{1.f}};

  EXPECT_EQ(10.f * i, (10.f * i - 3).grade(1));
  EXPECT_EQ(-3.f * one, (10.f * i - 3).grade(0));
}

TEST(MultivectorTest, ValidGradeOperatorOnTrivialSpacetimeNumbers) {
  const auto t{SpacetimeMultivector<float>::e<0>()};
  const auto x{SpacetimeMultivector<float>::e<1>()};
  const auto y{SpacetimeMultivector<float>::e<2>()};
  const auto z{SpacetimeMultivector<float>::e<3>()};
  const auto zero{SpacetimeMultivector<float>{0.f}};

  for (const auto& m : {t, x, y, z}) {
    EXPECT_EQ(m, m.grade(1));
    EXPECT_EQ(zero, m.grade(0));
  }

  EXPECT_EQ(zero, zero.grade(0));
  EXPECT_EQ(zero, zero.grade(1));
}

TEST(MultivectorTest, ValidGradeOperatorOnSimpleSpacetimeNumbers) {
  const auto t{SpacetimeMultivector<float>::e<0>()};
  const auto x{SpacetimeMultivector<float>::e<1>()};
  const auto y{SpacetimeMultivector<float>::e<2>()};
  const auto z{SpacetimeMultivector<float>::e<3>()};
  const auto zero{SpacetimeMultivector<float>{0.f}};

  for (const auto& a : {t, x, y, z}) {
    for (const auto& b : {t, x, y, z}) {
      for (const auto& c : {t, x, y, z}) {
        for (const auto& d : {t, x, y, z}) {
          const auto sum{a + b + c + d};
          for (size_t i = 0; i < SpacetimeMultivector<float>::grade_count(); ++i) {
            if (i == 1) {
              EXPECT_EQ(sum, sum.grade(i));
            } else {
              EXPECT_EQ(zero, sum.grade(i));
            }
          }
        }
      }
    }
  }
}

TEST(MultivectorTest, ValidGradeOperatorOnSpacetimeNumbers) {
  const auto t{SpacetimeMultivector<float>::e<0>()};
  const auto x{SpacetimeMultivector<float>::e<1>()};
  const auto y{SpacetimeMultivector<float>::e<2>()};
  const auto z{SpacetimeMultivector<float>::e<3>()};
  const auto zero{SpacetimeMultivector<float>{0.f}};

  const auto all_bases{
      1.f +                 //
      2.f * t +             //
      3.f * x +             //
      4.f * y +             //
      5.f * z +             //
      6.f * t * x +         //
      7.f * t * y +         //
      8.f * t * z +         //
      9.f * x * y +         //
      10.f * x * z +        //
      11.f * y * z +        //
      12.f * t * x * y +    //
      13.f * t * x * z +    //
      14.f * t * y * z +    //
      15.f * x * y * z +    //
      16.f * t * x * y * z  //
  };

  EXPECT_EQ(SpacetimeMultivector<float>{1.f}, all_bases.grade(0));

  EXPECT_EQ(2.f * t + 3.f * x + 4.f * y + 5.f * z, all_bases.grade(1));

  EXPECT_EQ(6.f * t * x + 7.f * t * y + 8.f * t * z + 9.f * x * y + 10.f * x * z + 11.f * y * z,
            all_bases.grade(2));

  EXPECT_EQ(12.f * t * x * y + 13.f * t * x * z + 14.f * t * y * z + 15.f * x * y * z,
            all_bases.grade(3));

  EXPECT_EQ(16.f * t * x * y * z, all_bases.grade(4));
}

}  // namespace ndyn::math
