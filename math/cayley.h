#pragma once

#include <array>
#include <bitset>
#include <cstring>
#include <limits>
#include <ostream>
#include <string>

#include "math/bitset.h"

namespace ndyn::math {

enum class Operations : unsigned char {
  UKNOWN,
  GEOMETRIC_PRODUCT,
  INNER_PRODUCT,
  OUTER_PRODUCT,
};

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

template <size_t QUADRATIC_FORM_COUNT, size_t POSITIVE_BASES, size_t NEGATIVE_BASES,
          size_t ZERO_BASES>
static constexpr BitSet<QUADRATIC_FORM_COUNT> positive_bases_bitmask() {
  return BitSet<QUADRATIC_FORM_COUNT>::create_mask(POSITIVE_BASES);
}

template <size_t QUADRATIC_FORM_COUNT, size_t POSITIVE_BASES, size_t NEGATIVE_BASES,
          size_t ZERO_BASES>
static constexpr BitSet<QUADRATIC_FORM_COUNT> negative_bases_bitmask() {
  return BitSet<QUADRATIC_FORM_COUNT>::create_mask(NEGATIVE_BASES, POSITIVE_BASES);
}

template <size_t QUADRATIC_FORM_COUNT, size_t POSITIVE_BASES, size_t NEGATIVE_BASES,
          size_t ZERO_BASES>
static constexpr BitSet<QUADRATIC_FORM_COUNT> zero_bases_bitmask() {
  return BitSet<QUADRATIC_FORM_COUNT>::create_mask(ZERO_BASES, POSITIVE_BASES + NEGATIVE_BASES);
}

template <Operations OPERATION, size_t QUADRATIC_FORM_COUNT, size_t current_bit>
constexpr char accumulate_commutative_order(const BitSet<QUADRATIC_FORM_COUNT>& lhs_component_bits,
                                            const BitSet<QUADRATIC_FORM_COUNT>& rhs_component_bits,
                                            char accumulated_order) {
  if constexpr (QUADRATIC_FORM_COUNT == 0 or current_bit == 0) {
    return accumulated_order;
  } else {
    if (lhs_component_bits.test(current_bit)) {
      const BitSet<QUADRATIC_FORM_COUNT> mask{
          create_mask_below_bit<QUADRATIC_FORM_COUNT>(current_bit)};
      if (count_bits_within_mask(rhs_component_bits, mask) % 2 == 1) {
        return accumulate_commutative_order<OPERATION, QUADRATIC_FORM_COUNT, current_bit - 1>(
            lhs_component_bits, rhs_component_bits, -1 * accumulated_order);
      }
    }
    return accumulate_commutative_order<OPERATION, QUADRATIC_FORM_COUNT, current_bit - 1>(
        lhs_component_bits, rhs_component_bits, accumulated_order);
  }
}

template <Operations OPERATION, size_t QUADRATIC_FORM_COUNT, size_t POSITIVE_BASES,
          size_t NEGATIVE_BASES, size_t ZERO_BASES>
constexpr char compute_commutative_order(const BitSet<QUADRATIC_FORM_COUNT>& lhs_component_bits,
                                         const BitSet<QUADRATIC_FORM_COUNT>& rhs_component_bits) {
  const BitSet<QUADRATIC_FORM_COUNT> self_multiplication{lhs_component_bits & rhs_component_bits};

  if ((self_multiplication &
       zero_bases_bitmask<QUADRATIC_FORM_COUNT, POSITIVE_BASES, NEGATIVE_BASES, ZERO_BASES>())
          .count() != 0) {
    return 0;
  } else {
    if ((self_multiplication &
         negative_bases_bitmask<QUADRATIC_FORM_COUNT, POSITIVE_BASES, NEGATIVE_BASES, ZERO_BASES>())
                .count() %
            2 ==
        1) {
      constexpr char INITIAL_COMMUTATIVE_ORDER{-1};
      return accumulate_commutative_order<OPERATION, QUADRATIC_FORM_COUNT,
                                          QUADRATIC_FORM_COUNT - 1>(lhs_component_bits, rhs_component_bits,
                                                                    INITIAL_COMMUTATIVE_ORDER);
    } else {
      constexpr char INITIAL_COMMUTATIVE_ORDER{1};
      return accumulate_commutative_order<OPERATION, QUADRATIC_FORM_COUNT,
                                          QUADRATIC_FORM_COUNT - 1>(lhs_component_bits, rhs_component_bits,
                                                                    INITIAL_COMMUTATIVE_ORDER);
    }
  }
}

template <Operations OPERATION, size_t QUADRATIC_FORM_COUNT>
constexpr unsigned char compute_result_grade(const BitSet<QUADRATIC_FORM_COUNT>& lhs_component_bits,
                                             const BitSet<QUADRATIC_FORM_COUNT>& rhs_component_bits) {
  return (lhs_component_bits xor rhs_component_bits).to_ulong();
}

}  // namespace

class TableEntry final {
 public:
  constexpr TableEntry() = default;

  constexpr TableEntry(const TableEntry& rhs)
      : grade(rhs.grade), quadratic_multiplier(rhs.quadratic_multiplier) {}

  constexpr TableEntry(TableEntry&& rhs)
      : grade(rhs.grade), quadratic_multiplier(rhs.quadratic_multiplier) {}

  constexpr TableEntry(size_t g, char q) : grade(g), quadratic_multiplier(q) {
    // Note that the CayleyTable class has defenses to avoid grades that would overflow the grade
    // data member below.
  }

  constexpr TableEntry& operator=(TableEntry&& rhs) {
    grade = rhs.grade;
    quadratic_multiplier = rhs.quadratic_multiplier;
    return *this;
  }

  constexpr bool operator==(const TableEntry& rhs) const {
    return grade == rhs.grade && quadratic_multiplier == rhs.quadratic_multiplier;
  }

  unsigned char grade{};
  char quadratic_multiplier{};
};

std::string to_string(const TableEntry& t) {
  using std::to_string;
  return std::string{}
      .append("(")  //
      .append(to_string(t.grade))
      .append(", ")
      .append(to_string(t.quadratic_multiplier))
      .append(")");
}

std::ostream& operator<<(std::ostream& os, const TableEntry& t) {
  os << to_string(t);
  return os;
}

template <Operations OPERATION, size_t POSITIVE_BASES, size_t NEGATIVE_BASES, size_t ZERO_BASES>
class CayleyTable;

template <Operations OPERATION, size_t POSITIVE_BASES, size_t NEGATIVE_BASES, size_t ZERO_BASES>
std::string to_string(const CayleyTable<OPERATION, POSITIVE_BASES, NEGATIVE_BASES, ZERO_BASES>& t);

template <Operations OPERATION, size_t POSITIVE_BASES, size_t NEGATIVE_BASES, size_t ZERO_BASES>
class CayleyTable final {
 public:
  static constexpr size_t QUADRATIC_FORM_COUNT{POSITIVE_BASES + NEGATIVE_BASES + ZERO_BASES};
  static constexpr size_t COMPONENT_COUNT{1UL << QUADRATIC_FORM_COUNT};

  // Note: failures in following situation can be avoided by templating the TableEntry class on the
  // number of grades/bases and using different storage sizes as needed.
  static_assert(COMPONENT_COUNT <= std::numeric_limits<unsigned char>::max(),
                "TableEntry cannot handle the number of grades required for this Cayley table.");

  static constexpr size_t SCALAR_GRADE{0};

  using Table = std::array<std::array<TableEntry, COMPONENT_COUNT>, COMPONENT_COUNT>;

 private:
  static constexpr TableEntry generate_entry(size_t lhs_component, size_t rhs_component) {
    const BitSet<QUADRATIC_FORM_COUNT> lhs_component_bits{lhs_component};
    const BitSet<QUADRATIC_FORM_COUNT> rhs_component_bits{rhs_component};
    return TableEntry{
        compute_result_grade<OPERATION, QUADRATIC_FORM_COUNT>(lhs_component, rhs_component),
        compute_commutative_order<OPERATION, QUADRATIC_FORM_COUNT, POSITIVE_BASES, NEGATIVE_BASES,
                                  ZERO_BASES>(lhs_component_bits, rhs_component_bits)};
  }

  static constexpr std::array<std::array<TableEntry, COMPONENT_COUNT>, COMPONENT_COUNT>
  generate_table() {
    std::array<std::array<TableEntry, COMPONENT_COUNT>, COMPONENT_COUNT> result{};
    for (size_t i = 0; i < COMPONENT_COUNT; ++i) {
      for (size_t j = 0; j < COMPONENT_COUNT; ++j) {
        result.at(i).at(j) = generate_entry(i, j);
      }
    }
    return result;
  }

  Table table_{generate_table()};

  friend std::string to_string<>(
      const CayleyTable<OPERATION, POSITIVE_BASES, NEGATIVE_BASES, ZERO_BASES>&);

 public:
  constexpr CayleyTable() = default;

  constexpr const TableEntry& entry(size_t lhs_component, size_t rhs_component) const {
    return table_.at(lhs_component).at(rhs_component);
  }
};

template <Operations OPERATION, size_t POSITIVE_BASES, size_t NEGATIVE_BASES, size_t ZERO_BASES>
std::string to_string(const CayleyTable<OPERATION, POSITIVE_BASES, NEGATIVE_BASES, ZERO_BASES>& t) {
  using std::to_string;
  static constexpr size_t COMPONENT_COUNT{
      CayleyTable<OPERATION, POSITIVE_BASES, NEGATIVE_BASES, ZERO_BASES>::COMPONENT_COUNT};

  std::string result{};
  result.append("\n<\n");
  for (size_t i = 0; i < COMPONENT_COUNT; ++i) {
    const std::array<TableEntry, COMPONENT_COUNT>& row{t.table_.at(i)};
    result.append("\t<");
    bool need_comma{false};
    for (size_t j = 0; j < COMPONENT_COUNT; ++j) {
      if (need_comma) {
        result.append(", ");
      }
      result.append(to_string(row.at(j)));
      need_comma = true;
    }
    result.append(">\n");
  }
  result.append(">\n");

  return result;
}

template <Operations OPERATION, size_t POSITIVE_BASES, size_t NEGATIVE_BASES, size_t ZERO_BASES>
std::ostream& operator<<(
    std::ostream& os, const CayleyTable<OPERATION, POSITIVE_BASES, NEGATIVE_BASES, ZERO_BASES>& t) {
  os << to_string(t);
  return os;
}

template <Operations OPERATION>
using ScalarCayleyTable = CayleyTable<OPERATION, 0, 0, 0>;

template <Operations OPERATION>
using ComplexCayleyTable = CayleyTable<OPERATION, 0, 1, 0>;

template <Operations OPERATION>
using DualCayleyTable = CayleyTable<OPERATION, 0, 0, 1>;

template <Operations OPERATION>
using SplitComplexCayleyTable = CayleyTable<OPERATION, 1, 0, 0>;

template <Operations OPERATION>
using SpacetimeCayleyTable = CayleyTable<OPERATION, 1, 3, 0>;

}  // namespace ndyn::math
