#pragma once

#include <array>
#include <bitset>
#include <cstring>
#include <stdexcept>

#include "base/except.h"
#include "math/caley.h"

namespace ndyn::math {

template <typename T, size_t POSITIVE_BASES, size_t NEGATIVE_BASES, size_t ZERO_BASES>
class Multivector final {
 public:
  static constexpr size_t SCALAR_BASES{1};
  static constexpr size_t SCALAR_GRADE{0};

  static constexpr size_t bases_count() { return POSITIVE_BASES + NEGATIVE_BASES + ZERO_BASES; }

  static constexpr size_t grade_count() { return 1UL << bases_count(); }

 private:
  static constexpr CaleyTable<T, POSITIVE_BASES, NEGATIVE_BASES, ZERO_BASES> caley_table_{};

  std::array<T, grade_count()> coefficients_{};

 public:
  constexpr Multivector() = default;

  constexpr Multivector(const Multivector& rhs) = default;
  constexpr Multivector(Multivector&& rhs) = default;

  explicit constexpr Multivector(const T& scalar) { coefficients_[SCALAR_GRADE] = scalar; }
  explicit constexpr Multivector(T&& scalar) {
    coefficients_[SCALAR_GRADE] = std::forward<T>(scalar);
  }

  constexpr Multivector& operator=(const Multivector& rhs) = default;
  constexpr Multivector& operator=(Multivector&& rhs) = default;

  constexpr const T& scalar() const { return coefficients_[SCALAR_GRADE]; }

  constexpr Multivector add(const T& rhs) const {
    Multivector result{*this};
    result.coefficients_[SCALAR_GRADE] += rhs;
    return result;
  }

  constexpr Multivector add(const Multivector& rhs) const {
    Multivector result{*this};
    for (size_t i = 0; i < grade_count(); ++i) {
      result.coefficients_[i] += rhs.coefficients_[i];
    }
    return result;
  }

  constexpr Multivector subtract(const T& rhs) const {
    Multivector result{*this};
    result.coefficients_[SCALAR_GRADE] -= rhs;
    return result;
  }

  constexpr Multivector subtract(const Multivector& rhs) const {
    Multivector result{*this};
    for (size_t i = 0; i < grade_count(); ++i) {
      result.coefficients_[i] -= rhs.coefficients_[i];
    }
    return result;
  }

  constexpr Multivector multiply(const T& rhs) const {
    Multivector result{*this};
    for (size_t i = 0; i < grade_count(); ++i) {
      result.coefficients_[i] *= rhs;
    }
    return result;
  }

  constexpr Multivector multiply(const Multivector& rhs) const {
    Multivector result{};
    for (size_t i = 0; i < grade_count(); ++i) {
      for (size_t j = 0; j < grade_count(); ++j) {
        const auto& caley_entry{caley_table_.entry(i, j)};
        result.coefficients_[caley_entry.grade] +=
            caley_entry.quadratic_multiplier * coefficients_[i] * rhs.coefficients_[j];
      }
    }
    return result;
  }

  // Operator overloads.
  constexpr bool operator==(const Multivector& rhs) const {
    return coefficients_ == rhs.coefficients_;
  }

  constexpr bool operator==(const T& rhs) const { return *this == Multivector{rhs}; }

  constexpr Multivector operator+(const T& rhs) const { return add(rhs); }
  constexpr Multivector operator+(const Multivector& rhs) const { return add(rhs); }

  constexpr Multivector operator-(const T& rhs) const { return subtract(rhs); }
  constexpr Multivector operator-(const Multivector& rhs) const { return subtract(rhs); }

  // Unary minus
  constexpr Multivector operator-() const { return multiply(-1); }

  constexpr Multivector operator*(const T& rhs) const { return multiply(rhs); }
  constexpr Multivector operator*(const Multivector& rhs) const { return multiply(rhs); }

  template <size_t N>
  static constexpr Multivector e() {
    if constexpr (N >= bases_count()) {
      // Note: we use a static_assert here to generate an error message for the user. We could have
      // used a template parameter to restrict which basis creation functions are generated by the
      // compiler, but that approach resulted in a much more cryptic error message from the
      // compiler. For completeness, here is the function signature with the template restriction:
      //   template <size_t N, std::enable_if_t<(N < bases_count()), bool> = true>
      //   static constexpr Multivector e();
      static_assert(
          N < bases_count(),
          "Template parameter to basis creation function is out of range of the number of "
          "bases. Template parameter must be less than the bases_count().");
    } else {
      Multivector result{};
      result.coefficients_.at(1UL << N) = 1;
      return result;
    }
  }
};

// Operator overloads where the multivector is not on the left side.

template <typename T, size_t POSITIVE_BASES, size_t NEGATIVE_BASES, size_t ZERO_BASES>
constexpr bool operator==(const T& scalar,
                          const Multivector<T, POSITIVE_BASES, NEGATIVE_BASES, ZERO_BASES>& v) {
  return v == scalar;
}

template <typename T, size_t POSITIVE_BASES, size_t NEGATIVE_BASES, size_t ZERO_BASES>
constexpr Multivector<T, POSITIVE_BASES, NEGATIVE_BASES, ZERO_BASES> operator+(
    const T& scalar, const Multivector<T, POSITIVE_BASES, NEGATIVE_BASES, ZERO_BASES>& v) {
  return v.add(scalar);
}

template <typename T, size_t POSITIVE_BASES, size_t NEGATIVE_BASES, size_t ZERO_BASES>
constexpr Multivector<T, POSITIVE_BASES, NEGATIVE_BASES, ZERO_BASES> operator-(
    const T& scalar, const Multivector<T, POSITIVE_BASES, NEGATIVE_BASES, ZERO_BASES>& v) {
  return v.multiply(-1).add(scalar);
}

template <typename T, size_t POSITIVE_BASES, size_t NEGATIVE_BASES, size_t ZERO_BASES>
constexpr Multivector<T, POSITIVE_BASES, NEGATIVE_BASES, ZERO_BASES> operator*(
    const T& scalar, const Multivector<T, POSITIVE_BASES, NEGATIVE_BASES, ZERO_BASES>& v) {
  return v.multiply(scalar);
}

template <typename T>
using ScalarMultivector = Multivector<T, 0, 0, 0>;

template <typename T>
using ComplexMultivector = Multivector<T, 0, 1, 0>;

template <typename T>
using DualMultivector = Multivector<T, 0, 0, 1>;

template <typename T>
using SplitComplexMultivector = Multivector<T, 1, 0, 0>;

template <typename T>
using SpacetimeMultivector = Multivector<T, 1, 3, 0>;

}  // namespace ndyn::math
