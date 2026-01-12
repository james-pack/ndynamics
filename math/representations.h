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
std::string vector_element_to_string(ScalarType s, std::string_view base_name) {
  using std::to_string;
  std::string result{};
  if (abs(s) > 0.000001) {
    result.append(to_string(s));
    if (!base_name.empty()) {
      result.append("*");
      result.append(base_name);
    }
  }
  return result;
}

/**
 * Class to hold the name of each basis multivector for each algebra.
 */
template <typename AlgebraT>
class Bases final {
 public:
  using AlgebraType = AlgebraT;
  static constexpr size_t BASES_COUNT{AlgebraType::bases_count()};
  // By default, an Algebra does not have any bases that have explicit names.
  // TODO(james): Autogenerate names of basis vectors in this default, unspecialized class.
  static constexpr size_t NAMED_BASES_COUNT{0};

  constexpr Bases() = default;

  static constexpr std::array<BasisName<AlgebraType>, NAMED_BASES_COUNT> bases() { return {}; }

  static std::string to_string(const math::Multivector<AlgebraType>& vec) {
    return math::to_string(vec);
  }
};

template <>
class Bases<math::Complex<>> final {
 public:
  using AlgebraType = math::Complex<>;
  static constexpr size_t BASES_COUNT{AlgebraType::bases_count()};
  static constexpr size_t NAMED_BASES_COUNT{BASES_COUNT - 1};

 private:
  static constexpr std::array<BasisName<AlgebraType>, NAMED_BASES_COUNT> bases_{
      BasisName<AlgebraType>{"i", AlgebraType::VectorType::e<0>()},
  };

 public:
  constexpr Bases() = default;

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

template <>
class Bases<math::Vga<>> final {
 public:
  using AlgebraType = math::Vga<>;
  static constexpr size_t BASES_COUNT{AlgebraType::bases_count()};
  static constexpr size_t NAMED_BASES_COUNT{BASES_COUNT - 1};

 private:
  static constexpr std::array<BasisName<AlgebraType>, NAMED_BASES_COUNT> bases_{
      BasisName<AlgebraType>{"e1", math::Multivector<AlgebraType>::e<0>()},
      {"e2", math::Multivector<AlgebraType>::e<1>()},
      {"e12", math::Multivector<AlgebraType>::e<0>() * math::Multivector<AlgebraType>::e<1>()},
      {"e3", math::Multivector<AlgebraType>::e<2>()},
      {"e13", math::Multivector<AlgebraType>::e<0>() * math::Multivector<AlgebraType>::e<2>()},
      {"e23", math::Multivector<AlgebraType>::e<1>() * math::Multivector<AlgebraType>::e<2>()},
      {"e123", math::Multivector<AlgebraType>::e<0>() * math::Multivector<AlgebraType>::e<1>() *
                   math::Multivector<AlgebraType>::e<2>()},
  };

 public:
  constexpr Bases() = default;

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

template <>
class Bases<math::Vga2d<>> final {
 public:
  using AlgebraType = math::Vga2d<>;
  static constexpr size_t BASES_COUNT{AlgebraType::bases_count()};
  static constexpr size_t NAMED_BASES_COUNT{BASES_COUNT - 1};

 private:
  static constexpr std::array<BasisName<AlgebraType>, NAMED_BASES_COUNT> bases_{
      BasisName<AlgebraType>{"e1", math::Multivector<AlgebraType>::e<0>()},
      {"e2", math::Multivector<AlgebraType>::e<1>()},
      {"e12", math::Multivector<AlgebraType>::e<0>() * math::Multivector<AlgebraType>::e<1>()},
  };

 public:
  constexpr Bases() = default;

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
