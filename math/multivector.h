#pragma once

#include <array>
#include <bitset>
#include <cstring>
#include <ostream>
#include <stdexcept>
#include <string>

#include "base/bits.h"
#include "base/except.h"
#include "math/cayley.h"

namespace ndyn::math {

enum class InnerProduct : uint8_t {
  LEFT_CONTRACTION,
  RIGHT_CONTRACTION,
  BIDIRECTIONAL,  // Left contraction when the grade of the lhs is lower, right contraction
                  // otherwise. Used in texts by Hestenes and others.
  NO_IMPLICIT_DEFINITION,
};

template <typename T, size_t POSITIVE_BASES, size_t NEGATIVE_BASES, size_t ZERO_BASES,
          InnerProduct INNER_PRODUCT_STYLE = InnerProduct::LEFT_CONTRACTION>
class Multivector final {
 public:
  static constexpr size_t SCALAR_COMPONENT_INDEX{0};

  static constexpr size_t bases_count() { return POSITIVE_BASES + NEGATIVE_BASES + ZERO_BASES; }
  static constexpr size_t grade_count() { return bases_count() + 1; }

  static constexpr size_t component_count() { return 1UL << bases_count(); }

 private:
  static constexpr CayleyTable<POSITIVE_BASES, NEGATIVE_BASES, ZERO_BASES> cayley_table_{};

  std::array<T, component_count()> coefficients_{};

 public:
  constexpr Multivector() = default;

  constexpr Multivector(const Multivector& rhs) = default;
  constexpr Multivector(Multivector&& rhs) = default;

  explicit constexpr Multivector(const T& scalar) {
    coefficients_[SCALAR_COMPONENT_INDEX] = scalar;
  }

  explicit constexpr Multivector(T&& scalar) {
    coefficients_[SCALAR_COMPONENT_INDEX] = std::forward<T>(scalar);
  }

  constexpr Multivector& operator=(const Multivector& rhs) = default;
  constexpr Multivector& operator=(Multivector&& rhs) = default;

  constexpr const T& scalar() const { return coefficients_[SCALAR_COMPONENT_INDEX]; }
  constexpr const T& component(size_t n) const { return coefficients_[n]; }

  constexpr Multivector grade(size_t grade) const {
    if (grade >= grade_count()) {
      except<std::domain_error>("Requested grade is larger than maximum grade of this multivector");
    }
    Multivector result{};
    for (size_t i = 0; i < component_count(); ++i) {
      if (bit_count(i) == grade) {
        result.coefficients_[i] = coefficients_[i];
      }
    }
    return result;
  }

  constexpr Multivector add(const T& rhs) const {
    Multivector result{*this};
    result.coefficients_[SCALAR_COMPONENT_INDEX] += rhs;
    return result;
  }

  constexpr Multivector add(const Multivector& rhs) const {
    Multivector result{*this};
    for (size_t i = 0; i < component_count(); ++i) {
      result.coefficients_[i] += rhs.coefficients_[i];
    }
    return result;
  }

  constexpr Multivector subtract(const T& rhs) const {
    Multivector result{*this};
    result.coefficients_[SCALAR_COMPONENT_INDEX] -= rhs;
    return result;
  }

  constexpr Multivector subtract(const Multivector& rhs) const {
    Multivector result{*this};
    for (size_t i = 0; i < component_count(); ++i) {
      result.coefficients_[i] -= rhs.coefficients_[i];
    }
    return result;
  }

  constexpr Multivector multiply(const T& rhs) const {
    Multivector result{*this};
    for (size_t i = 0; i < component_count(); ++i) {
      result.coefficients_[i] *= rhs;
    }
    return result;
  }

  constexpr Multivector multiply(const Multivector& rhs) const {
    Multivector result{};
    for (size_t i = 0; i < component_count(); ++i) {
      for (size_t j = 0; j < component_count(); ++j) {
        const auto& cayley_entry{cayley_table_.entry(i, j)};
        result.coefficients_[cayley_entry.grade()] +=
            cayley_entry.quadratic_multiplier() * coefficients_[i] * rhs.coefficients_[j];
      }
    }
    return result;
  }

  // Inner product variations.

  /**
   * Left contraction projects the lhs (this Multivector) onto the rhs.
   */
  constexpr Multivector left_contraction(const Multivector& rhs) const {
    Multivector result{};
    for (size_t i = 0; i < component_count(); ++i) {
      // Note the initialization in the for-loop below. All of the components where the grade of i
      // is less than the grade of j will not contribute to the result. Also, j must include every
      // basis in i otherwise the two components are orthogonal to each other.
      for (size_t j = i; j < component_count(); ++j) {
        // Here we ensure that the two components are not orthogonal to each other, in the sense
        // that the rhs component (j) must include all of the bases in the lhs component (i).
        // Otherwise, the left_contraction of these components is zero, and the result is unchanged.
        // Note that for i = 0 (the scalar component of the lhs), this case will always be true, and
        // the effective result is that the left contraction of scalars onto other components adds a
        // scaled version of the rhs to the result.
        if ((i & j) == i) {
          const auto& cayley_entry{cayley_table_.entry(i, j)};
          result.coefficients_[j - i] +=
              cayley_entry.quadratic_multiplier() * coefficients_[i] * rhs.coefficients_[j];
        }
      }
    }
    return result;
  }

  /**
   * Right contraction projects the rhs onto this Multivector, the lhs.
   */
  constexpr Multivector right_contraction(const Multivector& rhs) const {
    return rhs.left_contraction(*this);
  }

  /**
   * The bidirectional inner project projects each pair of components individually, according to
   * which component has the lower grade. When the lhs component has the lower grade, the lhs
   * component is projected onto the rhs component. Similarly, the rhs component is projected onto
   * the lhs component when the rhs component has the lower grade. When the two have the same grade,
   * the operation is symmetric and can be thought of as projecting the lhs component onto the rhs
   * component or vice versa.
   */
  constexpr Multivector bidirectional_inner(const Multivector& rhs) const {
    Multivector result{};
    for (size_t i = 0; i < component_count(); ++i) {
      for (size_t j = 0; j < component_count(); ++j) {
        // If the lhs component is a lower grade, compute the inner product as the lhs component
        // being projected on the rhs. Otherwise, project the rhs component on the lhs. The
        // implementation here is to simply select the appropriate Cayley table entry according to
        // which component is being projected.
        // Note that the grade of the component and value of the component are not the same. The
        // grade is the number of bits set in the value. But, if the value is lower and the grade is
        // higher (value of 3, meaning a grade of 2, is less than a value of 4, a grade of 1), the
        // resulting components are orthogonal and the inner product will be zero.
        if (i < j) {
          if ((i & j) == i) {
            const auto& cayley_entry{cayley_table_.entry(i, j)};
            result.coefficients_[j - i] +=
                cayley_entry.quadratic_multiplier() * coefficients_[i] * rhs.coefficients_[j];
          }
        } else {
          if ((i & j) == j) {
            const auto& cayley_entry{cayley_table_.entry(j, i)};
            result.coefficients_[i - j] +=
                cayley_entry.quadratic_multiplier() * coefficients_[i] * rhs.coefficients_[j];
          }
        }
      }
    }
    return result;
  }

  /**
   * The inner product. Note that the inner product is not uniformly defined across geometric
   * algebra texts. In some texts, particularly those by Hestenes, the inner product is defined as
   * the bidirectional_inner(). In other texts, usually those more focused on the mathematical
   * structure, the inner product is defined as the left_contraction(). The right_contraction()
   * style is provided for completeness. All of these approaches are useful, so the Multivector
   * includes the style of the inner product as part of the type. The inner() method below
   * implements selecting the style based on the type definition. Finally, all three approaches are
   * exposed in the API of this class, so they may be used explicitly as needed.
   */
  constexpr Multivector inner(const Multivector& rhs) const {
    static_assert(
        INNER_PRODUCT_STYLE != InnerProduct::NO_IMPLICIT_DEFINITION,
        "inner() method not defined since Multivector type has no implicit definition of "
        "the inner product. Must explicitly use either the left contraction, right "
        "contraction, or bidirectional inner product operations on this Multivector type.");
    if constexpr (INNER_PRODUCT_STYLE == InnerProduct::LEFT_CONTRACTION) {
      return left_contraction(rhs);
    } else if constexpr (INNER_PRODUCT_STYLE == InnerProduct::RIGHT_CONTRACTION) {
      return right_contraction(rhs);
    } else if constexpr (INNER_PRODUCT_STYLE == InnerProduct::BIDIRECTIONAL) {
      return bidirectional_inner(rhs);
    }
  }

  /**
   * The outer product, also known as the wedge operator.
   */
  constexpr Multivector outer(const Multivector& rhs) const {
    using std::to_string;
    Multivector result{};
    for (size_t i = 0; i < component_count(); ++i) {
      // Note the exit condition of this for-loop. We only loop while i+j is less than the component
      // count.
      for (size_t j = 0; i + j < component_count(); ++j) {
        const auto& cayley_entry{cayley_table_.entry(i, j)};
        if (bit_count(cayley_entry.grade()) == bit_count(i) + bit_count(j)) {
          result.coefficients_[i + j] +=
              cayley_entry.quadratic_multiplier() * coefficients_[i] * rhs.coefficients_[j];
        }
      }
    }
    return result;
  }

  // Operator overloads.
  constexpr bool operator==(const Multivector& rhs) const {
    // Note that std::array::operator==() does not work in a constexpr environment until C++20, so
    // we have to implement this ourselves for earlier versions.
    for (size_t i = 0; i < component_count(); ++i) {
      if (coefficients_[i] != rhs.coefficients_[i]) {
        return false;
      }
    }
    return true;
  }

  // Equality.
  constexpr bool operator==(const T& rhs) const { return *this == Multivector{rhs}; }

  // Addition.
  constexpr Multivector operator+(const T& rhs) const { return add(rhs); }
  constexpr Multivector operator+(const Multivector& rhs) const { return add(rhs); }

  // Subtraction.
  constexpr Multivector operator-(const T& rhs) const { return subtract(rhs); }
  constexpr Multivector operator-(const Multivector& rhs) const { return subtract(rhs); }

  // Unary minus.
  constexpr Multivector operator-() const { return multiply(-1); }

  // Geometric product.
  constexpr Multivector operator*(const T& rhs) const { return multiply(rhs); }
  constexpr Multivector operator*(const Multivector& rhs) const { return multiply(rhs); }

  // The inner product operator below is based on the operator expressions defined on
  // https://bivecctor.net/. Not completely sure this notation is useful, and it may actually create
  // readability issues, since it isn't standardized.

  // Inner product.
  constexpr Multivector operator|(const Multivector& rhs) const { return inner(rhs); }

  // Generate a Multivector of a single component. These can be combined to generate any
  // Multivector. See the tests for examples.
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

template <typename T, size_t POSITIVE_BASES, size_t NEGATIVE_BASES, size_t ZERO_BASES,
          InnerProduct INNER_PRODUCT_STYLE>
constexpr bool operator==(
    const T& scalar,
    const Multivector<T, POSITIVE_BASES, NEGATIVE_BASES, ZERO_BASES, INNER_PRODUCT_STYLE>& v) {
  return v == scalar;
}

template <typename T, size_t POSITIVE_BASES, size_t NEGATIVE_BASES, size_t ZERO_BASES,
          InnerProduct INNER_PRODUCT_STYLE>
constexpr Multivector<T, POSITIVE_BASES, NEGATIVE_BASES, ZERO_BASES, INNER_PRODUCT_STYLE> operator+(
    const T& scalar,
    const Multivector<T, POSITIVE_BASES, NEGATIVE_BASES, ZERO_BASES, INNER_PRODUCT_STYLE>& v) {
  return v.add(scalar);
}

template <typename T, size_t POSITIVE_BASES, size_t NEGATIVE_BASES, size_t ZERO_BASES,
          InnerProduct INNER_PRODUCT_STYLE>
constexpr Multivector<T, POSITIVE_BASES, NEGATIVE_BASES, ZERO_BASES, INNER_PRODUCT_STYLE> operator-(
    const T& scalar,
    const Multivector<T, POSITIVE_BASES, NEGATIVE_BASES, ZERO_BASES, INNER_PRODUCT_STYLE>& v) {
  return v.multiply(-1).add(scalar);
}

template <typename T, size_t POSITIVE_BASES, size_t NEGATIVE_BASES, size_t ZERO_BASES,
          InnerProduct INNER_PRODUCT_STYLE>
constexpr Multivector<T, POSITIVE_BASES, NEGATIVE_BASES, ZERO_BASES, INNER_PRODUCT_STYLE> operator*(
    const T& scalar,
    const Multivector<T, POSITIVE_BASES, NEGATIVE_BASES, ZERO_BASES, INNER_PRODUCT_STYLE>& v) {
  return v.multiply(scalar);
}

// String and printing operations.

template <typename T, size_t POSITIVE_BASES, size_t NEGATIVE_BASES, size_t ZERO_BASES,
          InnerProduct INNER_PRODUCT_STYLE>
std::string to_string(
    const Multivector<T, POSITIVE_BASES, NEGATIVE_BASES, ZERO_BASES, INNER_PRODUCT_STYLE>& v) {
  using std::to_string;
  std::string result{};
  result.append("[");
  bool need_comma{false};
  for (size_t i = 0; i < v.component_count(); ++i) {
    if (need_comma) {
      result.append(", ");
    }
    need_comma = true;
    result.append(to_string(v.component(i)));
  }
  result.append("]");
  return result;
}

template <typename T, size_t POSITIVE_BASES, size_t NEGATIVE_BASES, size_t ZERO_BASES,
          InnerProduct INNER_PRODUCT_STYLE>
std::ostream& operator<<(
    std::ostream& os,
    const Multivector<T, POSITIVE_BASES, NEGATIVE_BASES, ZERO_BASES, INNER_PRODUCT_STYLE>& v) {
  os << to_string(v);
  return os;
}

// Common algebras.
template <typename T, InnerProduct INNER_PRODUCT_STYLE = InnerProduct::LEFT_CONTRACTION>
using ScalarMultivector = Multivector<T, 0, 0, 0, INNER_PRODUCT_STYLE>;

template <typename T, InnerProduct INNER_PRODUCT_STYLE = InnerProduct::LEFT_CONTRACTION>
using ComplexMultivector = Multivector<T, 0, 1, 0, INNER_PRODUCT_STYLE>;

template <typename T, InnerProduct INNER_PRODUCT_STYLE = InnerProduct::LEFT_CONTRACTION>
using DualMultivector = Multivector<T, 0, 0, 1, INNER_PRODUCT_STYLE>;

template <typename T, InnerProduct INNER_PRODUCT_STYLE = InnerProduct::LEFT_CONTRACTION>
using SplitComplexMultivector = Multivector<T, 1, 0, 0, INNER_PRODUCT_STYLE>;

// VGA is a standard 3D vectorspace geometric algebra. It is used in non-relativistic physics and
// engineering applications.
template <typename T, InnerProduct INNER_PRODUCT_STYLE = InnerProduct::LEFT_CONTRACTION>
using VgaMultivector = Multivector<T, 3, 0, 0, INNER_PRODUCT_STYLE>;

// The spacetime algebra is primarily used in relativistic physics applications and research.
template <typename T, InnerProduct INNER_PRODUCT_STYLE = InnerProduct::LEFT_CONTRACTION>
using SpacetimeMultivector = Multivector<T, 1, 3, 0, INNER_PRODUCT_STYLE>;

}  // namespace ndyn::math
