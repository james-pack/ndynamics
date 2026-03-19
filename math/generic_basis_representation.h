#pragma once

#include <array>
#include <cmath>
#include <cstddef>
#include <string>

#include "math/algebra.h"
#include "math/basis_representation.h"
#include "math/multivector.h"

namespace ndyn::math {

/**
 * Structure to hold a generic name of each basis multivector for each algebra. This representation
 * is most useful in tests when the test will be run over multiple algebras.
 */
template <typename AlgebraT>
class GenericBasisRepresentation final {
 public:
  using AlgebraType = AlgebraT;
  static constexpr size_t BASES_COUNT{AlgebraType::NUM_BASIS_BLADES};
  static constexpr size_t NAMED_BASES_COUNT{BASES_COUNT - 1};

 private:
  static constexpr std::array<BasisName<AlgebraType>, NAMED_BASES_COUNT> bases_ = []() {
    std::array<BasisName<AlgebraType>, NAMED_BASES_COUNT> result{};
    size_t index{0};
    if constexpr (AlgebraType::NUM_BASIS_VECTORS >= 1) {
      result[index++] = BasisName<AlgebraType>{"e1", Multivector<AlgebraType>::template e<0>()};
    }
    if constexpr (AlgebraType::NUM_BASIS_VECTORS >= 2) {
      result[index++] = BasisName<AlgebraType>{"e2", Multivector<AlgebraType>::template e<1>()};
      result[index++] =
          BasisName<AlgebraType>{"e12", Multivector<AlgebraType>::template e<0>() *
                                            Multivector<AlgebraType>::template e<1>()};
    }
    if constexpr (AlgebraType::NUM_BASIS_VECTORS >= 3) {
      result[index++] = BasisName<AlgebraType>{"e3", Multivector<AlgebraType>::template e<2>()};
      result[index++] =
          BasisName<AlgebraType>{"e13", Multivector<AlgebraType>::template e<0>() *
                                            Multivector<AlgebraType>::template e<2>()};
      result[index++] =
          BasisName<AlgebraType>{"e23", Multivector<AlgebraType>::template e<1>() *
                                            Multivector<AlgebraType>::template e<2>()};
      result[index++] =
          BasisName<AlgebraType>{"e123", Multivector<AlgebraType>::template e<0>() *
                                             Multivector<AlgebraType>::template e<1>() *
                                             Multivector<AlgebraType>::template e<2>()};
    }
    return result;
  }();

 public:
  constexpr GenericBasisRepresentation() = default;

  static constexpr const BasisName<AlgebraType>* bases_begin() {
    if constexpr (NAMED_BASES_COUNT > 0) {
      return &bases_[0];
    } else {
      return nullptr;
    }
  }

  static constexpr const BasisName<AlgebraType>* bases_end() {
    if constexpr (NAMED_BASES_COUNT > 0) {
      return &bases_[0] + NAMED_BASES_COUNT;
    } else {
      return nullptr;
    }
  }

  static std::string to_string(const Multivector<AlgebraType>& vec) {
    std::string result{basis_element_to_string(vec.scalar(), "")};
    for (size_t i = 1; i < NAMED_BASES_COUNT; ++i) {
      std::string basis_result{basis_element_to_string(vec.coefficient(i), bases_[i - 1].name)};
      if (!basis_result.empty()) {
        if (!result.empty()) {
          result.append(" + ");
        }
        result.append(basis_result);
      }
    }
    return result;
  }
};
static_assert(BasisRepresentation<GenericBasisRepresentation<Vga<>>>);
static_assert(BasisRepresentation<GenericBasisRepresentation<Vga2d<>>>);
static_assert(BasisRepresentation<GenericBasisRepresentation<Pga<>>>);
static_assert(BasisRepresentation<GenericBasisRepresentation<Pga2d<>>>);
static_assert(BasisRepresentation<GenericBasisRepresentation<Scalar<>>>);
static_assert(BasisRepresentation<GenericBasisRepresentation<Complex<>>>);
static_assert(BasisRepresentation<GenericBasisRepresentation<Spacetime<>>>);

static_assert(GenericBasisRepresentation<Scalar<>>::NAMED_BASES_COUNT == 0);
static_assert(GenericBasisRepresentation<Scalar<>>::bases_begin() ==
              GenericBasisRepresentation<Scalar<>>::bases_end());
static_assert(GenericBasisRepresentation<Complex<>>::NAMED_BASES_COUNT == 1);
static_assert(GenericBasisRepresentation<Complex<>>::bases_begin() !=
              GenericBasisRepresentation<Complex<>>::bases_end());

}  // namespace ndyn::math
