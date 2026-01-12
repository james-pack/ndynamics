#pragma once

#include <array>
#include <string>

#include "math/algebra.h"
#include "math/multivector.h"

namespace ndyn::math {

template <typename AlgebraT, size_t NAME_SIZE = 5>
struct BasisName final {
  char name[NAME_SIZE];
  math::Multivector<AlgebraT> basis;
};

template <typename ScalarType>
std::string vector_element_to_string(ScalarType s, std::string_view basis_name) {
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

/**
 * Class to hold the name of each basis multivector for each algebra.
 */
template <typename AlgebraT>
class BasisRepresentation final {
 public:
  using AlgebraType = AlgebraT;
  static constexpr size_t BASES_COUNT{AlgebraType::bases_count()};
  // By default, an Algebra does not have any bases that have explicit names.
  // TODO(james): Autogenerate names of basis vectors in this default, unspecialized class.
  static constexpr size_t NAMED_BASES_COUNT{0};

  constexpr BasisRepresentation() = default;

  static constexpr std::array<BasisName<AlgebraType>, NAMED_BASES_COUNT> bases() { return {}; }

  static std::string to_string(const math::Multivector<AlgebraType>& vec) {
    return math::to_string(vec);
  }
};

template <typename ScalarType>
class BasisRepresentation<math::Complex<ScalarType>> final {
 public:
  using AlgebraType = math::Complex<ScalarType>;
  static constexpr size_t BASES_COUNT{AlgebraType::bases_count()};
  static constexpr size_t NAMED_BASES_COUNT{BASES_COUNT - 1};

 private:
  static constexpr std::array<BasisName<AlgebraType>, NAMED_BASES_COUNT> bases_{
      BasisName<AlgebraType>{"i", AlgebraType::VectorType::template e<0>()},
  };

 public:
  constexpr BasisRepresentation() = default;

  static constexpr const auto& bases() { return bases_; }

  static std::string to_string(const math::Multivector<AlgebraType>& vec) {
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
class BasisRepresentation<math::Vga<ScalarType>> final {
 public:
  using AlgebraType = math::Vga<ScalarType>;
  static constexpr size_t BASES_COUNT{AlgebraType::bases_count()};
  static constexpr size_t NAMED_BASES_COUNT{BASES_COUNT - 1};

 private:
  static constexpr auto e1{math::Multivector<AlgebraType>::template e<0>()};
  static constexpr auto e2{math::Multivector<AlgebraType>::template e<1>()};
  static constexpr auto e3{math::Multivector<AlgebraType>::template e<2>()};

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

  static std::string to_string(const math::Multivector<AlgebraType>& vec) {
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
class BasisRepresentation<math::Vga2d<ScalarType>> final {
 public:
  using AlgebraType = math::Vga2d<ScalarType>;
  static constexpr size_t BASES_COUNT{AlgebraType::bases_count()};
  static constexpr size_t NAMED_BASES_COUNT{BASES_COUNT - 1};

 private:
  static constexpr auto e1{math::Multivector<AlgebraType>::template e<0>()};
  static constexpr auto e2{math::Multivector<AlgebraType>::template e<1>()};

  static constexpr std::array<BasisName<AlgebraType>, NAMED_BASES_COUNT> bases_{
      BasisName<AlgebraType>{"e1", e1},
      {"e2", e2},
      {"e12", e1* e2},
  };

 public:
  constexpr BasisRepresentation() = default;

  static constexpr const auto& bases() { return bases_; }

  static std::string to_string(const math::Multivector<AlgebraType>& vec) {
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
