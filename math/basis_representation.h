#pragma once

#include <cmath>
#include <concepts>
#include <cstddef>
#include <string>
#include <type_traits>

#include "math/abs.h"
#include "math/multivector.h"

namespace ndyn::math {

template <typename AlgebraT, size_t NAME_SIZE = 10>
struct BasisName final {
  char name[NAME_SIZE];
  Multivector<AlgebraT> basis;
};

/**
 * Utility function to be used by implementors of BasisRepresentation. Generates a string of the
 * value times the basis_name, if the value is nonzero. Centralizing this helps keep output similar
 * across all algebras and representations.
 */
template <typename ScalarType>
std::string basis_element_to_string(ScalarType s, std::string_view basis_name) {
  using std::to_string;
  std::string result{};
  if (abs(s) > 0.000001) {
    result.append(to_string(s));
    if (!basis_name.empty()) {
      result.append("*");
      result.append(basis_name);
    }
  }
  return result;
}

template <typename T>
concept HasAlgebraType = requires { typename T::AlgebraType; };

template <typename T, typename AlgebraT = typename T::AlgebraType>
concept BasisRepresentation =
    // Must expose the associated AlgebraType.
    HasAlgebraType<T> &&                                //
    std::same_as<typename T::AlgebraType, AlgebraT> &&  //

    // Must be default-constructible (even though all members are static,
    // the class must be instantiable so generic code can hold values of T).
    std::is_default_constructible_v<T> &&  //

    requires {
      // Must include a NAMED_BASES_COUNT: static constexpr size_t.
      // This should be the available number of named bases.
      { std::integral_constant<size_t, T::NAMED_BASES_COUNT>{} };

      // NAMED_BASES_COUNT must equal or exceed the number of bases in the algebra. Note that we
      // subtract one from the number of basis blades as the scalar is considered a basis blade but
      // it does not need a name.
      requires(T::NAMED_BASES_COUNT >= AlgebraT::NUM_BASIS_BLADES - 1);
    } &&  //

    requires {
      // The names for the bases should be iterable.
      { T::bases_begin() } -> std::same_as<const BasisName<typename T::AlgebraType>*>;
      { T::bases_end() } -> std::same_as<const BasisName<typename T::AlgebraType>*>;
    } &&  //

    requires(const Multivector<typename T::AlgebraType>& vec) {
      // BasisRepresentations should provide mechanisms to print Multivectors in the same algebra.
      { T::to_string(vec) } -> std::same_as<std::string>;
    };

}  // namespace ndyn::math
