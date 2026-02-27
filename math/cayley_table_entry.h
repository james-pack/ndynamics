#pragma once

#include <array>
#include <cstdint>
#include <limits>
#include <ostream>
#include <string>

#include "math/bit_set.h"

namespace ndyn::math {

/**
 * The Cayley TableEntry class encodes the result of the product of two unit basis blades. The
 * CayleyTable below includes TableEntries for the product of every unit basis blade in the
 * algebra. It includes the index of the result basis blade, and it includes the structure constant,
 * which is the scalar value of the multiplication. The structure constant of the result will always
 * be [-1, 0, 1], according to the quadratic form of the vector space and the permutation sign of
 * commutativity of the two bases. For example, in Cl(2,0), with e1*e1=1, e2*e2=1, and
 * e1*e2=e12=-e2*e1, e12*e12=-1. The structure constant for e12*e12 is -1. Similarly, in
 * Cl(1, 0, 1), e1*e1=1 (the positive basis), e0*e0=0 (the zero basis), so e01*e01=0. The structure
 * constant for e01*e01 is 0.
 *
 * See https://en.wikipedia.org/wiki/Structure_constants for more information.
 *
 * Implementation details.
 * The TableEntry class is templated on the total number of basis blades in the algebra. This design
 * allows us to use smaller basis index values when the number of basis blades is small. In the
 * CayleyTable, this difference might be noticeable.
 */
template <size_t NUM_BASIS_BLADES, typename = void>
struct TableEntry final {
  // The basis index indicates which basis blade will be the result of the product of the basis
  // blades of this table entry.
  uint8_t basis_index{};
  int8_t structure_constant{};

  static constexpr size_t MAX_BASIS_BLADES{std::numeric_limits<decltype(basis_index)>::max()};
  static_assert(NUM_BASIS_BLADES <= MAX_BASIS_BLADES,
                "This TableEntry class definition does not support enough components. Note that "
                "this condition should not trigger; the design includes another template for table "
                "entries where the component count is larger.");

  constexpr bool operator==(const TableEntry& rhs) const {
    return basis_index == rhs.basis_index && structure_constant == rhs.structure_constant;
  }
};

template <size_t NUM_BASIS_BLADES>
struct TableEntry<NUM_BASIS_BLADES, std::enable_if_t<(NUM_BASIS_BLADES >= 8)> > final {
  uint64_t basis_index{};
  int8_t structure_constant{};

  static constexpr size_t MAX_BASIS_BLADES{std::numeric_limits<decltype(basis_index)>::max()};
  static_assert(NUM_BASIS_BLADES <= MAX_BASIS_BLADES,
                "This TableEntry class definition does not support enough components.");

  constexpr bool operator==(const TableEntry& rhs) const {
    return basis_index == rhs.basis_index && structure_constant == rhs.structure_constant;
  }
};

namespace {

template <size_t N>
constexpr BitSet<N> create_mask_below_bit(size_t bit) {
  return {(1UL << bit) - 1};
}

template <>
constexpr BitSet<0> create_mask_below_bit<0>(size_t bit) {
  return {};
}

template <size_t N>
constexpr size_t count_bits_within_mask(const BitSet<N>& bits, const BitSet<N>& mask) {
  const BitSet<N> masked{bits & mask};
  return masked.count();
}

template <size_t NUM_POSITIVE_BASES, size_t NUM_NEGATIVE_BASES, size_t NUM_ZERO_BASES>
static constexpr BitSet<NUM_POSITIVE_BASES + NUM_NEGATIVE_BASES + NUM_ZERO_BASES>
positive_bases_bitmask() {
  return BitSet<NUM_POSITIVE_BASES + NUM_NEGATIVE_BASES + NUM_ZERO_BASES>::create_mask(
      NUM_POSITIVE_BASES);
}

template <size_t NUM_POSITIVE_BASES, size_t NUM_NEGATIVE_BASES, size_t NUM_ZERO_BASES>
static constexpr BitSet<NUM_POSITIVE_BASES + NUM_NEGATIVE_BASES + NUM_ZERO_BASES>
negative_bases_bitmask() {
  return BitSet<NUM_POSITIVE_BASES + NUM_NEGATIVE_BASES + NUM_ZERO_BASES>::create_mask(
      NUM_NEGATIVE_BASES, NUM_POSITIVE_BASES);
}

template <size_t NUM_POSITIVE_BASES, size_t NUM_NEGATIVE_BASES, size_t NUM_ZERO_BASES>
static constexpr BitSet<NUM_POSITIVE_BASES + NUM_NEGATIVE_BASES + NUM_ZERO_BASES>
zero_bases_bitmask() {
  return BitSet<NUM_POSITIVE_BASES + NUM_NEGATIVE_BASES + NUM_ZERO_BASES>::create_mask(
      NUM_ZERO_BASES, NUM_POSITIVE_BASES + NUM_NEGATIVE_BASES);
}

template <size_t NUM_BASIS_VECTORS, size_t current_bit>
constexpr int8_t accumulate_commutative_order(const BitSet<NUM_BASIS_VECTORS>& lhs_component_bits,
                                              const BitSet<NUM_BASIS_VECTORS>& rhs_component_bits,
                                              int8_t accumulated_order) {
  if constexpr (NUM_BASIS_VECTORS == 0 or current_bit == 0) {
    return accumulated_order;
  } else {
    if (lhs_component_bits.test(current_bit)) {
      const BitSet<NUM_BASIS_VECTORS> mask{create_mask_below_bit<NUM_BASIS_VECTORS>(current_bit)};
      if (count_bits_within_mask(rhs_component_bits, mask) % 2 == 1) {
        return accumulate_commutative_order<NUM_BASIS_VECTORS, current_bit - 1>(
            lhs_component_bits, rhs_component_bits, -1 * accumulated_order);
      }
    }
    return accumulate_commutative_order<NUM_BASIS_VECTORS, current_bit - 1>(
        lhs_component_bits, rhs_component_bits, accumulated_order);
  }
}

}  // namespace

template <size_t NUM_POSITIVE_BASES, size_t NUM_NEGATIVE_BASES, size_t NUM_ZERO_BASES>
class CayleyEntryCalculator final {
 public:
  static constexpr size_t NUM_BASIS_VECTORS{NUM_POSITIVE_BASES + NUM_NEGATIVE_BASES +
                                            NUM_ZERO_BASES};
  static constexpr size_t NUM_BASIS_BLADES{1UL << NUM_BASIS_VECTORS};

  constexpr int8_t compute_structure_constant(const BitSet<NUM_BASIS_VECTORS>& lhs_bases,
                                              const BitSet<NUM_BASIS_VECTORS>& rhs_bases) const {
    // Track which basis vectors occur in both the lhs and rhs. These basis vectors will multiply
    // themselves (hence the name of self_multiplication), and the result will of that
    // multiplication will need to consider the quadratic form of the vector space of the algebra.
    // Other multiplications where the lhs and rhs do not share any basis vectors in common only
    // need to consider commutativity.
    const BitSet<NUM_BASIS_VECTORS> self_multiplication{lhs_bases & rhs_bases};

    if ((self_multiplication &
         zero_bases_bitmask<NUM_POSITIVE_BASES, NUM_NEGATIVE_BASES, NUM_ZERO_BASES>())
            .count() != 0) {
      // We are self-multiplying at least one zero basis vector. The result of this multiplication
      // will be zero.
      return 0;
    } else {
      if ((self_multiplication &
           negative_bases_bitmask<NUM_POSITIVE_BASES, NUM_NEGATIVE_BASES, NUM_ZERO_BASES>())
                  .count() %
              2 ==
          1) {
        // We are self-multiplying an odd number of negative basis vectors. -1 to an odd power is
        // -1, so we start with that as our result and adjust according to the anti-commutivity of
        // the bases in the operands.
        constexpr int8_t INITIAL_COMMUTATIVE_ORDER{-1};
        return accumulate_commutative_order<NUM_BASIS_VECTORS, NUM_BASIS_VECTORS - 1>(
            lhs_bases, rhs_bases, INITIAL_COMMUTATIVE_ORDER);
      } else {
        // We are self-multiplying an even number of negative basis vectors, possibly zero. That is,
        // all of the minus signs from the quadratic form cancel, and the entire result of
        // multiplying these basis vectors is determined by the anti-commutation of bases.
        constexpr int8_t INITIAL_COMMUTATIVE_ORDER{1};
        return accumulate_commutative_order<NUM_BASIS_VECTORS, NUM_BASIS_VECTORS - 1>(
            lhs_bases, rhs_bases, INITIAL_COMMUTATIVE_ORDER);
      }
    }
  }

  constexpr uint8_t compute_result_component(const BitSet<NUM_BASIS_VECTORS>& lhs_bases,
                                             const BitSet<NUM_BASIS_VECTORS>& rhs_bases) const {
    return (lhs_bases ^ rhs_bases).to_ulong();
  }

  constexpr TableEntry<NUM_BASIS_BLADES> calculate_entry(size_t left_basis_blade,
                                                         size_t right_basis_blade) const {
    const BitSet<NUM_BASIS_VECTORS> lhs_component_bits{left_basis_blade};
    const BitSet<NUM_BASIS_VECTORS> rhs_component_bits{right_basis_blade};

    return {compute_result_component(lhs_component_bits, rhs_component_bits),
            compute_structure_constant(lhs_component_bits, rhs_component_bits)};
  }
};

template <size_t NUM_BASIS_BLADES>
std::string to_string(const TableEntry<NUM_BASIS_BLADES>& t) {
  using std::to_string;
  return std::string{}
      .append("(")  //
      .append(to_string(t.basis_index))
      .append(", ")
      .append(to_string(t.structure_constant))
      .append(")");
}

template <size_t NUM_BASIS_BLADES>
std::ostream& operator<<(std::ostream& os, const TableEntry<NUM_BASIS_BLADES>& t) {
  os << to_string(t);
  return os;
}

}  // namespace ndyn::math
