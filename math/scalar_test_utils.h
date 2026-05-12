#pragma once

#include <array>
#include <type_traits>
#include <utility>

namespace ndyn::math {

template <typename F, typename T, T... Is>
constexpr auto transform_indices(F&& f, std::integer_sequence<T, Is...>) {
  using Element = std::invoke_result_t<F, T>;
  return std::array<Element, sizeof...(Is)>{f(Is)...};
}

template <typename Scalar, typename T, T... Is>
constexpr auto as_scalars(std::integer_sequence<T, Is...> seq) {
  return transform_indices([](T i) { return static_cast<Scalar>(i); }, seq);
}

}  // namespace ndyn::math
