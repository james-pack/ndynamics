#pragma once

namespace ndyn::math {

template <typename T>
constexpr T square_magnitude(T value) {
  return value * value;
}

}  // namespace ndyn::math
