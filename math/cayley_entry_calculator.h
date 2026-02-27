#pragma once

#include <cstdint>

#include "math/bit_set.h"

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

template <size_t NUM_POSITIVE_BASES, size_t NUM_NEGATIVE_BASES, size_t NUM_ZERO_BASES>
static constexpr BitSet<NUM_POSITIVE_BASES + NUM_NEGATIVE_BASES + NUM_ZERO_BASES> positive_bases_bitmask() {
  return BitSet<NUM_POSITIVE_BASES + NUM_NEGATIVE_BASES + NUM_ZERO_BASES>::create_mask(NUM_POSITIVE_BASES);
}

template <size_t NUM_POSITIVE_BASES, size_t NUM_NEGATIVE_BASES, size_t NUM_ZERO_BASES>
static constexpr BitSet<NUM_POSITIVE_BASES + NUM_NEGATIVE_BASES + NUM_ZERO_BASES> negative_bases_bitmask() {
  return BitSet<NUM_POSITIVE_BASES + NUM_NEGATIVE_BASES + NUM_ZERO_BASES>::create_mask(NUM_NEGATIVE_BASES,
                                                                           NUM_POSITIVE_BASES);
}

template <size_t NUM_POSITIVE_BASES, size_t NUM_NEGATIVE_BASES, size_t NUM_ZERO_BASES>
static constexpr BitSet<NUM_POSITIVE_BASES + NUM_NEGATIVE_BASES + NUM_ZERO_BASES> zero_bases_bitmask() {
  return BitSet<NUM_POSITIVE_BASES + NUM_NEGATIVE_BASES + NUM_ZERO_BASES>::create_mask(
      NUM_ZERO_BASES, NUM_POSITIVE_BASES + NUM_NEGATIVE_BASES);
}

template <size_t QUADRATIC_FORM_COUNT, size_t current_bit>
constexpr int8_t accumulate_commutative_order(
    const BitSet<QUADRATIC_FORM_COUNT>& lhs_component_bits,
    const BitSet<QUADRATIC_FORM_COUNT>& rhs_component_bits, int8_t accumulated_order) {
  if constexpr (QUADRATIC_FORM_COUNT == 0 or current_bit == 0) {
    return accumulated_order;
  } else {
    if (lhs_component_bits.test(current_bit)) {
      const BitSet<QUADRATIC_FORM_COUNT> mask{
          create_mask_below_bit<QUADRATIC_FORM_COUNT>(current_bit)};
      if (count_bits_within_mask(rhs_component_bits, mask) % 2 == 1) {
        return accumulate_commutative_order<QUADRATIC_FORM_COUNT, current_bit - 1>(
            lhs_component_bits, rhs_component_bits, -1 * accumulated_order);
      }
    }
    return accumulate_commutative_order<QUADRATIC_FORM_COUNT, current_bit - 1>(
        lhs_component_bits, rhs_component_bits, accumulated_order);
  }
}

}  // namespace

template <size_t NUM_POSITIVE_BASES, size_t NUM_NEGATIVE_BASES, size_t NUM_ZERO_BASES>
class CayleyEntryCalculator final {
 public:
  static constexpr size_t QUADRATIC_FORM_COUNT{NUM_POSITIVE_BASES + NUM_NEGATIVE_BASES + NUM_ZERO_BASES};

  constexpr int8_t compute_commutative_order(const BitSet<QUADRATIC_FORM_COUNT>& lhs_bases,
                                             const BitSet<QUADRATIC_FORM_COUNT>& rhs_bases) const {
    // Track which basis vectors occur in both the lhs and rhs. These basis vectors will multiply
    // themselves (hence the name of self_multiplication), and the result will of that
    // multiplication will need to consider the quadratic form of the vector space of the algebra.
    // Other multiplications where the lhs and rhs do not share any basis vectors in common only
    // need to consider commutativity.
    const BitSet<QUADRATIC_FORM_COUNT> self_multiplication{lhs_bases & rhs_bases};

    if ((self_multiplication & zero_bases_bitmask<NUM_POSITIVE_BASES, NUM_NEGATIVE_BASES, NUM_ZERO_BASES>())
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
        return accumulate_commutative_order<QUADRATIC_FORM_COUNT, QUADRATIC_FORM_COUNT - 1>(
            lhs_bases, rhs_bases, INITIAL_COMMUTATIVE_ORDER);
      } else {
        // We are self-multiplying an even number of negative basis vectors, possibly zero. That is,
        // all of the minus signs from the quadratic form cancel, and the entire result of
        // multiplying these basis vectors is determined by the anti-commutation of bases.
        constexpr int8_t INITIAL_COMMUTATIVE_ORDER{1};
        return accumulate_commutative_order<QUADRATIC_FORM_COUNT, QUADRATIC_FORM_COUNT - 1>(
            lhs_bases, rhs_bases, INITIAL_COMMUTATIVE_ORDER);
      }
    }
  }

  constexpr uint8_t compute_result_component(const BitSet<QUADRATIC_FORM_COUNT>& lhs_bases,
                                             const BitSet<QUADRATIC_FORM_COUNT>& rhs_bases) const {
    return (lhs_bases xor rhs_bases).to_ulong();
  }
};

}  // namespace ndyn::math
