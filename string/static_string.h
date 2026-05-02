/**
 * Set of utilities to make it easier to create static immutable strings that are computed at
 * compile-time.
 *
 * Why is this useful?
 * This technique has wide applicability. It is most often used to build strings for a UI when those
 * strings are repetitive or when they require processing. Imagine an application whose strings get
 * translated at compile-time to support additional languages.
 *
 * Big shout out to Jason Turner of C++ Weekly.
 * This idea is stolen directly from his video https://youtu.be/ABg4_EV5L3w and is heavily
 * influenced by his talk with Ben Deane titled "constexpr ALL the things!" .
 * Thanks Jason!
 */
#pragma once

#include <algorithm>
#include <array>
#include <iterator>
#include <string>
#include <string_view>

namespace ndyn::string {

// Note: if you get errors, look for an error message like this one from gcc 13.3:
// error: 'constexpr' loop iteration count exceeds limit of 262144 (use '-fconstexpr-loop-limit='
// to increase the limit)
static constexpr size_t MAX_STATIC_STRING_SIZE{1024 * 1024};

namespace {

struct oversized_array final {
  size_t size;
  std::array<char, MAX_STATIC_STRING_SIZE> data{};
};

[[nodiscard]] consteval auto to_oversized_array(const std::string& str) noexcept {
  oversized_array result{.size = str.size()};
  std::copy(str.begin(), str.end(), result.data.begin());
  return result;
}

template <typename Callable>
[[nodiscard]] consteval auto to_array(Callable generator) noexcept {
  constexpr auto oversized{to_oversized_array(generator())};
  std::array<char, oversized.size> result{};
  std::copy(oversized.data.begin(), std::next(oversized.data.begin(), oversized.size),
            result.begin());
  return result;
}

template <auto Data>
[[nodiscard]] consteval const auto& make_static() noexcept {
  return Data;
}

}  // namespace

consteval std::string_view to_static_string(auto generator) {
  constexpr auto& static_data{make_static<to_array(generator)>()};
  return std::string_view{static_data.begin(), static_data.size()};
}

}  // namespace ndyn::string
