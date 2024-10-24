#include <algorithm>
#include <array>
#include <initializer_list>

#include "gtest/gtest.h"
#include "math/multivector.h"
#include "math/multivector_test_utils.h"
#include "math/testing/bivector.h"

namespace ndyn::math {

template <size_t POSITIVE_BASES, size_t NEGATIVE_BASES, size_t ZERO_BASES>
constexpr bool is_reversed_basis(size_t index) {
  return false;
}

template <>
constexpr bool is_reversed_basis<3, 0, 1>(size_t index) {
  constexpr std::array<size_t, 3> reversed_bases{7 /* e021 */, 10 /* e31 */, 13 /* e032 */};
  for (size_t i = 0; i < reversed_bases.size(); ++i) {
    if (reversed_bases[i] == index) {
      return true;
    }
  }
  return false;
}

template <typename ScalarType, size_t POSITIVE_BASES, size_t NEGATIVE_BASES, size_t ZERO_BASES>
constexpr ScalarType correct_basis_sign(size_t basis_index, ScalarType coefficient) {
  if (is_reversed_basis<POSITIVE_BASES, NEGATIVE_BASES, ZERO_BASES>(basis_index)) {
    return -1 * coefficient;
  } else {
    return coefficient;
  }
}

template <typename ScalarType, size_t POSITIVE_BASES, size_t NEGATIVE_BASES, size_t ZERO_BASES>
constexpr Multivector<ScalarType, POSITIVE_BASES, NEGATIVE_BASES, ZERO_BASES> as_multivector(
    std::initializer_list<ScalarType> values) {
  Multivector<ScalarType, POSITIVE_BASES, NEGATIVE_BASES, ZERO_BASES> result{};
  const auto array{
      from_bivector_net_order<ScalarType, POSITIVE_BASES + NEGATIVE_BASES + ZERO_BASES>(values)};
  for (size_t i = 0; i < array.size(); ++i) {
    result.set_basis(
        i, correct_basis_sign<ScalarType, POSITIVE_BASES, NEGATIVE_BASES, ZERO_BASES>(i, array[i]));
  }
  return result;
}

template <typename BivectorNetType, typename VectorType>
constexpr VectorType to_multivector(const BivectorNetType& bvec) {
  constexpr size_t P{VectorType::NUM_POSITIVE_BASES};
  constexpr size_t N{VectorType::NUM_NEGATIVE_BASES};
  constexpr size_t Z{VectorType::NUM_ZERO_BASES};

  VectorType result{};

  for (size_t i = 0; i < VectorType::bases_count(); ++i) {
    result.set_basis(index_to_bit_basis<BivectorNetType>(i),
                     correct_basis_sign<typename VectorType::ScalarType, P, N, Z>(i, bvec[i]));
  }
  return result;
}

template <typename BivectorNetType>
BivectorNetType create(std::initializer_list<float> values) {
  BivectorNetType result{};
  size_t i{};
  for (float value : values) {
    result[i] = value;
    ++i;
  }
  return result;
}

TEST(R000BivectorNetDirectTest, ConjugateMatches) {
  using ScalarType = float;
  static constexpr size_t P{0};
  static constexpr size_t N{0};
  static constexpr size_t Z{0};
  using VectorType = Multivector<ScalarType, P, N, Z>;
  using BivectorNetType = typename BivectorNetTypes<P, N, Z>::type;

  BivectorNetType bvec{create<BivectorNetType>({1})};
  VectorType v{to_multivector<BivectorNetType, VectorType>(bvec)};
  VectorType result{v.conj()};
  VectorType expected{to_multivector<BivectorNetType, VectorType>(bvec.Conjugate())};
  EXPECT_EQ(expected, result);
}

TEST(R001BivectorNetDirectTest, ConjugateMatches) {
  using ScalarType = float;
  static constexpr size_t P{0};
  static constexpr size_t N{0};
  static constexpr size_t Z{1};
  using VectorType = Multivector<ScalarType, P, N, Z>;
  using BivectorNetType = typename BivectorNetTypes<P, N, Z>::type;

  BivectorNetType bvec{create<BivectorNetType>({1, 2})};
  VectorType v{to_multivector<BivectorNetType, VectorType>(bvec)};
  VectorType result{v.conj()};
  VectorType expected{to_multivector<BivectorNetType, VectorType>(bvec.Conjugate())};
  EXPECT_EQ(expected, result);
}

TEST(R010BivectorNetDirectTest, ConjugateMatches) {
  using ScalarType = float;
  static constexpr size_t P{0};
  static constexpr size_t N{1};
  static constexpr size_t Z{0};
  using VectorType = Multivector<ScalarType, P, N, Z>;
  using BivectorNetType = typename BivectorNetTypes<P, N, Z>::type;

  BivectorNetType bvec{create<BivectorNetType>({1, 2})};
  VectorType v{to_multivector<BivectorNetType, VectorType>(bvec)};
  VectorType result{v.conj()};
  VectorType expected{to_multivector<BivectorNetType, VectorType>(bvec.Conjugate())};
  EXPECT_EQ(expected, result);
}

TEST(R011BivectorNetDirectTest, ConjugateMatches) {
  using ScalarType = float;
  static constexpr size_t P{0};
  static constexpr size_t N{1};
  static constexpr size_t Z{1};
  using VectorType = Multivector<ScalarType, P, N, Z>;
  using BivectorNetType = typename BivectorNetTypes<P, N, Z>::type;

  BivectorNetType bvec{create<BivectorNetType>({1, 2, 3, 4})};
  VectorType v{to_multivector<BivectorNetType, VectorType>(bvec)};
  VectorType result{v.conj()};
  VectorType expected{to_multivector<BivectorNetType, VectorType>(bvec.Conjugate())};
  EXPECT_EQ(expected, result);
}

TEST(R100BivectorNetDirectTest, ConjugateMatches) {
  using ScalarType = float;
  static constexpr size_t P{1};
  static constexpr size_t N{0};
  static constexpr size_t Z{0};
  using VectorType = Multivector<ScalarType, P, N, Z>;
  using BivectorNetType = typename BivectorNetTypes<P, N, Z>::type;

  BivectorNetType bvec{create<BivectorNetType>({1, 2})};
  VectorType v{to_multivector<BivectorNetType, VectorType>(bvec)};
  VectorType result{v.conj()};
  VectorType expected{to_multivector<BivectorNetType, VectorType>(bvec.Conjugate())};
  EXPECT_EQ(expected, result);
}

TEST(R110BivectorNetDirectTest, ConjugateMatches) {
  using ScalarType = float;
  static constexpr size_t P{1};
  static constexpr size_t N{1};
  static constexpr size_t Z{0};
  using VectorType = Multivector<ScalarType, P, N, Z>;
  using BivectorNetType = typename BivectorNetTypes<P, N, Z>::type;

  BivectorNetType bvec{create<BivectorNetType>({1, 2, 3, 4})};
  VectorType v{to_multivector<BivectorNetType, VectorType>(bvec)};
  VectorType result{v.conj()};
  VectorType expected{to_multivector<BivectorNetType, VectorType>(bvec.Conjugate())};
  EXPECT_EQ(expected, result);
}

TEST(R101BivectorNetDirectTest, ConjugateMatches) {
  using ScalarType = float;
  static constexpr size_t P{1};
  static constexpr size_t N{0};
  static constexpr size_t Z{1};
  using VectorType = Multivector<ScalarType, P, N, Z>;
  using BivectorNetType = typename BivectorNetTypes<P, N, Z>::type;

  BivectorNetType bvec{create<BivectorNetType>({1, 2, 3, 4})};
  VectorType v{to_multivector<BivectorNetType, VectorType>(bvec)};
  VectorType result{v.conj()};
  VectorType expected{to_multivector<BivectorNetType, VectorType>(bvec.Conjugate())};
  EXPECT_EQ(expected, result);
}

TEST(R111BivectorNetDirectTest, ConjugateMatches) {
  using ScalarType = float;
  static constexpr size_t P{1};
  static constexpr size_t N{1};
  static constexpr size_t Z{1};
  using VectorType = Multivector<ScalarType, P, N, Z>;
  using BivectorNetType = typename BivectorNetTypes<P, N, Z>::type;

  BivectorNetType bvec{create<BivectorNetType>({1, 2, 3, 4, 5, 6, 7, 8})};
  VectorType v{to_multivector<BivectorNetType, VectorType>(bvec)};
  VectorType result{v.conj()};
  VectorType expected{to_multivector<BivectorNetType, VectorType>(bvec.Conjugate())};
  EXPECT_EQ(expected, result);
}

TEST(R130BivectorNetDirectTest, ConjugateMatches) {
  using ScalarType = float;
  static constexpr size_t P{1};
  static constexpr size_t N{3};
  static constexpr size_t Z{0};
  using VectorType = Multivector<ScalarType, P, N, Z>;
  using BivectorNetType = typename BivectorNetTypes<P, N, Z>::type;

  BivectorNetType bvec{
      create<BivectorNetType>({1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16})};
  VectorType v{to_multivector<BivectorNetType, VectorType>(bvec)};
  VectorType result{v.conj()};
  VectorType expected{to_multivector<BivectorNetType, VectorType>(bvec.Conjugate())};
  EXPECT_EQ(expected, result);
}

TEST(R200BivectorNetDirectTest, ConjugateMatches) {
  using ScalarType = float;
  static constexpr size_t P{2};
  static constexpr size_t N{0};
  static constexpr size_t Z{0};
  using VectorType = Multivector<ScalarType, P, N, Z>;
  using BivectorNetType = typename BivectorNetTypes<P, N, Z>::type;

  BivectorNetType bvec{create<BivectorNetType>({1, 2, 3, 4})};
  VectorType v{to_multivector<BivectorNetType, VectorType>(bvec)};
  VectorType result{v.conj()};
  VectorType expected{to_multivector<BivectorNetType, VectorType>(bvec.Conjugate())};
  EXPECT_EQ(expected, result);
}

TEST(R201BivectorNetDirectTest, ConjugateMatches) {
  using ScalarType = float;
  static constexpr size_t P{2};
  static constexpr size_t N{0};
  static constexpr size_t Z{1};
  using VectorType = Multivector<ScalarType, P, N, Z>;
  using BivectorNetType = typename BivectorNetTypes<P, N, Z>::type;

  BivectorNetType bvec{create<BivectorNetType>({1, 2, 3, 4, 5, 6, 7, 8})};
  VectorType v{to_multivector<BivectorNetType, VectorType>(bvec)};
  VectorType result{v.conj()};
  VectorType expected{to_multivector<BivectorNetType, VectorType>(bvec.Conjugate())};
  EXPECT_EQ(expected, result);
}

TEST(R210BivectorNetDirectTest, ConjugateMatches) {
  using ScalarType = float;
  static constexpr size_t P{2};
  static constexpr size_t N{1};
  static constexpr size_t Z{0};
  using VectorType = Multivector<ScalarType, P, N, Z>;
  using BivectorNetType = typename BivectorNetTypes<P, N, Z>::type;

  BivectorNetType bvec{create<BivectorNetType>({1, 2, 3, 4, 5, 6, 7, 8})};
  VectorType v{to_multivector<BivectorNetType, VectorType>(bvec)};
  VectorType result{v.conj()};
  VectorType expected{to_multivector<BivectorNetType, VectorType>(bvec.Conjugate())};
  EXPECT_EQ(expected, result);
}

TEST(R300BivectorNetDirectTest, ConjugateMatches) {
  using ScalarType = float;
  static constexpr size_t P{3};
  static constexpr size_t N{0};
  static constexpr size_t Z{0};
  using VectorType = Multivector<ScalarType, P, N, Z>;
  using BivectorNetType = typename BivectorNetTypes<P, N, Z>::type;

  BivectorNetType bvec{create<BivectorNetType>({1, 2, 3, 4, 5, 6, 7, 8})};
  VectorType v{to_multivector<BivectorNetType, VectorType>(bvec)};
  VectorType result{v.conj()};
  VectorType expected{to_multivector<BivectorNetType, VectorType>(bvec.Conjugate())};
  EXPECT_EQ(expected, result);
}

TEST(R301BivectorNetDirectTest, ConjugateMatches) {
  using ScalarType = float;
  static constexpr size_t P{3};
  static constexpr size_t N{0};
  static constexpr size_t Z{1};
  using VectorType = Multivector<ScalarType, P, N, Z>;
  using BivectorNetType = typename BivectorNetTypes<P, N, Z>::type;

  BivectorNetType bvec{
      create<BivectorNetType>({1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16})};
  VectorType v{to_multivector<BivectorNetType, VectorType>(bvec)};
  VectorType result{v.conj()};
  VectorType expected{to_multivector<BivectorNetType, VectorType>(bvec.Conjugate())};
  EXPECT_EQ(expected, result);
}

TEST(R410BivectorNetDirectTest, ConjugateMatches) {
  using ScalarType = float;
  static constexpr size_t P{4};
  static constexpr size_t N{1};
  static constexpr size_t Z{0};
  using VectorType = Multivector<ScalarType, P, N, Z>;
  using BivectorNetType = typename BivectorNetTypes<P, N, Z>::type;

  BivectorNetType bvec{
      create<BivectorNetType>({1,  2,  3,  4,  5,  6,  7,  8,  9,  10, 11, 12, 13, 14, 15, 16,
                               17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32})};
  VectorType v{to_multivector<BivectorNetType, VectorType>(bvec)};
  VectorType result{v.conj()};
  VectorType expected{to_multivector<BivectorNetType, VectorType>(bvec.Conjugate())};
  EXPECT_EQ(expected, result);
}

TEST(R000BivectorNetDirectTest, ReverseMatches) {
  using ScalarType = float;
  static constexpr size_t P{0};
  static constexpr size_t N{0};
  static constexpr size_t Z{0};
  using VectorType = Multivector<ScalarType, P, N, Z>;
  using BivectorNetType = typename BivectorNetTypes<P, N, Z>::type;

  BivectorNetType bvec{create<BivectorNetType>({1})};
  VectorType v{to_multivector<BivectorNetType, VectorType>(bvec)};
  VectorType result{v.reverse()};
  VectorType expected{to_multivector<BivectorNetType, VectorType>(~bvec)};
  EXPECT_EQ(expected, result);
}

TEST(R001BivectorNetDirectTest, ReverseMatches) {
  using ScalarType = float;
  static constexpr size_t P{0};
  static constexpr size_t N{0};
  static constexpr size_t Z{1};
  using VectorType = Multivector<ScalarType, P, N, Z>;
  using BivectorNetType = typename BivectorNetTypes<P, N, Z>::type;

  BivectorNetType bvec{create<BivectorNetType>({1, 2})};
  VectorType v{to_multivector<BivectorNetType, VectorType>(bvec)};
  VectorType result{v.reverse()};
  VectorType expected{to_multivector<BivectorNetType, VectorType>(~bvec)};
  EXPECT_EQ(expected, result);
}

TEST(R010BivectorNetDirectTest, ReverseMatches) {
  using ScalarType = float;
  static constexpr size_t P{0};
  static constexpr size_t N{1};
  static constexpr size_t Z{0};
  using VectorType = Multivector<ScalarType, P, N, Z>;
  using BivectorNetType = typename BivectorNetTypes<P, N, Z>::type;

  BivectorNetType bvec{create<BivectorNetType>({1, 2})};
  VectorType v{to_multivector<BivectorNetType, VectorType>(bvec)};
  VectorType result{v.reverse()};
  VectorType expected{to_multivector<BivectorNetType, VectorType>(~bvec)};
  EXPECT_EQ(expected, result);
}

TEST(R011BivectorNetDirectTest, ReverseMatches) {
  using ScalarType = float;
  static constexpr size_t P{0};
  static constexpr size_t N{1};
  static constexpr size_t Z{1};
  using VectorType = Multivector<ScalarType, P, N, Z>;
  using BivectorNetType = typename BivectorNetTypes<P, N, Z>::type;

  BivectorNetType bvec{create<BivectorNetType>({1, 2, 3, 4})};
  VectorType v{to_multivector<BivectorNetType, VectorType>(bvec)};
  VectorType result{v.reverse()};
  VectorType expected{to_multivector<BivectorNetType, VectorType>(~bvec)};
  EXPECT_EQ(expected, result);
}

TEST(R100BivectorNetDirectTest, ReverseMatches) {
  using ScalarType = float;
  static constexpr size_t P{1};
  static constexpr size_t N{0};
  static constexpr size_t Z{0};
  using VectorType = Multivector<ScalarType, P, N, Z>;
  using BivectorNetType = typename BivectorNetTypes<P, N, Z>::type;

  BivectorNetType bvec{create<BivectorNetType>({1, 2})};
  VectorType v{to_multivector<BivectorNetType, VectorType>(bvec)};
  VectorType result{v.reverse()};
  VectorType expected{to_multivector<BivectorNetType, VectorType>(~bvec)};
  EXPECT_EQ(expected, result);
}

TEST(R110BivectorNetDirectTest, ReverseMatches) {
  using ScalarType = float;
  static constexpr size_t P{1};
  static constexpr size_t N{1};
  static constexpr size_t Z{0};
  using VectorType = Multivector<ScalarType, P, N, Z>;
  using BivectorNetType = typename BivectorNetTypes<P, N, Z>::type;

  BivectorNetType bvec{create<BivectorNetType>({1, 2, 3, 4})};
  VectorType v{to_multivector<BivectorNetType, VectorType>(bvec)};
  VectorType result{v.reverse()};
  VectorType expected{to_multivector<BivectorNetType, VectorType>(~bvec)};
  EXPECT_EQ(expected, result);
}

TEST(R101BivectorNetDirectTest, ReverseMatches) {
  using ScalarType = float;
  static constexpr size_t P{1};
  static constexpr size_t N{0};
  static constexpr size_t Z{1};
  using VectorType = Multivector<ScalarType, P, N, Z>;
  using BivectorNetType = typename BivectorNetTypes<P, N, Z>::type;

  BivectorNetType bvec{create<BivectorNetType>({1, 2, 3, 4})};
  VectorType v{to_multivector<BivectorNetType, VectorType>(bvec)};
  VectorType result{v.reverse()};
  VectorType expected{to_multivector<BivectorNetType, VectorType>(~bvec)};
  EXPECT_EQ(expected, result);
}

TEST(R111BivectorNetDirectTest, ReverseMatches) {
  using ScalarType = float;
  static constexpr size_t P{1};
  static constexpr size_t N{1};
  static constexpr size_t Z{1};
  using VectorType = Multivector<ScalarType, P, N, Z>;
  using BivectorNetType = typename BivectorNetTypes<P, N, Z>::type;

  BivectorNetType bvec{create<BivectorNetType>({1, 2, 3, 4, 5, 6, 7, 8})};
  VectorType v{to_multivector<BivectorNetType, VectorType>(bvec)};
  VectorType result{v.reverse()};
  VectorType expected{to_multivector<BivectorNetType, VectorType>(~bvec)};
  EXPECT_EQ(expected, result);
}

TEST(R130BivectorNetDirectTest, ReverseMatches) {
  using ScalarType = float;
  static constexpr size_t P{1};
  static constexpr size_t N{3};
  static constexpr size_t Z{0};
  using VectorType = Multivector<ScalarType, P, N, Z>;
  using BivectorNetType = typename BivectorNetTypes<P, N, Z>::type;

  BivectorNetType bvec{
      create<BivectorNetType>({1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16})};
  VectorType v{to_multivector<BivectorNetType, VectorType>(bvec)};
  VectorType result{v.reverse()};
  VectorType expected{to_multivector<BivectorNetType, VectorType>(~bvec)};
  EXPECT_EQ(expected, result);
}

TEST(R200BivectorNetDirectTest, ReverseMatches) {
  using ScalarType = float;
  static constexpr size_t P{2};
  static constexpr size_t N{0};
  static constexpr size_t Z{0};
  using VectorType = Multivector<ScalarType, P, N, Z>;
  using BivectorNetType = typename BivectorNetTypes<P, N, Z>::type;

  BivectorNetType bvec{create<BivectorNetType>({1, 2, 3, 4})};
  VectorType v{to_multivector<BivectorNetType, VectorType>(bvec)};
  VectorType result{v.reverse()};
  VectorType expected{to_multivector<BivectorNetType, VectorType>(~bvec)};
  EXPECT_EQ(expected, result);
}

TEST(R201BivectorNetDirectTest, ReverseMatches) {
  using ScalarType = float;
  static constexpr size_t P{2};
  static constexpr size_t N{0};
  static constexpr size_t Z{1};
  using VectorType = Multivector<ScalarType, P, N, Z>;
  using BivectorNetType = typename BivectorNetTypes<P, N, Z>::type;

  BivectorNetType bvec{create<BivectorNetType>({1, 2, 3, 4, 5, 6, 7, 8})};
  VectorType v{to_multivector<BivectorNetType, VectorType>(bvec)};
  VectorType result{v.reverse()};
  VectorType expected{to_multivector<BivectorNetType, VectorType>(~bvec)};
  EXPECT_EQ(expected, result);
}

TEST(R210BivectorNetDirectTest, ReverseMatches) {
  using ScalarType = float;
  static constexpr size_t P{2};
  static constexpr size_t N{1};
  static constexpr size_t Z{0};
  using VectorType = Multivector<ScalarType, P, N, Z>;
  using BivectorNetType = typename BivectorNetTypes<P, N, Z>::type;

  BivectorNetType bvec{create<BivectorNetType>({1, 2, 3, 4, 5, 6, 7, 8})};
  VectorType v{to_multivector<BivectorNetType, VectorType>(bvec)};
  VectorType result{v.reverse()};
  VectorType expected{to_multivector<BivectorNetType, VectorType>(~bvec)};
  EXPECT_EQ(expected, result);
}

TEST(R300BivectorNetDirectTest, ReverseMatches) {
  using ScalarType = float;
  static constexpr size_t P{3};
  static constexpr size_t N{0};
  static constexpr size_t Z{0};
  using VectorType = Multivector<ScalarType, P, N, Z>;
  using BivectorNetType = typename BivectorNetTypes<P, N, Z>::type;

  BivectorNetType bvec{create<BivectorNetType>({1, 2, 3, 4, 5, 6, 7, 8})};
  VectorType v{to_multivector<BivectorNetType, VectorType>(bvec)};
  VectorType result{v.reverse()};
  VectorType expected{to_multivector<BivectorNetType, VectorType>(~bvec)};
  EXPECT_EQ(expected, result);
}

TEST(R301BivectorNetDirectTest, ReverseMatches) {
  using ScalarType = float;
  static constexpr size_t P{3};
  static constexpr size_t N{0};
  static constexpr size_t Z{1};
  using VectorType = Multivector<ScalarType, P, N, Z>;
  using BivectorNetType = typename BivectorNetTypes<P, N, Z>::type;

  BivectorNetType bvec{
      create<BivectorNetType>({1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16})};
  VectorType v{to_multivector<BivectorNetType, VectorType>(bvec)};
  VectorType result{v.reverse()};
  VectorType expected{to_multivector<BivectorNetType, VectorType>(~bvec)};
  EXPECT_EQ(expected, result);
}

TEST(R410BivectorNetDirectTest, ReverseMatches) {
  using ScalarType = float;
  static constexpr size_t P{4};
  static constexpr size_t N{1};
  static constexpr size_t Z{0};
  using VectorType = Multivector<ScalarType, P, N, Z>;
  using BivectorNetType = typename BivectorNetTypes<P, N, Z>::type;

  BivectorNetType bvec{
      create<BivectorNetType>({1,  2,  3,  4,  5,  6,  7,  8,  9,  10, 11, 12, 13, 14, 15, 16,
                               17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32})};
  VectorType v{to_multivector<BivectorNetType, VectorType>(bvec)};
  VectorType result{v.reverse()};
  VectorType expected{to_multivector<BivectorNetType, VectorType>(~bvec)};
  EXPECT_EQ(expected, result);
}

TEST(R000BivectorNetDirectTest, DualMatches) {
  using ScalarType = float;
  static constexpr size_t P{0};
  static constexpr size_t N{0};
  static constexpr size_t Z{0};
  using VectorType = Multivector<ScalarType, P, N, Z>;
  using BivectorNetType = typename BivectorNetTypes<P, N, Z>::type;

  BivectorNetType bvec{create<BivectorNetType>({1})};
  VectorType v{to_multivector<BivectorNetType, VectorType>(bvec)};
  VectorType result{v.dual()};
  VectorType expected{to_multivector<BivectorNetType, VectorType>(!bvec)};
  EXPECT_EQ(expected, result);
}

TEST(R001BivectorNetDirectTest, DualMatches) {
  using ScalarType = float;
  static constexpr size_t P{0};
  static constexpr size_t N{0};
  static constexpr size_t Z{1};
  using VectorType = Multivector<ScalarType, P, N, Z>;
  using BivectorNetType = typename BivectorNetTypes<P, N, Z>::type;

  BivectorNetType bvec{create<BivectorNetType>({1, 2})};
  VectorType v{to_multivector<BivectorNetType, VectorType>(bvec)};
  VectorType result{v.dual()};
  VectorType expected{to_multivector<BivectorNetType, VectorType>(!bvec)};
  EXPECT_EQ(expected, result);
}

TEST(R010BivectorNetDirectTest, DualMatches) {
  using ScalarType = float;
  static constexpr size_t P{0};
  static constexpr size_t N{1};
  static constexpr size_t Z{0};
  using VectorType = Multivector<ScalarType, P, N, Z>;
  using BivectorNetType = typename BivectorNetTypes<P, N, Z>::type;

  BivectorNetType bvec{create<BivectorNetType>({1, 2})};
  VectorType v{to_multivector<BivectorNetType, VectorType>(bvec)};
  VectorType result{v.dual()};
  VectorType expected{to_multivector<BivectorNetType, VectorType>(!bvec)};
  EXPECT_EQ(expected, result);
}

TEST(R011BivectorNetDirectTest, DualMatches) {
  using ScalarType = float;
  static constexpr size_t P{0};
  static constexpr size_t N{1};
  static constexpr size_t Z{1};
  using VectorType = Multivector<ScalarType, P, N, Z>;
  using BivectorNetType = typename BivectorNetTypes<P, N, Z>::type;

  BivectorNetType bvec{create<BivectorNetType>({1, 2, 3, 4})};
  VectorType v{to_multivector<BivectorNetType, VectorType>(bvec)};
  VectorType result{v.dual()};
  VectorType expected{to_multivector<BivectorNetType, VectorType>(!bvec)};
  EXPECT_EQ(expected, result);
}

TEST(R100BivectorNetDirectTest, DualMatches) {
  using ScalarType = float;
  static constexpr size_t P{1};
  static constexpr size_t N{0};
  static constexpr size_t Z{0};
  using VectorType = Multivector<ScalarType, P, N, Z>;
  using BivectorNetType = typename BivectorNetTypes<P, N, Z>::type;

  BivectorNetType bvec{create<BivectorNetType>({1, 2})};
  VectorType v{to_multivector<BivectorNetType, VectorType>(bvec)};
  VectorType result{v.dual()};
  VectorType expected{to_multivector<BivectorNetType, VectorType>(!bvec)};
  EXPECT_EQ(expected, result);
}

TEST(R110BivectorNetDirectTest, DualMatches) {
  using ScalarType = float;
  static constexpr size_t P{1};
  static constexpr size_t N{1};
  static constexpr size_t Z{0};
  using VectorType = Multivector<ScalarType, P, N, Z>;
  using BivectorNetType = typename BivectorNetTypes<P, N, Z>::type;

  BivectorNetType bvec{create<BivectorNetType>({1, 2, 3, 4})};
  VectorType v{to_multivector<BivectorNetType, VectorType>(bvec)};
  VectorType result{v.dual()};
  VectorType expected{to_multivector<BivectorNetType, VectorType>(!bvec)};
  EXPECT_EQ(expected, result);
}

TEST(R101BivectorNetDirectTest, DualMatches) {
  using ScalarType = float;
  static constexpr size_t P{1};
  static constexpr size_t N{0};
  static constexpr size_t Z{1};
  using VectorType = Multivector<ScalarType, P, N, Z>;
  using BivectorNetType = typename BivectorNetTypes<P, N, Z>::type;

  BivectorNetType bvec{create<BivectorNetType>({1, 2, 3, 4})};
  VectorType v{to_multivector<BivectorNetType, VectorType>(bvec)};
  VectorType result{v.dual()};
  VectorType expected{to_multivector<BivectorNetType, VectorType>(!bvec)};
  EXPECT_EQ(expected, result);
}

TEST(R111BivectorNetDirectTest, DualMatches) {
  using ScalarType = float;
  static constexpr size_t P{1};
  static constexpr size_t N{1};
  static constexpr size_t Z{1};
  using VectorType = Multivector<ScalarType, P, N, Z>;
  using BivectorNetType = typename BivectorNetTypes<P, N, Z>::type;

  BivectorNetType bvec{create<BivectorNetType>({1, 2, 3, 4, 5, 6, 7, 8})};
  VectorType v{to_multivector<BivectorNetType, VectorType>(bvec)};
  VectorType result{v.dual()};
  VectorType expected{to_multivector<BivectorNetType, VectorType>(!bvec)};
  EXPECT_EQ(expected, result);
}

TEST(R130BivectorNetDirectTest, DualMatches) {
  using ScalarType = float;
  static constexpr size_t P{1};
  static constexpr size_t N{3};
  static constexpr size_t Z{0};
  using VectorType = Multivector<ScalarType, P, N, Z>;
  using BivectorNetType = typename BivectorNetTypes<P, N, Z>::type;

  BivectorNetType bvec{
      create<BivectorNetType>({1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16})};
  VectorType v{to_multivector<BivectorNetType, VectorType>(bvec)};
  VectorType result{v.dual()};
  VectorType expected{to_multivector<BivectorNetType, VectorType>(!bvec)};
  EXPECT_EQ(expected, result);
}

TEST(R200BivectorNetDirectTest, DualMatches) {
  using ScalarType = float;
  static constexpr size_t P{2};
  static constexpr size_t N{0};
  static constexpr size_t Z{0};
  using VectorType = Multivector<ScalarType, P, N, Z>;
  using BivectorNetType = typename BivectorNetTypes<P, N, Z>::type;

  BivectorNetType bvec{create<BivectorNetType>({1, 2, 3, 4})};
  VectorType v{to_multivector<BivectorNetType, VectorType>(bvec)};
  VectorType result{v.dual()};
  VectorType expected{to_multivector<BivectorNetType, VectorType>(!bvec)};
  EXPECT_EQ(expected, result);
}

TEST(R201BivectorNetDirectTest, DualMatches) {
  using ScalarType = float;
  static constexpr size_t P{2};
  static constexpr size_t N{0};
  static constexpr size_t Z{1};
  using VectorType = Multivector<ScalarType, P, N, Z>;
  using BivectorNetType = typename BivectorNetTypes<P, N, Z>::type;

  BivectorNetType bvec{create<BivectorNetType>({1, 2, 3, 4, 5, 6, 7, 8})};
  VectorType v{to_multivector<BivectorNetType, VectorType>(bvec)};
  VectorType result{v.dual()};
  VectorType expected{to_multivector<BivectorNetType, VectorType>(!bvec)};
  EXPECT_EQ(expected, result);
}

TEST(R210BivectorNetDirectTest, DualMatches) {
  using ScalarType = float;
  static constexpr size_t P{2};
  static constexpr size_t N{1};
  static constexpr size_t Z{0};
  using VectorType = Multivector<ScalarType, P, N, Z>;
  using BivectorNetType = typename BivectorNetTypes<P, N, Z>::type;

  BivectorNetType bvec{create<BivectorNetType>({1, 2, 3, 4, 5, 6, 7, 8})};
  VectorType v{to_multivector<BivectorNetType, VectorType>(bvec)};
  VectorType result{v.dual()};
  VectorType expected{to_multivector<BivectorNetType, VectorType>(!bvec)};
  EXPECT_EQ(expected, result);
}

TEST(R300BivectorNetDirectTest, DualMatches) {
  using ScalarType = float;
  static constexpr size_t P{3};
  static constexpr size_t N{0};
  static constexpr size_t Z{0};
  using VectorType = Multivector<ScalarType, P, N, Z>;
  using BivectorNetType = typename BivectorNetTypes<P, N, Z>::type;

  BivectorNetType bvec{create<BivectorNetType>({1, 2, 3, 4, 5, 6, 7, 8})};
  VectorType v{to_multivector<BivectorNetType, VectorType>(bvec)};
  VectorType result{v.dual()};
  VectorType expected{to_multivector<BivectorNetType, VectorType>(!bvec)};
  EXPECT_EQ(expected, result);
}

TEST(R301BivectorNetDirectTest, DualMatches) {
  using ScalarType = float;
  static constexpr size_t P{3};
  static constexpr size_t N{0};
  static constexpr size_t Z{1};
  using VectorType = Multivector<ScalarType, P, N, Z>;
  using BivectorNetType = typename BivectorNetTypes<P, N, Z>::type;

  BivectorNetType bvec{
      create<BivectorNetType>({1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16})};
  VectorType v{to_multivector<BivectorNetType, VectorType>(bvec)};
  VectorType result{v.dual()};
  VectorType expected{to_multivector<BivectorNetType, VectorType>(!bvec)};
  EXPECT_EQ(expected, result);
}

TEST(R410BivectorNetDirectTest, DualMatches) {
  using ScalarType = float;
  static constexpr size_t P{4};
  static constexpr size_t N{1};
  static constexpr size_t Z{0};
  using VectorType = Multivector<ScalarType, P, N, Z>;
  using BivectorNetType = typename BivectorNetTypes<P, N, Z>::type;

  BivectorNetType bvec{
      create<BivectorNetType>({1,  2,  3,  4,  5,  6,  7,  8,  9,  10, 11, 12, 13, 14, 15, 16,
                               17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32})};
  VectorType v{to_multivector<BivectorNetType, VectorType>(bvec)};
  VectorType result{v.dual()};
  VectorType expected{to_multivector<BivectorNetType, VectorType>(!bvec)};
  EXPECT_EQ(expected, result);
}

}  // namespace ndyn::math
