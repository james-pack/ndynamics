#pragma once

#include <array>
#include <cmath>
#include <cstdint>
#include <initializer_list>
#include <ostream>
#include <stdexcept>
#include <string>

#include "base/bits.h"
#include "base/except.h"
#include "math/cayley.h"
#include "math/unitary_op_signs.h"

namespace ndyn::math {

enum class InnerProduct : uint8_t {
  LEFT_CONTRACTION,
  RIGHT_CONTRACTION,
  BIDIRECTIONAL,  // Left contraction when the grade of the lhs is lower, right contraction
                  // otherwise. Used in texts by Hestenes and others.
  NO_IMPLICIT_DEFINITION,
};

template <typename ScalarT, size_t POSITIVE_BASES, size_t NEGATIVE_BASES, size_t ZERO_BASES,
          InnerProduct INNER_PRODUCT_STYLE = InnerProduct::LEFT_CONTRACTION>
class Multivector final {
 public:
  using ScalarType = ScalarT;

  // Number of grade-1 basis elements (vectors) in this multivector.
  static constexpr size_t vector_count() { return POSITIVE_BASES + NEGATIVE_BASES + ZERO_BASES; }
  // Number of grades in this multivector, counting the scalar (grade-0) as its own grade.
  static constexpr size_t grade_count() { return vector_count() + 1; }

  // Number of bases in this multivector, including the number of vectors, bivectors, trivectors,
  // etc., and scalars.
  static constexpr size_t bases_count() { return 1UL << vector_count(); }

  static constexpr size_t NUM_POSITIVE_BASES{POSITIVE_BASES};
  static constexpr size_t NUM_NEGATIVE_BASES{NEGATIVE_BASES};
  static constexpr size_t NUM_ZERO_BASES{ZERO_BASES};

  static constexpr size_t SCALAR_BASIS_INDEX{0};

 private:
  static constexpr CayleyTable<POSITIVE_BASES, NEGATIVE_BASES, ZERO_BASES> cayley_table_{};
  using UnitaryOpSignsType = UnitaryOpSigns<POSITIVE_BASES, NEGATIVE_BASES, ZERO_BASES>;

  std::array<ScalarType, bases_count()> coefficients_{};

 public:
  constexpr Multivector() = default;

  constexpr Multivector(const Multivector& rhs) = default;
  constexpr Multivector(Multivector&& rhs) = default;

  constexpr Multivector(std::initializer_list<ScalarType> values) {
    size_t i = 0;
    for (ScalarType value : values) {
      coefficients_.at(i) = value;
      ++i;
    }
  }

  constexpr Multivector& operator=(const Multivector& rhs) = default;
  constexpr Multivector& operator=(Multivector&& rhs) = default;

  // TODO(james): This is way too slow. Rewrite using a more explicit sum of a function of the
  // coefficients.
  constexpr ScalarType square_magnitude() const {
    const Multivector reversed{reverse()};
    return multiply(reversed).scalar();
  }

  constexpr const ScalarType& scalar() const { return coefficients_[SCALAR_BASIS_INDEX]; }
  constexpr void set_scalar(const ScalarType& v) { coefficients_[SCALAR_BASIS_INDEX] = v; }

  constexpr Multivector grade_projection(size_t grade) const {
    if (grade >= grade_count()) {
      except<std::domain_error>("Requested grade is larger than maximum grade of this multivector");
    }
    Multivector result{};
    for (size_t i = 0; i < bases_count(); ++i) {
      if (bit_count(i) == grade) {
        result.coefficients_[i] = coefficients_[i];
      }
    }
    return result;
  }

  constexpr Multivector add(const ScalarType& rhs) const {
    Multivector result{*this};
    result.coefficients_[SCALAR_BASIS_INDEX] += rhs;
    return result;
  }

  constexpr Multivector add(const Multivector& rhs) const {
    Multivector result{*this};
    for (size_t i = 0; i < bases_count(); ++i) {
      result.coefficients_[i] += rhs.coefficients_[i];
    }
    return result;
  }

  constexpr Multivector subtract(const ScalarType& rhs) const {
    Multivector result{*this};
    result.coefficients_[SCALAR_BASIS_INDEX] -= rhs;
    return result;
  }

  constexpr Multivector subtract(const Multivector& rhs) const {
    Multivector result{*this};
    for (size_t i = 0; i < bases_count(); ++i) {
      result.coefficients_[i] -= rhs.coefficients_[i];
    }
    return result;
  }

  constexpr Multivector multiply(const ScalarType& rhs) const {
    Multivector result{*this};
    for (size_t i = 0; i < bases_count(); ++i) {
      result.coefficients_[i] *= rhs;
    }
    return result;
  }

  constexpr Multivector multiply(const Multivector& rhs) const {
    Multivector result{};
    for (size_t i = 0; i < bases_count(); ++i) {
      for (size_t j = 0; j < bases_count(); ++j) {
        const auto& cayley_entry{cayley_table_.entry(i, j)};
        result.coefficients_[cayley_entry.basis_index()] +=
            cayley_entry.quadratic_multiplier() * coefficients_[i] * rhs.coefficients_[j];
      }
    }
    return result;
  }

  constexpr Multivector divide(const ScalarType& rhs) const {
    Multivector result{*this};
    for (size_t i = 0; i < bases_count(); ++i) {
      result.coefficients_[i] /= rhs;
    }
    return result;
  }

  // Inner product variations.

  /**
   * Left contraction projects the lhs (this Multivector) onto the rhs.
   */
  constexpr Multivector left_contraction(const Multivector& rhs) const {
    Multivector result{};
    for (size_t i = 0; i < bases_count(); ++i) {
      // Note the initialization in the for-loop below. All of the basiss where the grade of i
      // is less than the grade of j will not contribute to the result. Also, j must include every
      // basis in i otherwise the two bases are orthogonal to each other.
      for (size_t j = i; j < bases_count(); ++j) {
        // Here we ensure that the two bases are not orthogonal to each other, in the sense
        // that the rhs basis (j) must include all of the bases in the lhs basis (i).
        // Otherwise, the left_contraction of these bases is zero, and the result is unchanged.
        // Note that for i = 0 (the scalar basis of the lhs), this case will always be true, and
        // the effective result is that the left contraction of scalars onto other bases adds a
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
   * The bidirectional inner project projects each pair of bases individually, according to
   * which basis has the lower grade. When the lhs basis has the lower grade, the lhs
   * basis is projected onto the rhs basis. Similarly, the rhs basis is projected onto
   * the lhs basis when the rhs basis has the lower grade. When the two have the same grade,
   * the operation is symmetric and can be thought of as projecting the lhs basis onto the rhs
   * basis or vice versa.
   */
  constexpr Multivector bidirectional_inner(const Multivector& rhs) const {
    Multivector result{};
    for (size_t i = 0; i < bases_count(); ++i) {
      for (size_t j = 0; j < bases_count(); ++j) {
        // If the lhs basis is a lower grade, compute the inner product as the lhs basis
        // being projected on the rhs. Otherwise, project the rhs basis on the lhs. The
        // implementation here is to simply select the appropriate Cayley table entry according to
        // which basis is being projected.
        // Note that the grade of the basis and value of the basis are not the same. The
        // grade is the number of bits set in the value. But, if the value is lower and the grade is
        // higher (value of 3, meaning a grade of 2, is less than a value of 4, a grade of 1), the
        // resulting bases are orthogonal and the inner product will be zero.
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
   * The outer product, also known as the wedge operator and the progressive product.
   */
  constexpr Multivector outer(const Multivector& rhs) const {
    Multivector result{};
    for (size_t i = 0; i < bases_count(); ++i) {
      // Note the exit condition of this for-loop. We only loop while i+j is less than the number of
      // bases.
      for (size_t j = 0; i + j < bases_count(); ++j) {
        const auto& cayley_entry{cayley_table_.entry(i, j)};
        if (bit_count(cayley_entry.basis_index()) == bit_count(i) + bit_count(j)) {
          result.coefficients_[i + j] +=
              cayley_entry.quadratic_multiplier() * coefficients_[i] * rhs.coefficients_[j];
        }
      }
    }
    return result;
  }

  /**
   * The regressive product, also known as the 'V' product. The dual of the regressive product is
   * the outer product of the duals of the operands.
   */
  constexpr Multivector regress(const Multivector& rhs) const {
    return dual().outer(rhs.dual()).dual();
  }

  /**
   * The reverse of this Multivector.
   */
  constexpr Multivector reverse() const {
    Multivector result{*this};
    for (size_t i = 0; i < bases_count(); ++i) {
      const auto grade{bit_count(i)};
      if (grade % 4 == 2 || grade % 4 == 3) {
        result.coefficients_[i] *= -1;
      }
    }
    return result;
  }

  /**
   * The conjugate of this Multivector.
   */
  constexpr Multivector conj() const {
    Multivector result{*this};
    for (size_t i = 0; i < bases_count(); ++i) {
      const auto grade{bit_count(i)};
      if (grade % 4 == 1 || grade % 4 == 2) {
        result.coefficients_[i] *= -1;
      }
    }
    return result;
  }

  /**
   * The inverse of a Multivector, if it exists. Note that this approach does not guarantee that an
   * inverse exists, and if it does not exist, this method gives an incorrect value.
   */
  constexpr Multivector inverse() const { return reverse() / square_magnitude(); }

  /**
   * The dual of a Multivector. The product of a unit multivector and its dual should be the
   * unit pseudoscalar.
   */
  constexpr Multivector dual() const {
    Multivector result{};
    for (size_t i = 0; i < bases_count(); ++i) {
      result.coefficients_[bases_count() - 1 - i] = UnitaryOpSignsType::dual[i] * coefficients_[i];
    }
    return result;
  }

  // Operator overloads.

  // Equality.
  constexpr bool operator==(const Multivector& rhs) const {
    // Note that std::array::operator==() does not work in a constexpr environment until C++20, so
    // we have to implement this ourselves for earlier versions.
    for (size_t i = 0; i < bases_count(); ++i) {
      if (coefficients_[i] != rhs.coefficients_[i]) {
        return false;
      }
    }
    return true;
  }

  constexpr bool operator!=(const Multivector& rhs) const {
    // Note that std::array::operator==() does not work in a constexpr environment until C++20, so
    // we have to implement this ourselves for earlier versions.
    for (size_t i = 0; i < bases_count(); ++i) {
      if (coefficients_[i] != rhs.coefficients_[i]) {
        return true;
      }
    }
    return false;
  }

  constexpr bool operator==(const ScalarType& rhs) const { return *this == Multivector{rhs}; }
  constexpr bool operator!=(const ScalarType& rhs) const { return *this != Multivector{rhs}; }

  // Addition.
  constexpr Multivector operator+(const ScalarType& rhs) const { return add(rhs); }
  constexpr Multivector operator+(const Multivector& rhs) const { return add(rhs); }

  // Subtraction.
  constexpr Multivector operator-(const ScalarType& rhs) const { return subtract(rhs); }
  constexpr Multivector operator-(const Multivector& rhs) const { return subtract(rhs); }

  // Unary minus.
  constexpr Multivector operator-() const { return multiply(-1); }

  // Reversion.
  constexpr Multivector operator~() const { return reverse(); }

  // Dual.
  constexpr Multivector operator!() const { return dual(); }

  // Geometric product.
  constexpr Multivector operator*(const ScalarType& rhs) const { return multiply(rhs); }
  constexpr Multivector operator*(const Multivector& rhs) const { return multiply(rhs); }

  // Wedge, or outer, product.
  constexpr Multivector operator^(const Multivector& rhs) const { return outer(rhs); }

  // Regressive, or 'V', product.
  constexpr Multivector operator&(const Multivector& rhs) const { return regress(rhs); }

  // Division by a scalar.
  constexpr Multivector operator/(const ScalarType& rhs) const { return divide(rhs); }

  // Self-modifying operators.
  constexpr Multivector& operator+=(const Multivector& rhs) {
    for (size_t i = 0; i < bases_count(); ++i) {
      coefficients_[i] += rhs.coefficients_[i];
    }
    return *this;
  }

  constexpr Multivector& operator-=(const Multivector& rhs) {
    for (size_t i = 0; i < bases_count(); ++i) {
      coefficients_[i] -= rhs.coefficients_[i];
    }
    return *this;
  }

  /*************************************************************************************************
   * These accessors rely on the notion of a "bit index". A bit index is a particular ordering of
   * the bases in a multivector so that the basis vectors are indexed by powers of 2. In particular,
   * the basis vector e0 is at index 1; the basis vector e1 is at index 2; the basis vector e2 is at
   * index 4; etc. This particular indexing strategy allows for easy calculation of the Cayley
   * tables for each algebra. It can also make for consistent indexing across algebras, except where
   * particular conventions in a specific algebra force deviations from the pattern.
   *
   * In general, using these accessors requires a solid understanding of this indexing strategy and
   * can be error-prone.
   *
   * TODO(james): Move these to the private section of the class or remove them altogether.
   *************************************************************************************************/
  template <size_t N>
  constexpr const ScalarType& element() {
    static_assert(
        N < vector_count(),
        "Template parameter to vector element function is out of range of the number of "
        "vectors (grade 1 bases). Template parameter must be less than the vector_count().");
    return coefficients_[1UL << N];
  }

  // Generate a Multivector of a single vector (grade 1) basis. These can be combined to generate
  // any Multivector. See the tests for examples.
  template <size_t N>
  static constexpr Multivector e() {
    if constexpr (N >= vector_count()) {
      // Note: we use a static_assert here to generate an error message for the user. We could have
      // used a template parameter to restrict which basis creation functions are generated by the
      // compiler, but that approach resulted in a much more cryptic error message from the
      // compiler. For completeness, here is the function signature with the template restriction:
      //   template <size_t N, std::enable_if_t<(N < vector_count()), bool> = true>
      //   static constexpr Multivector e();
      static_assert(
          N < vector_count(),
          "Template parameter to basis creation function is out of range of the number of "
          "vectors (grade 1 bases). Template parameter must be less than the vector_count().");
    } else {
      Multivector result{};
      result.coefficients_[1UL << N] = 1;
      return result;
    }
  }

  template <size_t n>
  constexpr const ScalarType& basis() const {
    static_assert(n < bases_count(), "Basis index out of range.");
    return coefficients_[n];
  }
  constexpr const ScalarType& basis(size_t n) const { return coefficients_.at(n); }

  constexpr const ScalarType& operator[](size_t n) const { return coefficients_.at(n); }

  template <size_t n>
  constexpr void set_basis(const ScalarType& v) {
    static_assert(n < bases_count(), "Basis index out of range.");
    coefficients_[n] = v;
  }
  constexpr void set_basis(size_t n, const ScalarType& v) { coefficients_.at(n) = v; }

  /*************************************************************************************************
   * This set of accessors is incorrect for most implementations.
   *
   * TODO(james): Remove them and port usages to the geometric abstractions, rather than using
   * Multivectors directly in most applications.
   *************************************************************************************************/
  constexpr const ScalarType& t() const {
    // If there are NEGATIVE_BASES, we assume that we are working in a variant of the spacetime
    // algebra with negative space-like bases. In this case, t() is traditionally the first
    // positive basis. Note that we assume Cl(1, 3) as the spacetime algebra here, rather than
    // Cl(3, 1). This decision is arbitrary and is based on the variant that seems to fit our
    // problem space best.
    static_assert(
        NEGATIVE_BASES >= 1,
        "The t() basis does not exist in this multivector. This selector is only designed to "
        "work in Cl(1, 3), the spacetime algebra with negative space-like bases.");
    return basis<1>();
  }
  constexpr void set_t(const ScalarType& v) {
    // If there are NEGATIVE_BASES, we assume that we are working in a variant of the spacetime
    // algebra with negative space-like bases. In this case, t() is traditionally the first
    // positive basis. Note that we assume Cl(1, 3) as the spacetime algebra here, rather than
    // Cl(3, 1). This decision is arbitrary and is based on the variant that seems to fit our
    // problem space best.
    static_assert(
        NEGATIVE_BASES >= 1,
        "The t() basis does not exist in this multivector. This selector is only designed to "
        "work in Cl(1, 3), the spacetime algebra with negative space-like bases.");
    set_basis<1>(v);
  }

  constexpr const ScalarType& x() const {
    if constexpr (NEGATIVE_BASES >= 1) {
      // If there are NEGATIVE_BASES, we assume that we are working in a variant of the spacetime
      // algebra with negative space-like bases. In this case, x() is traditionally the first
      // negative basis, y() is second, and z() is third.
      return basis<1 << POSITIVE_BASES>();
    } else {
      // If there are no NEGATIVE_BASES, we assume that we are in a vectorspace geometric algebra,
      // such as Cl(3,0). In this case, x() is traditionally the first positive basis, y() is
      // second, and z() is third.
      return basis<1>();
    }
  }

  constexpr void set_x(const ScalarType& v) {
    if constexpr (NEGATIVE_BASES >= 1) {
      // If there are NEGATIVE_BASES, we assume that we are working in a variant of the spacetime
      // algebra with negative space-like bases. In this case, x() is traditionally the first
      // negative basis, y() is second, and z() is third.
      set_basis<1 << POSITIVE_BASES>(v);
    } else {
      // If there are no NEGATIVE_BASES, we assume that we are in a vectorspace geometric algebra,
      // such as Cl(3,0). In this case, x() is traditionally the first positive basis, y() is
      // second, and z() is third.
      set_basis<1>(v);
    }
  }

  constexpr const ScalarType& y() const {
    if constexpr (NEGATIVE_BASES >= 1) {
      // If there are NEGATIVE_BASES, we assume that we are working in a variant of the spacetime
      // algebra with negative space-like bases. In this case, x() is traditionally the first
      // negative basis, y() is second, and z() is third.
      return basis<2 << POSITIVE_BASES>();
    } else {
      // If there are no NEGATIVE_BASES, we assume that we are in a vectorspace geometric algebra,
      // such as Cl(3,0). In this case, x() is traditionally the first positive basis, y() is
      // second, and z() is third.
      return basis<2>();
    }
  }

  constexpr void set_y(const ScalarType& v) {
    if constexpr (NEGATIVE_BASES >= 1) {
      // If there are NEGATIVE_BASES, we assume that we are working in a variant of the spacetime
      // algebra with negative space-like bases. In this case, x() is traditionally the first
      // negative basis, y() is second, and z() is third.
      set_basis<2 << POSITIVE_BASES>(v);
    } else {
      // If there are no NEGATIVE_BASES, we assume that we are in a vectorspace geometric algebra,
      // such as Cl(3,0). In this case, x() is traditionally the first positive basis, y() is
      // second, and z() is third.
      set_basis<2>(v);
    }
  }

  constexpr const ScalarType& z() const {
    if constexpr (NEGATIVE_BASES >= 1) {
      // If there are NEGATIVE_BASES, we assume that we are working in a variant of the spacetime
      // algebra with negative space-like bases. In this case, x() is traditionally the first
      // negative basis, y() is second, and z() is third.
      return basis<4 << POSITIVE_BASES>();
    } else {
      // If there are no NEGATIVE_BASES, we assume that we are in a vectorspace geometric algebra,
      // such as Cl(3,0). In this case, x() is traditionally the first positive basis, y() is
      // second, and z() is third.
      return basis<4>();
    }
  }

  constexpr void set_z(const ScalarType& v) {
    if constexpr (NEGATIVE_BASES >= 1) {
      // If there are NEGATIVE_BASES, we assume that we are working in a variant of the spacetime
      // algebra with negative space-like bases. In this case, x() is traditionally the first
      // negative basis, y() is second, and z() is third.
      set_basis<4 << POSITIVE_BASES>(v);
    } else {
      // If there are no NEGATIVE_BASES, we assume that we are in a vectorspace geometric algebra,
      // such as Cl(3,0). In this case, x() is traditionally the first positive basis, y() is
      // second, and z() is third.
      set_basis<4>(v);
    }
  }

  constexpr const ScalarType& r() const {
    if constexpr (NEGATIVE_BASES >= 1) {
      // If there are NEGATIVE_BASES, we assume that we are working in a cylindrical or spherical
      // variant of the spacetime algebra with negative space-like bases. In this case, r() is
      // traditionally the first negative basis, theta() is second, and psi() (or z()) is third.
      return basis<1 << POSITIVE_BASES>();
    } else {
      // If there are no NEGATIVE_BASES, we assume that we are in a vectorspace geometric algebra,
      // such as Cl(3,0). In this case, r() is traditionally the first positive basis, theta() is
      // second, and psi() (or z()) is third.
      return basis<1>();
    }
  }

  constexpr void set_r(const ScalarType& v) {
    if constexpr (NEGATIVE_BASES >= 1) {
      // If there are NEGATIVE_BASES, we assume that we are working in a cylindrical or spherical
      // variant of the spacetime algebra with negative space-like bases. In this case, r() is
      // traditionally the first negative basis, theta() is second, and psi() (or z()) is third.
      set_basis<1 << POSITIVE_BASES>(v);
    } else {
      // If there are no NEGATIVE_BASES, we assume that we are in a vectorspace geometric algebra,
      // such as Cl(3,0). In this case, r() is traditionally the first positive basis, theta() is
      // second, and psi() (or z()) is third.
      set_basis<1>(v);
    }
  }

  constexpr const ScalarType& theta() const {
    if constexpr (NEGATIVE_BASES >= 1) {
      // If there are NEGATIVE_BASES, we assume that we are working in a cylindrical or spherical
      // variant of the spacetime algebra with negative space-like bases. In this case, r() is
      // traditionally the first negative basis, theta() is second, and psi() (or z()) is third.
      return basis<2 << POSITIVE_BASES>();
    } else {
      // If there are no NEGATIVE_BASES, we assume that we are in a vectorspace geometric algebra,
      // such as Cl(3,0). In this case, r() is traditionally the first positive basis, theta() is
      // second, and psi() (or z()) is third.
      return basis<2>();
    }
  }

  constexpr void set_theta(const ScalarType& v) {
    if constexpr (NEGATIVE_BASES >= 1) {
      // If there are NEGATIVE_BASES, we assume that we are working in a cylindrical or spherical
      // variant of the spacetime algebra with negative space-like bases. In this case, r() is
      // traditionally the first negative basis, theta() is second, and psi() (or z()) is third.
      set_basis<2 << POSITIVE_BASES>(v);
    } else {
      // If there are no NEGATIVE_BASES, we assume that we are in a vectorspace geometric algebra,
      // such as Cl(3,0). In this case, r() is traditionally the first positive basis, theta() is
      // second, and psi() (or z()) is third.
      set_basis<2>(v);
    }
  }

  constexpr const ScalarType& phi() const {
    if constexpr (NEGATIVE_BASES >= 1) {
      // If there are NEGATIVE_BASES, we assume that we are working in a cylindrical or spherical
      // variant of the spacetime algebra with negative space-like bases. In this case, r() is
      // traditionally the first negative basis, theta() is second, and psi() (or z()) is third.
      return basis<4 << POSITIVE_BASES>();
    } else {
      // If there are no NEGATIVE_BASES, we assume that we are in a vectorspace geometric algebra,
      // such as Cl(3,0). In this case, r() is traditionally the first positive basis, theta() is
      // second, and psi() (or z()) is third.
      return basis<4>();
    }
  }

  constexpr void set_phi(const ScalarType& v) {
    if constexpr (NEGATIVE_BASES >= 1) {
      // If there are NEGATIVE_BASES, we assume that we are working in a cylindrical or spherical
      // variant of the spacetime algebra with negative space-like bases. In this case, r() is
      // traditionally the first negative basis, theta() is second, and psi() (or z()) is third.
      set_basis<4 << POSITIVE_BASES>(v);
    } else {
      // If there are no NEGATIVE_BASES, we assume that we are in a vectorspace geometric algebra,
      // such as Cl(3,0). In this case, r() is traditionally the first positive basis, theta() is
      // second, and psi() (or z()) is third.
      set_basis<4>(v);
    }
  }

  /**
   * Selector for the real component of a complex number.
   */
  constexpr const ScalarType& real() const {
    // Verify that we are in the complex numbers. This selector makes no sense in any other algebra.
    static_assert(NEGATIVE_BASES == 1 && POSITIVE_BASES == 0 && ZERO_BASES == 0,
                  "The real() selector is only defined for the complex numbers.");
    return basis<0>();
  }

  constexpr void set_real(const ScalarType& v) {
    // Verify that we are in the complex numbers. This selector makes no sense in any other algebra.
    static_assert(NEGATIVE_BASES == 1 && POSITIVE_BASES == 0 && ZERO_BASES == 0,
                  "The real() selector is only defined for the complex numbers.");
    set_basis<0>(v);
  }

  /**
   * Selector for the imaginary component of a complex number.
   */
  constexpr const ScalarType& imag() const {
    // Verify that we are in the complex numbers. This selector makes no sense in any other algebra.
    static_assert(NEGATIVE_BASES == 1 && POSITIVE_BASES == 0 && ZERO_BASES == 0,
                  "The imag() selector is only defined for the complex numbers.");
    return basis<1>();
  }

  constexpr void set_imag(const ScalarType& v) {
    // Verify that we are in the complex numbers. This selector makes no sense in any other algebra.
    static_assert(NEGATIVE_BASES == 1 && POSITIVE_BASES == 0 && ZERO_BASES == 0,
                  "The imag() selector is only defined for the complex numbers.");
    set_basis<1>(v);
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
  using std::abs;
  using std::to_string;

  std::string result{};
  bool need_comma{false};
  for (size_t i = 0; i < v.bases_count(); ++i) {
    // Don't show bases with zero coefficients.
    if (abs(v.basis(i)) > 0.000001) {
      if (need_comma) {
        result.append(" + ");
      }

      need_comma = true;
      result.append(to_string(v.basis(i)));
      if (i > 0) {
        result.append("*");
        if constexpr (ZERO_BASES > 0) {
          static constexpr std::array<const char*, 32> zero_basis_names{
              "",    "e0",   "e1",   "e01",   "e2",   "e02",   "e12",   "e012",    //
              "e3",  "e03",  "e13",  "e013",  "e23",  "e023",  "e123",  "e0123",   //
              "e4",  "e04",  "e14",  "e014",  "e24",  "e024",  "e124",  "e0124",   //
              "e34", "e034", "e134", "e0134", "e234", "e0234", "e1234", "e01234",  //
          };

          result.append(zero_basis_names[i]);
        } else {
          static constexpr std::array<const char*, 32> basis_names{
              "",    "e1",   "e2",   "e12",   "e3",   "e13",   "e23",   "e123",    //
              "e4",  "e14",  "e24",  "e124",  "e34",  "e134",  "e234",  "e1234",   //
              "e5",  "e15",  "e25",  "e125",  "e35",  "e135",  "e235",  "e1235",   //
              "e45", "e145", "e245", "e1245", "e345", "e1345", "e2345", "e12345",  //
          };

          result.append(basis_names[i]);
        }
      }
    }
  }

  if (result.empty()) {
    result.append("0");
  }

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

// VGA 2D is a standard ("vanilla") 2D vectorspace geometric algebra. It is used in non-relativistic
// physics and engineering applications.
template <typename T, InnerProduct INNER_PRODUCT_STYLE = InnerProduct::LEFT_CONTRACTION>
using Vga2dMultivector = Multivector<T, 2, 0, 0, INNER_PRODUCT_STYLE>;

// VGA is a standard ("vanilla") 3D vectorspace geometric algebra. It is used in non-relativistic
// physics and engineering applications.
template <typename T, InnerProduct INNER_PRODUCT_STYLE = InnerProduct::LEFT_CONTRACTION>
using VgaMultivector = Multivector<T, 3, 0, 0, INNER_PRODUCT_STYLE>;

// PGA 2D is a 2D vectorspace geometric algebra with an additional zero dimension. It is used in
// computer graphics, non-relativistic physics, and engineering applications.
template <typename T, InnerProduct INNER_PRODUCT_STYLE = InnerProduct::LEFT_CONTRACTION>
using Pga2dMultivector = Multivector<T, 2, 0, 1, INNER_PRODUCT_STYLE>;

// PGA is a 3D vectorspace geometric algebra with an additional zero dimension. It is used in
// computer graphics, non-relativistic physics, and engineering applications.
template <typename T, InnerProduct INNER_PRODUCT_STYLE = InnerProduct::LEFT_CONTRACTION>
using PgaMultivector = Multivector<T, 3, 0, 1, INNER_PRODUCT_STYLE>;

// The spacetime algebra is primarily used in relativistic physics applications and research.
// Note that we assume Cl(1, 3) as the spacetime algebra here, rather than
// Cl(3, 1). This decision is arbitrary and is based on the variant that seems to fit our
// problem space best.
template <typename T, InnerProduct INNER_PRODUCT_STYLE = InnerProduct::LEFT_CONTRACTION>
using SpacetimeMultivector = Multivector<T, 1, 3, 0, INNER_PRODUCT_STYLE>;

}  // namespace ndyn::math
