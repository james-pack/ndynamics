#pragma once

#include <array>
#include <mutex>
#include <string>
#include <string_view>

#include "math/algebra.h"
#include "math/multivector.h"
#include "string/static_string.h"
#include "string/string_utils.h"

namespace ndyn::math {

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

  std::string_view name{};
  Multivector basis{Scalar{1}};
};

template <typename Algebra>
std::string to_string(const Basis<Algebra>& basis) {
  return std::string{basis.name};
}

template <typename Algebra, auto BASIS_PREFIX, size_t BASIS_NAME_OFFSET, size_t BASIS_BLADE>
  requires string::StringLike<decltype(BASIS_PREFIX)>
consteval Basis<Algebra> generate_blade_representation() {
  using Multivector = Algebra::VectorType;
  return {
      .name = string::to_static_string(
          []() { return generate_basis_blade_name(BASIS_PREFIX, BASIS_NAME_OFFSET, BASIS_BLADE); }),
      .basis = Multivector::template blade<BASIS_BLADE>(),
  };
}

template <typename Algebra, auto BASIS_PREFIX, size_t BASIS_NAME_OFFSET = 0>
  requires string::StringLike<decltype(BASIS_PREFIX)>
consteval std::array<Basis<Algebra>, Algebra::NUM_BASIS_BLADES> generate_representation() {
  constexpr auto indices{std::make_index_sequence<Algebra::NUM_BASIS_BLADES>()};
  return []<size_t... I>(std::index_sequence<I...>) {
    return std::array<Basis<Algebra>, Algebra::NUM_BASIS_BLADES>{
        generate_blade_representation<Algebra, BASIS_PREFIX, BASIS_NAME_OFFSET, I>()...};
  }(indices);
}

template <typename AlgebraT, auto BASIS_PREFIX, size_t BASIS_NAME_OFFSET = 0>
  requires string::StringLike<decltype(BASIS_PREFIX)>
class AlgebraRepresentation final {
 public:
  using Algebra = AlgebraT;
  using Multivector = Algebra::VectorType;
  using Scalar = Algebra::ScalarType;
  static constexpr Scalar EPSILON{Algebra::EPSILON};

 private:
  // We arbitrarily set a threshold for when to generate the bases at compile-time. If this value is
  // too large, compilation times become ridiculous. This specific threshold value was chosen as it
  // is the maximum size of a typical algebra, and it results in reasonable, usually subsecond,
  // compile times. Note that there is very little practical reason to actually generate these bases
  // at compile-time. This was originally done as a learning exercise.
  static constexpr bool IS_SMALL_ALGEBRA{Algebra::NUM_BASIS_VECTORS <= 6};

  static constexpr auto bases_ = []() {
    if constexpr (IS_SMALL_ALGEBRA) {
      return generate_representation<Algebra, BASIS_PREFIX, BASIS_NAME_OFFSET>();
    } else {
      // Note that bases_ should only be used when IS_SMALL_ALGEBRA is true. Any usages of bases_
      // when it's not true are supposed to result in errors. We use a different type to hopefully
      // cause compilation errors, rather than runtime errors.
      return 0;
    }
  }();

  [[nodiscard]] static const auto& get_bases_large_algebra() noexcept {
    static_assert(!IS_SMALL_ALGEBRA);
    static std::array<std::string, Algebra::NUM_BASIS_BLADES> names;
    static std::array<Basis<Algebra>, Algebra::NUM_BASIS_BLADES> bases;
    static std::once_flag initialized;
    std::call_once(initialized, []() {
      for (size_t i = 0; i < Algebra::NUM_BASIS_BLADES; ++i) {
        // The small algebra form computes the names into the text of the executable and uses a
        // string_view of that text. At runtime, we need to manually provide storage for the name.
        names[i] = generate_basis_blade_name(BASIS_PREFIX, BASIS_NAME_OFFSET, i);

        bases[i] = {
            .name = names[i],                //
            .basis = Multivector::blade(i),  //
        };
      }
    });
    return bases;
  }

  [[nodiscard]] static constexpr const auto& get_bases() noexcept {
    if constexpr (IS_SMALL_ALGEBRA) {
      return bases_;
    } else {
      return get_bases_large_algebra();
    }
  }

  [[nodiscard]] static constexpr const auto& get_basis(size_t i) noexcept { return get_bases()[i]; }

  [[nodiscard]] static constexpr std::string basis_element_to_string(Scalar s,
                                                                     std::string_view basis_name,
                                                                     Scalar threshold) noexcept {
    using std::to_string;
    std::string result{};
    if (abs(s) > threshold) {
      result.append(to_string(s));
      if (!basis_name.empty()) {
        result.append("*");
        result.append(basis_name);
      }
    }
    return result;
  }

 public:
  [[nodiscard]] static constexpr std::string to_string(const Multivector& vec,
                                                       Scalar threshold = EPSILON) noexcept {
    std::string result{basis_element_to_string(vec.scalar(), "", threshold)};
    for (size_t i = 1; i < Algebra::NUM_BASIS_BLADES; ++i) {
      std::string basis_result{
          basis_element_to_string(vec.coefficient(i), get_basis(i).name, threshold)};
      if (!basis_result.empty()) {
        if (!result.empty()) {
          result.append(" + ");
        }
        result.append(basis_result);
      }
    }
    return result;
  }

  [[nodiscard]] static constexpr const Multivector* lookup_basis(
      std::string_view representation) noexcept {
    for (size_t i = 1; i < Algebra::NUM_BASIS_BLADES; ++i) {
      const auto& basis{get_basis(i)};
      if (representation == basis.name) {
        return &basis.basis;
      }
    }
    return nullptr;
  }

  [[nodiscard]] static constexpr auto bases_begin() noexcept { return get_bases().begin(); }
  [[nodiscard]] static constexpr auto bases_end() noexcept { return get_bases().end(); }
};

static constexpr char GENERIC_BASIS_PREFIX[] = "e";
static constexpr size_t GENERIC_BASIS_STARTING_INDEX = 1;

template <typename Algebra>
using GenericRepresentation =
    AlgebraRepresentation<Algebra, GENERIC_BASIS_PREFIX, GENERIC_BASIS_STARTING_INDEX>;

template <typename Algebra>
[[nodiscard]] std::string to_string(
    const Multivector<Algebra>& v,
    typename Algebra::ScalarType threshold = Algebra::EPSILON) noexcept {
  return GenericRepresentation<Algebra>::to_string(v, threshold);
}

template <typename Algebra>
std::ostream& operator<<(std::ostream& os, const Multivector<Algebra>& v) {
  os << to_string(v);
  return os;
}

}  // namespace ndyn::math
