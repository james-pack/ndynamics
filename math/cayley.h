#pragma once

#include <array>
#include <cstdint>
#include <limits>
#include <ostream>
#include <string>

#include "math/bit_set.h"
#include "math/cayley_entry_calculator.h"

namespace ndyn::math {

/**
 * The Cayley TableEntry class encodes the result of the product of two unit basis vectors. The
 * CayleyTable below includes TableEntries for the product of every unit basis vector in the
 * algebra. It includes the index of the result basis vector, and it includes the scalar value of
 * the multiplication. The scalar value of the result will always be [-1, 0, 1], according to the
 * quadratic form of the vector space and the commutativity of the two bases. For example, in
 * Cl(2,0), e1*e1=1 and e2*e2=1, and e1*e2=e12=-e2*e1, and e12*e12=-1 due to the anti-commutation of
 * e1 and e2. The quadratic multiplier for e12*e12 is therefore -1. Similarly, in Cl(1, 0, 1),
 * e1*e1=1 (the positive basis), e0*e0=0 (the zero basis), and e0*e1=e01, but e01*e01=0. The
 * quadratic multiplier for e01*e01 is therefore 0.
 */
template <size_t COMPONENT_COUNT, typename = void>
class TableEntry final {
 private:
  // The basis index indicates which basis will be the result of the product of the unit basis
  // vectors of this table entry.
  uint8_t basis_index_{};

  // The quadratic multiplier indicates if the result of the product of unit basis vectors
  // represented by this table entry should be -1,0,1 according to the quadratic form of the vector
  // space along with the commutativity relationships. It is the scalar portion of the product of
  // the two unit basis vectors.
  int8_t quadratic_multiplier_{};

 public:
  static constexpr size_t MAX_COMPONENT_COUNT{std::numeric_limits<decltype(basis_index_)>::max()};
  static_assert(COMPONENT_COUNT <= MAX_COMPONENT_COUNT,
                "This TableEntry class definition does not support enough components. Note that "
                "this condition should not trigger; the design includes another template for table "
                "entries where the component count is larger.");

  constexpr TableEntry() = default;

  constexpr TableEntry(const TableEntry& rhs)
      : basis_index_(rhs.basis_index_), quadratic_multiplier_(rhs.quadratic_multiplier_) {}

  constexpr TableEntry(TableEntry&& rhs)
      : basis_index_(rhs.basis_index_), quadratic_multiplier_(rhs.quadratic_multiplier_) {}

  constexpr TableEntry(size_t b, int8_t q) : basis_index_(b), quadratic_multiplier_(q) {
    // Note that the CayleyTable class has defenses to avoid basis indices that would overflow the
    // basis_index_ data member, so we do not add extra checks to verify that this method is called
    // with valid parameters.
  }

  constexpr TableEntry& operator=(TableEntry&& rhs) {
    basis_index_ = rhs.basis_index_;
    quadratic_multiplier_ = rhs.quadratic_multiplier_;
    return *this;
  }

  constexpr bool operator==(const TableEntry& rhs) const {
    return basis_index_ == rhs.basis_index_ && quadratic_multiplier_ == rhs.quadratic_multiplier_;
  }

  constexpr uint8_t basis_index() const { return basis_index_; }
  constexpr int8_t quadratic_multiplier() const { return quadratic_multiplier_; }
};

template <size_t COMPONENT_COUNT>
class TableEntry<COMPONENT_COUNT, std::enable_if_t<COMPONENT_COUNT >= 8> > final {
 private:
  uint64_t basis_index_{};
  int8_t quadratic_multiplier_{};

 public:
  static constexpr size_t MAX_COMPONENT_COUNT{std::numeric_limits<decltype(basis_index_)>::max()};
  static_assert(COMPONENT_COUNT <= MAX_COMPONENT_COUNT,
                "This TableEntry class definition does not support enough components.");

  constexpr TableEntry() = default;

  constexpr TableEntry(const TableEntry& rhs)
      : basis_index_(rhs.basis_index_), quadratic_multiplier_(rhs.quadratic_multiplier_) {}

  constexpr TableEntry(TableEntry&& rhs)
      : basis_index_(rhs.basis_index_), quadratic_multiplier_(rhs.quadratic_multiplier_) {}

  constexpr TableEntry(size_t b, int8_t q) : basis_index_(b), quadratic_multiplier_(q) {}

  constexpr TableEntry& operator=(TableEntry&& rhs) {
    basis_index_ = rhs.basis_index_;
    quadratic_multiplier_ = rhs.quadratic_multiplier_;
    return *this;
  }

  constexpr bool operator==(const TableEntry& rhs) const {
    return basis_index_ == rhs.basis_index_ && quadratic_multiplier_ == rhs.quadratic_multiplier_;
  }

  constexpr uint64_t basis_index() const { return basis_index_; }
  constexpr int8_t quadratic_multiplier() const { return quadratic_multiplier_; }
};

template <size_t COMPONENT_COUNT>
std::string to_string(const TableEntry<COMPONENT_COUNT>& t) {
  using std::to_string;
  return std::string{}
      .append("(")  //
      .append(to_string(t.basis_index()))
      .append(", ")
      .append(to_string(t.quadratic_multiplier()))
      .append(")");
}

template <size_t COMPONENT_COUNT>
std::ostream& operator<<(std::ostream& os, const TableEntry<COMPONENT_COUNT>& t) {
  os << to_string(t);
  return os;
}

template <size_t NUM_POSITIVE_BASES, size_t NUM_NEGATIVE_BASES, size_t NUM_ZERO_BASES>
class CayleyTable;

template <size_t NUM_POSITIVE_BASES, size_t NUM_NEGATIVE_BASES, size_t NUM_ZERO_BASES>
std::string to_string(const CayleyTable<NUM_POSITIVE_BASES, NUM_NEGATIVE_BASES, NUM_ZERO_BASES>& t);

template <size_t NUM_POSITIVE_BASES, size_t NUM_NEGATIVE_BASES, size_t NUM_ZERO_BASES>
class CayleyTable final {
 public:
  static constexpr size_t QUADRATIC_FORM_COUNT{NUM_POSITIVE_BASES + NUM_NEGATIVE_BASES + NUM_ZERO_BASES};
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

    CayleyEntryCalculator<NUM_POSITIVE_BASES, NUM_NEGATIVE_BASES, NUM_ZERO_BASES> entry_calculator{};

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

  friend std::string to_string<>(const CayleyTable<NUM_POSITIVE_BASES, NUM_NEGATIVE_BASES, NUM_ZERO_BASES>&);

 public:
  constexpr CayleyTable() = default;

  constexpr const TableEntry<COMPONENT_COUNT>& entry(size_t lhs_component,
                                                     size_t rhs_component) const {
    return table_.at(lhs_component).at(rhs_component);
  }
};

template <size_t NUM_POSITIVE_BASES, size_t NUM_NEGATIVE_BASES, size_t NUM_ZERO_BASES>
std::string to_string(const CayleyTable<NUM_POSITIVE_BASES, NUM_NEGATIVE_BASES, NUM_ZERO_BASES>& t) {
  using std::to_string;
  static constexpr size_t COMPONENT_COUNT{
      CayleyTable<NUM_POSITIVE_BASES, NUM_NEGATIVE_BASES, NUM_ZERO_BASES>::COMPONENT_COUNT};

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

template <size_t NUM_POSITIVE_BASES, size_t NUM_NEGATIVE_BASES, size_t NUM_ZERO_BASES>
std::ostream& operator<<(std::ostream& os,
                         const CayleyTable<NUM_POSITIVE_BASES, NUM_NEGATIVE_BASES, NUM_ZERO_BASES>& t) {
  os << to_string(t);
  return os;
}

}  // namespace ndyn::math
