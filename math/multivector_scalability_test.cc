#include "gtest/gtest.h"
#include "math/algebra.h"
#include "math/multivector.h"

namespace ndyn::math {

TEST(MultivectorScalabilityTest, CanInstantiateSeveralBases) {
  static constexpr size_t NUMBER_BASES{7};
  using AlgebraType = Algebra<float, NUMBER_BASES, 0, 0>;
  static constexpr auto x{Multivector<AlgebraType>::e<0>()};
  static constexpr auto u{1.f + x};
  EXPECT_EQ(x + 1.f, u);
}

TEST(MultivectorScalabilityTest, CanInstantiateManyBases) {
  static constexpr size_t NUMBER_BASES{20};
  using AlgebraType = Algebra<float, NUMBER_BASES, 0, 0>;
  static constexpr auto x{Multivector<AlgebraType>::e<0>()};
  static constexpr auto y{Multivector<AlgebraType>::e<1>()};
  static constexpr auto z{Multivector<AlgebraType>::e<2>()};
  static constexpr auto a{Multivector<AlgebraType>{1.f}};

  static constexpr auto u{1.f + x};
  EXPECT_EQ(x + 1.f, u);
}

TEST(MultivectorScalabilityTest, CanMultiplyMultivectorsOfSeveralBases) {
  // Once we start using the Cayley tables, by multiplying Multivectors or taking inner or outer
  // products, the number of bases we can use decreases without setting the
  // -fconstexpr-ops-limit compile flag.
  static constexpr size_t NUMBER_BASES{6};
  using AlgebraType = Algebra<float, NUMBER_BASES, 0, 0>;
  static constexpr auto x{Multivector<AlgebraType>::e<0>()};
  static constexpr auto a{Multivector<AlgebraType>{1.f}};

  EXPECT_EQ(x, a * x);
}

TEST(MultivectorScalabilityTest, CanHandleSeveralBases) {
  // For the smallest Cayley table sizes, the number of positive, negative, and zero bases must
  // sum to 7 or fewer.
  // Also, with 7 bases, we run into limits on the number of constexpr operations. This
  // limit can be raised by setting the -fconstexpr-ops-limit compile flag. To keep the testing
  // configuration simpler, we set verify the lower limit below.
  static constexpr size_t NUMBER_BASES{6};
  using AlgebraType = Algebra<float, NUMBER_BASES, 0, 0>;
  static constexpr auto x{Multivector<AlgebraType>::e<0>()};
  static constexpr auto y{Multivector<AlgebraType>::e<1>()};
  static constexpr auto z{Multivector<AlgebraType>::e<2>()};
  static constexpr auto a{Multivector<AlgebraType>{1.f}};

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
  // orthogonal to all the bases of the rhs. So, all of the results will be that constant times
  // the rhs.
  EXPECT_EQ(v * w, u.left_contraction(v * w)) << "v * w: " << v * w;
  EXPECT_EQ(w * u, v.left_contraction(w * u));
  EXPECT_EQ(2.f * u * v, w.left_contraction(u * v));

  EXPECT_EQ(4.f + 8.f * y + 6.f * z + 12.f * y * z + 9.f - 18.f * y, w.left_contraction(v * w));

  EXPECT_EQ(2.f * (2.f + 2.f * x + 3.f * z - 3.f * x * z) + 9.f + 9.f * x,
            w.left_contraction(w * u));

  EXPECT_EQ(u * v + +4.f - 4.f * x, v.left_contraction(u * v));
}

TEST(MultivectorScalabilityTest, DISABLED_CanHandleManyBases) {
  // The expanded Cayley TableEntry class can handle many more bases, but at the expensive of
  // lots more memory and longer compile times.
  // The value below seems to be an upper limit given the current implementation strategy. I do not
  // know of any practical algebras that need more than this.
  // Also, with this many bases, we run into limits on the number of constexpr operations. This
  // limit can be raised by setting the -fconstexpr-ops-limit compile flag. See the
  // large_multivectors configuration in .bazelrc.
  // This test is commented out as it takes a very long time to compile and fails without setting
  // the compile flag above. On my development machine, compiling this test takes ~6min and uses
  // ~32GB of RAM.
  // Note that this test is also DISABLED by default in order to signal that it exists when tests
  // are run.

  /*
  static constexpr size_t NUMBER_BASES{10};
  using AlgebraType = Algebra<float, NUMBER_BASES, 0, 0>;
  static constexpr auto x{Multivector<AlgebraType>::e<0>()};
  static constexpr auto y{Multivector<AlgebraType>::e<1>()};
  static constexpr auto z{Multivector<AlgebraType>::e<2>()};
  static constexpr auto a{Multivector<AlgebraType>{1.f}};

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
  // orthogonal to all the bases of the rhs. So, all of the results will be that constant times
  // the rhs.
  EXPECT_EQ(v * w, u.left_contraction(v * w)) << "v * w: " << v * w;
  EXPECT_EQ(w * u, v.left_contraction(w * u));
  EXPECT_EQ(2.f * u * v, w.left_contraction(u * v));

  EXPECT_EQ(4.f + 8.f * y + 6.f * z + 12.f * y * z + 9.f - 18.f * y, w.left_contraction(v * w));

  EXPECT_EQ(2.f * (2.f + 2.f * x + 3.f * z - 3.f * x * z) + 9.f + 9.f * x,
            w.left_contraction(w * u));

  EXPECT_EQ(u * v + +4.f - 4.f * x, v.left_contraction(u * v));
  */
}

}  // namespace ndyn::math
