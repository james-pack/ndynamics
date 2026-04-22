#pragma once

#include <array>
#include <cmath>
#include <cstddef>
#include <ostream>
#include <string>

#include "base/bits.h"
#include "math/algebra.h"
#include "math/basis_representation.h"
#include "math/multivector.h"

namespace ndyn::math {

/**
 * Class to hold the name of each basis multivector for each algebra.
 */
template <typename AlgebraT, typename Enable = void>
class CanonicalBasisRepresentation final {
 public:
  using AlgebraType = AlgebraT;
  static constexpr size_t BASES_COUNT{AlgebraType::NUM_BASIS_BLADES};
  // By default, an Algebra does not have any bases that have explicit names.
  // TODO(james): Autogenerate names of basis vectors in this default, unspecialized class.
  static constexpr size_t NAMED_BASES_COUNT{0};

  constexpr CanonicalBasisRepresentation() = default;

  static constexpr const BasisName<AlgebraType>* bases_begin() { return nullptr; }
  static constexpr const BasisName<AlgebraType>* bases_end() { return nullptr; }

  static std::string to_string(const Multivector<AlgebraType>& vec) {
    return "<no representation specified for this algebra>";
  }
};

template <typename AlgebraType>
std::string to_string(const Multivector<AlgebraType>& v) {
  return CanonicalBasisRepresentation<AlgebraType>::to_string(v);
}

template <typename AlgebraType>
std::ostream& operator<<(std::ostream& os, const Multivector<AlgebraType>& v) {
  os << to_string(v);
  return os;
}

template <typename ScalarType>
class CanonicalBasisRepresentation<Complex<ScalarType>> final {
 public:
  using AlgebraType = Complex<ScalarType>;
  static constexpr size_t BASES_COUNT{AlgebraType::NUM_BASIS_BLADES};
  static constexpr size_t NAMED_BASES_COUNT{BASES_COUNT - 1};

 private:
  static constexpr std::array<BasisName<AlgebraType>, NAMED_BASES_COUNT> bases_{
      BasisName<AlgebraType>{"i", AlgebraType::VectorType::template e<0>()},
  };

 public:
  constexpr CanonicalBasisRepresentation() = default;

  static constexpr const BasisName<AlgebraType>* bases_begin() { return &bases_[0]; }
  static constexpr const BasisName<AlgebraType>* bases_end() {
    return &bases_[0] + NAMED_BASES_COUNT;
  }

  static std::string to_string(const Multivector<AlgebraType>& vec) {
    std::string result{basis_element_to_string(vec.scalar(), "")};
    for (size_t i = 1; i < BASES_COUNT; ++i) {
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
static_assert(BasisRepresentation<CanonicalBasisRepresentation<Complex<>>>);

/**
 * Canonical representation for algebras with only positive basis metrics. These are primarily the
 * Euclidean algebras like VGA.
 */
template <typename AlgebraT>
class CanonicalBasisRepresentation<AlgebraT,
                                   std::enable_if_t<AlgebraT::NUM_POSITIVE_BASES <= 3 &&  //
                                                    AlgebraT::NUM_NEGATIVE_BASES == 0 &&  //
                                                    AlgebraT::NUM_ZERO_BASES == 0>>
    final {
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
  constexpr CanonicalBasisRepresentation() = default;

  static constexpr const BasisName<AlgebraType>* bases_begin() { return &bases_[0]; }
  static constexpr const BasisName<AlgebraType>* bases_end() {
    return &bases_[0] + NAMED_BASES_COUNT;
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
static_assert(BasisRepresentation<CanonicalBasisRepresentation<Vga<>>>);
static_assert(BasisRepresentation<CanonicalBasisRepresentation<Vga2d<>>>);

/**
 * Canonical representation for algebras with a single degenerate basis metric. These are primarily
 * the projective algebras like PGA.
 */
template <typename AlgebraT>
class CanonicalBasisRepresentation<AlgebraT,
                                   std::enable_if_t<AlgebraT::NUM_POSITIVE_BASES <= 4 &&  //
                                                    AlgebraT::NUM_NEGATIVE_BASES == 0 &&  //
                                                    AlgebraT::NUM_ZERO_BASES == 1>>
    final {
 public:
  using AlgebraType = AlgebraT;
  static constexpr size_t BASES_COUNT{AlgebraType::NUM_BASIS_BLADES};
  static constexpr size_t NAMED_BASES_COUNT{BASES_COUNT - 1};

 private:
  static constexpr std::array<BasisName<AlgebraType>, NAMED_BASES_COUNT> bases_ = []() {
    std::array<BasisName<AlgebraType>, NAMED_BASES_COUNT> result{};
    size_t index{0};
    result[index++] = BasisName<AlgebraType>{"e0", Multivector<AlgebraType>::template e<0>()};
    if constexpr (AlgebraType::NUM_POSITIVE_BASES >= 1) {
      result[index++] = BasisName<AlgebraType>{"e1", Multivector<AlgebraType>::template e<1>()};
      result[index++] =
          BasisName<AlgebraType>{"e01", Multivector<AlgebraType>::template e<0>() *
                                            Multivector<AlgebraType>::template e<1>()};
    }
    if constexpr (AlgebraType::NUM_POSITIVE_BASES >= 2) {
      result[index++] = BasisName<AlgebraType>{"e2", Multivector<AlgebraType>::template e<2>()};
      result[index++] =
          BasisName<AlgebraType>{"e02", Multivector<AlgebraType>::template e<0>() *
                                            Multivector<AlgebraType>::template e<2>()};
      result[index++] =
          BasisName<AlgebraType>{"e12", Multivector<AlgebraType>::template e<1>() *
                                            Multivector<AlgebraType>::template e<2>()};
      result[index++] =
          BasisName<AlgebraType>{"e012", Multivector<AlgebraType>::template e<0>() *
                                             Multivector<AlgebraType>::template e<1>() *
                                             Multivector<AlgebraType>::template e<2>()};
    }
    if constexpr (AlgebraType::NUM_POSITIVE_BASES >= 3) {
      result[index++] = BasisName<AlgebraType>{"e3", Multivector<AlgebraType>::template e<3>()};
      result[index++] =
          BasisName<AlgebraType>{"e03", Multivector<AlgebraType>::template e<0>() *
                                            Multivector<AlgebraType>::template e<3>()};
      result[index++] =
          BasisName<AlgebraType>{"e13", Multivector<AlgebraType>::template e<1>() *
                                            Multivector<AlgebraType>::template e<3>()};
      result[index++] =
          BasisName<AlgebraType>{"e013", Multivector<AlgebraType>::template e<0>() *
                                             Multivector<AlgebraType>::template e<1>() *
                                             Multivector<AlgebraType>::template e<3>()};
      result[index++] =
          BasisName<AlgebraType>{"e23", Multivector<AlgebraType>::template e<2>() *
                                            Multivector<AlgebraType>::template e<3>()};
      result[index++] =
          BasisName<AlgebraType>{"e023", Multivector<AlgebraType>::template e<0>() *
                                             Multivector<AlgebraType>::template e<2>() *
                                             Multivector<AlgebraType>::template e<3>()};
      result[index++] =
          BasisName<AlgebraType>{"e123", Multivector<AlgebraType>::template e<1>() *
                                             Multivector<AlgebraType>::template e<2>() *
                                             Multivector<AlgebraType>::template e<3>()};
      result[index++] =
          BasisName<AlgebraType>{"e0123", Multivector<AlgebraType>::template e<0>() *
                                              Multivector<AlgebraType>::template e<1>() *
                                              Multivector<AlgebraType>::template e<2>() *
                                              Multivector<AlgebraType>::template e<3>()};
    }
    if constexpr (AlgebraType::NUM_POSITIVE_BASES >= 4) {
      result[index++] = BasisName<AlgebraType>{"e4", Multivector<AlgebraType>::template e<4>()};
      result[index++] =
          BasisName<AlgebraType>{"e04", Multivector<AlgebraType>::template e<0>() *
                                            Multivector<AlgebraType>::template e<4>()};
      result[index++] =
          BasisName<AlgebraType>{"e14", Multivector<AlgebraType>::template e<1>() *
                                            Multivector<AlgebraType>::template e<4>()};
      result[index++] =
          BasisName<AlgebraType>{"e014", Multivector<AlgebraType>::template e<0>() *
                                             Multivector<AlgebraType>::template e<1>() *
                                             Multivector<AlgebraType>::template e<4>()};
      result[index++] =
          BasisName<AlgebraType>{"e24", Multivector<AlgebraType>::template e<2>() *
                                            Multivector<AlgebraType>::template e<4>()};
      result[index++] =
          BasisName<AlgebraType>{"e024", Multivector<AlgebraType>::template e<0>() *
                                             Multivector<AlgebraType>::template e<2>() *
                                             Multivector<AlgebraType>::template e<4>()};
      result[index++] =
          BasisName<AlgebraType>{"e124", Multivector<AlgebraType>::template e<1>() *
                                             Multivector<AlgebraType>::template e<2>() *
                                             Multivector<AlgebraType>::template e<4>()};
      result[index++] =
          BasisName<AlgebraType>{"e0124", Multivector<AlgebraType>::template e<0>() *
                                              Multivector<AlgebraType>::template e<1>() *
                                              Multivector<AlgebraType>::template e<2>() *
                                              Multivector<AlgebraType>::template e<4>()};
      result[index++] =
          BasisName<AlgebraType>{"e34", Multivector<AlgebraType>::template e<3>() *
                                            Multivector<AlgebraType>::template e<4>()};
      result[index++] =
          BasisName<AlgebraType>{"e034", Multivector<AlgebraType>::template e<0>() *
                                             Multivector<AlgebraType>::template e<3>() *
                                             Multivector<AlgebraType>::template e<4>()};
      result[index++] =
          BasisName<AlgebraType>{"e134", Multivector<AlgebraType>::template e<1>() *
                                             Multivector<AlgebraType>::template e<3>() *
                                             Multivector<AlgebraType>::template e<4>()};
      result[index++] =
          BasisName<AlgebraType>{"e0134", Multivector<AlgebraType>::template e<0>() *
                                              Multivector<AlgebraType>::template e<1>() *
                                              Multivector<AlgebraType>::template e<3>() *
                                              Multivector<AlgebraType>::template e<4>()};
      result[index++] =
          BasisName<AlgebraType>{"e234", Multivector<AlgebraType>::template e<2>() *
                                             Multivector<AlgebraType>::template e<3>() *
                                             Multivector<AlgebraType>::template e<4>()};
      result[index++] =
          BasisName<AlgebraType>{"e0234", Multivector<AlgebraType>::template e<0>() *
                                              Multivector<AlgebraType>::template e<2>() *
                                              Multivector<AlgebraType>::template e<3>() *
                                              Multivector<AlgebraType>::template e<4>()};
      result[index++] =
          BasisName<AlgebraType>{"e1234", Multivector<AlgebraType>::template e<1>() *
                                              Multivector<AlgebraType>::template e<2>() *
                                              Multivector<AlgebraType>::template e<3>() *
                                              Multivector<AlgebraType>::template e<4>()};
      result[index++] =
          BasisName<AlgebraType>{"e01234", Multivector<AlgebraType>::template e<0>() *
                                               Multivector<AlgebraType>::template e<1>() *
                                               Multivector<AlgebraType>::template e<2>() *
                                               Multivector<AlgebraType>::template e<3>() *
                                               Multivector<AlgebraType>::template e<4>()};
    }
    return result;
  }();
  static_assert(bases_[NAMED_BASES_COUNT - 1].basis != Multivector<AlgebraType>{});
  static_assert(bases_[NAMED_BASES_COUNT - 1].basis == Multivector<AlgebraType>::pseudoscalar());

 public:
  constexpr CanonicalBasisRepresentation() = default;

  static constexpr const BasisName<AlgebraType>* bases_begin() { return &bases_[0]; }
  static constexpr const BasisName<AlgebraType>* bases_end() {
    return &bases_[0] + NAMED_BASES_COUNT;
  }

  static std::string to_string(const Multivector<AlgebraType>& vec) {
    std::string result{basis_element_to_string(vec.scalar(), "")};
    for (size_t i = 1; i < BASES_COUNT; ++i) {
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
static_assert(BasisRepresentation<CanonicalBasisRepresentation<Pga2d<>>>);
static_assert(BasisRepresentation<CanonicalBasisRepresentation<Pga<>>>);

/**
 * Canonical representation for algebras with a single negative basis metric. These are primarily
 * the conformal algebras like CGA, but it also covers the Cl(3,1) version of the Spacetime Algebra
 * .
 */
template <typename AlgebraT>
class CanonicalBasisRepresentation<AlgebraT,
                                   std::enable_if_t<AlgebraT::NUM_POSITIVE_BASES >= 2 &&  //
                                                    AlgebraT::NUM_POSITIVE_BASES <= 5 &&  //
                                                    AlgebraT::NUM_NEGATIVE_BASES == 1 &&  //
                                                    AlgebraT::NUM_ZERO_BASES == 0>>
    final {
 public:
  using AlgebraType = AlgebraT;
  using MultivectorType = Multivector<AlgebraType>;
  static constexpr size_t BASES_COUNT{AlgebraType::NUM_BASIS_BLADES};
  static constexpr size_t NAMED_BASES_COUNT{BASES_COUNT - 1};

 private:
  static constexpr std::array<BasisName<AlgebraType>, NAMED_BASES_COUNT> bases_ = []() {
    std::array<BasisName<AlgebraType>, NAMED_BASES_COUNT> result{};
    size_t index{0};
    for (size_t i = 0; i < AlgebraType::NUM_BASIS_VECTORS; ++i) {
      result.at(index).name[0] = 'e';
      result[index].name[1] = '0' + static_cast<char>(i);
      result[index].name[2] = '\0';
      result[index].basis = MultivectorType::e(i);
      ++index;
      for (size_t j = 1; j < (1UL << i); ++j) {
        result.at(index) = result.at(j - 1);
        result[index].name[bit_count(j) + 1] = '0' + static_cast<char>(i);
        result[index].name[bit_count(j) + 2] = '\0';
        result[index].basis = result[j - 1].basis * MultivectorType::e(i);
        ++index;
      }
    }

    return result;
  }();
  static_assert(bases_[NAMED_BASES_COUNT - 1].basis != MultivectorType{});
  static_assert(bases_[NAMED_BASES_COUNT - 1].basis == MultivectorType::pseudoscalar());

  static constexpr size_t basis_name_length(size_t basis_index) {
    const BasisName<AlgebraType>& basis{bases_.at(basis_index)};
    size_t result{0};
    while (basis.name[result] != '\0') {
      ++result;
    }
    return result;
  }
  static_assert(basis_name_length(NAMED_BASES_COUNT - 1) ==
                AlgebraType::NUM_BASIS_VECTORS + 1 /* +1 for the initial 'e' */);

 public:
  constexpr CanonicalBasisRepresentation() = default;

  static constexpr const BasisName<AlgebraType>* bases_begin() { return &bases_[0]; }
  static constexpr const BasisName<AlgebraType>* bases_end() {
    return &bases_[0] + NAMED_BASES_COUNT;
  }

  static std::string to_string(const Multivector<AlgebraType>& vec) {
    std::string result{basis_element_to_string(vec.scalar(), "")};
    for (size_t i = 1; i < BASES_COUNT; ++i) {
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
static_assert(BasisRepresentation<CanonicalBasisRepresentation<Cga<>>>);

template <typename ScalarType>
class CanonicalBasisRepresentation<Spacetime<ScalarType>> final {
 public:
  using AlgebraType = Spacetime<ScalarType>;
  static constexpr size_t BASES_COUNT{AlgebraType::NUM_BASIS_BLADES};
  static constexpr size_t NAMED_BASES_COUNT{BASES_COUNT - 1};

 private:
  static constexpr auto ct{Multivector<AlgebraType>::template e<0>()};
  static constexpr auto x{Multivector<AlgebraType>::template e<1>()};
  static constexpr auto y{Multivector<AlgebraType>::template e<2>()};
  static constexpr auto z{Multivector<AlgebraType>::template e<3>()};

  static constexpr std::array<BasisName<AlgebraType>, NAMED_BASES_COUNT> bases_{
      BasisName<AlgebraType>{"ct", ct},
      {"x", x},
      {"ctx", ct* x},
      {"y", y},
      {"cty", ct* y},
      {"xy", x* y},
      {"ctxy", ct* x* y},
      {"z", z},
      {"ctz", ct* z},
      {"xz", x* z},
      {"yz", y* z},
      {"ctxz", ct* x* z},
      {"ctyz", ct* y* z},
      {"xyz", x* y* z},
      {"ctxyz", ct* x* y* z},
  };

 public:
  constexpr CanonicalBasisRepresentation() = default;

  static constexpr const BasisName<AlgebraType>* bases_begin() { return &bases_[0]; }
  static constexpr const BasisName<AlgebraType>* bases_end() {
    return &bases_[0] + NAMED_BASES_COUNT;
  }

  static std::string to_string(const Multivector<AlgebraType>& vec) {
    std::string result{basis_element_to_string(vec.scalar(), "")};
    for (size_t i = 1; i < BASES_COUNT; ++i) {
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
static_assert(BasisRepresentation<CanonicalBasisRepresentation<Spacetime<>>>);

}  // namespace ndyn::math
