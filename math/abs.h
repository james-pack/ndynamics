#pragma once

#include <cstdlib>
#include <type_traits>

namespace ndyn::math {

template <typename T>
[[nodiscard]] constexpr T abs(T val) noexcept {
  // C++23: std::abs is constexpr. Use that implementation.
#if defined(__cpp_lib_constexpr_cmath) && __cpp_lib_constexpr_cmath >= 202202L
  return std::abs(val);
#else
  // Pre-C++23: roll our own constexpr abs.
  static_assert(std::is_arithmetic_v<T>, "abs requires an arithmetic type");
  return val < T{0} ? -val : val;
#endif
}

}  // namespace ndyn::math
