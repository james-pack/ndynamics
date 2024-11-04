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
BivectorNetType create_from_multivector_coefficients(std::initializer_list<float> values) {
  BivectorNetType result{};
  size_t i{};
  for (float value : values) {
    result[index_from_bit_basis<BivectorNetType>(i)] = value;
    ++i;
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
  using AlgebraType = Algebra<ScalarType, P, N, Z>;
  using VectorType = typename AlgebraType::VectorType;
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
  using AlgebraType = Algebra<ScalarType, P, N, Z>;
  using VectorType = typename AlgebraType::VectorType;
  using BivectorNetType = typename BivectorNetTypes<P, N, Z>::type;

  BivectorNetType bvec{create_with_counting<BivectorNetType>()};
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
  using AlgebraType = Algebra<ScalarType, P, N, Z>;
  using VectorType = typename AlgebraType::VectorType;
  using BivectorNetType = typename BivectorNetTypes<P, N, Z>::type;

  BivectorNetType bvec{create_with_counting<BivectorNetType>()};
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
  using AlgebraType = Algebra<ScalarType, P, N, Z>;
  using VectorType = typename AlgebraType::VectorType;
  using BivectorNetType = typename BivectorNetTypes<P, N, Z>::type;

  BivectorNetType bvec{create_with_counting<BivectorNetType>()};
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
  using AlgebraType = Algebra<ScalarType, P, N, Z>;
  using VectorType = typename AlgebraType::VectorType;
  using BivectorNetType = typename BivectorNetTypes<P, N, Z>::type;

  BivectorNetType bvec{create_with_counting<BivectorNetType>()};
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
  using AlgebraType = Algebra<ScalarType, P, N, Z>;
  using VectorType = typename AlgebraType::VectorType;
  using BivectorNetType = typename BivectorNetTypes<P, N, Z>::type;

  BivectorNetType bvec{create_with_counting<BivectorNetType>()};
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
  using AlgebraType = Algebra<ScalarType, P, N, Z>;
  using VectorType = typename AlgebraType::VectorType;
  using BivectorNetType = typename BivectorNetTypes<P, N, Z>::type;

  BivectorNetType bvec{create_with_counting<BivectorNetType>()};
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
  using AlgebraType = Algebra<ScalarType, P, N, Z>;
  using VectorType = typename AlgebraType::VectorType;
  using BivectorNetType = typename BivectorNetTypes<P, N, Z>::type;

  BivectorNetType bvec{create_with_counting<BivectorNetType>()};
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
  using AlgebraType = Algebra<ScalarType, P, N, Z>;
  using VectorType = typename AlgebraType::VectorType;
  using BivectorNetType = typename BivectorNetTypes<P, N, Z>::type;

  BivectorNetType bvec{create_with_counting<BivectorNetType>()};
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
  using AlgebraType = Algebra<ScalarType, P, N, Z>;
  using VectorType = typename AlgebraType::VectorType;
  using BivectorNetType = typename BivectorNetTypes<P, N, Z>::type;

  BivectorNetType bvec{create_with_counting<BivectorNetType>()};
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
  using AlgebraType = Algebra<ScalarType, P, N, Z>;
  using VectorType = typename AlgebraType::VectorType;
  using BivectorNetType = typename BivectorNetTypes<P, N, Z>::type;

  BivectorNetType bvec{create_with_counting<BivectorNetType>()};
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
  using AlgebraType = Algebra<ScalarType, P, N, Z>;
  using VectorType = typename AlgebraType::VectorType;
  using BivectorNetType = typename BivectorNetTypes<P, N, Z>::type;

  BivectorNetType bvec{create_with_counting<BivectorNetType>()};
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
  using AlgebraType = Algebra<ScalarType, P, N, Z>;
  using VectorType = typename AlgebraType::VectorType;
  using BivectorNetType = typename BivectorNetTypes<P, N, Z>::type;

  BivectorNetType bvec{create_with_counting<BivectorNetType>()};
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
  using AlgebraType = Algebra<ScalarType, P, N, Z>;
  using VectorType = typename AlgebraType::VectorType;
  using BivectorNetType = typename BivectorNetTypes<P, N, Z>::type;

  BivectorNetType bvec{create_with_counting<BivectorNetType>()};
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
  using AlgebraType = Algebra<ScalarType, P, N, Z>;
  using VectorType = typename AlgebraType::VectorType;
  using BivectorNetType = typename BivectorNetTypes<P, N, Z>::type;

  BivectorNetType bvec{create_with_counting<BivectorNetType>()};
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
  using AlgebraType = Algebra<ScalarType, P, N, Z>;
  using VectorType = typename AlgebraType::VectorType;
  using BivectorNetType = typename BivectorNetTypes<P, N, Z>::type;

  BivectorNetType bvec{create_with_counting<BivectorNetType>()};
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
  using AlgebraType = Algebra<ScalarType, P, N, Z>;
  using VectorType = typename AlgebraType::VectorType;
  using BivectorNetType = typename BivectorNetTypes<P, N, Z>::type;

  BivectorNetType bvec{create_with_counting<BivectorNetType>()};
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
  using AlgebraType = Algebra<ScalarType, P, N, Z>;
  using VectorType = typename AlgebraType::VectorType;
  using BivectorNetType = typename BivectorNetTypes<P, N, Z>::type;

  BivectorNetType bvec{create_with_counting<BivectorNetType>()};
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
  using AlgebraType = Algebra<ScalarType, P, N, Z>;
  using VectorType = typename AlgebraType::VectorType;
  using BivectorNetType = typename BivectorNetTypes<P, N, Z>::type;

  BivectorNetType bvec{create_with_counting<BivectorNetType>()};
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
  using AlgebraType = Algebra<ScalarType, P, N, Z>;
  using VectorType = typename AlgebraType::VectorType;
  using BivectorNetType = typename BivectorNetTypes<P, N, Z>::type;

  BivectorNetType bvec{create_with_counting<BivectorNetType>()};
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
  using AlgebraType = Algebra<ScalarType, P, N, Z>;
  using VectorType = typename AlgebraType::VectorType;
  using BivectorNetType = typename BivectorNetTypes<P, N, Z>::type;

  BivectorNetType bvec{create_with_counting<BivectorNetType>()};
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
  using AlgebraType = Algebra<ScalarType, P, N, Z>;
  using VectorType = typename AlgebraType::VectorType;
  using BivectorNetType = typename BivectorNetTypes<P, N, Z>::type;

  BivectorNetType bvec{create_with_counting<BivectorNetType>()};
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
  using AlgebraType = Algebra<ScalarType, P, N, Z>;
  using VectorType = typename AlgebraType::VectorType;
  using BivectorNetType = typename BivectorNetTypes<P, N, Z>::type;

  BivectorNetType bvec{create_with_counting<BivectorNetType>()};
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
  using AlgebraType = Algebra<ScalarType, P, N, Z>;
  using VectorType = typename AlgebraType::VectorType;
  using BivectorNetType = typename BivectorNetTypes<P, N, Z>::type;

  BivectorNetType bvec{create_with_counting<BivectorNetType>()};
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
  using AlgebraType = Algebra<ScalarType, P, N, Z>;
  using VectorType = typename AlgebraType::VectorType;
  using BivectorNetType = typename BivectorNetTypes<P, N, Z>::type;

  BivectorNetType bvec{create_with_counting<BivectorNetType>()};
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
  using AlgebraType = Algebra<ScalarType, P, N, Z>;
  using VectorType = typename AlgebraType::VectorType;
  using BivectorNetType = typename BivectorNetTypes<P, N, Z>::type;

  BivectorNetType bvec{create_with_counting<BivectorNetType>()};
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
  using AlgebraType = Algebra<ScalarType, P, N, Z>;
  using VectorType = typename AlgebraType::VectorType;
  using BivectorNetType = typename BivectorNetTypes<P, N, Z>::type;

  BivectorNetType bvec{create_with_counting<BivectorNetType>()};
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
  using AlgebraType = Algebra<ScalarType, P, N, Z>;
  using VectorType = typename AlgebraType::VectorType;
  using BivectorNetType = typename BivectorNetTypes<P, N, Z>::type;

  BivectorNetType bvec{create_with_counting<BivectorNetType>()};
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
  using AlgebraType = Algebra<ScalarType, P, N, Z>;
  using VectorType = typename AlgebraType::VectorType;
  using BivectorNetType = typename BivectorNetTypes<P, N, Z>::type;

  BivectorNetType bvec{create_with_counting<BivectorNetType>()};
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
  using AlgebraType = Algebra<ScalarType, P, N, Z>;
  using VectorType = typename AlgebraType::VectorType;
  using BivectorNetType = typename BivectorNetTypes<P, N, Z>::type;

  BivectorNetType bvec{create_with_counting<BivectorNetType>()};
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
  using AlgebraType = Algebra<ScalarType, P, N, Z>;
  using VectorType = typename AlgebraType::VectorType;
  using BivectorNetType = typename BivectorNetTypes<P, N, Z>::type;

  BivectorNetType bvec{create_with_counting<BivectorNetType>()};
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
  using AlgebraType = Algebra<ScalarType, P, N, Z>;
  using VectorType = typename AlgebraType::VectorType;
  using BivectorNetType = typename BivectorNetTypes<P, N, Z>::type;

  BivectorNetType bvec{create_with_counting<BivectorNetType>()};
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
  using AlgebraType = Algebra<ScalarType, P, N, Z>;
  using VectorType = typename AlgebraType::VectorType;
  using BivectorNetType = typename BivectorNetTypes<P, N, Z>::type;

  BivectorNetType bvec{create_with_counting<BivectorNetType>()};
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
  using AlgebraType = Algebra<ScalarType, P, N, Z>;
  using VectorType = typename AlgebraType::VectorType;
  using BivectorNetType = typename BivectorNetTypes<P, N, Z>::type;

  BivectorNetType bvec{create_with_counting<BivectorNetType>()};
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
  using AlgebraType = Algebra<ScalarType, P, N, Z>;
  using VectorType = typename AlgebraType::VectorType;
  using BivectorNetType = typename BivectorNetTypes<P, N, Z>::type;

  BivectorNetType bvec{create_with_counting<BivectorNetType>()};
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
  using AlgebraType = Algebra<ScalarType, P, N, Z>;
  using VectorType = typename AlgebraType::VectorType;
  using BivectorNetType = typename BivectorNetTypes<P, N, Z>::type;

  BivectorNetType bvec{create_with_counting<BivectorNetType>()};
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
  using AlgebraType = Algebra<ScalarType, P, N, Z>;
  using VectorType = typename AlgebraType::VectorType;
  using BivectorNetType = typename BivectorNetTypes<P, N, Z>::type;

  BivectorNetType bvec{create_with_counting<BivectorNetType>()};
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
  using AlgebraType = Algebra<ScalarType, P, N, Z>;
  using VectorType = typename AlgebraType::VectorType;
  using BivectorNetType = typename BivectorNetTypes<P, N, Z>::type;

  BivectorNetType bvec{create_with_counting<BivectorNetType>()};
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
  using AlgebraType = Algebra<ScalarType, P, N, Z>;
  using VectorType = typename AlgebraType::VectorType;
  using BivectorNetType = typename BivectorNetTypes<P, N, Z>::type;

  BivectorNetType bvec{create_with_counting<BivectorNetType>()};
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
  using AlgebraType = Algebra<ScalarType, P, N, Z>;
  using VectorType = typename AlgebraType::VectorType;
  using BivectorNetType = typename BivectorNetTypes<P, N, Z>::type;

  BivectorNetType bvec{create_with_counting<BivectorNetType>()};
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
  using AlgebraType = Algebra<ScalarType, P, N, Z>;
  using VectorType = typename AlgebraType::VectorType;
  using BivectorNetType = typename BivectorNetTypes<P, N, Z>::type;

  BivectorNetType bvec{create_with_counting<BivectorNetType>()};
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
  using AlgebraType = Algebra<ScalarType, P, N, Z>;
  using VectorType = typename AlgebraType::VectorType;
  using BivectorNetType = typename BivectorNetTypes<P, N, Z>::type;

  BivectorNetType bvec{create_with_counting<BivectorNetType>()};
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
  using AlgebraType = Algebra<ScalarType, P, N, Z>;
  using VectorType = typename AlgebraType::VectorType;
  using BivectorNetType = typename BivectorNetTypes<P, N, Z>::type;

  BivectorNetType bvec{create_with_counting<BivectorNetType>()};
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
  using AlgebraType = Algebra<ScalarType, P, N, Z>;
  using VectorType = typename AlgebraType::VectorType;
  using BivectorNetType = typename BivectorNetTypes<P, N, Z>::type;

  BivectorNetType bvec{create_with_counting<BivectorNetType>()};
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
  using AlgebraType = Algebra<ScalarType, P, N, Z>;
  using VectorType = typename AlgebraType::VectorType;
  using BivectorNetType = typename BivectorNetTypes<P, N, Z>::type;

  BivectorNetType bvec{create_with_counting<BivectorNetType>()};
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
  using AlgebraType = Algebra<ScalarType, P, N, Z>;
  using VectorType = typename AlgebraType::VectorType;
  using BivectorNetType = typename BivectorNetTypes<P, N, Z>::type;

  BivectorNetType bvec{create_with_counting<BivectorNetType>()};
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
  using AlgebraType = Algebra<ScalarType, P, N, Z>;
  using VectorType = typename AlgebraType::VectorType;
  using BivectorNetType = typename BivectorNetTypes<P, N, Z>::type;

  BivectorNetType bvec{create_with_counting<BivectorNetType>()};
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
  using AlgebraType = Algebra<ScalarType, P, N, Z>;
  using VectorType = typename AlgebraType::VectorType;
  using BivectorNetType = typename BivectorNetTypes<P, N, Z>::type;

  BivectorNetType bvec{create_with_counting<BivectorNetType>()};
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
  using AlgebraType = Algebra<ScalarType, P, N, Z>;
  using VectorType = typename AlgebraType::VectorType;
  using BivectorNetType = typename BivectorNetTypes<P, N, Z>::type;

  BivectorNetType bvec1{create_with_counting<BivectorNetType>()};
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
  using AlgebraType = Algebra<ScalarType, P, N, Z>;
  using VectorType = typename AlgebraType::VectorType;
  using BivectorNetType = typename BivectorNetTypes<P, N, Z>::type;

  BivectorNetType bvec1{create_with_counting<BivectorNetType>()};
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
  using AlgebraType = Algebra<ScalarType, P, N, Z>;
  using VectorType = typename AlgebraType::VectorType;
  using BivectorNetType = typename BivectorNetTypes<P, N, Z>::type;

  BivectorNetType bvec1{create_with_counting<BivectorNetType>()};
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
  using AlgebraType = Algebra<ScalarType, P, N, Z>;
  using VectorType = typename AlgebraType::VectorType;
  using BivectorNetType = typename BivectorNetTypes<P, N, Z>::type;

  BivectorNetType bvec1{create_with_counting<BivectorNetType>()};
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
  using AlgebraType = Algebra<ScalarType, P, N, Z>;
  using VectorType = typename AlgebraType::VectorType;
  using BivectorNetType = typename BivectorNetTypes<P, N, Z>::type;

  BivectorNetType bvec1{create_with_counting<BivectorNetType>()};
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
  using AlgebraType = Algebra<ScalarType, P, N, Z>;
  using VectorType = typename AlgebraType::VectorType;
  using BivectorNetType = typename BivectorNetTypes<P, N, Z>::type;

  BivectorNetType bvec1{create_with_counting<BivectorNetType>()};
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
  using AlgebraType = Algebra<ScalarType, P, N, Z>;
  using VectorType = typename AlgebraType::VectorType;
  using BivectorNetType = typename BivectorNetTypes<P, N, Z>::type;

  BivectorNetType bvec1{create_with_counting<BivectorNetType>()};
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
  using AlgebraType = Algebra<ScalarType, P, N, Z>;
  using VectorType = typename AlgebraType::VectorType;
  using BivectorNetType = typename BivectorNetTypes<P, N, Z>::type;

  BivectorNetType bvec1{create_with_counting<BivectorNetType>()};
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
  using AlgebraType = Algebra<ScalarType, P, N, Z>;
  using VectorType = typename AlgebraType::VectorType;
  using BivectorNetType = typename BivectorNetTypes<P, N, Z>::type;

  BivectorNetType bvec1{create_with_counting<BivectorNetType>()};
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
  using AlgebraType = Algebra<ScalarType, P, N, Z>;
  using VectorType = typename AlgebraType::VectorType;
  using BivectorNetType = typename BivectorNetTypes<P, N, Z>::type;

  BivectorNetType bvec1{create_with_counting<BivectorNetType>()};
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
  using AlgebraType = Algebra<ScalarType, P, N, Z>;
  using VectorType = typename AlgebraType::VectorType;
  using BivectorNetType = typename BivectorNetTypes<P, N, Z>::type;

  BivectorNetType bvec1{create_with_counting<BivectorNetType>()};
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
  using AlgebraType = Algebra<ScalarType, P, N, Z>;
  using VectorType = typename AlgebraType::VectorType;
  using BivectorNetType = typename BivectorNetTypes<P, N, Z>::type;

  BivectorNetType bvec1{create_with_counting<BivectorNetType>()};
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
  using AlgebraType = Algebra<ScalarType, P, N, Z>;
  using VectorType = typename AlgebraType::VectorType;
  using BivectorNetType = typename BivectorNetTypes<P, N, Z>::type;

  BivectorNetType bvec1{create_with_counting<BivectorNetType>()};
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
  using AlgebraType = Algebra<ScalarType, P, N, Z>;
  using VectorType = typename AlgebraType::VectorType;
  using BivectorNetType = typename BivectorNetTypes<P, N, Z>::type;

  BivectorNetType bvec1{create_with_counting<BivectorNetType>()};
  BivectorNetType bvec2{32 * bvec1};
  VectorType v1{to_multivector<BivectorNetType, VectorType>(bvec1)};
  VectorType v2{to_multivector<BivectorNetType, VectorType>(bvec2)};
  VectorType result{v1.regress(v2)};
  VectorType expected{to_multivector<BivectorNetType, VectorType>(bvec1 & bvec2)};

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
  using AlgebraType = Algebra<ScalarType, P, N, Z>;
  using VectorType = typename AlgebraType::VectorType;
  using BivectorNetType = typename BivectorNetTypes<P, N, Z>::type;

  BivectorNetType bvec1{create_with_counting<BivectorNetType>()};
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
  using AlgebraType = Algebra<ScalarType, P, N, Z>;
  using VectorType = typename AlgebraType::VectorType;
  using BivectorNetType = typename BivectorNetTypes<P, N, Z>::type;

  BivectorNetType bvec1{create_with_counting<BivectorNetType>()};
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
  using AlgebraType = Algebra<ScalarType, P, N, Z>;
  using VectorType = typename AlgebraType::VectorType;
  using BivectorNetType = typename BivectorNetTypes<P, N, Z>::type;

  BivectorNetType bvec1{create_with_counting<BivectorNetType>()};
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
  using AlgebraType = Algebra<ScalarType, P, N, Z>;
  using VectorType = typename AlgebraType::VectorType;
  using BivectorNetType = typename BivectorNetTypes<P, N, Z>::type;

  BivectorNetType bvec1{create_with_counting<BivectorNetType>()};
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
  using AlgebraType = Algebra<ScalarType, P, N, Z>;
  using VectorType = typename AlgebraType::VectorType;
  using BivectorNetType = typename BivectorNetTypes<P, N, Z>::type;

  BivectorNetType bvec1{create_with_counting<BivectorNetType>()};
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
  using AlgebraType = Algebra<ScalarType, P, N, Z>;
  using VectorType = typename AlgebraType::VectorType;
  using BivectorNetType = typename BivectorNetTypes<P, N, Z>::type;

  BivectorNetType bvec1{create_with_counting<BivectorNetType>()};
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
  using AlgebraType = Algebra<ScalarType, P, N, Z>;
  using VectorType = typename AlgebraType::VectorType;
  using BivectorNetType = typename BivectorNetTypes<P, N, Z>::type;

  BivectorNetType bvec1{create_with_counting<BivectorNetType>()};
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
  using AlgebraType = Algebra<ScalarType, P, N, Z>;
  using VectorType = typename AlgebraType::VectorType;
  using BivectorNetType = typename BivectorNetTypes<P, N, Z>::type;

  BivectorNetType bvec1{create_with_counting<BivectorNetType>()};
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
  using AlgebraType = Algebra<ScalarType, P, N, Z>;
  using VectorType = typename AlgebraType::VectorType;
  using BivectorNetType = typename BivectorNetTypes<P, N, Z>::type;

  BivectorNetType bvec1{create_with_counting<BivectorNetType>()};
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
  using AlgebraType = Algebra<ScalarType, P, N, Z>;
  using VectorType = typename AlgebraType::VectorType;
  using BivectorNetType = typename BivectorNetTypes<P, N, Z>::type;

  BivectorNetType bvec1{create_with_counting<BivectorNetType>()};
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
  using AlgebraType = Algebra<ScalarType, P, N, Z>;
  using VectorType = typename AlgebraType::VectorType;
  using BivectorNetType = typename BivectorNetTypes<P, N, Z>::type;

  BivectorNetType bvec1{create_with_counting<BivectorNetType>()};
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
  using AlgebraType = Algebra<ScalarType, P, N, Z>;
  using VectorType = typename AlgebraType::VectorType;
  using BivectorNetType = typename BivectorNetTypes<P, N, Z>::type;

  BivectorNetType bvec1{create_with_counting<BivectorNetType>()};
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
  using AlgebraType = Algebra<ScalarType, P, N, Z>;
  using VectorType = typename AlgebraType::VectorType;
  using BivectorNetType = typename BivectorNetTypes<P, N, Z>::type;

  BivectorNetType bvec1{create_with_counting<BivectorNetType>()};
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
  using AlgebraType = Algebra<ScalarType, P, N, Z>;
  using VectorType = typename AlgebraType::VectorType;
  using BivectorNetType = typename BivectorNetTypes<P, N, Z>::type;

  BivectorNetType bvec1{create_with_counting<BivectorNetType>()};
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
  using AlgebraType = Algebra<ScalarType, P, N, Z>;
  using VectorType = typename AlgebraType::VectorType;
  using BivectorNetType = typename BivectorNetTypes<P, N, Z>::type;

  BivectorNetType bvec1{create_with_counting<BivectorNetType>()};
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
  using AlgebraType = Algebra<ScalarType, P, N, Z>;
  using VectorType = typename AlgebraType::VectorType;
  using BivectorNetType = typename BivectorNetTypes<P, N, Z>::type;

  BivectorNetType bvec1{create_with_counting<BivectorNetType>()};
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
  using AlgebraType = Algebra<ScalarType, P, N, Z>;
  using VectorType = typename AlgebraType::VectorType;
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
  using AlgebraType = Algebra<ScalarType, P, N, Z>;
  using VectorType = typename AlgebraType::VectorType;
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
  using AlgebraType = Algebra<ScalarType, P, N, Z>;
  using VectorType = typename AlgebraType::VectorType;
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
  using AlgebraType = Algebra<ScalarType, P, N, Z>;
  using VectorType = typename AlgebraType::VectorType;
  using BivectorNetType = typename BivectorNetTypes<P, N, Z>::type;

  BivectorNetType bvec1{create_with_counting<BivectorNetType>()};
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
  using AlgebraType = Algebra<ScalarType, P, N, Z>;
  using VectorType = typename AlgebraType::VectorType;
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
  using AlgebraType = Algebra<ScalarType, P, N, Z>;
  using VectorType = typename AlgebraType::VectorType;
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
  using AlgebraType = Algebra<ScalarType, P, N, Z>;
  using VectorType = typename AlgebraType::VectorType;
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
  using AlgebraType = Algebra<ScalarType, P, N, Z>;
  using VectorType = typename AlgebraType::VectorType;
  using BivectorNetType = typename BivectorNetTypes<P, N, Z>::type;

  BivectorNetType bvec1{create_with_counting<BivectorNetType>()};
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
  using AlgebraType = Algebra<ScalarType, P, N, Z>;
  using VectorType = typename AlgebraType::VectorType;
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
  using AlgebraType = Algebra<ScalarType, P, N, Z>;
  using VectorType = typename AlgebraType::VectorType;
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
  using AlgebraType = Algebra<ScalarType, P, N, Z>;
  using VectorType = typename AlgebraType::VectorType;
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

TEST(R301BivectorNetDirectTest, OuterProductOfTwoLines) {
  using ScalarType = float;
  static constexpr size_t P{3};
  static constexpr size_t N{0};
  static constexpr size_t Z{1};
  using AlgebraType = Algebra<ScalarType, P, N, Z>;
  using VectorType = typename AlgebraType::VectorType;
  using BivectorNetType = typename BivectorNetTypes<P, N, Z>::type;

  BivectorNetType bvec1{create_from_multivector_coefficients<BivectorNetType>(
      {1, 0, -0, 0, 0, -0, 0, -0, -0, 4, -2, 0, -2, -0, 0, 1})};
  BivectorNetType bvec2{create_from_multivector_coefficients<BivectorNetType>(
      {1, 0, -0, 0, 0, -0, 0, -0, -0, 0, -3, 0, 3, -0, 0, 1})};

  VectorType v1{to_multivector<BivectorNetType, VectorType>(bvec1)};
  VectorType v2{to_multivector<BivectorNetType, VectorType>(bvec2)};
  VectorType result{v1.outer(v2)};
  VectorType expected{to_multivector<BivectorNetType, VectorType>(bvec1 ^ bvec2)};
  LOG(INFO) << "result: " << result << ", expected: " << expected;
  EXPECT_EQ(expected, result);
}

TEST(R401BivectorNetDirectTest, OuterProductMatches) {
  using ScalarType = float;
  static constexpr size_t P{4};
  static constexpr size_t N{0};
  static constexpr size_t Z{1};
  using AlgebraType = Algebra<ScalarType, P, N, Z>;
  using VectorType = typename AlgebraType::VectorType;
  using BivectorNetType = typename BivectorNetTypes<P, N, Z>::type;

  BivectorNetType bvec1{create_with_counting<BivectorNetType>()};
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
  using AlgebraType = Algebra<ScalarType, P, N, Z>;
  using VectorType = typename AlgebraType::VectorType;
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
  using AlgebraType = Algebra<ScalarType, P, N, Z>;
  using VectorType = typename AlgebraType::VectorType;
  using BivectorNetType = typename BivectorNetTypes<P, N, Z>::type;

  BivectorNetType bvec1{create<BivectorNetType>({0, 0, 3, 0, 0, 6, 0, 0})};
  BivectorNetType bvec2{32 * bvec1};
  VectorType v1{to_multivector<BivectorNetType, VectorType>(bvec1)};
  VectorType v2{to_multivector<BivectorNetType, VectorType>(bvec2)};
  VectorType result{v1.outer(v2)};
  VectorType expected{to_multivector<BivectorNetType, VectorType>(bvec1 ^ bvec2)};
  EXPECT_EQ(expected, result);
}

TEST(R410BivectorNetDirectTest, OuterProductMatches) {
  using ScalarType = float;
  static constexpr size_t P{4};
  static constexpr size_t N{1};
  static constexpr size_t Z{0};
  using AlgebraType = Algebra<ScalarType, P, N, Z>;
  using VectorType = typename AlgebraType::VectorType;
  using BivectorNetType = typename BivectorNetTypes<P, N, Z>::type;

  BivectorNetType bvec1{create_with_counting<BivectorNetType>()};
  BivectorNetType bvec2{32 * bvec1};
  VectorType v1{to_multivector<BivectorNetType, VectorType>(bvec1)};
  VectorType v2{to_multivector<BivectorNetType, VectorType>(bvec2)};
  VectorType result{v1.outer(v2)};
  VectorType expected{to_multivector<BivectorNetType, VectorType>(bvec1 ^ bvec2)};
  EXPECT_EQ(expected, result);
}

}  // namespace ndyn::math
