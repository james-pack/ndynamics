#pragma once

#include <cstdint>

#ifdef __has_include
#if __has_include(<bit>)
#include <bit>
#endif
#endif

namespace ndyn {

namespace internal {

// If possible, we want to use the std method for computing the bit width, but it is only available
// in C++20.
#if __cpp_lib_int_pow2 >= 202002L
inline constexpr std::size_t number_of_bits(std::size_t x) noexcept { return std::bit_width(x); }
#else
inline constexpr std::size_t number_of_bits(std::size_t x) noexcept {
  return x < 2 ? x : 1 + number_of_bits(x >> 1);
}
#endif

// If possible, we want to use the std method for computing the number of bits in a value, but it is
// only available in C++20.
#if __cpp_lib_int_pow2 >= 202002L
inline constexpr std::size_t bit_count(std::size_t x) noexcept { return std::popcount(x); }
#else
inline constexpr std::size_t bit_count(std::size_t x) noexcept {
  std::size_t result{};
  while (x > 0) {
    ++result;
    x = x & (x - 1);
  }
  return result;
}
#endif

}  // namespace internal

/**
 * Compute the number of bits required to store the given value.
 */
inline constexpr uint8_t bit_width(std::size_t value) {
  return static_cast<uint8_t>(internal::number_of_bits(value));
}

/**
 * Compute the number of bits set in the given value.
 */
inline constexpr uint8_t bit_count(std::size_t value) {
  return static_cast<uint8_t>(internal::bit_count(value));
}

}  // namespace ndyn
