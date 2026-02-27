#pragma once

#include <array>
#include <cmath>
#include <ostream>
#include <string>

#include "math/algebra.h"
#include "math/multivector.h"

namespace ndyn::math {

template <typename AlgebraT, size_t NAME_SIZE = 5>
struct BasisName final {
  char name[NAME_SIZE];
  Multivector<AlgebraT> basis;
};

template <typename ScalarType>
std::string vector_element_to_string(ScalarType s, std::string_view basis_name) {
  using std::to_string;
  std::string result{};
  if (std::abs(s) > 0.000001) {
    result.append(to_string(s));
    if (!basis_name.empty()) {
      result.append("*");
      result.append(basis_name);
    }
  }
  return result;
}

/**
 * Class to hold the name of each basis multivector for each algebra.
 */
template <typename AlgebraT>
class BasisRepresentation final {
 public:
  using AlgebraType = AlgebraT;
  static constexpr size_t BASES_COUNT{AlgebraType::NUM_BASIS_BLADES};
  // By default, an Algebra does not have any bases that have explicit names.
  // TODO(james): Autogenerate names of basis vectors in this default, unspecialized class.
  static constexpr size_t NAMED_BASES_COUNT{0};

  constexpr BasisRepresentation() = default;

  static constexpr std::array<BasisName<AlgebraType>, NAMED_BASES_COUNT> bases() { return {}; }

  static std::string to_string(const Multivector<AlgebraType>& vec) {
    return "<no representation specified for this algebra>";
  }
};

template <typename AlgebraType>
std::string to_string(const Multivector<AlgebraType>& v) {
  return BasisRepresentation<AlgebraType>::to_string(v);
}

template <typename AlgebraType>
std::ostream& operator<<(std::ostream& os, const Multivector<AlgebraType>& v) {
  os << to_string(v);
  return os;
}

template <typename ScalarType>
class BasisRepresentation<Complex<ScalarType>> final {
 public:
  using AlgebraType = Complex<ScalarType>;
  static constexpr size_t BASES_COUNT{AlgebraType::NUM_BASIS_BLADES};
  static constexpr size_t NAMED_BASES_COUNT{BASES_COUNT - 1};

 private:
  static constexpr std::array<BasisName<AlgebraType>, NAMED_BASES_COUNT> bases_{
      BasisName<AlgebraType>{"i", AlgebraType::VectorType::template e<0>()},
  };

 public:
  constexpr BasisRepresentation() = default;

  static constexpr const auto& bases() { return bases_; }

  static std::string to_string(const Multivector<AlgebraType>& vec) {
    std::string result{vector_element_to_string(vec.scalar(), "")};
    for (size_t i = 1; i < BASES_COUNT; ++i) {
      std::string basis_result{vector_element_to_string(vec.basis(i), bases_[i - 1].name)};
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

template <typename ScalarType>
class BasisRepresentation<Vga<ScalarType>> final {
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
  constexpr BasisRepresentation() = default;

  static constexpr const auto& bases() { return bases_; }

  static std::string to_string(const Multivector<AlgebraType>& vec) {
    std::string result{vector_element_to_string(vec.scalar(), "")};
    for (size_t i = 1; i < BASES_COUNT; ++i) {
      std::string basis_result{vector_element_to_string(vec.basis(i), bases_[i - 1].name)};
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

template <typename ScalarType>
class BasisRepresentation<Vga2d<ScalarType>> final {
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
  constexpr BasisRepresentation() = default;

  static constexpr const auto& bases() { return bases_; }

  static std::string to_string(const Multivector<AlgebraType>& vec) {
    std::string result{vector_element_to_string(vec.scalar(), "")};
    for (size_t i = 1; i < BASES_COUNT; ++i) {
      std::string basis_result{vector_element_to_string(vec.basis(i), bases_[i - 1].name)};
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

}  // namespace ndyn::math
