#include <algorithm>
#include <array>
#include <initializer_list>

#include "gtest/gtest.h"
#include "math/multivector.h"
#include "math/multivector_test_utils.h"
#include "math/testing/bivector.h"

namespace ndyn::math {

template <typename BivectorNetType>
constexpr float correct_basis_sign(size_t basis_index, float coefficient) {
  if (BivectorNetType::reversed_bases[basis_index]) {
    return -1 * coefficient;
  } else {
    return coefficient;
  }
}

template <typename BivectorNetType, typename VectorType>
constexpr VectorType to_multivector(const BivectorNetType& bvec) {
  VectorType result{};
  for (size_t i = 0; i < VectorType::bases_count(); ++i) {
    result.set_basis(index_to_bit_basis<BivectorNetType>(i),
                     correct_basis_sign<BivectorNetType>(i, bvec[i]));
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

template <typename BivectorNetType>
BivectorNetType create_with_counting() {
  BivectorNetType result{};
  for (size_t i = 0; i < BivectorNetType::NUM_BASES; ++i) {
    result[i] = i;
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

  BivectorNetType bvec{create_with_counting<BivectorNetType>()};
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

TEST(R401BivectorNetDirectTest, ConjugateMatches) {
  using ScalarType = float;
  static constexpr size_t P{4};
  static constexpr size_t N{0};
  static constexpr size_t Z{1};
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

TEST(R401BivectorNetDirectTest, ReverseMatches) {
  using ScalarType = float;
  static constexpr size_t P{4};
  static constexpr size_t N{0};
  static constexpr size_t Z{1};
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

TEST(R401BivectorNetDirectTest, DualMatches) {
  using ScalarType = float;
  static constexpr size_t P{4};
  static constexpr size_t N{0};
  static constexpr size_t Z{1};
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

TEST(R000BivectorNetDirectTest, RegressiveProductMatches) {
  using ScalarType = float;
  static constexpr size_t P{0};
  static constexpr size_t N{0};
  static constexpr size_t Z{0};
  using VectorType = Multivector<ScalarType, P, N, Z>;
  using BivectorNetType = typename BivectorNetTypes<P, N, Z>::type;

  BivectorNetType bvec1{create<BivectorNetType>({1})};
  BivectorNetType bvec2{32 * bvec1};
  VectorType v1{to_multivector<BivectorNetType, VectorType>(bvec1)};
  VectorType v2{to_multivector<BivectorNetType, VectorType>(bvec2)};
  VectorType result{v1.regress(v2)};
  VectorType expected{to_multivector<BivectorNetType, VectorType>(bvec1 & bvec2)};
  EXPECT_EQ(expected, result);
}

TEST(R001BivectorNetDirectTest, RegressiveProductMatches) {
  using ScalarType = float;
  static constexpr size_t P{0};
  static constexpr size_t N{0};
  static constexpr size_t Z{1};
  using VectorType = Multivector<ScalarType, P, N, Z>;
  using BivectorNetType = typename BivectorNetTypes<P, N, Z>::type;

  BivectorNetType bvec1{create<BivectorNetType>({1, 2})};
  BivectorNetType bvec2{32 * bvec1};
  VectorType v1{to_multivector<BivectorNetType, VectorType>(bvec1)};
  VectorType v2{to_multivector<BivectorNetType, VectorType>(bvec2)};
  VectorType result{v1.regress(v2)};
  VectorType expected{to_multivector<BivectorNetType, VectorType>(bvec1 & bvec2)};
  EXPECT_EQ(expected, result);
}

TEST(R010BivectorNetDirectTest, RegressiveProductMatches) {
  using ScalarType = float;
  static constexpr size_t P{0};
  static constexpr size_t N{1};
  static constexpr size_t Z{0};
  using VectorType = Multivector<ScalarType, P, N, Z>;
  using BivectorNetType = typename BivectorNetTypes<P, N, Z>::type;

  BivectorNetType bvec1{create<BivectorNetType>({1, 2})};
  BivectorNetType bvec2{32 * bvec1};
  VectorType v1{to_multivector<BivectorNetType, VectorType>(bvec1)};
  VectorType v2{to_multivector<BivectorNetType, VectorType>(bvec2)};
  VectorType result{v1.regress(v2)};
  VectorType expected{to_multivector<BivectorNetType, VectorType>(bvec1 & bvec2)};
  EXPECT_EQ(expected, result);
}

TEST(R011BivectorNetDirectTest, RegressiveProductMatches) {
  using ScalarType = float;
  static constexpr size_t P{0};
  static constexpr size_t N{1};
  static constexpr size_t Z{1};
  using VectorType = Multivector<ScalarType, P, N, Z>;
  using BivectorNetType = typename BivectorNetTypes<P, N, Z>::type;

  BivectorNetType bvec1{create<BivectorNetType>({1, 2, 3, 4})};
  BivectorNetType bvec2{32 * bvec1};
  VectorType v1{to_multivector<BivectorNetType, VectorType>(bvec1)};
  VectorType v2{to_multivector<BivectorNetType, VectorType>(bvec2)};
  VectorType result{v1.regress(v2)};
  VectorType expected{to_multivector<BivectorNetType, VectorType>(bvec1 & bvec2)};
  EXPECT_EQ(expected, result);
}

TEST(R100BivectorNetDirectTest, RegressiveProductMatches) {
  using ScalarType = float;
  static constexpr size_t P{1};
  static constexpr size_t N{0};
  static constexpr size_t Z{0};
  using VectorType = Multivector<ScalarType, P, N, Z>;
  using BivectorNetType = typename BivectorNetTypes<P, N, Z>::type;

  BivectorNetType bvec1{create<BivectorNetType>({1, 2})};
  BivectorNetType bvec2{32 * bvec1};
  VectorType v1{to_multivector<BivectorNetType, VectorType>(bvec1)};
  VectorType v2{to_multivector<BivectorNetType, VectorType>(bvec2)};
  VectorType result{v1.regress(v2)};
  VectorType expected{to_multivector<BivectorNetType, VectorType>(bvec1 & bvec2)};
  EXPECT_EQ(expected, result);
}

TEST(R110BivectorNetDirectTest, RegressiveProductMatches) {
  using ScalarType = float;
  static constexpr size_t P{1};
  static constexpr size_t N{1};
  static constexpr size_t Z{0};
  using VectorType = Multivector<ScalarType, P, N, Z>;
  using BivectorNetType = typename BivectorNetTypes<P, N, Z>::type;

  BivectorNetType bvec1{create<BivectorNetType>({1, 2, 3, 4})};
  BivectorNetType bvec2{32 * bvec1};
  VectorType v1{to_multivector<BivectorNetType, VectorType>(bvec1)};
  VectorType v2{to_multivector<BivectorNetType, VectorType>(bvec2)};
  VectorType result{v1.regress(v2)};
  VectorType expected{to_multivector<BivectorNetType, VectorType>(bvec1 & bvec2)};
  EXPECT_EQ(expected, result);
}

TEST(R101BivectorNetDirectTest, RegressiveProductMatches) {
  using ScalarType = float;
  static constexpr size_t P{1};
  static constexpr size_t N{0};
  static constexpr size_t Z{1};
  using VectorType = Multivector<ScalarType, P, N, Z>;
  using BivectorNetType = typename BivectorNetTypes<P, N, Z>::type;

  BivectorNetType bvec1{create<BivectorNetType>({1, 2, 3, 4})};
  BivectorNetType bvec2{32 * bvec1};
  VectorType v1{to_multivector<BivectorNetType, VectorType>(bvec1)};
  VectorType v2{to_multivector<BivectorNetType, VectorType>(bvec2)};
  VectorType result{v1.regress(v2)};
  VectorType expected{to_multivector<BivectorNetType, VectorType>(bvec1 & bvec2)};
  EXPECT_EQ(expected, result);
}

TEST(R111BivectorNetDirectTest, RegressiveProductMatches) {
  using ScalarType = float;
  static constexpr size_t P{1};
  static constexpr size_t N{1};
  static constexpr size_t Z{1};
  using VectorType = Multivector<ScalarType, P, N, Z>;
  using BivectorNetType = typename BivectorNetTypes<P, N, Z>::type;

  BivectorNetType bvec1{create<BivectorNetType>({1, 2, 3, 4, 5, 6, 7, 8})};
  BivectorNetType bvec2{32 * bvec1};
  VectorType v1{to_multivector<BivectorNetType, VectorType>(bvec1)};
  VectorType v2{to_multivector<BivectorNetType, VectorType>(bvec2)};
  VectorType result{v1.regress(v2)};
  VectorType expected{to_multivector<BivectorNetType, VectorType>(bvec1 & bvec2)};
  EXPECT_EQ(expected, result);
}

TEST(R130BivectorNetDirectTest, RegressiveProductMatches) {
  using ScalarType = float;
  static constexpr size_t P{1};
  static constexpr size_t N{3};
  static constexpr size_t Z{0};
  using VectorType = Multivector<ScalarType, P, N, Z>;
  using BivectorNetType = typename BivectorNetTypes<P, N, Z>::type;

  BivectorNetType bvec1{
      create<BivectorNetType>({1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16})};
  BivectorNetType bvec2{32 * bvec1};
  VectorType v1{to_multivector<BivectorNetType, VectorType>(bvec1)};
  VectorType v2{to_multivector<BivectorNetType, VectorType>(bvec2)};
  VectorType result{v1.regress(v2)};
  VectorType expected{to_multivector<BivectorNetType, VectorType>(bvec1 & bvec2)};
  EXPECT_EQ(expected, result);
}

TEST(R200BivectorNetDirectTest, RegressiveProductMatches) {
  using ScalarType = float;
  static constexpr size_t P{2};
  static constexpr size_t N{0};
  static constexpr size_t Z{0};
  using VectorType = Multivector<ScalarType, P, N, Z>;
  using BivectorNetType = typename BivectorNetTypes<P, N, Z>::type;

  BivectorNetType bvec1{create<BivectorNetType>({1, 2, 3, 4})};
  BivectorNetType bvec2{32 * bvec1};
  VectorType v1{to_multivector<BivectorNetType, VectorType>(bvec1)};
  VectorType v2{to_multivector<BivectorNetType, VectorType>(bvec2)};
  VectorType result{v1.regress(v2)};
  VectorType expected{to_multivector<BivectorNetType, VectorType>(bvec1 & bvec2)};
  EXPECT_EQ(expected, result);
}

TEST(R201BivectorNetDirectTest, RegressiveProductMatches) {
  using ScalarType = float;
  static constexpr size_t P{2};
  static constexpr size_t N{0};
  static constexpr size_t Z{1};
  using VectorType = Multivector<ScalarType, P, N, Z>;
  using BivectorNetType = typename BivectorNetTypes<P, N, Z>::type;

  BivectorNetType bvec1{create<BivectorNetType>({1, 2, 3, 4, 5, 6, 7, 8})};
  BivectorNetType bvec2{32 * bvec1};
  VectorType v1{to_multivector<BivectorNetType, VectorType>(bvec1)};
  VectorType v2{to_multivector<BivectorNetType, VectorType>(bvec2)};
  VectorType result{v1.regress(v2)};
  VectorType expected{to_multivector<BivectorNetType, VectorType>(bvec1 & bvec2)};
  EXPECT_EQ(expected, result);
}

TEST(R210BivectorNetDirectTest, RegressiveProductMatches) {
  using ScalarType = float;
  static constexpr size_t P{2};
  static constexpr size_t N{1};
  static constexpr size_t Z{0};
  using VectorType = Multivector<ScalarType, P, N, Z>;
  using BivectorNetType = typename BivectorNetTypes<P, N, Z>::type;

  BivectorNetType bvec1{create<BivectorNetType>({1, 2, 3, 4, 5, 6, 7, 8})};
  BivectorNetType bvec2{32 * bvec1};
  VectorType v1{to_multivector<BivectorNetType, VectorType>(bvec1)};
  VectorType v2{to_multivector<BivectorNetType, VectorType>(bvec2)};
  VectorType result{v1.regress(v2)};
  VectorType expected{to_multivector<BivectorNetType, VectorType>(bvec1 & bvec2)};
  EXPECT_EQ(expected, result);
}

TEST(R300BivectorNetDirectTest, RegressiveProductMatches) {
  using ScalarType = float;
  static constexpr size_t P{3};
  static constexpr size_t N{0};
  static constexpr size_t Z{0};
  using VectorType = Multivector<ScalarType, P, N, Z>;
  using BivectorNetType = typename BivectorNetTypes<P, N, Z>::type;

  BivectorNetType bvec1{create<BivectorNetType>({1, 2, 3, 4, 5, 6, 7, 8})};
  BivectorNetType bvec2{32 * bvec1};
  VectorType v1{to_multivector<BivectorNetType, VectorType>(bvec1)};
  VectorType v2{to_multivector<BivectorNetType, VectorType>(bvec2)};
  VectorType result{v1.regress(v2)};
  VectorType expected{to_multivector<BivectorNetType, VectorType>(bvec1 & bvec2)};
  EXPECT_EQ(expected, result);
}

TEST(R301BivectorNetDirectTest, RegressiveProductMatches) {
  using ScalarType = float;
  static constexpr size_t P{3};
  static constexpr size_t N{0};
  static constexpr size_t Z{1};
  using VectorType = Multivector<ScalarType, P, N, Z>;
  using BivectorNetType = typename BivectorNetTypes<P, N, Z>::type;

  BivectorNetType bvec1{
      create<BivectorNetType>({1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16})};
  BivectorNetType bvec2{32 * bvec1};
  VectorType v1{to_multivector<BivectorNetType, VectorType>(bvec1)};
  VectorType v2{to_multivector<BivectorNetType, VectorType>(bvec2)};
  VectorType result{v1.regress(v2)};
  VectorType expected{to_multivector<BivectorNetType, VectorType>(bvec1 & bvec2)};
  // VectorType expected{to_multivector<BivectorNetType, VectorType>(!((!bvec1) ^ (!bvec2)))};

  LOG(INFO) << "bvec1: " << bvec1;
  LOG(INFO) << "v1: " << v1;

  LOG(INFO) << "bvec2: " << bvec2;
  LOG(INFO) << "v2: " << v2;

  EXPECT_EQ(expected, result);
}

TEST(R401BivectorNetDirectTest, RegressiveProductMatches) {
  using ScalarType = float;
  static constexpr size_t P{4};
  static constexpr size_t N{0};
  static constexpr size_t Z{1};
  using VectorType = Multivector<ScalarType, P, N, Z>;
  using BivectorNetType = typename BivectorNetTypes<P, N, Z>::type;

  BivectorNetType bvec1{
      create<BivectorNetType>({1,  2,  3,  4,  5,  6,  7,  8,  9,  10, 11, 12, 13, 14, 15, 16,
                               17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32})};
  BivectorNetType bvec2{32 * bvec1};
  VectorType v1{to_multivector<BivectorNetType, VectorType>(bvec1)};
  VectorType v2{to_multivector<BivectorNetType, VectorType>(bvec2)};
  VectorType result{v1.regress(v2)};
  VectorType expected{to_multivector<BivectorNetType, VectorType>(bvec1 & bvec2)};
  EXPECT_EQ(expected, result);
}

TEST(R410BivectorNetDirectTest, RegressiveProductMatches) {
  using ScalarType = float;
  static constexpr size_t P{4};
  static constexpr size_t N{1};
  static constexpr size_t Z{0};
  using VectorType = Multivector<ScalarType, P, N, Z>;
  using BivectorNetType = typename BivectorNetTypes<P, N, Z>::type;

  BivectorNetType bvec1{
      create<BivectorNetType>({1,  2,  3,  4,  5,  6,  7,  8,  9,  10, 11, 12, 13, 14, 15, 16,
                               17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32})};
  BivectorNetType bvec2{32 * bvec1};
  VectorType v1{to_multivector<BivectorNetType, VectorType>(bvec1)};
  VectorType v2{to_multivector<BivectorNetType, VectorType>(bvec2)};
  VectorType result{v1.regress(v2)};
  VectorType expected{to_multivector<BivectorNetType, VectorType>(bvec1 & bvec2)};
  EXPECT_EQ(expected, result);
}

TEST(R000BivectorNetDirectTest, OuterProductMatches) {
  using ScalarType = float;
  static constexpr size_t P{0};
  static constexpr size_t N{0};
  static constexpr size_t Z{0};
  using VectorType = Multivector<ScalarType, P, N, Z>;
  using BivectorNetType = typename BivectorNetTypes<P, N, Z>::type;

  BivectorNetType bvec1{create<BivectorNetType>({1})};
  BivectorNetType bvec2{32 * bvec1};
  VectorType v1{to_multivector<BivectorNetType, VectorType>(bvec1)};
  VectorType v2{to_multivector<BivectorNetType, VectorType>(bvec2)};
  VectorType result{v1.outer(v2)};
  VectorType expected{to_multivector<BivectorNetType, VectorType>(bvec1 ^ bvec2)};
  EXPECT_EQ(expected, result);
}

TEST(R001BivectorNetDirectTest, OuterProductMatches) {
  using ScalarType = float;
  static constexpr size_t P{0};
  static constexpr size_t N{0};
  static constexpr size_t Z{1};
  using VectorType = Multivector<ScalarType, P, N, Z>;
  using BivectorNetType = typename BivectorNetTypes<P, N, Z>::type;

  BivectorNetType bvec1{create<BivectorNetType>({1, 2})};
  BivectorNetType bvec2{32 * bvec1};
  VectorType v1{to_multivector<BivectorNetType, VectorType>(bvec1)};
  VectorType v2{to_multivector<BivectorNetType, VectorType>(bvec2)};
  VectorType result{v1.outer(v2)};
  VectorType expected{to_multivector<BivectorNetType, VectorType>(bvec1 ^ bvec2)};
  EXPECT_EQ(expected, result);
}

TEST(R010BivectorNetDirectTest, OuterProductMatches) {
  using ScalarType = float;
  static constexpr size_t P{0};
  static constexpr size_t N{1};
  static constexpr size_t Z{0};
  using VectorType = Multivector<ScalarType, P, N, Z>;
  using BivectorNetType = typename BivectorNetTypes<P, N, Z>::type;

  BivectorNetType bvec1{create<BivectorNetType>({1, 2})};
  BivectorNetType bvec2{32 * bvec1};
  VectorType v1{to_multivector<BivectorNetType, VectorType>(bvec1)};
  VectorType v2{to_multivector<BivectorNetType, VectorType>(bvec2)};
  VectorType result{v1.outer(v2)};
  VectorType expected{to_multivector<BivectorNetType, VectorType>(bvec1 ^ bvec2)};
  EXPECT_EQ(expected, result);
}

TEST(R011BivectorNetDirectTest, OuterProductMatches) {
  using ScalarType = float;
  static constexpr size_t P{0};
  static constexpr size_t N{1};
  static constexpr size_t Z{1};
  using VectorType = Multivector<ScalarType, P, N, Z>;
  using BivectorNetType = typename BivectorNetTypes<P, N, Z>::type;

  BivectorNetType bvec1{create<BivectorNetType>({1, 2, 3, 4})};
  BivectorNetType bvec2{32 * bvec1};
  VectorType v1{to_multivector<BivectorNetType, VectorType>(bvec1)};
  VectorType v2{to_multivector<BivectorNetType, VectorType>(bvec2)};
  VectorType result{v1.outer(v2)};
  VectorType expected{to_multivector<BivectorNetType, VectorType>(bvec1 ^ bvec2)};
  EXPECT_EQ(expected, result);
}

TEST(R100BivectorNetDirectTest, OuterProductMatches) {
  using ScalarType = float;
  static constexpr size_t P{1};
  static constexpr size_t N{0};
  static constexpr size_t Z{0};
  using VectorType = Multivector<ScalarType, P, N, Z>;
  using BivectorNetType = typename BivectorNetTypes<P, N, Z>::type;

  BivectorNetType bvec1{create<BivectorNetType>({1, 2})};
  BivectorNetType bvec2{32 * bvec1};
  VectorType v1{to_multivector<BivectorNetType, VectorType>(bvec1)};
  VectorType v2{to_multivector<BivectorNetType, VectorType>(bvec2)};
  VectorType result{v1.outer(v2)};
  VectorType expected{to_multivector<BivectorNetType, VectorType>(bvec1 ^ bvec2)};
  EXPECT_EQ(expected, result);
}

TEST(R110BivectorNetDirectTest, OuterProductMatches) {
  using ScalarType = float;
  static constexpr size_t P{1};
  static constexpr size_t N{1};
  static constexpr size_t Z{0};
  using VectorType = Multivector<ScalarType, P, N, Z>;
  using BivectorNetType = typename BivectorNetTypes<P, N, Z>::type;

  BivectorNetType bvec1{create<BivectorNetType>({1, 2, 3, 4})};
  BivectorNetType bvec2{32 * bvec1};
  VectorType v1{to_multivector<BivectorNetType, VectorType>(bvec1)};
  VectorType v2{to_multivector<BivectorNetType, VectorType>(bvec2)};
  VectorType result{v1.outer(v2)};
  VectorType expected{to_multivector<BivectorNetType, VectorType>(bvec1 ^ bvec2)};
  EXPECT_EQ(expected, result);
}

TEST(R101BivectorNetDirectTest, OuterProductMatches) {
  using ScalarType = float;
  static constexpr size_t P{1};
  static constexpr size_t N{0};
  static constexpr size_t Z{1};
  using VectorType = Multivector<ScalarType, P, N, Z>;
  using BivectorNetType = typename BivectorNetTypes<P, N, Z>::type;

  BivectorNetType bvec1{create<BivectorNetType>({1, 2, 3, 4})};
  BivectorNetType bvec2{32 * bvec1};
  VectorType v1{to_multivector<BivectorNetType, VectorType>(bvec1)};
  VectorType v2{to_multivector<BivectorNetType, VectorType>(bvec2)};
  VectorType result{v1.outer(v2)};
  VectorType expected{to_multivector<BivectorNetType, VectorType>(bvec1 ^ bvec2)};
  EXPECT_EQ(expected, result);
}

TEST(R111BivectorNetDirectTest, OuterProductMatches) {
  using ScalarType = float;
  static constexpr size_t P{1};
  static constexpr size_t N{1};
  static constexpr size_t Z{1};
  using VectorType = Multivector<ScalarType, P, N, Z>;
  using BivectorNetType = typename BivectorNetTypes<P, N, Z>::type;

  BivectorNetType bvec1{create<BivectorNetType>({1, 2, 3, 4, 5, 6, 7, 8})};
  BivectorNetType bvec2{32 * bvec1};
  VectorType v1{to_multivector<BivectorNetType, VectorType>(bvec1)};
  VectorType v2{to_multivector<BivectorNetType, VectorType>(bvec2)};
  VectorType result{v1.outer(v2)};
  VectorType expected{to_multivector<BivectorNetType, VectorType>(bvec1 ^ bvec2)};
  EXPECT_EQ(expected, result);
}

TEST(R130BivectorNetDirectTest, OuterProductMatches) {
  using ScalarType = float;
  static constexpr size_t P{1};
  static constexpr size_t N{3};
  static constexpr size_t Z{0};
  using VectorType = Multivector<ScalarType, P, N, Z>;
  using BivectorNetType = typename BivectorNetTypes<P, N, Z>::type;

  BivectorNetType bvec1{
      create<BivectorNetType>({1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16})};
  BivectorNetType bvec2{32 * bvec1};
  VectorType v1{to_multivector<BivectorNetType, VectorType>(bvec1)};
  VectorType v2{to_multivector<BivectorNetType, VectorType>(bvec2)};
  VectorType result{v1.outer(v2)};
  VectorType expected{to_multivector<BivectorNetType, VectorType>(bvec1 ^ bvec2)};
  EXPECT_EQ(expected, result);
}

TEST(R200BivectorNetDirectTest, OuterProductMatches) {
  using ScalarType = float;
  static constexpr size_t P{2};
  static constexpr size_t N{0};
  static constexpr size_t Z{0};
  using VectorType = Multivector<ScalarType, P, N, Z>;
  using BivectorNetType = typename BivectorNetTypes<P, N, Z>::type;

  BivectorNetType bvec1{create<BivectorNetType>({1, 2, 3, 4})};
  BivectorNetType bvec2{32 * bvec1};
  VectorType v1{to_multivector<BivectorNetType, VectorType>(bvec1)};
  VectorType v2{to_multivector<BivectorNetType, VectorType>(bvec2)};
  VectorType result{v1.outer(v2)};
  VectorType expected{to_multivector<BivectorNetType, VectorType>(bvec1 ^ bvec2)};
  EXPECT_EQ(expected, result);
}

TEST(R201BivectorNetDirectTest, OuterProductMatches) {
  using ScalarType = float;
  static constexpr size_t P{2};
  static constexpr size_t N{0};
  static constexpr size_t Z{1};
  using VectorType = Multivector<ScalarType, P, N, Z>;
  using BivectorNetType = typename BivectorNetTypes<P, N, Z>::type;

  BivectorNetType bvec1{create<BivectorNetType>({1, 2, 3, 4, 5, 6, 7, 8})};
  BivectorNetType bvec2{32 * bvec1};
  VectorType v1{to_multivector<BivectorNetType, VectorType>(bvec1)};
  VectorType v2{to_multivector<BivectorNetType, VectorType>(bvec2)};
  VectorType result{v1.outer(v2)};
  VectorType expected{to_multivector<BivectorNetType, VectorType>(bvec1 ^ bvec2)};
  EXPECT_EQ(expected, result);
}

TEST(R210BivectorNetDirectTest, OuterProductMatches) {
  using ScalarType = float;
  static constexpr size_t P{2};
  static constexpr size_t N{1};
  static constexpr size_t Z{0};
  using VectorType = Multivector<ScalarType, P, N, Z>;
  using BivectorNetType = typename BivectorNetTypes<P, N, Z>::type;

  BivectorNetType bvec1{create<BivectorNetType>({1, 2, 3, 4, 5, 6, 7, 8})};
  BivectorNetType bvec2{32 * bvec1};
  VectorType v1{to_multivector<BivectorNetType, VectorType>(bvec1)};
  VectorType v2{to_multivector<BivectorNetType, VectorType>(bvec2)};
  VectorType result{v1.outer(v2)};
  VectorType expected{to_multivector<BivectorNetType, VectorType>(bvec1 ^ bvec2)};
  EXPECT_EQ(expected, result);
}

TEST(R300BivectorNetDirectTest, OuterProductMatches) {
  using ScalarType = float;
  static constexpr size_t P{3};
  static constexpr size_t N{0};
  static constexpr size_t Z{0};
  using VectorType = Multivector<ScalarType, P, N, Z>;
  using BivectorNetType = typename BivectorNetTypes<P, N, Z>::type;

  BivectorNetType bvec1{create<BivectorNetType>({1, 2, 3, 4, 5, 6, 7, 8})};
  BivectorNetType bvec2{32 * bvec1};
  VectorType v1{to_multivector<BivectorNetType, VectorType>(bvec1)};
  VectorType v2{to_multivector<BivectorNetType, VectorType>(bvec2)};
  VectorType result{v1.outer(v2)};
  VectorType expected{to_multivector<BivectorNetType, VectorType>(bvec1 ^ bvec2)};
  EXPECT_EQ(expected, result);
}

TEST(R301BivectorNetDirectTest, OuterProductMatches) {
  using ScalarType = float;
  static constexpr size_t P{3};
  static constexpr size_t N{0};
  static constexpr size_t Z{1};
  using VectorType = Multivector<ScalarType, P, N, Z>;
  using BivectorNetType = typename BivectorNetTypes<P, N, Z>::type;

  BivectorNetType bvec1{
      create<BivectorNetType>({1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16})};
  BivectorNetType bvec2{32 * bvec1};
  VectorType v1{to_multivector<BivectorNetType, VectorType>(bvec1)};
  VectorType v2{to_multivector<BivectorNetType, VectorType>(bvec2)};
  VectorType result{v1.outer(v2)};
  VectorType expected{to_multivector<BivectorNetType, VectorType>(bvec1 ^ bvec2)};
  EXPECT_EQ(expected, result);
}

TEST(R301BivectorNetDirectTest, OuterProductMatchesPrimes) {
  using ScalarType = float;
  static constexpr size_t P{3};
  static constexpr size_t N{0};
  static constexpr size_t Z{1};
  using VectorType = Multivector<ScalarType, P, N, Z>;
  using BivectorNetType = typename BivectorNetTypes<P, N, Z>::type;

  BivectorNetType bvec1{create<BivectorNetType>({
      2, 3, 5, 7, 11, 13, 17, 19,     //
      23, 29, 31, 37, 41, 43, 47, 53  //
  })};
  BivectorNetType bvec2{32 * bvec1};
  VectorType v1{to_multivector<BivectorNetType, VectorType>(bvec1)};
  VectorType v2{to_multivector<BivectorNetType, VectorType>(bvec2)};
  VectorType result{v1.outer(v2)};
  VectorType expected{to_multivector<BivectorNetType, VectorType>(bvec1 ^ bvec2)};
  EXPECT_EQ(expected, result);
}

TEST(R301BivectorNetDirectTest, OuterProductMatchesPrimesNegated) {
  using ScalarType = float;
  static constexpr size_t P{3};
  static constexpr size_t N{0};
  static constexpr size_t Z{1};
  using VectorType = Multivector<ScalarType, P, N, Z>;
  using BivectorNetType = typename BivectorNetTypes<P, N, Z>::type;

  BivectorNetType bvec1{create<BivectorNetType>({
      -2, -3, -5, -7, -11, -13, -17, -19,     //
      -23, -29, -31, -37, -41, -43, -47, -53  //
  })};
  BivectorNetType bvec2{32 * bvec1};
  VectorType v1{to_multivector<BivectorNetType, VectorType>(bvec1)};
  VectorType v2{to_multivector<BivectorNetType, VectorType>(bvec2)};
  VectorType result{v1.outer(v2)};
  VectorType expected{to_multivector<BivectorNetType, VectorType>(bvec1 ^ bvec2)};
  EXPECT_EQ(expected, result);
}

TEST(R301BivectorNetDirectTest, OuterProductMatchesPrimesNegatedAlternately) {
  using ScalarType = float;
  static constexpr size_t P{3};
  static constexpr size_t N{0};
  static constexpr size_t Z{1};
  using VectorType = Multivector<ScalarType, P, N, Z>;
  using BivectorNetType = typename BivectorNetTypes<P, N, Z>::type;

  BivectorNetType bvec1{create<BivectorNetType>({
      -2, 3, -5, 7, -11, 13, 17, -19,     //
      -23, 29, -31, 37, -41, 43, -47, 53  //
  })};
  BivectorNetType bvec2{32 * bvec1};
  VectorType v1{to_multivector<BivectorNetType, VectorType>(bvec1)};
  VectorType v2{to_multivector<BivectorNetType, VectorType>(bvec2)};
  VectorType result{v1.outer(v2)};
  VectorType expected{to_multivector<BivectorNetType, VectorType>(bvec1 ^ bvec2)};
  EXPECT_EQ(expected, result);
}

TEST(R301BivectorNetDirectTest, OuterProductMatchesDuals) {
  using ScalarType = float;
  static constexpr size_t P{3};
  static constexpr size_t N{0};
  static constexpr size_t Z{1};
  using VectorType = Multivector<ScalarType, P, N, Z>;
  using BivectorNetType = typename BivectorNetTypes<P, N, Z>::type;

  BivectorNetType bvec1{
      create<BivectorNetType>({1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16})};
  BivectorNetType bvec2{32 * bvec1};
  VectorType v1{to_multivector<BivectorNetType, VectorType>(bvec1)};
  VectorType v2{to_multivector<BivectorNetType, VectorType>(bvec2)};
  VectorType result{v1.dual().outer(v2.dual())};
  VectorType expected{to_multivector<BivectorNetType, VectorType>((!bvec1) ^ (!bvec2))};
  EXPECT_EQ(expected, result);
}

TEST(R301BivectorNetDirectTest, OuterProductMatchesPrimesDuals) {
  using ScalarType = float;
  static constexpr size_t P{3};
  static constexpr size_t N{0};
  static constexpr size_t Z{1};
  using VectorType = Multivector<ScalarType, P, N, Z>;
  using BivectorNetType = typename BivectorNetTypes<P, N, Z>::type;

  BivectorNetType bvec1{create<BivectorNetType>({
      2, 3, 5, 7, 11, 13, 17, 19,     //
      23, 29, 31, 37, 41, 43, 47, 53  //
  })};
  BivectorNetType bvec2{32 * bvec1};
  VectorType v1{to_multivector<BivectorNetType, VectorType>(bvec1)};
  VectorType v2{to_multivector<BivectorNetType, VectorType>(bvec2)};
  VectorType result{v1.dual().outer(v2.dual())};
  VectorType expected{to_multivector<BivectorNetType, VectorType>((!bvec1) ^ (!bvec2))};
  EXPECT_EQ(expected, result);
}

TEST(R301BivectorNetDirectTest, OuterProductMatchesPrimesNegatedDuals) {
  using ScalarType = float;
  static constexpr size_t P{3};
  static constexpr size_t N{0};
  static constexpr size_t Z{1};
  using VectorType = Multivector<ScalarType, P, N, Z>;
  using BivectorNetType = typename BivectorNetTypes<P, N, Z>::type;

  BivectorNetType bvec1{create<BivectorNetType>({
      -2, -3, -5, -7, -11, -13, -17, -19,     //
      -23, -29, -31, -37, -41, -43, -47, -53  //
  })};
  BivectorNetType bvec2{32 * bvec1};
  VectorType v1{to_multivector<BivectorNetType, VectorType>(bvec1)};
  VectorType v2{to_multivector<BivectorNetType, VectorType>(bvec2)};
  VectorType result{v1.dual().outer(v2.dual())};
  VectorType expected{to_multivector<BivectorNetType, VectorType>((!bvec1) ^ (!bvec2))};
  EXPECT_EQ(expected, result);
}

TEST(R301BivectorNetDirectTest, OuterProductMatchesPrimesNegatedAlternatelyDuals) {
  using ScalarType = float;
  static constexpr size_t P{3};
  static constexpr size_t N{0};
  static constexpr size_t Z{1};
  using VectorType = Multivector<ScalarType, P, N, Z>;
  using BivectorNetType = typename BivectorNetTypes<P, N, Z>::type;

  BivectorNetType bvec1{create<BivectorNetType>({
      -2, 3, -5, 7, -11, 13, 17, -19,     //
      -23, 29, -31, 37, -41, 43, -47, 53  //
  })};
  BivectorNetType bvec2{32 * bvec1};
  VectorType v1{to_multivector<BivectorNetType, VectorType>(bvec1)};
  VectorType v2{to_multivector<BivectorNetType, VectorType>(bvec2)};
  VectorType result{v1.dual().outer(v2.dual())};
  VectorType expected{to_multivector<BivectorNetType, VectorType>((!bvec1) ^ (!bvec2))};
  EXPECT_EQ(expected, result);
}

TEST(R301BivectorNetDirectTest, OuterProductMatchesDualsDualed) {
  using ScalarType = float;
  static constexpr size_t P{3};
  static constexpr size_t N{0};
  static constexpr size_t Z{1};
  using VectorType = Multivector<ScalarType, P, N, Z>;
  using BivectorNetType = typename BivectorNetTypes<P, N, Z>::type;

  BivectorNetType bvec1{
      create<BivectorNetType>({1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16})};
  BivectorNetType bvec2{32 * bvec1};
  VectorType v1{to_multivector<BivectorNetType, VectorType>(bvec1)};
  VectorType v2{to_multivector<BivectorNetType, VectorType>(bvec2)};
  VectorType result{v1.dual().outer(v2.dual()).dual()};
  VectorType expected{to_multivector<BivectorNetType, VectorType>(!((!bvec1) ^ (!bvec2)))};
  EXPECT_EQ(expected, result);
}

TEST(R301BivectorNetDirectTest, OuterProductMatchesPrimesDualsDualed) {
  using ScalarType = float;
  static constexpr size_t P{3};
  static constexpr size_t N{0};
  static constexpr size_t Z{1};
  using VectorType = Multivector<ScalarType, P, N, Z>;
  using BivectorNetType = typename BivectorNetTypes<P, N, Z>::type;

  BivectorNetType bvec1{create<BivectorNetType>({
      2, 3, 5, 7, 11, 13, 17, 19,     //
      23, 29, 31, 37, 41, 43, 47, 53  //
  })};
  BivectorNetType bvec2{32 * bvec1};
  VectorType v1{to_multivector<BivectorNetType, VectorType>(bvec1)};
  VectorType v2{to_multivector<BivectorNetType, VectorType>(bvec2)};
  VectorType result{v1.dual().outer(v2.dual()).dual()};
  VectorType expected{to_multivector<BivectorNetType, VectorType>(!((!bvec1) ^ (!bvec2)))};
  EXPECT_EQ(expected, result);
}

TEST(R301BivectorNetDirectTest, OuterProductMatchesPrimesNegatedDualsDualed) {
  using ScalarType = float;
  static constexpr size_t P{3};
  static constexpr size_t N{0};
  static constexpr size_t Z{1};
  using VectorType = Multivector<ScalarType, P, N, Z>;
  using BivectorNetType = typename BivectorNetTypes<P, N, Z>::type;

  BivectorNetType bvec1{create<BivectorNetType>({
      -2, -3, -5, -7, -11, -13, -17, -19,     //
      -23, -29, -31, -37, -41, -43, -47, -53  //
  })};
  BivectorNetType bvec2{32 * bvec1};
  VectorType v1{to_multivector<BivectorNetType, VectorType>(bvec1)};
  VectorType v2{to_multivector<BivectorNetType, VectorType>(bvec2)};
  VectorType result{v1.dual().outer(v2.dual()).dual()};
  VectorType expected{to_multivector<BivectorNetType, VectorType>(!((!bvec1) ^ (!bvec2)))};
  EXPECT_EQ(expected, result);
}

TEST(R301BivectorNetDirectTest, OuterProductMatchesPrimesNegatedAlternatelyDualsDualed) {
  using ScalarType = float;
  static constexpr size_t P{3};
  static constexpr size_t N{0};
  static constexpr size_t Z{1};
  using VectorType = Multivector<ScalarType, P, N, Z>;
  using BivectorNetType = typename BivectorNetTypes<P, N, Z>::type;

  BivectorNetType bvec1{create<BivectorNetType>({
      -2, 3, -5, 7, -11, 13, 17, -19,     //
      -23, 29, -31, 37, -41, 43, -47, 53  //
  })};
  BivectorNetType bvec2{32 * bvec1};
  VectorType v1{to_multivector<BivectorNetType, VectorType>(bvec1)};
  VectorType v2{to_multivector<BivectorNetType, VectorType>(bvec2)};
  VectorType result{v1.dual().outer(v2.dual()).dual()};
  VectorType expected{to_multivector<BivectorNetType, VectorType>(!((!bvec1) ^ (!bvec2)))};
  EXPECT_EQ(expected, result);
}

TEST(R401BivectorNetDirectTest, OuterProductMatches) {
  using ScalarType = float;
  static constexpr size_t P{4};
  static constexpr size_t N{0};
  static constexpr size_t Z{1};
  using VectorType = Multivector<ScalarType, P, N, Z>;
  using BivectorNetType = typename BivectorNetTypes<P, N, Z>::type;

  BivectorNetType bvec1{
      create<BivectorNetType>({1,  2,  3,  4,  5,  6,  7,  8,  9,  10, 11, 12, 13, 14, 15, 16,
                               17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32})};
  BivectorNetType bvec2{32 * bvec1};
  VectorType v1{to_multivector<BivectorNetType, VectorType>(bvec1)};
  VectorType v2{to_multivector<BivectorNetType, VectorType>(bvec2)};
  VectorType result{v1.outer(v2)};
  VectorType expected{to_multivector<BivectorNetType, VectorType>(bvec1 ^ bvec2)};
  EXPECT_EQ(expected, result);
}

TEST(R201BivectorNetDirectTest, OuterProductMatchesUsingR401) {
  using ScalarType = float;
  static constexpr size_t P{4};
  static constexpr size_t N{0};
  static constexpr size_t Z{1};
  using VectorType = Multivector<ScalarType, P, N, Z>;
  using BivectorNetType = typename BivectorNetTypes<P, N, Z>::type;

  BivectorNetType bvec1{
      create<BivectorNetType>({0, 0, 3, 0, 0, 0, 0, 6, 0, 0, 0, 0, 0, 0, 0, 0, 0})};
  BivectorNetType bvec2{32 * bvec1};
  VectorType v1{to_multivector<BivectorNetType, VectorType>(bvec1)};
  VectorType v2{to_multivector<BivectorNetType, VectorType>(bvec2)};
  VectorType result{v1.outer(v2)};
  VectorType expected{to_multivector<BivectorNetType, VectorType>(bvec1 ^ bvec2)};
  EXPECT_EQ(expected, result);
}

TEST(R201BivectorNetDirectTest, OuterProductMatchesBugSearch) {
  using ScalarType = float;
  static constexpr size_t P{2};
  static constexpr size_t N{0};
  static constexpr size_t Z{1};
  using VectorType = Multivector<ScalarType, P, N, Z>;
  using BivectorNetType = typename BivectorNetTypes<P, N, Z>::type;

  BivectorNetType bvec1{create<BivectorNetType>({0, 0, 3, 0, 0, 6, 0, 0})};
  BivectorNetType bvec2{32 * bvec1};
  VectorType v1{to_multivector<BivectorNetType, VectorType>(bvec1)};
  VectorType v2{to_multivector<BivectorNetType, VectorType>(bvec2)};
  VectorType result{v1.outer(v2)};
  VectorType expected{to_multivector<BivectorNetType, VectorType>(bvec1 ^ bvec2)};
  EXPECT_EQ(expected, result);
}

TEST(BivectorNetTest, OuterProductR201R301R401Consistency) {
  using ScalarType = float;
  static constexpr size_t P2{2};
  static constexpr size_t P3{3};
  static constexpr size_t P4{4};
  static constexpr size_t N{0};
  static constexpr size_t Z{1};

  using BivectorNet201Type = typename BivectorNetTypes<P2, N, Z>::type;
  using BivectorNet301Type = typename BivectorNetTypes<P3, N, Z>::type;
  using BivectorNet401Type = typename BivectorNetTypes<P4, N, Z>::type;

  using Vector201Type = Multivector<ScalarType, P2, N, Z>;
  using Vector301Type = Multivector<ScalarType, P3, N, Z>;
  using Vector401Type = Multivector<ScalarType, P4, N, Z>;

  BivectorNet201Type bvec_201_1{create<BivectorNet201Type>({0, 0, 2, 0, 0, -3})};
  BivectorNet201Type bvec_201_2{5 * bvec_201_1};
  Vector201Type mv_201_1{to_multivector<BivectorNet201Type, Vector201Type>(bvec_201_1)};
  Vector201Type mv_201_2{to_multivector<BivectorNet201Type, Vector201Type>(bvec_201_2)};
  BivectorNet201Type result_201{bvec_201_1 ^ bvec_201_2};
  Vector201Type mv_result_201{to_multivector<BivectorNet201Type, Vector201Type>(result_201)};

  BivectorNet301Type bvec_301_1{create<BivectorNet301Type>({0, 0, 2, 0, 0, 0, 3})};
  BivectorNet301Type bvec_301_2{5 * bvec_301_1};
  Vector301Type mv_301_1{to_multivector<BivectorNet301Type, Vector301Type>(bvec_301_1)};
  Vector301Type mv_301_2{to_multivector<BivectorNet301Type, Vector301Type>(bvec_301_2)};
  BivectorNet301Type result_301{bvec_301_1 ^ bvec_301_2};
  Vector301Type mv_result_301{to_multivector<BivectorNet301Type, Vector301Type>(result_301)};

  BivectorNet401Type bvec_401_1{create<BivectorNet401Type>({0, 0, 2, 0, 0, 0, 0, 3})};
  BivectorNet401Type bvec_401_2{5 * bvec_401_1};
  Vector401Type mv_401_1{to_multivector<BivectorNet401Type, Vector401Type>(bvec_401_1)};
  Vector401Type mv_401_2{to_multivector<BivectorNet401Type, Vector401Type>(bvec_401_2)};
  BivectorNet401Type result_401{bvec_401_1 ^ bvec_401_2};
  Vector401Type mv_result_401{to_multivector<BivectorNet401Type, Vector401Type>(result_401)};
  for (size_t i = 0; i < Vector201Type::bases_count(); ++i) {
    EXPECT_EQ(mv_result_201.basis(i), mv_result_401.basis(i))
        << "i: " << i << ", 201: " << mv_result_201 << ", 401: " << mv_result_401;
  }

  for (size_t i = 0; i < Vector301Type::bases_count(); ++i) {
    EXPECT_EQ(mv_result_301.basis(i), mv_result_401.basis(i))
        << "i: " << i << ", 301: " << mv_result_301 << ", 401: " << mv_result_401;
  }
}

TEST(R410BivectorNetDirectTest, OuterProductMatches) {
  using ScalarType = float;
  static constexpr size_t P{4};
  static constexpr size_t N{1};
  static constexpr size_t Z{0};
  using VectorType = Multivector<ScalarType, P, N, Z>;
  using BivectorNetType = typename BivectorNetTypes<P, N, Z>::type;

  BivectorNetType bvec1{
      create<BivectorNetType>({1,  2,  3,  4,  5,  6,  7,  8,  9,  10, 11, 12, 13, 14, 15, 16,
                               17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32})};
  BivectorNetType bvec2{32 * bvec1};
  VectorType v1{to_multivector<BivectorNetType, VectorType>(bvec1)};
  VectorType v2{to_multivector<BivectorNetType, VectorType>(bvec2)};
  VectorType result{v1.outer(v2)};
  VectorType expected{to_multivector<BivectorNetType, VectorType>(bvec1 ^ bvec2)};
  EXPECT_EQ(expected, result);
}

TEST(BivectorNetDirectTest, R000RegressiveProductConsistentWithOuter) {
  using BivectorNetType = R000;
  BivectorNetType bvec1{create_with_counting<BivectorNetType>()};
  BivectorNetType bvec2{32 * bvec1};
  BivectorNetType regressive{bvec1 & bvec2};
  BivectorNetType via_outer{!((!bvec1) ^ (!bvec2))};
  EXPECT_EQ(via_outer, regressive);
}

TEST(BivectorNetDirectTest, R001RegressiveProductConsistentWithOuter) {
  using BivectorNetType = R001;
  BivectorNetType bvec1{create_with_counting<BivectorNetType>()};
  BivectorNetType bvec2{32 * bvec1};
  BivectorNetType regressive{bvec1 & bvec2};
  BivectorNetType via_outer{!((!bvec1) ^ (!bvec2))};
  EXPECT_EQ(via_outer, regressive);
}

TEST(BivectorNetDirectTest, R010RegressiveProductConsistentWithOuter) {
  using BivectorNetType = R010;
  BivectorNetType bvec1{create_with_counting<BivectorNetType>()};
  BivectorNetType bvec2{32 * bvec1};
  BivectorNetType regressive{bvec1 & bvec2};
  BivectorNetType via_outer{!((!bvec1) ^ (!bvec2))};
  EXPECT_EQ(via_outer, regressive);
}

TEST(BivectorNetDirectTest, R011RegressiveProductConsistentWithOuter) {
  using BivectorNetType = R011;
  BivectorNetType bvec1{create_with_counting<BivectorNetType>()};
  BivectorNetType bvec2{32 * bvec1};
  BivectorNetType regressive{bvec1 & bvec2};
  BivectorNetType via_outer{!((!bvec1) ^ (!bvec2))};
  EXPECT_EQ(via_outer, regressive);
}

TEST(BivectorNetDirectTest, R100RegressiveProductConsistentWithOuter) {
  using BivectorNetType = R100;
  BivectorNetType bvec1{create_with_counting<BivectorNetType>()};
  BivectorNetType bvec2{32 * bvec1};
  BivectorNetType regressive{bvec1 & bvec2};
  BivectorNetType via_outer{!((!bvec1) ^ (!bvec2))};
  EXPECT_EQ(via_outer, regressive);
}

TEST(BivectorNetDirectTest, R110RegressiveProductConsistentWithOuter) {
  using BivectorNetType = R110;
  BivectorNetType bvec1{create_with_counting<BivectorNetType>()};
  BivectorNetType bvec2{32 * bvec1};
  BivectorNetType regressive{bvec1 & bvec2};
  BivectorNetType via_outer{!((!bvec1) ^ (!bvec2))};
  EXPECT_EQ(via_outer, regressive);
}

TEST(BivectorNetDirectTest, R101RegressiveProductConsistentWithOuter) {
  using BivectorNetType = R101;
  BivectorNetType bvec1{create_with_counting<BivectorNetType>()};
  BivectorNetType bvec2{32 * bvec1};
  BivectorNetType regressive{bvec1 & bvec2};
  BivectorNetType via_outer{!((!bvec1) ^ (!bvec2))};
  EXPECT_EQ(via_outer, regressive);
}

TEST(BivectorNetDirectTest, R111RegressiveProductConsistentWithOuter) {
  using BivectorNetType = R111;
  BivectorNetType bvec1{create_with_counting<BivectorNetType>()};
  BivectorNetType bvec2{32 * bvec1};
  BivectorNetType regressive{bvec1 & bvec2};
  BivectorNetType via_outer{!((!bvec1) ^ (!bvec2))};
  EXPECT_EQ(via_outer, regressive);
}

TEST(BivectorNetDirectTest, R130RegressiveProductConsistentWithOuter) {
  using BivectorNetType = R130;
  BivectorNetType bvec1{create_with_counting<BivectorNetType>()};
  BivectorNetType bvec2{32 * bvec1};
  BivectorNetType regressive{bvec1 & bvec2};
  BivectorNetType via_outer{!((!bvec1) ^ (!bvec2))};
  EXPECT_EQ(via_outer, regressive);
}

TEST(BivectorNetDirectTest, R200RegressiveProductConsistentWithOuter) {
  using BivectorNetType = R200;
  BivectorNetType bvec1{create_with_counting<BivectorNetType>()};
  BivectorNetType bvec2{32 * bvec1};
  BivectorNetType regressive{bvec1 & bvec2};
  BivectorNetType via_outer{!((!bvec1) ^ (!bvec2))};
  EXPECT_EQ(via_outer, regressive);
}

TEST(BivectorNetDirectTest, R201RegressiveProductConsistentWithOuter) {
  using BivectorNetType = PGA2D;

  BivectorNetType bvec1{create_with_counting<BivectorNetType>()};
  BivectorNetType bvec2{32 * bvec1};
  BivectorNetType regressive{bvec1 & bvec2};
  BivectorNetType via_outer{!((!bvec1) ^ (!bvec2))};
  EXPECT_EQ(via_outer, regressive);
}

TEST(BivectorNetDirectTest, R210RegressiveProductConsistentWithOuter) {
  using BivectorNetType = R210;

  BivectorNetType bvec1{create_with_counting<BivectorNetType>()};
  BivectorNetType bvec2{32 * bvec1};
  BivectorNetType regressive{bvec1 & bvec2};
  BivectorNetType via_outer{!((!bvec1) ^ (!bvec2))};
  EXPECT_EQ(via_outer, regressive);
}

TEST(BivectorNetDirectTest, R300RegressiveProductConsistentWithOuter) {
  using BivectorNetType = R300;

  BivectorNetType bvec1{create_with_counting<BivectorNetType>()};
  BivectorNetType bvec2{32 * bvec1};
  BivectorNetType regressive{bvec1 & bvec2};
  BivectorNetType via_outer{!((!bvec1) ^ (!bvec2))};
  EXPECT_EQ(via_outer, regressive);
}

TEST(BivectorNetDirectTest, R301RegressiveProductConsistentWithOuter) {
  using BivectorNetType = PGA3D;

  BivectorNetType bvec1{create_with_counting<BivectorNetType>()};
  BivectorNetType bvec2{32 * bvec1};
  BivectorNetType regressive{bvec1 & bvec2};
  BivectorNetType via_outer{!((!bvec1) ^ (!bvec2))};
  EXPECT_EQ(via_outer, regressive);
}

TEST(BivectorNetDirectTest, R401RegressiveProductConsistentWithOuter) {
  using BivectorNetType = R401;

  BivectorNetType bvec1{create_with_counting<BivectorNetType>()};
  BivectorNetType bvec2{32 * bvec1};
  BivectorNetType regressive{bvec1 & bvec2};
  BivectorNetType via_outer{!((!bvec1) ^ (!bvec2))};
  EXPECT_EQ(via_outer, regressive);
}

TEST(BivectorNetDirectTest, R410RegressiveProductConsistentWithOuter) {
  using BivectorNetType = R410;

  BivectorNetType bvec1{create_with_counting<BivectorNetType>()};
  BivectorNetType bvec2{32 * bvec1};
  BivectorNetType regressive{bvec1 & bvec2};
  BivectorNetType via_outer{!((!bvec1) ^ (!bvec2))};
  EXPECT_EQ(via_outer, regressive);
}

TEST(BivectorNetDirectTest, PGA2DRegressiveProductConsistentWithOuter) {
  using BivectorNetType = PGA2D;

  BivectorNetType bvec1{create_with_counting<BivectorNetType>()};
  BivectorNetType bvec2{32 * bvec1};
  BivectorNetType regressive{bvec1 & bvec2};
  BivectorNetType via_outer{!((!bvec1) ^ (!bvec2))};

  LOG(INFO) << "bvec1: " << bvec1;
  LOG(INFO) << "bvec2: " << bvec2;

  LOG(INFO) << "regressive product expression: "
            << "(" << bvec1 << ") & (" << bvec2 << ")";
  LOG(INFO) << "outer product expression: "
            << "!((!(" << bvec1 << ")) ^ (!(" << bvec2 << ")))";

  EXPECT_EQ(via_outer, regressive);
}

TEST(BivectorNetDirectTest, PGA3DRegressiveProductConsistentWithDualOfOuterOfDuals) {
  using BivectorNetType = PGA3D;

  BivectorNetType bvec1{create_with_counting<BivectorNetType>()};
  BivectorNetType bvec2{32 * bvec1};
  BivectorNetType regressive{bvec1 & bvec2};
  BivectorNetType via_outer{!((!bvec1) ^ (!bvec2))};

  LOG(INFO) << "bvec1: " << bvec1;
  LOG(INFO) << "bvec2: " << bvec2;

  LOG(INFO) << "regressive product expression: "
            << "(" << bvec1 << ") & (" << bvec2 << ")";
  LOG(INFO) << "outer product expression: "
            << "!((!(" << bvec1 << ")) ^ (!(" << bvec2 << ")))";

  EXPECT_EQ(via_outer, regressive);
}

TEST(BivectorNetDirectTest, R401RegressiveProductConsistentWithDualOfOuterOfDuals) {
  using BivectorNetType = R401;

  BivectorNetType bvec1{create_with_counting<BivectorNetType>()};
  BivectorNetType bvec2{32 * bvec1};
  BivectorNetType regressive{bvec1 & bvec2};
  BivectorNetType via_outer{!((!bvec1) ^ (!bvec2))};

  LOG(INFO) << "bvec1: " << bvec1;
  LOG(INFO) << "bvec2: " << bvec2;

  LOG(INFO) << "regressive product expression: "
            << "(" << bvec1 << ") & (" << bvec2 << ")";
  LOG(INFO) << "outer product expression: "
            << "!((!(" << bvec1 << ")) ^ (!(" << bvec2 << ")))";

  EXPECT_EQ(via_outer, regressive);
}

}  // namespace ndyn::math
