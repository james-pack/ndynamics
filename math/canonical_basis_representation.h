#pragma once

#include <array>
#include <cmath>
#include <cstddef>
#include <ostream>
#include <string>

#include "math/algebra.h"
#include "math/basis_representation.h"
#include "math/multivector.h"

namespace ndyn::math {

/**
 * Class to hold the name of each basis multivector for each algebra.
 */
template <typename AlgebraT>
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

template <typename ScalarType>
class CanonicalBasisRepresentation<Vga<ScalarType>> final {
 public:
  using AlgebraType = Vga<ScalarType>;
  static constexpr size_t BASES_COUNT{AlgebraType::NUM_BASIS_BLADES};
  static constexpr size_t NAMED_BASES_COUNT{BASES_COUNT - 1};

 private:
  static constexpr auto e1{Multivector<AlgebraType>::template e<0>()};
  static constexpr auto e2{Multivector<AlgebraType>::template e<1>()};
  static constexpr auto e3{Multivector<AlgebraType>::template e<2>()};

  static constexpr std::array<BasisName<AlgebraType>, NAMED_BASES_COUNT> bases_{
      BasisName<AlgebraType>{"e1", e1},
      {"e2", e2},
      {"e12", e1* e2},
      {"e3", e3},
      {"e13", e1* e3},
      {"e23", e2* e3},
      {"e123", e1* e2* e3},
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
static_assert(BasisRepresentation<CanonicalBasisRepresentation<Vga<>>>);

template <typename ScalarType>
class CanonicalBasisRepresentation<Vga2d<ScalarType>> final {
 public:
  using AlgebraType = Vga2d<ScalarType>;
  static constexpr size_t BASES_COUNT{AlgebraType::NUM_BASIS_BLADES};
  static constexpr size_t NAMED_BASES_COUNT{BASES_COUNT - 1};

 private:
  static constexpr auto e1{Multivector<AlgebraType>::template e<0>()};
  static constexpr auto e2{Multivector<AlgebraType>::template e<1>()};

  static constexpr std::array<BasisName<AlgebraType>, NAMED_BASES_COUNT> bases_{
      BasisName<AlgebraType>{"e1", e1},
      {"e2", e2},
      {"e12", e1* e2},
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
static_assert(BasisRepresentation<CanonicalBasisRepresentation<Vga2d<>>>);

template <typename ScalarType>
class CanonicalBasisRepresentation<Pga2d<ScalarType>> final {
 public:
  using AlgebraType = Pga2d<ScalarType>;
  static constexpr size_t BASES_COUNT{AlgebraType::NUM_BASIS_BLADES};
  static constexpr size_t NAMED_BASES_COUNT{BASES_COUNT - 1};

 private:
  static constexpr auto e0{Multivector<AlgebraType>::template e<0>()};
  static constexpr auto e1{Multivector<AlgebraType>::template e<1>()};
  static constexpr auto e2{Multivector<AlgebraType>::template e<2>()};

  static constexpr std::array<BasisName<AlgebraType>, NAMED_BASES_COUNT> bases_{
      BasisName<AlgebraType>{"e0", e0},
      {"e1", e1},
      {"e01", e0* e1},
      {"e2", e2},
      {"e02", e0* e2},
      {"e12", e1* e2},
      {"e012", e0* e1* e2},
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
static_assert(BasisRepresentation<CanonicalBasisRepresentation<Pga2d<>>>);

template <typename ScalarType>
class CanonicalBasisRepresentation<Pga<ScalarType>> final {
 public:
  using AlgebraType = Pga<ScalarType>;
  static constexpr size_t BASES_COUNT{AlgebraType::NUM_BASIS_BLADES};
  static constexpr size_t NAMED_BASES_COUNT{BASES_COUNT - 1};

 private:
  static constexpr auto e0{Multivector<AlgebraType>::template e<0>()};
  static constexpr auto e1{Multivector<AlgebraType>::template e<1>()};
  static constexpr auto e2{Multivector<AlgebraType>::template e<2>()};
  static constexpr auto e3{Multivector<AlgebraType>::template e<3>()};

  static constexpr std::array<BasisName<AlgebraType>, NAMED_BASES_COUNT> bases_{
      BasisName<AlgebraType>{"e0", e0},
      {"e1", e1},
      {"e01", e0* e1},
      {"e2", e2},
      {"e02", e0* e2},
      {"e12", e1* e2},
      {"e012", e0* e1* e2},
      {"e3", e3},
      {"e03", e0* e3},
      {"e13", e1* e3},
      {"e23", e2* e3},
      {"e013", e0* e1* e3},
      {"e023", e0* e2* e3},
      {"e123", e1* e2* e3},
      {"e0123", e0* e1* e2* e3},
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
static_assert(BasisRepresentation<CanonicalBasisRepresentation<Pga<>>>);

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
