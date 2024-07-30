#pragma once

#include <array>
#include <bitset>
#include <cstring>

#include "math/bitset.h"

namespace ndyn::math {

namespace {

template <size_t N>
constexpr BitSet<N> create_mask_below_bit(size_t bit) {
  const BitSet<N> mask{(1UL << bit) - 1};
  return mask;
}

template <>
constexpr BitSet<0> create_mask_below_bit<0>(size_t bit) {
  return BitSet<0>{};
}

template <size_t N>
constexpr size_t count_bits_within_mask(const BitSet<N>& bits, const BitSet<N>& mask) {
  const BitSet<N> masked{bits & mask};
  return masked.count();
}

template <typename T, size_t BASES_COUNT, size_t current_bit>
constexpr T compute_commutative_order(const BitSet<BASES_COUNT>& lhs_grade_bits,
                                      const BitSet<BASES_COUNT>& rhs_grade_bits,
                                      T accumulated_order) {
  if constexpr (BASES_COUNT == 0 or current_bit == 0) {
    return accumulated_order;
  } else {
    if (lhs_grade_bits.test(current_bit)) {
      const BitSet<BASES_COUNT> mask{create_mask_below_bit<BASES_COUNT>(current_bit)};
      if (count_bits_within_mask(rhs_grade_bits, mask) % 2 == 1) {
        return compute_commutative_order<T, BASES_COUNT, current_bit - 1>(
            lhs_grade_bits, rhs_grade_bits, -1 * accumulated_order);
      }
    }
    return compute_commutative_order<T, BASES_COUNT, current_bit - 1>(
        lhs_grade_bits, rhs_grade_bits, accumulated_order);
  }
}

}  // namespace

template <typename T>
class TableEntry final {
 public:
  constexpr TableEntry() = default;

  constexpr TableEntry(const TableEntry& rhs)
      : grade(rhs.grade), quadratic_multiplier(rhs.quadratic_multiplier) {}

  constexpr TableEntry(TableEntry&& rhs)
      : grade(rhs.grade), quadratic_multiplier(rhs.quadratic_multiplier) {}

  constexpr TableEntry(size_t g, T q) : grade(g), quadratic_multiplier(q) {}

  constexpr TableEntry& operator=(TableEntry&& rhs) {
    grade = rhs.grade;
    quadratic_multiplier = rhs.quadratic_multiplier;
    return *this;
  }

  constexpr bool operator==(const TableEntry& rhs) const {
    return grade == rhs.grade && quadratic_multiplier == rhs.quadratic_multiplier;
  }

  size_t grade{};
  T quadratic_multiplier{};
};

template <typename T, size_t NEGATIVE_BASES, size_t ZERO_BASES, size_t POSITIVE_BASES>
class CaleyTable final {
 public:
  static constexpr size_t BASES_COUNT{NEGATIVE_BASES + ZERO_BASES + POSITIVE_BASES};
  static constexpr size_t GRADE_COUNT{1UL << BASES_COUNT};

  static constexpr size_t SCALAR_GRADE{0};

  using Table = std::array<std::array<TableEntry<T>, GRADE_COUNT>, GRADE_COUNT>;

 private:
  static constexpr BitSet<BASES_COUNT> negative_bases_bitmask() {
    return BitSet<BASES_COUNT>::create_mask(NEGATIVE_BASES);
  }

  static constexpr BitSet<BASES_COUNT> zero_bases_bitmask() {
    return BitSet<BASES_COUNT>::create_mask(ZERO_BASES, NEGATIVE_BASES);
  }

  static constexpr BitSet<BASES_COUNT> positive_bases_bitmask() {
    return BitSet<BASES_COUNT>::create_mask(POSITIVE_BASES, NEGATIVE_BASES + ZERO_BASES);
  }

  static constexpr TableEntry<T> generate_entry(size_t lhs_grade, size_t rhs_grade) {
    const BitSet<BASES_COUNT> self_multiplication{lhs_grade & rhs_grade};

    if ((self_multiplication & zero_bases_bitmask()).count() != 0) {
      return TableEntry<T>{lhs_grade xor rhs_grade, 0};
    } else {
      const BitSet<BASES_COUNT> lhs_grade_bits{lhs_grade};
      const BitSet<BASES_COUNT> rhs_grade_bits{rhs_grade};

      if ((self_multiplication & negative_bases_bitmask()).count() % 2 == 1) {
        constexpr T INITIAL_COMMUTATIVE_ORDER{-1};
        const T commutative_order = compute_commutative_order<T, BASES_COUNT, BASES_COUNT - 1>(
            lhs_grade_bits, rhs_grade_bits, INITIAL_COMMUTATIVE_ORDER);
        const TableEntry<T> result{lhs_grade xor rhs_grade, commutative_order};
        return result;
      } else {
        constexpr T INITIAL_COMMUTATIVE_ORDER{1};
        const T commutative_order = compute_commutative_order<T, BASES_COUNT, BASES_COUNT - 1>(
            lhs_grade_bits, rhs_grade_bits, INITIAL_COMMUTATIVE_ORDER);
        const TableEntry<T> result{lhs_grade xor rhs_grade, commutative_order};
        return result;
      }
    }
  }

  static constexpr std::array<std::array<TableEntry<T>, GRADE_COUNT>, GRADE_COUNT>
  generate_table() {
    std::array<std::array<TableEntry<T>, GRADE_COUNT>, GRADE_COUNT> result{};
    for (size_t i = 0; i < GRADE_COUNT; ++i) {
      for (size_t j = 0; j < GRADE_COUNT; ++j) {
        result.at(i).at(j) = generate_entry(i, j);
      }
    }
    return result;
  }

  Table table_{generate_table()};

 public:
  constexpr CaleyTable() = default;

  constexpr const TableEntry<T>& entry(size_t lhs_grade, size_t rhs_grade) const {
    return table_.at(lhs_grade).at(rhs_grade);
  }
};

template <typename T>
using ScalarCaleyTable = CaleyTable<T, 0, 0, 0>;

template <typename T>
using ComplexCaleyTable = CaleyTable<T, 1, 0, 0>;

template <typename T>
using DualCaleyTable = CaleyTable<T, 0, 1, 0>;

template <typename T>
using SplitComplexCaleyTable = CaleyTable<T, 0, 0, 1>;

template <typename T>
using QuaternionCaleyTable = CaleyTable<T, 3, 0, 0>;

}  // namespace ndyn::math
