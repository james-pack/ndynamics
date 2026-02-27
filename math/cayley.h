#pragma once

#include <array>
#include <cstdint>
#include <limits>
#include <ostream>
#include <string>

#include "math/bit_set.h"
#include "math/cayley_table_entry.h"

namespace ndyn::math {

template <size_t NUM_POSITIVE_BASES, size_t NUM_NEGATIVE_BASES, size_t NUM_ZERO_BASES>
class CayleyTable;

template <size_t NUM_POSITIVE_BASES, size_t NUM_NEGATIVE_BASES, size_t NUM_ZERO_BASES>
std::string to_string(const CayleyTable<NUM_POSITIVE_BASES, NUM_NEGATIVE_BASES, NUM_ZERO_BASES>& t);

template <size_t NUM_POSITIVE_BASES, size_t NUM_NEGATIVE_BASES, size_t NUM_ZERO_BASES>
class CayleyTable final {
 public:
  static constexpr size_t NUM_BASIS_VECTORS{NUM_POSITIVE_BASES + NUM_NEGATIVE_BASES +
                                            NUM_ZERO_BASES};
  static constexpr size_t NUM_BASIS_BLADES{1UL << NUM_BASIS_VECTORS};

  // Note: failures in following situation can be avoided by templating the TableEntry class on the
  // number of grades/bases and using different storage sizes as needed.
  static_assert(NUM_BASIS_BLADES <= TableEntry<NUM_BASIS_BLADES>::MAX_BASIS_BLADES,
                "TableEntry cannot handle the number of grades required for this Cayley table.");

  static constexpr size_t SCALAR_GRADE{0};

 private:
  using Table =
      std::array<std::array<TableEntry<NUM_BASIS_BLADES>, NUM_BASIS_BLADES>, NUM_BASIS_BLADES>;

  static constexpr Table generate_table() {
    Table result{};

    CayleyEntryCalculator<NUM_POSITIVE_BASES, NUM_NEGATIVE_BASES, NUM_ZERO_BASES>
        entry_calculator{};

    for (size_t i = 0; i < NUM_BASIS_BLADES; ++i) {
      for (size_t j = 0; j < NUM_BASIS_BLADES; ++j) {
        result.at(i).at(j) = entry_calculator.calculate_entry(i, j);
      }
    }

    return result;
  }

  Table table_{generate_table()};

  friend std::string to_string<>(
      const CayleyTable<NUM_POSITIVE_BASES, NUM_NEGATIVE_BASES, NUM_ZERO_BASES>&);

 public:
  constexpr CayleyTable() = default;

  constexpr const TableEntry<NUM_BASIS_BLADES>& entry(size_t lhs_component,
                                                      size_t rhs_component) const {
    return table_.at(lhs_component).at(rhs_component);
  }
};

template <size_t NUM_POSITIVE_BASES, size_t NUM_NEGATIVE_BASES, size_t NUM_ZERO_BASES>
std::string to_string(
    const CayleyTable<NUM_POSITIVE_BASES, NUM_NEGATIVE_BASES, NUM_ZERO_BASES>& t) {
  using std::to_string;
  static constexpr size_t NUM_BASIS_BLADES{
      CayleyTable<NUM_POSITIVE_BASES, NUM_NEGATIVE_BASES, NUM_ZERO_BASES>::NUM_BASIS_BLADES};

  std::string result{};
  result.append("\n<\n");
  for (size_t i = 0; i < NUM_BASIS_BLADES; ++i) {
    const std::array<TableEntry<NUM_BASIS_BLADES>, NUM_BASIS_BLADES>& row{t.table_.at(i)};
    result.append("\t<");
    bool need_comma{false};
    for (size_t j = 0; j < NUM_BASIS_BLADES; ++j) {
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

template <size_t NUM_POSITIVE_BASES, size_t NUM_NEGATIVE_BASES, size_t NUM_ZERO_BASES>
std::ostream& operator<<(
    std::ostream& os,
    const CayleyTable<NUM_POSITIVE_BASES, NUM_NEGATIVE_BASES, NUM_ZERO_BASES>& t) {
  os << to_string(t);
  return os;
}

}  // namespace ndyn::math
