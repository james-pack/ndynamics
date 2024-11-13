#pragma once

#include <array>
#include <cstddef>

namespace ndyn::math {

template <size_t BASES>
constexpr size_t to_bit_basis_index(size_t common_index);

template <>
constexpr size_t to_bit_basis_index<0>(size_t common_index) {
  constexpr std::array<size_t, 1> mapping{0};
  return mapping.at(common_index);
}

template <>
constexpr size_t to_bit_basis_index<1>(size_t common_index) {
  constexpr std::array<size_t, 2> mapping{0, 1};
  return mapping.at(common_index);
}

template <>
constexpr size_t to_bit_basis_index<2>(size_t common_index) {
  constexpr std::array<size_t, 4> mapping{0, 1, 2, 3};
  return mapping.at(common_index);
}

template <>
constexpr size_t to_bit_basis_index<3>(size_t common_index) {
  constexpr std::array<size_t, 8> mapping{0, 1, 2, 4, 3, 5, 6, 7};
  return mapping.at(common_index);
}

template <>
constexpr size_t to_bit_basis_index<4>(size_t common_index) {
  constexpr std::array<size_t, 16> mapping{0, 1, 2, 4, 8, 3, 5, 9, 6, 10, 12, 7, 11, 13, 14, 15};
  return mapping.at(common_index);
}

template <>
constexpr size_t to_bit_basis_index<5>(size_t common_index) {
  constexpr std::array<size_t, 32> mapping{0,  1,  2,  4,  8,  16,  //
                                           3,  5,  9,  17,          //
                                           6,  10, 18,              //
                                           12, 20, 24,              //
                                           7,  11, 19,              //
                                           13, 21, 25,              //
                                           14, 22, 26, 28,          //
                                           15, 23, 27, 29, 30, 31};
  return mapping.at(common_index);
}

template <size_t BASES>
constexpr size_t from_bit_basis_index(size_t bit_basis_index);

template <>
constexpr size_t from_bit_basis_index<0>(size_t bit_basis_index) {
  constexpr std::array<size_t, 1> mapping{0};
  return mapping.at(bit_basis_index);
}

template <>
constexpr size_t from_bit_basis_index<1>(size_t bit_basis_index) {
  constexpr std::array<size_t, 2> mapping{0, 1};
  return mapping.at(bit_basis_index);
}

template <>
constexpr size_t from_bit_basis_index<2>(size_t bit_basis_index) {
  constexpr std::array<size_t, 4> mapping{0, 1, 2, 3};
  return mapping.at(bit_basis_index);
}

template <>
constexpr size_t from_bit_basis_index<3>(size_t bit_basis_index) {
  constexpr std::array<size_t, 8> mapping{0, 1, 2, 4, 3, 5, 6, 7};
  return mapping.at(bit_basis_index);
}

template <>
constexpr size_t from_bit_basis_index<4>(size_t bit_basis_index) {
  constexpr std::array<size_t, 16> mapping{
      0,  /*s*/
      1,  /*e0*/
      2,  /*e1*/
      5,  /*e01*/
      3,  /*e2*/
      6,  /*e02*/
      8,  /*e12*/
      11, /*e012*/
      4,  /*e3*/
      7,  /*e03*/
      9,  /*e13*/
      12, /*e013*/
      10, /*e23*/
      13, /*e023*/
      14, /*e123*/
      15, /*e0123*/
  };
  return mapping.at(bit_basis_index);
}

template <>
constexpr size_t from_bit_basis_index<5>(size_t bit_basis_index) {
  constexpr std::array<size_t, 32> mapping{
      0,  1,  2,  6,   //
      3,  7,  10, 16,  //
      4,  8,  11, 17,  //
      13, 19, 22, 26,  //
      5,  9,  12, 18,  //
      14, 20, 23, 27,  //
      15, 21, 24, 28,  //
      25, 29, 30, 31,  //
  };
  return mapping.at(bit_basis_index);
}

/**
 * Rearrange a list of values in the common order to be in the order of bit basis indices.
 */
template <typename ScalarType, size_t N>
constexpr std::array<ScalarType, (1UL << N)> to_bit_basis_order(
    std::initializer_list<ScalarType> values) {
  std::array<ScalarType, (1UL << N)> result{};
  size_t common_index{};
  for (ScalarType value : values) {
    result[to_bit_basis_index<N>(common_index)] = value;
    ++common_index;
  }
  return result;
}

}  // namespace ndyn::math
