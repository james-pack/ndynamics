#pragma once

#include <cstring>
#include <limits>
#include <ostream>
#include <stdexcept>
#include <string>

#include "base/except.h"

namespace ndyn::math {

template <size_t N>
class BitSetT final {
 private:
  constexpr unsigned long masked_bits() const {
    if constexpr (N < MAX_SIZE) {
      return bits & ((1UL << N) - 1);
    } else {
      return bits;
    }
  }

 public:
  static constexpr size_t MAX_SIZE{8 * sizeof(unsigned long)};

  static_assert(N <= MAX_SIZE,
                "Can't handle BitSets larger than the number of bits in an unsigned long");

  static constexpr BitSetT create_mask(size_t width = N, size_t left_shift = 0) {
    if (width + left_shift < N) {
      return BitSetT{((1UL << width) - 1) << left_shift};
    } else {
      return BitSetT{std::numeric_limits<unsigned long>::max() << left_shift};
    }
  }

  constexpr BitSetT() = default;

  template <size_t M>
  constexpr BitSetT(const BitSetT<M>& rhs) : bits(rhs.bits) {}

  template <size_t M>
  constexpr BitSetT(BitSetT<M>&& rhs) : bits(rhs.bits) {}

  constexpr BitSetT(unsigned long b) : bits(b) {}

  template <size_t M>
  constexpr BitSetT& operator=(const BitSetT<M>& rhs) {
    bits = rhs.bits;
    return *this;
  }

  template <size_t M>
  constexpr BitSetT& operator=(BitSetT<M>&& rhs) {
    bits = rhs.bits;
    return *this;
  }

  constexpr bool operator==(const BitSetT& rhs) const { return masked_bits() == rhs.masked_bits(); }

  constexpr BitSetT operator xor(const BitSetT& rhs) const { return bits xor rhs.bits; }

  constexpr bool test(size_t bit) const {
    if (bit >= MAX_SIZE) {
      except<std::out_of_range>("Attempt to test bit out of range");
    }
    return (1UL << bit) & bits;
  }

  constexpr void set(size_t bit) {
    if (bit >= MAX_SIZE) {
      except<std::out_of_range>("Attempt to set bit out of range");
    }
    bits |= (1UL << bit);
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

  constexpr size_t size() const { return N; }

  constexpr BitSetT operator~() const { return BitSetT{~bits}; }
  constexpr BitSetT operator|(const BitSetT& rhs) const { return BitSetT{bits | rhs.bits}; }
  constexpr BitSetT operator|(unsigned long rhs) const { return BitSetT{bits | rhs}; }
  constexpr BitSetT operator&(const BitSetT& rhs) const { return BitSetT{bits & rhs.bits}; }
  constexpr BitSetT operator&(unsigned long rhs) const { return BitSetT{bits & rhs}; }

  constexpr BitSetT operator<<(size_t b) const { return BitSetT{bits << b}; }
  constexpr BitSetT operator>>(size_t b) const { return BitSetT(masked_bits() >> b); }

  constexpr BitSetT& operator<<=(size_t b) {
    bits <<= b;
    return *this;
  }

  constexpr BitSetT& operator>>=(size_t b) {
    bits = masked_bits() >> b;
    return *this;
  }

  constexpr unsigned long to_ulong() const { return static_cast<unsigned long>(masked_bits()); }

  unsigned long bits{};
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
