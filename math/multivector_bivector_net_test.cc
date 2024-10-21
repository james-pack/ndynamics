#include <array>
#include <initializer_list>

#include "gtest/gtest.h"
#include "math/multivector.h"
#include "math/multivector_test_utils.h"

namespace ndyn::math {

template <size_t BASES>
constexpr size_t from_bivector_net_index(size_t bivector_index);

template <>
constexpr size_t from_bivector_net_index<1>(size_t bivector_index) {
  constexpr std::array<size_t, 2> mapping{0, 1};
  return mapping.at(bivector_index);
}

template <>
constexpr size_t from_bivector_net_index<2>(size_t bivector_index) {
  constexpr std::array<size_t, 4> mapping{0, 1, 2, 3};
  return mapping.at(bivector_index);
}

template <>
constexpr size_t from_bivector_net_index<3>(size_t bivector_index) {
  constexpr std::array<size_t, 8> mapping{0, 1, 2, 4, 3, 5, 6, 7};
  return mapping.at(bivector_index);
}

template <>
constexpr size_t from_bivector_net_index<4>(size_t bivector_index) {
  constexpr std::array<size_t, 16> mapping{0, 1, 2, 4, 8, 3, 5, 9, 6, 10, 12, 7, 11, 13, 14, 15};
  return mapping.at(bivector_index);
}

template <>
constexpr size_t from_bivector_net_index<5>(size_t bivector_index) {
  constexpr std::array<size_t, 32> mapping{0,  1,  2,  4,  8,  16,  //
                                           3,  5,  9,  17,          //
                                           6,  10, 18,              //
                                           12, 20, 24,              //
                                           7,  11, 19,              //
                                           13, 21, 25,              //
                                           14, 22, 26, 28,          //
                                           15, 23, 27, 29, 30, 31};
  return mapping.at(bivector_index);
}

template <typename ScalarType, size_t N>
constexpr std::array<ScalarType, (1UL << N)> from_bivector_net_order(
    std::initializer_list<ScalarType> values) {
  std::array<ScalarType, (1UL << N)> result{};
  size_t bivector_index{};
  for (ScalarType value : values) {
    result[from_bivector_net_index<N>(bivector_index)] = value;
    ++bivector_index;
  }
  return result;
}

template <typename ScalarType, size_t POSITIVE_BASES, size_t NEGATIVE_BASES, size_t ZERO_BASES>
constexpr Multivector<ScalarType, POSITIVE_BASES, NEGATIVE_BASES, ZERO_BASES> as_multivector(
    std::initializer_list<ScalarType> values) {
  Multivector<ScalarType, POSITIVE_BASES, NEGATIVE_BASES, ZERO_BASES> result{};
  const auto array{
      from_bivector_net_order<ScalarType, POSITIVE_BASES + NEGATIVE_BASES + ZERO_BASES>(values)};
  for (size_t i = 0; i < array.size(); ++i) {
    result.set_basis(i, array[i]);
  }
  return result;
}

TEST(R100BivectorNetTest, ConjugateMatches) {
  using ScalarType = float;
  static constexpr size_t P{1};
  static constexpr size_t N{0};
  static constexpr size_t Z{0};
  using VectorType = Multivector<ScalarType, P, N, Z>;
  constexpr VectorType v{as_multivector<ScalarType, P, N, Z>({0, 1})};
  constexpr VectorType result{v.conj()};
  constexpr VectorType expected{as_multivector<ScalarType, P, N, Z>({0, -1})};
  EXPECT_EQ(expected, result);
}

TEST(R200BivectorNetTest, ConjugateMatches) {
  using ScalarType = float;
  static constexpr size_t P{2};
  static constexpr size_t N{0};
  static constexpr size_t Z{0};
  using VectorType = Multivector<ScalarType, P, N, Z>;
  constexpr VectorType v{as_multivector<ScalarType, P, N, Z>({0, 1, 2, 3})};
  constexpr VectorType result{v.conj()};
  constexpr VectorType expected{as_multivector<ScalarType, P, N, Z>({0, -1, -2, -3})};
  EXPECT_EQ(expected, result);
}

TEST(R101BivectorNetTest, ConjugateMatches) {
  using ScalarType = float;
  static constexpr size_t P{1};
  static constexpr size_t N{0};
  static constexpr size_t Z{1};
  using VectorType = Multivector<ScalarType, P, N, Z>;
  constexpr VectorType v{as_multivector<ScalarType, P, N, Z>({0, 1, 2, 3})};
  constexpr VectorType result{v.conj()};
  constexpr VectorType expected{as_multivector<ScalarType, P, N, Z>({0, -1, -2, -3})};
  EXPECT_EQ(expected, result);
}

TEST(R110BivectorNetTest, ConjugateMatches) {
  using ScalarType = float;
  static constexpr size_t P{1};
  static constexpr size_t N{1};
  static constexpr size_t Z{0};
  using VectorType = Multivector<ScalarType, P, N, Z>;
  constexpr VectorType v{as_multivector<ScalarType, P, N, Z>({0, 1, 2, 3})};
  constexpr VectorType result{v.conj()};
  constexpr VectorType expected{as_multivector<ScalarType, P, N, Z>({0, -1, -2, -3})};
  EXPECT_EQ(expected, result);
}

TEST(R300BivectorNetTest, ConjugateMatches) {
  using ScalarType = float;
  static constexpr size_t P{3};
  static constexpr size_t N{0};
  static constexpr size_t Z{0};
  using VectorType = Multivector<ScalarType, P, N, Z>;
  constexpr VectorType v{as_multivector<ScalarType, P, N, Z>({0, 1, 2, 3, 4, 5, 6, 7})};
  constexpr VectorType result{v.conj()};
  constexpr VectorType expected{
      as_multivector<ScalarType, P, N, Z>({0, -1, -2, -3, -4, -5, -6, 7})};
  EXPECT_EQ(expected, result);
}

TEST(R111BivectorNetTest, ConjugateMatches) {
  using ScalarType = float;
  static constexpr size_t P{1};
  static constexpr size_t N{1};
  static constexpr size_t Z{1};
  using VectorType = Multivector<ScalarType, P, N, Z>;
  constexpr VectorType v{as_multivector<ScalarType, P, N, Z>({0, 1, 2, 3, 4, 5, 6, 7})};
  constexpr VectorType result{v.conj()};
  constexpr VectorType expected{
      as_multivector<ScalarType, P, N, Z>({0, -1, -2, -3, -4, -5, -6, 7})};
  EXPECT_EQ(expected, result);
}

TEST(R210BivectorNetTest, ConjugateMatches) {
  using ScalarType = float;
  static constexpr size_t P{2};
  static constexpr size_t N{1};
  static constexpr size_t Z{0};
  using VectorType = Multivector<ScalarType, P, N, Z>;
  constexpr VectorType v{as_multivector<ScalarType, P, N, Z>({0, 1, 2, 3, 4, 5, 6, 7})};
  constexpr VectorType result{v.conj()};
  constexpr VectorType expected{
      as_multivector<ScalarType, P, N, Z>({0, -1, -2, -3, -4, -5, -6, 7})};
  EXPECT_EQ(expected, result);
}

TEST(R410BivectorNetTest, ConjugateMatches) {
  using ScalarType = float;
  static constexpr size_t P{4};
  static constexpr size_t N{1};
  static constexpr size_t Z{0};
  using VectorType = Multivector<ScalarType, P, N, Z>;
  constexpr VectorType v{as_multivector<ScalarType, P, N, Z>(
      {0,  1,  2,  3,  4,  5,  6,  7,  8,  9,  10, 11, 12, 13, 14, 15,
       16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31})};
  constexpr VectorType result{v.conj()};
  constexpr VectorType expected{as_multivector<ScalarType, P, N, Z>(
      {0,  -1, -2, -3, -4, -5, -6, -7, -8, -9, -10, -11, -12, -13, -14, -15,
       16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26,  27,  28,  29,  30,  -31})};
  EXPECT_EQ(expected, result);
}

TEST(R100BivectorNetTest, ReverseMatches) {
  using ScalarType = float;
  static constexpr size_t P{1};
  static constexpr size_t N{0};
  static constexpr size_t Z{0};
  using VectorType = Multivector<ScalarType, P, N, Z>;
  constexpr VectorType v{as_multivector<ScalarType, P, N, Z>({0, 1})};
  constexpr VectorType result{v.reverse()};
  constexpr VectorType expected{as_multivector<ScalarType, P, N, Z>({0, 1})};
  EXPECT_EQ(expected, result);
}

TEST(R200BivectorNetTest, ReverseMatches) {
  using ScalarType = float;
  static constexpr size_t P{2};
  static constexpr size_t N{0};
  static constexpr size_t Z{0};
  using VectorType = Multivector<ScalarType, P, N, Z>;
  constexpr VectorType v{as_multivector<ScalarType, P, N, Z>({0, 1, 2, 3})};
  constexpr VectorType result{v.reverse()};
  constexpr VectorType expected{as_multivector<ScalarType, P, N, Z>({0, 1, 2, -3})};
  EXPECT_EQ(expected, result);
}

TEST(R101BivectorNetTest, ReverseMatches) {
  using ScalarType = float;
  static constexpr size_t P{1};
  static constexpr size_t N{0};
  static constexpr size_t Z{1};
  using VectorType = Multivector<ScalarType, P, N, Z>;
  constexpr VectorType v{as_multivector<ScalarType, P, N, Z>({0, 1, 2, 3})};
  constexpr VectorType result{v.reverse()};
  constexpr VectorType expected{as_multivector<ScalarType, P, N, Z>({0, 1, 2, -3})};
  EXPECT_EQ(expected, result);
}

TEST(R110BivectorNetTest, ReverseMatches) {
  using ScalarType = float;
  static constexpr size_t P{1};
  static constexpr size_t N{1};
  static constexpr size_t Z{0};
  using VectorType = Multivector<ScalarType, P, N, Z>;
  constexpr VectorType v{as_multivector<ScalarType, P, N, Z>({0, 1, 2, 3})};
  constexpr VectorType result{v.reverse()};
  constexpr VectorType expected{as_multivector<ScalarType, P, N, Z>({0, 1, 2, -3})};
  EXPECT_EQ(expected, result);
}

TEST(R300BivectorNetTest, ReverseMatches) {
  using ScalarType = float;
  static constexpr size_t P{3};
  static constexpr size_t N{0};
  static constexpr size_t Z{0};
  using VectorType = Multivector<ScalarType, P, N, Z>;
  constexpr VectorType v{as_multivector<ScalarType, P, N, Z>({0, 1, 2, 3, 4, 5, 6, 7})};
  constexpr VectorType result{v.reverse()};
  constexpr VectorType expected{as_multivector<ScalarType, P, N, Z>({0, 1, 2, 3, -4, -5, -6, -7})};
  EXPECT_EQ(expected, result);
}

TEST(R111BivectorNetTest, ReverseMatches) {
  using ScalarType = float;
  static constexpr size_t P{1};
  static constexpr size_t N{1};
  static constexpr size_t Z{1};
  using VectorType = Multivector<ScalarType, P, N, Z>;
  constexpr VectorType v{as_multivector<ScalarType, P, N, Z>({0, 1, 2, 3, 4, 5, 6, 7})};
  constexpr VectorType result{v.reverse()};
  constexpr VectorType expected{as_multivector<ScalarType, P, N, Z>({0, 1, 2, 3, -4, -5, -6, -7})};
  EXPECT_EQ(expected, result);
}

TEST(R210BivectorNetTest, ReverseMatches) {
  using ScalarType = float;
  static constexpr size_t P{2};
  static constexpr size_t N{1};
  static constexpr size_t Z{0};
  using VectorType = Multivector<ScalarType, P, N, Z>;
  constexpr VectorType v{as_multivector<ScalarType, P, N, Z>({0, 1, 2, 3, 4, 5, 6, 7})};
  constexpr VectorType result{v.reverse()};
  constexpr VectorType expected{as_multivector<ScalarType, P, N, Z>({0, 1, 2, 3, -4, -5, -6, -7})};
  EXPECT_EQ(expected, result);
}

TEST(R410BivectorNetTest, ReverseMatches) {
  using ScalarType = float;
  static constexpr size_t P{4};
  static constexpr size_t N{1};
  static constexpr size_t Z{0};
  using VectorType = Multivector<ScalarType, P, N, Z>;
  constexpr VectorType v{as_multivector<ScalarType, P, N, Z>(
      {0,  1,  2,  3,  4,  5,  6,  7,  8,  9,  10, 11, 12, 13, 14, 15,
       16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31})};
  constexpr VectorType result{v.reverse()};
  constexpr VectorType expected{as_multivector<ScalarType, P, N, Z>(
      {0,   1,   2,   3,   4,   5,   -6,  -7,  -8,  -9,  -10, -11, -12, -13, -14, -15,
       -16, -17, -18, -19, -20, -21, -22, -23, -24, -25, 26,  27,  28,  29,  30,  31})};
  EXPECT_EQ(expected, result);
}

}  // namespace ndyn::math
