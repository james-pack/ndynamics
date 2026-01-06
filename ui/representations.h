#pragma once

#include "math/algebra.h"
#include "math/multivector.h"

namespace ndyn::ui {

template <typename AlgebraT, size_t NAME_SIZE = 5>
struct BasisName final {
  char name[NAME_SIZE];
  math::Multivector<AlgebraT> basis;
};

/**
 * Class to hold the name of each basis multivector for each algebra.
 */
template <typename AlgebraT>
class Bases final {
 public:
  using AlgebraType = AlgebraT;
  static constexpr size_t BASES_COUNT{AlgebraType::bases_count()};

  constexpr Bases() = default;

  static constexpr const std::array<BasisName<AlgebraType>, 0>& bases() {
    return std::array<BasisName<AlgebraType>, 0>();
  }

  static std::string to_string(const math::Multivector<AlgebraType>& vec) {
    return math::to_string(vec);
  }
};

template <>
class Bases<math::Vga<>> final {
 public:
  using AlgebraType = math::Vga<>;
  static constexpr size_t BASES_COUNT{AlgebraType::bases_count()};

 private:
  static constexpr std::array<BasisName<AlgebraType>, BASES_COUNT> bases_{
      BasisName<AlgebraType>{"e1", math::Multivector<AlgebraType>::e<0>()},
      {"e2", math::Multivector<AlgebraType>::e<1>()},
      {"e3", math::Multivector<AlgebraType>::e<2>()},
      {"e12", math::Multivector<AlgebraType>::e<0>() * math::Multivector<AlgebraType>::e<1>()},
      {"e13", math::Multivector<AlgebraType>::e<0>() * math::Multivector<AlgebraType>::e<2>()},
      {"e23", math::Multivector<AlgebraType>::e<1>() * math::Multivector<AlgebraType>::e<2>()},
      {"e123", math::Multivector<AlgebraType>::e<0>() * math::Multivector<AlgebraType>::e<1>() *
                   math::Multivector<AlgebraType>::e<2>()},
  };

 public:
  constexpr Bases() = default;

  static constexpr const std::array<BasisName<AlgebraType>, BASES_COUNT>& bases() { return bases_; }

  static std::string to_string(const math::Multivector<AlgebraType>& vec) {
    return math::to_string(vec);
  }
};

template <>
class Bases<math::Vga2d<>> final {
 public:
  using AlgebraType = math::Vga2d<>;
  static constexpr size_t BASES_COUNT{AlgebraType::bases_count()};

 private:
  static constexpr std::array<BasisName<AlgebraType>, BASES_COUNT> bases_{
      BasisName<AlgebraType>{"e1", math::Multivector<AlgebraType>::e<0>()},
      {"e2", math::Multivector<AlgebraType>::e<1>()},
      {"e12", math::Multivector<AlgebraType>::e<0>() * math::Multivector<AlgebraType>::e<1>()},
  };

 public:
  constexpr Bases() = default;

  static constexpr const std::array<BasisName<AlgebraType>, BASES_COUNT>& bases() { return bases_; }

  static std::string to_string(const math::Multivector<AlgebraType>& vec) {
    return math::to_string(vec);
  }
};

}  // namespace ndyn::ui
