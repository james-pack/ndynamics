#pragma once

#include <cstdint>
#include <limits>
#include <ostream>
#include <stdexcept>
#include <string>

#include "base/except.h"

namespace ndyn::math {

template <size_t N>
class BitSetT final {
 private:
  constexpr uint64_t masked_bits() const {
    if constexpr (N < MAX_SIZE) {
      return bits_ & ((1UL << N) - 1);
    } else {
      return bits_;
    }
  }

  uint64_t bits_{};

 public:
  static constexpr size_t MAX_SIZE{8 * sizeof(uint64_t)};

  static_assert(N <= MAX_SIZE,
                "Can't handle BitSets larger than the number of bits in an uint64_t");

  static constexpr BitSetT create_mask(size_t width = N, size_t left_shift = 0) {
    if (width + left_shift < N) {
      return BitSetT{((1UL << width) - 1) << left_shift};
    } else {
      return BitSetT{std::numeric_limits<uint64_t>::max() << left_shift};
    }
  }

  constexpr BitSetT() = default;

  constexpr BitSetT(uint64_t b) : bits_(b) {}

  template <size_t M>
  constexpr BitSetT(const BitSetT<M>& rhs) : bits_(rhs.bits_) {}

  template <size_t M>
  constexpr BitSetT(BitSetT<M>&& rhs) : bits_(rhs.bits_) {}

  template <size_t M>
  constexpr BitSetT& operator=(const BitSetT<M>& rhs) {
    bits_ = rhs.bits_;
    return *this;
  }

  template <size_t M>
  constexpr BitSetT& operator=(BitSetT<M>&& rhs) {
    bits_ = rhs.bits_;
    return *this;
  }

  constexpr bool operator==(const BitSetT& rhs) const { return masked_bits() == rhs.masked_bits(); }

  constexpr bool test(size_t bit) const {
    if (bit >= MAX_SIZE) {
      except<std::out_of_range>("Attempt to test bit out of range");
    }
    return (1UL << bit) & bits_;
  }

  constexpr void set(size_t bit) {
    if (bit >= MAX_SIZE) {
      except<std::out_of_range>("Attempt to set bit out of range");
    }
    bits_ |= (1UL << bit);
  }

  constexpr size_t count() const {
    size_t result{};
    for (size_t i = 0; i < N; ++i) {
      if (test(i)) {
        ++result;
      }
    }
    return result;
  }

  constexpr bool is_zero() const {
    for (size_t i = 0; i < N; ++i) {
      if (test(i)) {
        return false;
      }
    }
    return true;
  }

  constexpr size_t size() const { return N; }

  // Bit-wise operators.
  // NOT
  constexpr BitSetT operator~() const { return BitSetT{~bits_}; }
  // OR
  constexpr BitSetT operator|(const BitSetT& rhs) const { return BitSetT{bits_ | rhs.bits_}; }
  constexpr BitSetT operator|(uint64_t rhs) const { return BitSetT{bits_ | rhs}; }
  // AND
  constexpr BitSetT operator&(const BitSetT& rhs) const { return BitSetT{bits_ & rhs.bits_}; }
  constexpr BitSetT operator&(uint64_t rhs) const { return BitSetT{bits_ & rhs}; }
  // XOR
  constexpr BitSetT operator^(const BitSetT& rhs) const { return bits_ ^ rhs.bits_; }

  constexpr BitSetT operator<<(size_t b) const { return BitSetT{bits_ << b}; }
  constexpr BitSetT operator>>(size_t b) const { return BitSetT{masked_bits() >> b}; }

  constexpr BitSetT& operator<<=(size_t b) {
    bits_ <<= b;
    return *this;
  }

  constexpr BitSetT& operator>>=(size_t b) {
    bits_ = masked_bits() >> b;
    return *this;
  }

  constexpr uint64_t to_ulong() const { return static_cast<uint64_t>(masked_bits()); }
};

template <size_t N>
std::string to_string(const BitSetT<N>& b) {
  std::string result{};
  result.reserve(N + 2);
  result.append("0b");
  for (size_t i = 0; i < N; ++i) {
    result.append(b.test(i) ? "1" : "0");
  }
  return result;
}

template <size_t N>
std::ostream& operator<<(std::ostream& os, const BitSetT<N>& b) {
  os << to_string(b);
  return os;
}

// TODO(james): Consider replacing with std::bitset when we switch to C++23.
template <size_t N>
using BitSet = BitSetT<N>; /* using BitSet = std::bitset<N>; */

}  // namespace ndyn::math
