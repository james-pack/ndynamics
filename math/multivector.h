#pragma once

#include <array>
#include <cmath>
#include <cstdint>
#include <initializer_list>
#include <stdexcept>

#include "base/bits.h"
#include "base/except.h"
#include "math/algebra.h"
#include "math/cayley.h"
#include "math/unitary_ops.h"

namespace ndyn::math {

// template <typename ScalarT, size_t POSITIVE_BASES, size_t NEGATIVE_BASES, size_t ZERO_BASES,
//           InnerProduct INNER_PRODUCT_STYLE = InnerProduct::LEFT_CONTRACTION>
template <typename AlgebraT>
class Multivector final {
 public:
  using AlgebraType = AlgebraT;

  using ScalarType = typename AlgebraType::ScalarType;

  // Number of grade-1 basis elements (vectors) in this multivector.
  static constexpr size_t vector_count() { return AlgebraType::vector_count(); }
  // Number of grades in this multivector, counting the scalar (grade-0) as its own grade.
  static constexpr size_t grade_count() { return AlgebraType::grade_count(); }

  // Number of bases in this multivector, including the number of vectors, bivectors, trivectors,
  // etc., and scalars.
  static constexpr size_t bases_count() { return AlgebraType::bases_count(); }

  static constexpr size_t POSITIVE_BASES{AlgebraType::POSITIVE_BASES};
  static constexpr size_t NEGATIVE_BASES{AlgebraType::NEGATIVE_BASES};
  static constexpr size_t ZERO_BASES{AlgebraType::ZERO_BASES};

  static constexpr size_t NUM_POSITIVE_BASES{AlgebraType::POSITIVE_BASES};
  static constexpr size_t NUM_NEGATIVE_BASES{AlgebraType::NEGATIVE_BASES};
  static constexpr size_t NUM_ZERO_BASES{AlgebraType::ZERO_BASES};

  static constexpr InnerProduct INNER_PRODUCT{AlgebraType::INNER_PRODUCT};

  static constexpr size_t SCALAR_BASIS_INDEX{0};

 private:
  static constexpr CayleyTable<POSITIVE_BASES, NEGATIVE_BASES, ZERO_BASES> cayley_table_{};
  using UnitaryOpsType = UnitaryOps<POSITIVE_BASES, NEGATIVE_BASES, ZERO_BASES>;

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

  constexpr Multivector normalized() const {
    using std::sqrt;
    return divide(sqrt(square_magnitude()));
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
        INNER_PRODUCT != InnerProduct::NO_IMPLICIT_DEFINITION,
        "inner() method not defined since Multivector type has no implicit definition of "
        "the inner product. Must explicitly use either the left contraction, right "
        "contraction, or bidirectional inner product operations on this Multivector type.");
    if constexpr (INNER_PRODUCT == InnerProduct::LEFT_CONTRACTION) {
      return left_contraction(rhs);
    } else if constexpr (INNER_PRODUCT == InnerProduct::RIGHT_CONTRACTION) {
      return right_contraction(rhs);
    } else if constexpr (INNER_PRODUCT == InnerProduct::BIDIRECTIONAL) {
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
      result.coefficients_[bases_count() - 1 - i] = UnitaryOpsType::dual[i] * coefficients_[i];
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
};

// Operator overloads where the multivector is not on the left side.

template <typename AlgebraType>
constexpr bool operator==(const typename AlgebraType::ScalarType& scalar,
                          const Multivector<AlgebraType>& v) {
  return v == scalar;
}

template <typename AlgebraType>
constexpr Multivector<AlgebraType> operator+(const typename AlgebraType::ScalarType& scalar,
                                             const Multivector<AlgebraType>& v) {
  return v.add(scalar);
}

template <typename AlgebraType>
constexpr Multivector<AlgebraType> operator-(const typename AlgebraType::ScalarType& scalar,
                                             const Multivector<AlgebraType>& v) {
  return v.multiply(-1).add(scalar);
}

template <typename AlgebraType>
constexpr Multivector<AlgebraType> operator*(const typename AlgebraType::ScalarType& scalar,
                                             const Multivector<AlgebraType>& v) {
  return v.multiply(scalar);
}

// Multivector types of common algebras.
template <typename T = DefaultScalarType,
          InnerProduct INNER_PRODUCT = InnerProduct::LEFT_CONTRACTION>
using ScalarMultivector = typename Scalar<T, INNER_PRODUCT>::VectorType;

template <typename T = DefaultScalarType,
          InnerProduct INNER_PRODUCT = InnerProduct::LEFT_CONTRACTION>
using ComplexMultivector = typename Complex<T, INNER_PRODUCT>::VectorType;

template <typename T = DefaultScalarType,
          InnerProduct INNER_PRODUCT = InnerProduct::LEFT_CONTRACTION>
using DualMultivector = typename Dual<T, INNER_PRODUCT>::VectorType;

template <typename T = DefaultScalarType,
          InnerProduct INNER_PRODUCT = InnerProduct::LEFT_CONTRACTION>
using SplitComplexMultivector = typename SplitComplex<T, INNER_PRODUCT>::VectorType;

template <typename T = DefaultScalarType,
          InnerProduct INNER_PRODUCT = InnerProduct::LEFT_CONTRACTION>
using Vga2dMultivector = typename Vga2d<T, INNER_PRODUCT>::VectorType;

template <typename T = DefaultScalarType,
          InnerProduct INNER_PRODUCT = InnerProduct::LEFT_CONTRACTION>
using VgaMultivector = typename Vga<T, INNER_PRODUCT>::VectorType;

template <typename T = DefaultScalarType,
          InnerProduct INNER_PRODUCT = InnerProduct::LEFT_CONTRACTION>
using Pga2dMultivector = typename Pga2d<T, INNER_PRODUCT>::VectorType;

template <typename T = DefaultScalarType,
          InnerProduct INNER_PRODUCT = InnerProduct::LEFT_CONTRACTION>
using PgaMultivector = typename Pga<T, INNER_PRODUCT>::VectorType;

template <typename T = DefaultScalarType,
          InnerProduct INNER_PRODUCT = InnerProduct::LEFT_CONTRACTION>
using SpacetimeMultivector = typename Spacetime<T, INNER_PRODUCT>::VectorType;

}  // namespace ndyn::math
