#pragma once

#include <array>

namespace ndyn::math {

template <size_t BASES>
constexpr size_t from_bivector_net_index(size_t bivector_index);

template <>
constexpr size_t from_bivector_net_index<0>(size_t bivector_index) {
  constexpr std::array<size_t, 1> mapping{0};
  return mapping.at(bivector_index);
}

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

template <size_t POSITIVE_BASES, size_t NEGATIVE_BASES, size_t ZERO_BASES>
class UnitaryOpSigns final {};

template <>
class UnitaryOpSigns<0, 0, 0> final {
 public:
  static constexpr auto dual{from_bivector_net_order<int8_t, 0>({1})};
};

template <>
class UnitaryOpSigns<0, 0, 1> final {
 public:
  static constexpr auto dual{from_bivector_net_order<int8_t, 1>({1, 1})};
};

template <>
class UnitaryOpSigns<0, 1, 0> final {
 public:
  static constexpr auto dual{from_bivector_net_order<int8_t, 1>({1, -1})};
};

template <>
class UnitaryOpSigns<0, 1, 1> final {
 public:
  static constexpr auto dual{from_bivector_net_order<int8_t, 2>({1, 1, 1, 1})};
};

template <>
class UnitaryOpSigns<1, 0, 0> final {
 public:
  static constexpr auto dual{from_bivector_net_order<int8_t, 1>({1, 1})};
};

template <>
class UnitaryOpSigns<1, 0, 1> final {
 public:
  static constexpr auto dual{from_bivector_net_order<int8_t, 2>({1, 1, 1, 1})};
};

template <>
class UnitaryOpSigns<1, 1, 0> final {
 public:
  static constexpr auto dual{from_bivector_net_order<int8_t, 2>({1, 1, 1, 1})};
};

template <>
class UnitaryOpSigns<1, 3, 0> final {
 public:
  static constexpr auto dual{from_bivector_net_order<int8_t, 4>({
      1, 1, 1, -1, 1, 1, -1, 1,      //
      -1, 1, -1, -1, 1, -1, -1, -1,  //
  })};
};

template <>
class UnitaryOpSigns<2, 0, 0> final {
 public:
  static constexpr auto dual{from_bivector_net_order<int8_t, 2>({1, 1, -1, -1})};
};

template <>
class UnitaryOpSigns<3, 0, 0> final {
 public:
  static constexpr auto dual{from_bivector_net_order<int8_t, 3>({1, 1, -1, 1, -1, 1, -1, -1})};
};

template <>
class UnitaryOpSigns<1, 1, 1> final {
 public:
  static constexpr auto dual{from_bivector_net_order<int8_t, 3>({1, 1, 1, 1, 1, 1, 1, 1})};
};

template <>
class UnitaryOpSigns<2, 1, 0> final {
 public:
  static constexpr auto dual{from_bivector_net_order<int8_t, 3>({1, 1, -1, -1, -1, -1, 1, 1})};
};

template <>
class UnitaryOpSigns<2, 0, 1> final {
 public:
  static constexpr auto dual{from_bivector_net_order<int8_t, 3>({1, 1, -1, 1, 1, -1, 1, 1})};
};

template <>
class UnitaryOpSigns<3, 0, 1> final {
 public:
  static constexpr auto dual{from_bivector_net_order<int8_t, 4>({
      1, 1, 1, -1, -1, 1, -1, 1,  //
      1, -1, 1, -1, -1, 1, 1, 1,  //
  })};
};

template <>
class UnitaryOpSigns<4, 0, 1> final {
 public:
  static constexpr auto dual{from_bivector_net_order<int8_t, 5>({
      1, 1, 1, 1, 1, 1, 1, 1,  //
      1, 1, 1, 1, 1, 1, 1, 1,  //
      1, 1, 1, 1, 1, 1, 1, 1,  //
      1, 1, 1, 1, 1, 1, 1, 1,  //
  })};
};

template <>
class UnitaryOpSigns<4, 1, 0> final {
 public:
  static constexpr auto dual{from_bivector_net_order<int8_t, 5>({
      1,  1,  -1, 1,  -1, -1, -1, 1,   //
      -1, -1, -1, 1,  1,  -1, -1, 1,   //
      -1, 1,  1,  -1, -1, 1,  1,  1,   //
      -1, 1,  1,  1,  -1, 1,  -1, -1,  //
  })};
};

}  // namespace ndyn::math
