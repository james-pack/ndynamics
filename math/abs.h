#pragma once

#include <algorithm>
#include <cmath>
#include <concepts>
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

namespace impl {

// Stable 3-argument hypot.
constexpr std::floating_point auto hypot_impl_3(std::floating_point auto x,
                                                std::floating_point auto y,
                                                std::floating_point auto z) {
  using T = std::common_type_t<decltype(x), decltype(y), decltype(z)>;
  const T ax = abs(static_cast<T>(x));
  const T ay = abs(static_cast<T>(y));
  const T az = abs(static_cast<T>(z));

  const T m = std::max({ax, ay, az});
  if (m == 0) return T{0};

  const T rx = ax / m;
  const T ry = ay / m;
  const T rz = az / m;

  return m * std::sqrt(rx * rx + ry * ry + rz * rz);
}

// Stable 4-argument hypot.
constexpr std::floating_point auto hypot_impl_4(std::floating_point auto x,
                                                std::floating_point auto y,
                                                std::floating_point auto z,
                                                std::floating_point auto w) {
  using T = std::common_type_t<decltype(x), decltype(y), decltype(z), decltype(w)>;
  const T ax = abs(static_cast<T>(x));
  const T ay = abs(static_cast<T>(y));
  const T az = abs(static_cast<T>(z));
  const T aw = abs(static_cast<T>(w));

  const T m = std::max({ax, ay, az, aw});
  if (m == 0) return T{0};

  const T rx = ax / m;
  const T ry = ay / m;
  const T rz = az / m;
  const T rw = aw / m;

  return m * std::sqrt(rx * rx + ry * ry + rz * rz + rw * rw);
}

}  // namespace impl

/**
 * Modern C++20 variadic hypotenuse.
 * Dispatches to optimized implementations for 3 and 4 arguments.
 */
constexpr std::floating_point auto hypot(std::floating_point auto... args)
  requires(sizeof...(args) >= 3)
{
  using T = std::common_type_t<decltype(args)...>;

#if defined(__cpp_lib_constexpr_cmath) && __cpp_lib_constexpr_cmath >= 202202L
  return std::hypot(static_cast<T>(args)...);
#else
  constexpr size_t N = sizeof...(args);

  if constexpr (N == 3) {
    return impl::hypot_impl_3(args...);
  } else if constexpr (N == 4) {
    return impl::hypot_impl_4(args...);
  } else {
    const T m = std::max({static_cast<T>(abs(args))...});
    if (m == 0) return T{0};

    const T sum_sq = (((static_cast<T>(args) / m) * (static_cast<T>(args) / m)) + ...);
    return m * std::sqrt(sum_sq);
  }
#endif
}

}  // namespace ndyn::math
