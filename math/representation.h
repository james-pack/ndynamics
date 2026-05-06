#pragma once

#include <array>
#include <string_view>

#include "math/algebra.h"
#include "math/multivector.h"
#include "string/static_string.h"
#include "string/string_utils.h"

namespace ndyn::math {

template <typename Algebra>
struct NameTable final {};

template <typename Algebra>
auto generate_name_table() {}

constexpr std::string generate_basis_blade_name(string::StringLike auto&& prefix,
                                                size_t name_offset, size_t basis_blade) {
  constexpr std::array<char, 36> NAMES{
      '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h',
      'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z',
  };
  std::string result{};
  // The name of the basis blade for scalar values is the empty string in all algebras.
  if (basis_blade != 0) {
    result.append(prefix);
    for (size_t i = 0; (1UL << i) <= basis_blade; ++i) {
      if ((basis_blade & (1UL << i)) != 0) {
        result += NAMES.at(i + name_offset);
      }
    }
  }
  return result;
}

template <auto BASIS_PREFIX, size_t BASIS_NAME_OFFSET, size_t NUMBER_NAMES>
  requires string::StringLike<decltype(BASIS_PREFIX)>
consteval std::array<std::string_view, NUMBER_NAMES> generate_basis_blade_names() {
  constexpr auto indices{std::make_index_sequence<NUMBER_NAMES>{}};
  return []<size_t... I>(std::index_sequence<I...>) {
    return std::array<std::string_view, NUMBER_NAMES>{string::to_static_string(
        []() { return generate_basis_blade_name(BASIS_PREFIX, BASIS_NAME_OFFSET, I); })...};
  }(indices);
}

template <typename Algebra>
struct Basis final {
  using Multivector = Algebra::VectorType;
  using Scalar = Algebra::ScalarType;

  const std::string_view name{};
  const Multivector basis{Scalar{1}};
};

template <typename Algebra>
std::string to_string(const Basis<Algebra>& basis) {
  return std::string{basis.name};
}

template <typename Algebra, auto BASIS_PREFIX, size_t BASIS_NAME_OFFSET, size_t BASIS_BLADE>
  requires string::StringLike<decltype(BASIS_PREFIX)>
consteval Basis<Algebra> generate_blade_representation() {
  return {.name = string::to_static_string([]() {
            return generate_basis_blade_name(BASIS_PREFIX, BASIS_NAME_OFFSET, BASIS_BLADE);
          })};
}

template <typename Algebra, auto BASIS_PREFIX, size_t BASIS_NAME_OFFSET>
  requires string::StringLike<decltype(BASIS_PREFIX)>
consteval std::array<Basis<Algebra>, Algebra::NUM_BASIS_BLADES> generate_representation() {
  constexpr auto indices{std::make_index_sequence<Algebra::NUM_BASIS_BLADES>()};
  return []<size_t... I>(std::index_sequence<I...>) {
    return std::array<Basis<Algebra>, Algebra::NUM_BASIS_BLADES>{
        generate_blade_representation<Algebra, BASIS_PREFIX, BASIS_NAME_OFFSET, I>()...};
  }(indices);
}

}  // namespace ndyn::math
