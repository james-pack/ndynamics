#pragma once

#ifdef __has_include
#if __has_include(<numbers>)
#include <numbers>
#endif
#endif

namespace ndyn {

// If possible, we want to use the std values for pi, but these are only available in C++20 and
// later.
#if __cpp_lib_math_constants >= 201907L

template <typename T>
inline constexpr T pi_v{std::numbers::pi_v<T>};

inline constexpr double pi{std::numbers::pi};

#else

template <typename T>
inline constexpr T pi_v;

template <>
inline constexpr float pi_v<float>{3.141592653589793238462643383279502884L};

template <>
inline constexpr double pi_v<double>{3.141592653589793238462643383279502884L};

inline constexpr double pi{pi_v<double>};

#endif

}  // namespace ndyn
