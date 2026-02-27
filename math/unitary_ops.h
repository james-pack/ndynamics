#pragma once

#include "math/bit_basis.h"

namespace ndyn::math {

template <size_t NUM_POSITIVE_BASES, size_t NUM_NEGATIVE_BASES, size_t NUM_ZERO_BASES>
class UnitaryOps final {};

template <>
class UnitaryOps<0, 0, 0> final {
 public:
  static constexpr auto dual{to_bit_basis_order<int8_t, 0>({1})};
};

template <>
class UnitaryOps<0, 0, 1> final {
 public:
  static constexpr auto dual{to_bit_basis_order<int8_t, 1>({1, 1})};
};

template <>
class UnitaryOps<0, 1, 0> final {
 public:
  static constexpr auto dual{to_bit_basis_order<int8_t, 1>({1, -1})};
};

template <>
class UnitaryOps<0, 1, 1> final {
 public:
  static constexpr auto dual{to_bit_basis_order<int8_t, 2>({1, 1, 1, 1})};
};

template <>
class UnitaryOps<1, 0, 0> final {
 public:
  static constexpr auto dual{to_bit_basis_order<int8_t, 1>({1, 1})};
};

template <>
class UnitaryOps<1, 0, 1> final {
 public:
  static constexpr auto dual{to_bit_basis_order<int8_t, 2>({1, 1, 1, 1})};
};

template <>
class UnitaryOps<1, 1, 0> final {
 public:
  static constexpr auto dual{to_bit_basis_order<int8_t, 2>({1, 1, 1, 1})};
};

template <>
class UnitaryOps<1, 3, 0> final {
 public:
  static constexpr auto dual{to_bit_basis_order<int8_t, 4>({
      1, 1, 1, -1, 1, 1, -1, 1,      //
      -1, 1, -1, -1, 1, -1, -1, -1,  //
  })};
};

template <>
class UnitaryOps<2, 0, 0> final {
 public:
  static constexpr auto dual{to_bit_basis_order<int8_t, 2>({1, 1, -1, -1})};
};

template <>
class UnitaryOps<3, 0, 0> final {
 public:
  static constexpr auto dual{to_bit_basis_order<int8_t, 3>({1, 1, -1, 1, -1, 1, -1, -1})};
};

template <>
class UnitaryOps<1, 1, 1> final {
 public:
  static constexpr auto dual{to_bit_basis_order<int8_t, 3>({1, 1, 1, 1, 1, 1, 1, 1})};
};

template <>
class UnitaryOps<2, 1, 0> final {
 public:
  static constexpr auto dual{to_bit_basis_order<int8_t, 3>({1, 1, -1, -1, -1, -1, 1, 1})};
};

template <>
class UnitaryOps<2, 0, 1> final {
 public:
  static constexpr auto dual{to_bit_basis_order<int8_t, 3>({1, 1, -1, 1, 1, -1, 1, 1})};
};

template <>
class UnitaryOps<3, 0, 1> final {
 public:
  static constexpr auto dual{to_bit_basis_order<int8_t, 4>({
      1, 1, -1, 1, -1, 1, -1, 1,  //
      1, -1, 1, -1, 1, -1, 1, 1,  //
  })};
};

template <>
class UnitaryOps<4, 0, 1> final {
 public:
  static constexpr auto dual{to_bit_basis_order<int8_t, 5>({
      1, 1, 1, 1, 1, 1, 1, 1,  //
      1, 1, 1, 1, 1, 1, 1, 1,  //
      1, 1, 1, 1, 1, 1, 1, 1,  //
      1, 1, 1, 1, 1, 1, 1, 1,  //
  })};
};

template <>
class UnitaryOps<4, 1, 0> final {
 public:
  static constexpr auto dual{to_bit_basis_order<int8_t, 5>({
      1,  1,  -1, 1,  -1, -1, -1, 1,   //
      -1, -1, -1, 1,  1,  -1, -1, 1,   //
      -1, 1,  1,  -1, -1, 1,  1,  1,   //
      -1, 1,  1,  1,  -1, 1,  -1, -1,  //
  })};
};

}  // namespace ndyn::math
