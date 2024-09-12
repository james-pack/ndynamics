#pragma once

#include <array>
#include <cstdint>
#include <limits>
#include <ostream>
#include <string>

#include "math/bitset.h"
#include "math/cayley_entry_calculator.h"

namespace ndyn::math {

template <size_t COMPONENT_COUNT, typename = void>
class TableEntry final {
 private:
  uint8_t grade_{};
  int8_t quadratic_multiplier_{};

 public:
  static constexpr size_t MAX_COMPONENT_COUNT{std::numeric_limits<decltype(grade_)>::max()};
  static_assert(COMPONENT_COUNT <= MAX_COMPONENT_COUNT,
                "This TableEntry class definition does not support enough components");

  constexpr TableEntry() = default;

  constexpr TableEntry(const TableEntry& rhs)
      : grade_(rhs.grade_), quadratic_multiplier_(rhs.quadratic_multiplier_) {}

  constexpr TableEntry(TableEntry&& rhs)
      : grade_(rhs.grade_), quadratic_multiplier_(rhs.quadratic_multiplier_) {}

  constexpr TableEntry(size_t g, int8_t q) : grade_(g), quadratic_multiplier_(q) {
    // Note that the CayleyTable class has defenses to avoid grades that would overflow the grade
    // data member below.
  }

  constexpr TableEntry& operator=(TableEntry&& rhs) {
    grade_ = rhs.grade_;
    quadratic_multiplier_ = rhs.quadratic_multiplier_;
    return *this;
  }

  constexpr bool operator==(const TableEntry& rhs) const {
    return grade_ == rhs.grade_ && quadratic_multiplier_ == rhs.quadratic_multiplier_;
  }

  constexpr uint8_t grade() const { return grade_; }
  constexpr int8_t quadratic_multiplier() const { return quadratic_multiplier_; }
};

template <size_t COMPONENT_COUNT>
class TableEntry<COMPONENT_COUNT, std::enable_if_t<COMPONENT_COUNT >= 8> > final {
 private:
  uint64_t grade_{};
  int8_t quadratic_multiplier_{};

 public:
  static constexpr size_t MAX_COMPONENT_COUNT{std::numeric_limits<decltype(grade_)>::max()};
  static_assert(COMPONENT_COUNT <= MAX_COMPONENT_COUNT,
                "This TableEntry class definition does not support enough components");

  constexpr TableEntry() = default;

  constexpr TableEntry(const TableEntry& rhs)
      : grade_(rhs.grade_), quadratic_multiplier_(rhs.quadratic_multiplier_) {}

  constexpr TableEntry(TableEntry&& rhs)
      : grade_(rhs.grade_), quadratic_multiplier_(rhs.quadratic_multiplier_) {}

  constexpr TableEntry(size_t g, int8_t q) : grade_(g), quadratic_multiplier_(q) {
    // Note that the CayleyTable class has defenses to avoid grades that would overflow the grade
    // data member below.
  }

  constexpr TableEntry& operator=(TableEntry&& rhs) {
    grade_ = rhs.grade_;
    quadratic_multiplier_ = rhs.quadratic_multiplier_;
    return *this;
  }

  constexpr bool operator==(const TableEntry& rhs) const {
    return grade_ == rhs.grade_ && quadratic_multiplier_ == rhs.quadratic_multiplier_;
  }

  constexpr uint64_t grade() const { return grade_; }
  constexpr int8_t quadratic_multiplier() const { return quadratic_multiplier_; }
};

template <size_t COMPONENT_COUNT>
std::string to_string(const TableEntry<COMPONENT_COUNT>& t) {
  using std::to_string;
  return std::string{}
      .append("(")  //
      .append(to_string(t.grade()))
      .append(", ")
      .append(to_string(t.quadratic_multiplier()))
      .append(")");
}

template <size_t COMPONENT_COUNT>
std::ostream& operator<<(std::ostream& os, const TableEntry<COMPONENT_COUNT>& t) {
  os << to_string(t);
  return os;
}

template <size_t POSITIVE_BASES, size_t NEGATIVE_BASES, size_t ZERO_BASES>
class CayleyTable;

template <size_t POSITIVE_BASES, size_t NEGATIVE_BASES, size_t ZERO_BASES>
std::string to_string(const CayleyTable<POSITIVE_BASES, NEGATIVE_BASES, ZERO_BASES>& t);

template <size_t POSITIVE_BASES, size_t NEGATIVE_BASES, size_t ZERO_BASES>
class CayleyTable final {
 public:
  static constexpr size_t QUADRATIC_FORM_COUNT{POSITIVE_BASES + NEGATIVE_BASES + ZERO_BASES};
  static constexpr size_t COMPONENT_COUNT{1UL << QUADRATIC_FORM_COUNT};

  // Note: failures in following situation can be avoided by templating the TableEntry class on the
  // number of grades/bases and using different storage sizes as needed.
  static_assert(COMPONENT_COUNT <= TableEntry<COMPONENT_COUNT>::MAX_COMPONENT_COUNT,
                "TableEntry cannot handle the number of grades required for this Cayley table.");

  static constexpr size_t SCALAR_GRADE{0};

 private:
  using Table =
      std::array<std::array<TableEntry<COMPONENT_COUNT>, COMPONENT_COUNT>, COMPONENT_COUNT>;

  static constexpr Table generate_table() {
    Table result{};

    CayleyEntryCalculator<POSITIVE_BASES, NEGATIVE_BASES, ZERO_BASES> entry_calculator{};

    for (size_t i = 0; i < COMPONENT_COUNT; ++i) {
      for (size_t j = 0; j < COMPONENT_COUNT; ++j) {
        const BitSet<QUADRATIC_FORM_COUNT> lhs_component_bits{i};
        const BitSet<QUADRATIC_FORM_COUNT> rhs_component_bits{j};

        result.at(i).at(j) = TableEntry<COMPONENT_COUNT>{
            entry_calculator.compute_result_component(i, j),
            entry_calculator.compute_commutative_order(lhs_component_bits, rhs_component_bits)};
      }
    }

    return result;
  }

  Table table_{generate_table()};

  friend std::string to_string<>(const CayleyTable<POSITIVE_BASES, NEGATIVE_BASES, ZERO_BASES>&);

 public:
  constexpr CayleyTable() = default;

  constexpr const TableEntry<COMPONENT_COUNT>& entry(size_t lhs_component,
                                                     size_t rhs_component) const {
    return table_.at(lhs_component).at(rhs_component);
  }
};

template <size_t POSITIVE_BASES, size_t NEGATIVE_BASES, size_t ZERO_BASES>
std::string to_string(const CayleyTable<POSITIVE_BASES, NEGATIVE_BASES, ZERO_BASES>& t) {
  using std::to_string;
  static constexpr size_t COMPONENT_COUNT{
      CayleyTable<POSITIVE_BASES, NEGATIVE_BASES, ZERO_BASES>::COMPONENT_COUNT};

  std::string result{};
  result.append("\n<\n");
  for (size_t i = 0; i < COMPONENT_COUNT; ++i) {
    const std::array<TableEntry<COMPONENT_COUNT>, COMPONENT_COUNT>& row{t.table_.at(i)};
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

template <size_t POSITIVE_BASES, size_t NEGATIVE_BASES, size_t ZERO_BASES>
std::ostream& operator<<(std::ostream& os,
                         const CayleyTable<POSITIVE_BASES, NEGATIVE_BASES, ZERO_BASES>& t) {
  os << to_string(t);
  return os;
}

using ScalarCayleyTable = CayleyTable<0, 0, 0>;
using ComplexCayleyTable = CayleyTable<0, 1, 0>;
using DualCayleyTable = CayleyTable<0, 0, 1>;
using SplitComplexCayleyTable = CayleyTable<1, 0, 0>;
using SpacetimeCayleyTable = CayleyTable<1, 3, 0>;

}  // namespace ndyn::math
