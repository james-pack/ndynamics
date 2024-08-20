#pragma once

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

template <size_t POSITIVE_BASES, size_t NEGATIVE_BASES, size_t ZERO_BASES>
static constexpr BitSet<POSITIVE_BASES + NEGATIVE_BASES + ZERO_BASES> positive_bases_bitmask() {
  return BitSet<POSITIVE_BASES + NEGATIVE_BASES + ZERO_BASES>::create_mask(POSITIVE_BASES);
}

template <size_t POSITIVE_BASES, size_t NEGATIVE_BASES, size_t ZERO_BASES>
static constexpr BitSet<POSITIVE_BASES + NEGATIVE_BASES + ZERO_BASES> negative_bases_bitmask() {
  return BitSet<POSITIVE_BASES + NEGATIVE_BASES + ZERO_BASES>::create_mask(NEGATIVE_BASES,
                                                                           POSITIVE_BASES);
}

template <size_t POSITIVE_BASES, size_t NEGATIVE_BASES, size_t ZERO_BASES>
static constexpr BitSet<POSITIVE_BASES + NEGATIVE_BASES + ZERO_BASES> zero_bases_bitmask() {
  return BitSet<POSITIVE_BASES + NEGATIVE_BASES + ZERO_BASES>::create_mask(
      ZERO_BASES, POSITIVE_BASES + NEGATIVE_BASES);
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

template <size_t POSITIVE_BASES, size_t NEGATIVE_BASES, size_t ZERO_BASES>
class CayleyEntryCalculator final {
 public:
  static constexpr size_t QUADRATIC_FORM_COUNT{POSITIVE_BASES + NEGATIVE_BASES + ZERO_BASES};

  constexpr int8_t compute_commutative_order(
      const BitSet<QUADRATIC_FORM_COUNT>& lhs_component_bits,
      const BitSet<QUADRATIC_FORM_COUNT>& rhs_component_bits) const {
    const BitSet<QUADRATIC_FORM_COUNT> self_multiplication{lhs_component_bits & rhs_component_bits};

    if ((self_multiplication & zero_bases_bitmask<POSITIVE_BASES, NEGATIVE_BASES, ZERO_BASES>())
            .count() != 0) {
      return 0;
    } else {
      if ((self_multiplication &
           negative_bases_bitmask<POSITIVE_BASES, NEGATIVE_BASES, ZERO_BASES>())
                  .count() %
              2 ==
          1) {
        constexpr int8_t INITIAL_COMMUTATIVE_ORDER{-1};
        return accumulate_commutative_order<QUADRATIC_FORM_COUNT, QUADRATIC_FORM_COUNT - 1>(
            lhs_component_bits, rhs_component_bits, INITIAL_COMMUTATIVE_ORDER);
      } else {
        constexpr int8_t INITIAL_COMMUTATIVE_ORDER{1};
        return accumulate_commutative_order<QUADRATIC_FORM_COUNT, QUADRATIC_FORM_COUNT - 1>(
            lhs_component_bits, rhs_component_bits, INITIAL_COMMUTATIVE_ORDER);
      }
    }
  }

  constexpr uint8_t compute_result_component(
      const BitSet<QUADRATIC_FORM_COUNT>& lhs_component_bits,
      const BitSet<QUADRATIC_FORM_COUNT>& rhs_component_bits) const {
    return (lhs_component_bits xor rhs_component_bits).to_ulong();
  }
};

}  // namespace ndyn::math
