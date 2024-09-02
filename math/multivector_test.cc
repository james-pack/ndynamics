#include "math/multivector.h"

#include "gtest/gtest.h"

namespace ndyn::math {

TEST(MultivectorTest, CanCompile) {
  static constexpr ScalarMultivector<float> a{};
  EXPECT_TRUE(true);
}

TEST(MultivectorTest, CanInitializeFromScalar) {
  static constexpr float SCALAR{1};
  static constexpr ScalarMultivector<float> a{SCALAR};
  EXPECT_EQ(a.scalar(), SCALAR);
}

TEST(MultivectorTest, CanInitializeFromInitializerList) {
  static constexpr float REAL{1};
  static constexpr float IMAG{3};
  static constexpr ComplexMultivector<float> a{REAL, IMAG};
  EXPECT_EQ(a.scalar(), REAL);
  EXPECT_EQ(a.component(1), IMAG);
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

TEST(MultivectorTest, CanDoLeftContractionOnComplexNumbers) {
  static constexpr auto i{ComplexMultivector<float>::e<0>()};
  static constexpr auto a{ComplexMultivector<float>{1.f}};  // 1
  static constexpr auto u{i.add(1.f)};                      // 1 + i
  static constexpr auto v{u.multiply(u)};                   // (1 + i)^2 = 2i
  static constexpr auto w{v.multiply(v)};                   // (2i)^2 = -4

  EXPECT_EQ(-1.f, i.left_contraction(i));
  EXPECT_EQ(1.f, a.left_contraction(a));
  EXPECT_EQ(i, u.left_contraction(u));
  EXPECT_EQ(-2.f, v.left_contraction(i));
  EXPECT_EQ(-2.f, v.left_contraction(u));
  EXPECT_EQ(0.f, v.left_contraction(w));
  EXPECT_EQ(w * u, w.left_contraction(u));
  EXPECT_EQ(-4.f, u.left_contraction(w));
}

TEST(MultivectorTest, CanDoLeftContractionOnDualNumbers) {
  static constexpr auto e{DualMultivector<float>::e<0>()};
  static constexpr auto a{DualMultivector<float>{1.f}};  // 1
  static constexpr auto u{e.add(1.f)};                   // 1 + e
  static constexpr auto v{u.multiply(u)};                // (1 + e)^2 = 1 + 2e
  static constexpr auto w{4.f + 2.f * e};                // 4 + 2e

  EXPECT_EQ(0.f, e.left_contraction(e));
  EXPECT_EQ(1.f, a.left_contraction(a));
  EXPECT_EQ(u, u.left_contraction(u));
  EXPECT_EQ(v, v.left_contraction(v));

  // Interesting result for dual numbers -- for any multivector m,
  //   m << m = m.scalar() * m
  EXPECT_EQ(e.scalar() * e, e.left_contraction(e));
  EXPECT_EQ(a.scalar() * a, a.left_contraction(a));
  EXPECT_EQ(u.scalar() * u, u.left_contraction(u));
  EXPECT_EQ(v.scalar() * v, v.left_contraction(v));
  EXPECT_EQ(w.scalar() * w, w.left_contraction(w));
}

TEST(MultivectorTest, CanDoLeftContractionOnSimpleVga) {
  static constexpr auto x{VgaMultivector<float>::e<0>()};
  static constexpr auto a{VgaMultivector<float>{1.f}};  // 1
  static constexpr auto u{x.add(1.f)};                  // 1 + x
  static constexpr auto v{u.multiply(u)};               // (1 + x)^2 = 2 + 2x
  static constexpr auto w{v.multiply(v)};               // (2 + 2x)^2 = 8 + 8x

  EXPECT_EQ(1.f, x.left_contraction(x));
  EXPECT_EQ(1.f, a.left_contraction(a));
  EXPECT_EQ(1.f, x.left_contraction(u));
  EXPECT_EQ(u, u.left_contraction(x));
  EXPECT_EQ(u + 1.f, u.left_contraction(u));
  EXPECT_EQ(v, v.left_contraction(x));
  EXPECT_EQ(2.f, x.left_contraction(v));
  EXPECT_EQ(4.f + 2.f * x, v.left_contraction(u));
  EXPECT_EQ(32.f + 16.f * x, v.left_contraction(w));
  EXPECT_EQ(v.left_contraction(w), w.left_contraction(v));
  EXPECT_EQ(16.f + 8.f * x, w.left_contraction(u));
  EXPECT_EQ(w.left_contraction(u), u.left_contraction(w));
}

TEST(MultivectorTest, CanDoLeftContractionOnVga) {
  static constexpr auto x{VgaMultivector<float>::e<0>()};
  static constexpr auto y{VgaMultivector<float>::e<1>()};
  static constexpr auto z{VgaMultivector<float>::e<2>()};
  static constexpr auto a{VgaMultivector<float>{1.f}};

  static constexpr auto u{1.f + x};
  static constexpr auto v{1.f + 2.f * y};
  static constexpr auto w{2.f + 3.f * z};

  EXPECT_EQ(v, u.left_contraction(v));
  EXPECT_EQ(w, v.left_contraction(w));
  EXPECT_EQ(2.f * u, w.left_contraction(u));

  // Document the values of these products, since they will be used in more complicated tests below.
  ASSERT_EQ(1.f + x + 2.f * y + 2.f * x * y, u * v);
  ASSERT_EQ(2.f + 4.f * y + 3.f * z + 6.f * y * z, v * w);
  ASSERT_EQ(2.f + 2.f * x + 3.f * z - 3.f * x * z, w * u);

  // In these three cases, the lhs of the left contraction has a constant plus a term that is
  // orthogonal to all the components of the rhs. So, all of the results will be that constant times
  // the rhs.
  EXPECT_EQ(v * w, u.left_contraction(v * w)) << "v * w: " << v * w;
  EXPECT_EQ(w * u, v.left_contraction(w * u));
  EXPECT_EQ(2.f * u * v, w.left_contraction(u * v));

  EXPECT_EQ(4.f + 8.f * y + 6.f * z + 12.f * y * z + 9.f - 18.f * y, w.left_contraction(v * w));

  EXPECT_EQ(2.f * (2.f + 2.f * x + 3.f * z - 3.f * x * z) + 9.f + 9.f * x,
            w.left_contraction(w * u));

  EXPECT_EQ(u * v + +4.f - 4.f * x, v.left_contraction(u * v));
}

TEST(MultivectorTest, CanDoLeftContractionOnSimpleSpacetime) {
  static constexpr auto x{SpacetimeMultivector<float>::e<1>()};
  static constexpr auto a{SpacetimeMultivector<float>{1.f}};  // 1
  static constexpr auto u{1.f + x};                           // 1 + x
  static constexpr auto v{u * u};                             // (1 + x)^2 = 2x
  static constexpr auto w{v * v};                             // (2x)^2 = -4

  EXPECT_EQ(-1.f, x.left_contraction(x));
  EXPECT_EQ(1.f, a.left_contraction(a));
  EXPECT_EQ(-1.f, x.left_contraction(u));
  EXPECT_EQ(x - 1.f, u.left_contraction(x));
  EXPECT_EQ(u - 1.f, u.left_contraction(u));
  EXPECT_EQ(-2.f, v.left_contraction(x));
  EXPECT_EQ(-2.f, x.left_contraction(v));
  EXPECT_EQ(-2.f, v.left_contraction(u));
  EXPECT_EQ(0.f, v.left_contraction(w));
  EXPECT_EQ(-8.f * x, w.left_contraction(v));
  EXPECT_EQ(-4.f - 4.f * x, w.left_contraction(u));
  EXPECT_EQ(-4.f, u.left_contraction(w));
}

TEST(MultivectorTest, CanDoLeftContractionOnSpacetime) {
  static constexpr auto t{SpacetimeMultivector<float>::e<0>()};
  static constexpr auto x{SpacetimeMultivector<float>::e<1>()};
  static constexpr auto y{SpacetimeMultivector<float>::e<2>()};
  static constexpr auto z{SpacetimeMultivector<float>::e<3>()};
  static constexpr auto a{SpacetimeMultivector<float>{1.f}};

  static constexpr auto r{1.f + t};
  static constexpr auto u{1.f + x};
  static constexpr auto v{1.f + 2.f * y};
  static constexpr auto w{2.f + 3.f * z};

  EXPECT_EQ(u, r.left_contraction(u));
  EXPECT_EQ(0.f, t.left_contraction(u));
  EXPECT_EQ(v, u.left_contraction(v));
  EXPECT_EQ(w, v.left_contraction(w));
  EXPECT_EQ(2.f * u, w.left_contraction(u));

  // Document the values of these products, since they will be used in more complicated tests below.
  ASSERT_EQ(1.f + x + 2.f * y + 2.f * x * y, u * v);
  ASSERT_EQ(2.f + 4.f * y + 3.f * z + 6.f * y * z, v * w);
  ASSERT_EQ(2.f + 2.f * x + 3.f * z - 3.f * x * z, w * u);

  // In these three cases, the lhs of the left contraction has a constant plus a term that is
  // orthogonal to all the components of the rhs. So, all of the results will be that constant times
  // the rhs.
  EXPECT_EQ(v * w, u.left_contraction(v * w)) << "v * w: " << v * w;
  EXPECT_EQ(w * u, v.left_contraction(w * u));
  EXPECT_EQ(2.f * u * v, w.left_contraction(u * v));

  EXPECT_EQ(4.f + 8.f * y + 6.f * z + 12.f * y * z + -9.f + 18.f * y, w.left_contraction(v * w));

  EXPECT_EQ(4.f + 4.f * x + 6.f * z - 6.f * x * z + -9.f - 9.f * x, w.left_contraction(w * u));

  EXPECT_EQ(u * v - 4.f + 4.f * x, v.left_contraction(u * v));
}

TEST(MultivectorTest, CanDoBidirectionalInnerProductOnComplexNumbers) {
  static constexpr auto i{ComplexMultivector<float>::e<0>()};
  static constexpr auto a{ComplexMultivector<float>{1.f}};  // 1
  static constexpr auto u{1.f + i};                         // 1 + i
  static constexpr auto v{u * u};                           // (1 + i)^2 = 2i
  static constexpr auto w{v * v};                           // (2i)^2 = -4

  EXPECT_EQ(-1.f, i.bidirectional_inner(i));
  EXPECT_EQ(1.f, a.bidirectional_inner(a));
  EXPECT_EQ(2.f * i, u.bidirectional_inner(u));
  EXPECT_EQ(-2.f, v.bidirectional_inner(i));
  EXPECT_EQ(-2.f + 2.f * i, v.bidirectional_inner(u));
  EXPECT_EQ(-8.f * i, v.bidirectional_inner(w));
  EXPECT_EQ(w * u, w.bidirectional_inner(u));
  EXPECT_EQ(w * u, u.bidirectional_inner(w));

  for (const auto& p : {i, a, u, v, w}) {
    for (const auto& q : {i, a, u, v, w}) {
      EXPECT_EQ(p.bidirectional_inner(q), q.bidirectional_inner(p));
    }
  }
}

TEST(MultivectorTest, CanDoBidirectionalInnerProductOnDualNumbers) {
  static constexpr auto e{DualMultivector<float>::e<0>()};
  static constexpr auto a{DualMultivector<float>{1.f}};  // 1
  static constexpr auto u{e.add(1.f)};                   // 1 + e
  static constexpr auto v{u.multiply(u)};                // (1 + e)^2 = 1 + 2e
  static constexpr auto w{4.f + 2.f * e};                // 4 + 2e

  EXPECT_EQ(0.f, e.bidirectional_inner(e));
  EXPECT_EQ(1.f, a.bidirectional_inner(a));
  EXPECT_EQ(1.f + 2.f * e, u.bidirectional_inner(u));
  EXPECT_EQ(1.f + 4.f * e, v.bidirectional_inner(v));

  EXPECT_EQ(4.f + 10.f * e, w.bidirectional_inner(v));

  for (const auto& p : {e, a, u, v, w}) {
    for (const auto& q : {e, a, u, v, w}) {
      EXPECT_EQ(p.bidirectional_inner(q), q.bidirectional_inner(p));
    }
  }
}

TEST(MultivectorTest, CanDoBidirectionalInnerProductOnSimpleVga) {
  static constexpr auto x{VgaMultivector<float>::e<0>()};
  static constexpr auto a{VgaMultivector<float>{1.f}};  // 1
  static constexpr auto u{x.add(1.f)};                  // 1 + x
  static constexpr auto v{u.multiply(u)};               // (1 + x)^2 = 2 + 2x
  static constexpr auto w{v.multiply(v)};               // (2 + 2x)^2 = 8 + 8x

  EXPECT_EQ(1.f, x.bidirectional_inner(x));
  EXPECT_EQ(1.f, a.bidirectional_inner(a));
  EXPECT_EQ(1.f + x, x.bidirectional_inner(u));
  EXPECT_EQ(u, u.bidirectional_inner(x));
  EXPECT_EQ(2.f + 2.f * x, u.bidirectional_inner(u));
  EXPECT_EQ(v, v.bidirectional_inner(x));
  EXPECT_EQ(v, x.bidirectional_inner(v));
  EXPECT_EQ(4.f + 4.f * x, v.bidirectional_inner(u));
  EXPECT_EQ(32.f + 32.f * x, v.bidirectional_inner(w));
  EXPECT_EQ(16.f + 16.f * x, w.bidirectional_inner(u));

  for (const auto& p : {x, a, u, v, w}) {
    for (const auto& q : {x, a, u, v, w}) {
      EXPECT_EQ(p.bidirectional_inner(q), q.bidirectional_inner(p));
    }
  }
}

TEST(MultivectorTest, CanDoBidirectionalInnerProductOnVga) {
  static constexpr auto x{VgaMultivector<float>::e<0>()};
  static constexpr auto y{VgaMultivector<float>::e<1>()};
  static constexpr auto z{VgaMultivector<float>::e<2>()};
  static constexpr auto a{VgaMultivector<float>{1.f}};

  static constexpr auto u{1.f + x};
  static constexpr auto v{1.f + 2.f * y};
  static constexpr auto w{2.f + 3.f * z};

  EXPECT_EQ(1.f + x + 2.f * y, u.bidirectional_inner(v));
  EXPECT_EQ(2.f + 3.f * z + 4.f * y, v.bidirectional_inner(w));
  EXPECT_EQ(2.f + 3.f * z + 2.f * x, w.bidirectional_inner(u));

  // Document the values of these products, since they will be used in more complicated tests below.
  ASSERT_EQ(1.f + x + 2.f * y + 2.f * x * y, u * v);
  ASSERT_EQ(2.f + 4.f * y + 3.f * z + 6.f * y * z, v * w);
  ASSERT_EQ(2.f + 2.f * x + 3.f * z - 3.f * x * z, w * u);

  EXPECT_EQ(v * w + 2.f * x, u.bidirectional_inner(v * w)) << "v * w: " << v * w;
  EXPECT_EQ(w * u + 4.f * y, v.bidirectional_inner(w * u));
  EXPECT_EQ(2.f * u * v + 3.f * z, w.bidirectional_inner(u * v));

  EXPECT_EQ(4.f + 8.f * y + 6.f * z + 12.f * y * z + 6.f * z + 9.f - 18.f * y,
            w.bidirectional_inner(v * w));

  EXPECT_EQ(4.f + 4.f * x + 6.f * z - 6.f * x * z + 6.f * z + 9.f + 9.f * x,
            w.bidirectional_inner(w * u));

  EXPECT_EQ(1.f + x + 2.f * y + 2.f * x * y + 2.f * y + 4.f - 4.f * x,
            v.bidirectional_inner(u * v));

  for (const auto& p : {x, y, z, a, u, v, w}) {
    for (const auto& q : {x, y, z, a, u, v, w}) {
      EXPECT_EQ(p.bidirectional_inner(q), q.bidirectional_inner(p));
    }
  }
}

TEST(MultivectorTest, CanDoBidirectionalInnerProductOnSimpleSpacetime) {
  static constexpr auto x{SpacetimeMultivector<float>::e<1>()};
  static constexpr auto a{SpacetimeMultivector<float>{1.f}};  // 1
  static constexpr auto u{1.f + x};                           // 1 + x
  static constexpr auto v{u * u};                             // (1 + x)^2 = 2x
  static constexpr auto w{v * v};                             // (2x)^2 = -4

  EXPECT_EQ(-1.f, x.bidirectional_inner(x));
  EXPECT_EQ(1.f, a.bidirectional_inner(a));

  EXPECT_EQ(x - 1.f, x.bidirectional_inner(u));
  EXPECT_EQ(2.f * x, u.bidirectional_inner(u));
  EXPECT_EQ(-2.f, v.bidirectional_inner(x));
  EXPECT_EQ(-2.f, x.bidirectional_inner(v));
  EXPECT_EQ(-2.f + 2.f * x, v.bidirectional_inner(u));

  EXPECT_EQ(-8.f * x, v.bidirectional_inner(w));

  EXPECT_EQ(-4.f - 4.f * x, w.bidirectional_inner(u));

  for (const auto& p : {x, a, u, v, w}) {
    for (const auto& q : {x, a, u, v, w}) {
      EXPECT_EQ(p.bidirectional_inner(q), q.bidirectional_inner(p));
    }
  }
}

TEST(MultivectorTest, CanDoBidirectionalInnerProductOnSpacetime) {
  static constexpr auto t{SpacetimeMultivector<float>::e<0>()};
  static constexpr auto x{SpacetimeMultivector<float>::e<1>()};
  static constexpr auto y{SpacetimeMultivector<float>::e<2>()};
  static constexpr auto z{SpacetimeMultivector<float>::e<3>()};
  static constexpr auto a{SpacetimeMultivector<float>{1.f}};

  static constexpr auto r{1.f + t};
  static constexpr auto u{1.f + x};
  static constexpr auto v{1.f + 2.f * y};
  static constexpr auto w{2.f + 3.f * z};

  EXPECT_EQ(1.f + x + t, r.bidirectional_inner(u));
  EXPECT_EQ(t, t.bidirectional_inner(u));
  EXPECT_EQ(1.f + x + 2.f * y, u.bidirectional_inner(v));
  EXPECT_EQ(2.f + 4.f * y + 3.f * z, v.bidirectional_inner(w));
  EXPECT_EQ(2.f + 2.f * x + 3.f * z, w.bidirectional_inner(u));

  // Document the values of these products, since they will be used in more complicated tests below.
  ASSERT_EQ(1.f + x + 2.f * y + 2.f * x * y, u * v);
  ASSERT_EQ(2.f + 4.f * y + 3.f * z + 6.f * y * z, v * w);
  ASSERT_EQ(2.f + 2.f * x + 3.f * z - 3.f * x * z, w * u);

  EXPECT_EQ(2.f + 4.f * y + 3.f * z + 6.f * y * z + 2.f * x, u.bidirectional_inner(v * w));

  EXPECT_EQ(2.f + 2.f * x + 3.f * z - 3.f * x * z + 4.f * y, v.bidirectional_inner(w * u));

  EXPECT_EQ(2.f + 2.f * x + 4.f * y + 4.f * x * y + 3.f * z, w.bidirectional_inner(u * v));

  EXPECT_EQ(4.f + 8.f * y + 6.f * z + 12.f * y * z + 6.f * z - 9.f + 18.f * y,
            w.bidirectional_inner(v * w));

  EXPECT_EQ(4.f + 4.f * x + 6.f * z - 6.f * x * z + 6.f * z - 9.f - 9.f * x,
            w.bidirectional_inner(w * u));

  EXPECT_EQ(1.f + x + 2.f * y + 2.f * x * y + 2.f * y - 4.f + 4.f * x,
            v.bidirectional_inner(u * v));

  for (const auto& p : {t, x, y, z, a, u, v, w}) {
    for (const auto& q : {t, x, y, z, a, u, v, w}) {
      EXPECT_EQ(p.bidirectional_inner(q), q.bidirectional_inner(p));
    }
  }
}

TEST(MultivectorTest, InnerProductStyleAsLeftContraction) {
  static constexpr auto t{SpacetimeMultivector<float, InnerProduct::LEFT_CONTRACTION>::e<0>()};
  static constexpr auto x{SpacetimeMultivector<float, InnerProduct::LEFT_CONTRACTION>::e<1>()};
  static constexpr auto y{SpacetimeMultivector<float, InnerProduct::LEFT_CONTRACTION>::e<2>()};
  static constexpr auto z{SpacetimeMultivector<float, InnerProduct::LEFT_CONTRACTION>::e<3>()};
  static constexpr auto a{SpacetimeMultivector<float, InnerProduct::LEFT_CONTRACTION>{1.f}};

  static constexpr auto r{1.f + t};
  static constexpr auto u{1.f + x};
  static constexpr auto v{1.f + 2.f * y};
  static constexpr auto w{2.f + 3.f * z};

  EXPECT_EQ(r.left_contraction(u), r.inner(u));
  EXPECT_EQ(t.left_contraction(u), t.inner(u));
  EXPECT_EQ(u.left_contraction(v), u.inner(v));
  EXPECT_EQ(v.left_contraction(w), v.inner(w));
  EXPECT_EQ(w.left_contraction(u), w.inner(u));

  // Document the values of these products, since they will be used in more complicated tests below.
  ASSERT_EQ(1.f + x + 2.f * y + 2.f * x * y, u * v);
  ASSERT_EQ(2.f + 4.f * y + 3.f * z + 6.f * y * z, v * w);
  ASSERT_EQ(2.f + 2.f * x + 3.f * z - 3.f * x * z, w * u);

  // In these three cases, the lhs of the left contraction has a constant plus a term that is
  // orthogonal to all the components of the rhs. So, all of the results will be that constant times
  // the rhs.
  EXPECT_EQ(u.left_contraction(v * w), u.inner(v * w)) << "v * w: " << v * w;
  EXPECT_EQ(v.left_contraction(w * u), v.inner(w * u));
  EXPECT_EQ(w.left_contraction(u * v), w.inner(u * v));

  EXPECT_EQ(w.left_contraction(v * w), w.inner(v * w));

  EXPECT_EQ(w.left_contraction(w * u), w.inner(w * u));

  EXPECT_EQ(v.left_contraction(u * v), v.inner(u * v));
}

TEST(MultivectorTest, InnerProductStyleAsRightContraction) {
  static constexpr auto t{SpacetimeMultivector<float, InnerProduct::RIGHT_CONTRACTION>::e<0>()};
  static constexpr auto x{SpacetimeMultivector<float, InnerProduct::RIGHT_CONTRACTION>::e<1>()};
  static constexpr auto y{SpacetimeMultivector<float, InnerProduct::RIGHT_CONTRACTION>::e<2>()};
  static constexpr auto z{SpacetimeMultivector<float, InnerProduct::RIGHT_CONTRACTION>::e<3>()};
  static constexpr auto a{SpacetimeMultivector<float, InnerProduct::RIGHT_CONTRACTION>{1.f}};

  static constexpr auto r{1.f + t};
  static constexpr auto u{1.f + x};
  static constexpr auto v{1.f + 2.f * y};
  static constexpr auto w{2.f + 3.f * z};

  EXPECT_EQ(r.right_contraction(u), r.inner(u));
  EXPECT_EQ(t.right_contraction(u), t.inner(u));
  EXPECT_EQ(u.right_contraction(v), u.inner(v));
  EXPECT_EQ(v.right_contraction(w), v.inner(w));
  EXPECT_EQ(w.right_contraction(u), w.inner(u));

  // Document the values of these products, since they will be used in more complicated tests below.
  ASSERT_EQ(1.f + x + 2.f * y + 2.f * x * y, u * v);
  ASSERT_EQ(2.f + 4.f * y + 3.f * z + 6.f * y * z, v * w);
  ASSERT_EQ(2.f + 2.f * x + 3.f * z - 3.f * x * z, w * u);

  // In these three cases, the lhs of the right contraction has a constant plus a term that is
  // orthogonal to all the components of the rhs. So, all of the results will be that constant times
  // the rhs.
  EXPECT_EQ(u.right_contraction(v * w), u.inner(v * w)) << "v * w: " << v * w;
  EXPECT_EQ(v.right_contraction(w * u), v.inner(w * u));
  EXPECT_EQ(w.right_contraction(u * v), w.inner(u * v));

  EXPECT_EQ(w.right_contraction(v * w), w.inner(v * w));

  EXPECT_EQ(w.right_contraction(w * u), w.inner(w * u));

  EXPECT_EQ(v.right_contraction(u * v), v.inner(u * v));
}

TEST(MultivectorTest, InnerProductStyleAsBidirectional) {
  static constexpr auto t{SpacetimeMultivector<float, InnerProduct::BIDIRECTIONAL>::e<0>()};
  static constexpr auto x{SpacetimeMultivector<float, InnerProduct::BIDIRECTIONAL>::e<1>()};
  static constexpr auto y{SpacetimeMultivector<float, InnerProduct::BIDIRECTIONAL>::e<2>()};
  static constexpr auto z{SpacetimeMultivector<float, InnerProduct::BIDIRECTIONAL>::e<3>()};
  static constexpr auto a{SpacetimeMultivector<float, InnerProduct::BIDIRECTIONAL>{1.f}};

  static constexpr auto r{1.f + t};
  static constexpr auto u{1.f + x};
  static constexpr auto v{1.f + 2.f * y};
  static constexpr auto w{2.f + 3.f * z};

  EXPECT_EQ(r.bidirectional_inner(u), r.inner(u));
  EXPECT_EQ(t.bidirectional_inner(u), t.inner(u));
  EXPECT_EQ(u.bidirectional_inner(v), u.inner(v));
  EXPECT_EQ(v.bidirectional_inner(w), v.inner(w));
  EXPECT_EQ(w.bidirectional_inner(u), w.inner(u));

  // Document the values of these products, since they will be used in more complicated tests below.
  ASSERT_EQ(1.f + x + 2.f * y + 2.f * x * y, u * v);
  ASSERT_EQ(2.f + 4.f * y + 3.f * z + 6.f * y * z, v * w);
  ASSERT_EQ(2.f + 2.f * x + 3.f * z - 3.f * x * z, w * u);

  // In these three cases, the lhs of the right contraction has a constant plus a term that is
  // orthogonal to all the components of the rhs. So, all of the results will be that constant times
  // the rhs.
  EXPECT_EQ(u.bidirectional_inner(v * w), u.inner(v * w)) << "v * w: " << v * w;
  EXPECT_EQ(v.bidirectional_inner(w * u), v.inner(w * u));
  EXPECT_EQ(w.bidirectional_inner(u * v), w.inner(u * v));

  EXPECT_EQ(w.bidirectional_inner(v * w), w.inner(v * w));

  EXPECT_EQ(w.bidirectional_inner(w * u), w.inner(w * u));

  EXPECT_EQ(v.bidirectional_inner(u * v), v.inner(u * v));
}

TEST(MultivectorTest, InnerProductStyleAsNoImplicitDefinition) {
  static constexpr auto t{
      SpacetimeMultivector<float, InnerProduct::NO_IMPLICIT_DEFINITION>::e<0>()};
  static constexpr auto x{
      SpacetimeMultivector<float, InnerProduct::NO_IMPLICIT_DEFINITION>::e<1>()};

  static constexpr auto r{1.f + t};
  static constexpr auto u{1.f + x};

  // Uncomment the line below to verify that we have a compile failure when there is no implicit
  // inner product definition provided by the type.
  // r.inner(u);
}

TEST(MultivectorTest, CanDoOuterProductOnComplexNumbers) {
  static constexpr auto i{ComplexMultivector<float>::e<0>()};
  static constexpr auto a{ComplexMultivector<float>{1.f}};  // 1
  static constexpr auto u{1.f + i};                         // 1 + i
  static constexpr auto v{u * u};                           // (1 + i)^2 = 2i
  static constexpr auto w{v * v};                           // (2i)^2 = -4
  static constexpr auto x{-2.f + 2.f * i};                  // -2 + 2i

  EXPECT_EQ(1.f, a.outer(a));

  EXPECT_EQ(0.f, i.outer(i));

  EXPECT_EQ(1.f + 2.f * i, u.outer(u));
  EXPECT_EQ(0.f, v.outer(i));
  EXPECT_EQ(v, v.outer(u));

  EXPECT_EQ(w * v, v.outer(w));
  EXPECT_EQ(w * u, w.outer(u));
  EXPECT_EQ(w * u, u.outer(w));

  EXPECT_EQ(-2.f * u + 2.f * i, x.outer(u));
  EXPECT_EQ(-2.f, x.outer(u));
  EXPECT_EQ(x - 2.f * i, u.outer(x));
  EXPECT_EQ(-2.f, u.outer(x));
}

TEST(MultivectorTest, CanDoOuterProductOnSplitComplexNumbers) {
  static constexpr auto i{SplitComplexMultivector<float>::e<0>()};
  static constexpr auto a{SplitComplexMultivector<float>{1.f}};  // 1
  static constexpr auto u{1.f + i};                              // 1 + i
  static constexpr auto v{u * u};                                // (1 + i)^2 = 2 + 2i
  static constexpr auto w{v * v};                                // 8 + 8i
  static constexpr auto x{-2.f + 2.f * i};                       // -2 + 2i

  EXPECT_EQ(1.f, a.outer(a));

  EXPECT_EQ(0.f, i.outer(i));

  EXPECT_EQ(1.f + 2.f * i, u.outer(u));
  EXPECT_EQ(2.f * i, v.outer(i));
  EXPECT_EQ(2.f + 4.f * i, v.outer(u));

  ASSERT_EQ(8.f + 8.f * i, w);
  EXPECT_EQ(16.f + 32.f * i, v.outer(w));
  EXPECT_EQ(8.f + 16.f * i, w.outer(u));
  EXPECT_EQ(8.f + 16.f * i, u.outer(w));

  EXPECT_EQ(-2.f * u + 2.f * i, x.outer(u));
  EXPECT_EQ(-2.f, x.outer(u));
  EXPECT_EQ(x - 2.f * i, u.outer(x));
  EXPECT_EQ(-2.f, u.outer(x));
}

TEST(MultivectorTest, CanDoOuterProductOnDualNumbers) {
  static constexpr auto i{DualMultivector<float>::e<0>()};
  static constexpr auto a{DualMultivector<float>{1.f}};  // 1
  static constexpr auto u{1.f + i};                      // 1 + i
  static constexpr auto v{u * u};                        // (1 + i)^2 = 1 + 2i
  static constexpr auto w{v * v};                        // 1 + 4i
  static constexpr auto x{-2.f + 2.f * i};               // -2 + 2i

  EXPECT_EQ(1.f, a.outer(a));

  EXPECT_EQ(0.f, i.outer(i));

  EXPECT_EQ(1.f + 2.f * i, u.outer(u));
  EXPECT_EQ(i, v.outer(i));
  EXPECT_EQ(1.f + 3.f * i, v.outer(u));

  EXPECT_EQ(w * v, v.outer(w));
  EXPECT_EQ(w * u, w.outer(u));
  EXPECT_EQ(w * u, u.outer(w));

  EXPECT_EQ(-2.f * u + 2.f * i, x.outer(u));
  EXPECT_EQ(-2.f, x.outer(u));
  EXPECT_EQ(x - 2.f * i, u.outer(x));
  EXPECT_EQ(-2.f, u.outer(x));
}

TEST(MultivectorTest, CanDoOuterProductOnVga) {
  static constexpr auto i{VgaMultivector<float>::e<0>()};
  static constexpr auto j{VgaMultivector<float>::e<1>()};
  static constexpr auto k{VgaMultivector<float>::e<2>()};
  static constexpr auto a{VgaMultivector<float>{1.f}};  // 1
  static constexpr auto u{1.f + i};                     // 1 + i
  static constexpr auto v{u * u};                       // (1 + i)^2 = 2 + 2i
  static constexpr auto w{v * v};                       // 8 + 8i
  static constexpr auto x{-2.f + 2.f * i};              // -2 + 2i

  ASSERT_EQ(2.f + 2.f * i, v);
  ASSERT_EQ(8.f + 8.f * i, w);

  EXPECT_EQ(1.f, a.outer(a));

  EXPECT_EQ(0.f, i.outer(i));
  EXPECT_EQ(0.f, j.outer(j));
  EXPECT_EQ(0.f, k.outer(k));

  EXPECT_EQ(i * j, i.outer(j));
  EXPECT_EQ(j * k, j.outer(k));
  EXPECT_EQ(-i * k, k.outer(i));

  EXPECT_EQ(1.f + 2.f * i, u.outer(u));
  EXPECT_EQ(2.f * i, v.outer(i));
  EXPECT_EQ(2.f + 4.f * i, v.outer(u));

  EXPECT_EQ(16.f + 32.f * i, v.outer(w));
  EXPECT_EQ(8.f + 16.f * i, w.outer(u));
  EXPECT_EQ(8.f + 16.f * i, u.outer(w));

  EXPECT_EQ(-2.f * u + 2.f * i, x.outer(u));
  EXPECT_EQ(-2.f, x.outer(u));
  EXPECT_EQ(x - 2.f * i, u.outer(x));
  EXPECT_EQ(-2.f, u.outer(x));
}

TEST(MultivectorTest, CanDoOuterProductInSpacetimeAlgebra) {
  static constexpr auto t{SpacetimeMultivector<float>::e<0>()};
  static constexpr auto x{SpacetimeMultivector<float>::e<1>()};
  static constexpr auto y{SpacetimeMultivector<float>::e<2>()};
  static constexpr auto z{SpacetimeMultivector<float>::e<3>()};
  static constexpr auto a{SpacetimeMultivector<float>{1.f}};  // 1
  static constexpr auto u{1.f + t};                           // 1 + t
  static constexpr auto v{u * u};                             // (1 + t)^2 = 2 + 2t
  static constexpr auto w{v * v};                             // 8 + 8t

  ASSERT_EQ(2.f + 2.f * t, v);
  ASSERT_EQ(8.f + 8.f * t, w);

  EXPECT_EQ(1.f, a.outer(a));

  EXPECT_EQ(0.f, t.outer(t));
  EXPECT_EQ(0.f, x.outer(x));
  EXPECT_EQ(0.f, y.outer(y));
  EXPECT_EQ(0.f, z.outer(z));

  EXPECT_EQ(t * x, t.outer(x));
  EXPECT_EQ(t * y, t.outer(y));
  EXPECT_EQ(t * z, t.outer(z));
  EXPECT_EQ(x * y, x.outer(y));
  EXPECT_EQ(y * z, y.outer(z));
  EXPECT_EQ(-t * z, z.outer(t));

  EXPECT_EQ(1.f + 2.f * t, u.outer(u));
  EXPECT_EQ(2.f * t, v.outer(t));
  EXPECT_EQ(2.f * x + 2.f * t * x, v.outer(x));
  EXPECT_EQ(2.f + 4.f * t, v.outer(u));

  EXPECT_EQ(16.f + 32.f * t, v.outer(w));
  EXPECT_EQ(8.f + 16.f * t, w.outer(u));
  EXPECT_EQ(8.f + 16.f * t, u.outer(w));

  EXPECT_EQ(x - t * x, x.outer(u));
  EXPECT_EQ(t, t.outer(u));
  EXPECT_EQ(x + t * x, u.outer(x));
  EXPECT_EQ(2.f * t, v.outer(t));
}

TEST(MultivectorTEST, CanDoAllProductsInConstexprContexts) {
  static constexpr auto t{SpacetimeMultivector<float>::e<0>()};
  static constexpr auto u{1.f + t};  // 1 + t
  static constexpr auto v{u * u};    // (1 + t)^2 = 2 + 2t

  static constexpr auto left{v.left_contraction(t)};
  static constexpr auto right{v.right_contraction(t)};
  static constexpr auto bidi{v.bidirectional_inner(t)};
  static constexpr auto inner{v.inner(t)};
  static constexpr auto outer{v.outer(t)};

  static_assert(2.f * t + 2.f == left, "Inner products should be valid in constexpr contexts");
  static_assert(2.f == right, "Inner products should be valid in constexpr contexts");
  static_assert(2.f * t + 2.f == bidi, "Inner products should be valid in constexpr contexts");
  static_assert(2.f * t + 2.f == inner, "Inner products should be valid in constexpr contexts");
  static_assert(2.f * t == outer, "Outer product should be valid in constexpr contexts");

  EXPECT_EQ(2.f * t + 2.f, left);
  EXPECT_EQ(2.f, right);
  EXPECT_EQ(2.f * t + 2.f, bidi);
  EXPECT_EQ(2.f * t + 2.f, inner);
  EXPECT_EQ(2.f * t, outer);
}

TEST(MultivectorTest, CanDoScalarConjugate) {
  static constexpr ScalarMultivector<float> u{1.f};
  static constexpr ScalarMultivector<float> v{-1.f};
  static constexpr ScalarMultivector<float> w{-4.f};

  EXPECT_EQ(u, u.conj());
  EXPECT_EQ(v, v.conj());
  EXPECT_EQ(w, w.conj());

  EXPECT_EQ(1.f, u * u.conj());
  EXPECT_EQ(1.f, v * v.conj());
  EXPECT_EQ(16.f, w * w.conj());
}

TEST(MultivectorTest, CanDoComplexConjugate) {
  static constexpr auto i{ComplexMultivector<float>::e<0>()};
  static constexpr auto u{1.f + i};
  static constexpr auto v{1.f - i};
  static constexpr auto w{ComplexMultivector<float>{-1.f}};

  EXPECT_EQ(2.f, u * u.conj());
  EXPECT_EQ(2.f, v * v.conj());
  EXPECT_EQ(1.f, w * w.conj());
}

TEST(MultivectorTest, CanDoDualConjugate) {
  static constexpr auto e{DualMultivector<float>::e<0>()};
  static constexpr auto u{1.f + e};
  static constexpr auto v{1.f - e};
  static constexpr auto w{DualMultivector<float>{-1.f}};

  EXPECT_EQ(1.f, u * u.conj());
  EXPECT_EQ(1.f, v * v.conj());
  EXPECT_EQ(1.f, w * w.conj());
}

TEST(MultivectorTest, CanDoSimpleVgaConjugate) {
  static constexpr auto x{VgaMultivector<float>::e<0>()};
  static constexpr auto u{1.f + x};
  static constexpr auto v{1.f - x};
  static constexpr auto w{VgaMultivector<float>{-1.f}};

  EXPECT_EQ(0.f, u * u.conj());
  EXPECT_EQ(0.f, v * v.conj());
  EXPECT_EQ(1.f, w * w.conj());
}

TEST(MultivectorTest, CanDoVgaConjugate) {
  static constexpr auto x{VgaMultivector<float>::e<0>()};
  static constexpr auto y{VgaMultivector<float>::e<1>()};
  static constexpr auto z{VgaMultivector<float>::e<2>()};
  static constexpr auto u{1.f + x + y};
  static constexpr auto v{1.f - x + y};
  static constexpr auto w{1.f - x + y - z};

  EXPECT_EQ(1.f - x - y + x - 1.f - x * y + y - y * x - 1.f, u * u.conj());
  EXPECT_EQ(1.f + x - y - x - 1.f - x * y + y - y * x - 1.f, v * v.conj());
  EXPECT_EQ((1.f + x - y + z) + (-x - 1.f + x * y - x * z) + (y + y * x - 1.f + y * z) +
                (-z - z * x + z * y - 1.f),
            w * w.conj());
}

TEST(MultivectorTest, CanDoSimpleSpacetimeConjugateWithTimeCoordinate) {
  static constexpr auto t{SpacetimeMultivector<float>::e<0>()};
  static constexpr auto u{1.f + t};
  static constexpr auto v{1.f - t};
  static constexpr auto w{SpacetimeMultivector<float>{-1.f}};

  EXPECT_EQ(0.f, u * u.conj());
  EXPECT_EQ(0.f, v * v.conj());
  EXPECT_EQ(1.f, w * w.conj());
}

TEST(MultivectorTest, CanDoSimpleSpacetimeConjugateWithSpaceCoordinate) {
  static constexpr auto x{SpacetimeMultivector<float>::e<1>()};
  static constexpr auto u{1.f + x};
  static constexpr auto v{1.f - x};
  static constexpr auto w{SpacetimeMultivector<float>{-1.f}};

  EXPECT_EQ(2.f, u * u.conj());
  EXPECT_EQ(2.f, v * v.conj());
  EXPECT_EQ(1.f, w * w.conj());
}

TEST(MultivectorTest, CanDoSpacetimeConjugate) {
  static constexpr auto t{SpacetimeMultivector<float>::e<0>()};
  static constexpr auto x{SpacetimeMultivector<float>::e<1>()};
  static constexpr auto y{SpacetimeMultivector<float>::e<2>()};
  static constexpr auto z{SpacetimeMultivector<float>::e<3>()};
  static constexpr auto u{1.f + t + x};
  static constexpr auto v{1.f + t - x};
  static constexpr auto w{1.f - x + y - z};

  EXPECT_EQ(1.f - t - x + t - 1.f - t * x + x - x * t + 1.f, u * u.conj());
  EXPECT_EQ(1.f - t + x + t - 1.f + t * x - x + x * t + 1.f, v * v.conj());
  EXPECT_EQ((1.f + x - y + z) + (-x + 1.f + x * y - x * z) + (y + y * x + 1.f + y * z) +
                (-z - z * x + z * y + 1.f),
            w * w.conj());
}

}  // namespace ndyn::math
