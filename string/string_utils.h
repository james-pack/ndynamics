#pragma once

#include <algorithm>
#include <concepts>
#include <string>

namespace ndyn::string {

template <typename T>
concept StringLike = std::constructible_from<std::string, T>;

[[nodiscard]] constexpr std::string as_string(StringLike auto&& result) {
  // If it's already a string, move it or copy it normally.
  using T = std::decay_t<decltype(result)>;
  if constexpr (std::same_as<T, std::string>) {
    return std::forward<decltype(result)>(result);
  } else {
    // If it's a string_view or const char*, use explicit construction.
    return std::string(result);
  }
}

[[nodiscard]] constexpr std::string as_string(std::integral auto n) {
  std::string result{};
  if (n < 0) {
    result += static_cast<char>('-');
    n = -n;
  }
  while (n > 0) {
    result += static_cast<char>('0' + (n % 10));
    n /= 10;
  }
  std::reverse(result.begin(), result.end());
  return result;
}

}  // namespace ndyn::string
