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

template <typename AlgebraT>
class Multivector final {
 public:
  using AlgebraType = AlgebraT;
  using ScalarType = typename AlgebraType::ScalarType;

  // Number of grade-1 basis elements (vectors) in this multivector.
  static constexpr size_t vector_count() { return AlgebraType::vector_count(); }

  static constexpr size_t NUM_POSITIVE_BASES{AlgebraType::NUM_POSITIVE_BASES};
  static constexpr size_t NUM_NEGATIVE_BASES{AlgebraType::NUM_NEGATIVE_BASES};
  static constexpr size_t NUM_ZERO_BASES{AlgebraType::NUM_ZERO_BASES};

  static constexpr InnerProduct INNER_PRODUCT{AlgebraType::INNER_PRODUCT};

  static constexpr size_t NUM_BASIS_VECTORS{AlgebraType::NUM_BASIS_VECTORS};
  static constexpr size_t NUM_BASIS_BLADES{AlgebraType::NUM_BASIS_BLADES};
  static constexpr size_t NUM_GRADES{NUM_BASIS_VECTORS + 1};

  static constexpr size_t SCALAR_BASIS_INDEX{0};

  static constexpr ScalarType EPSILON{AlgebraType::EPSILON};

 private:
  static constexpr CayleyTable<NUM_POSITIVE_BASES, NUM_NEGATIVE_BASES, NUM_ZERO_BASES>
      cayley_table_{};
  using UnitaryOpsType = UnitaryOps<NUM_POSITIVE_BASES, NUM_NEGATIVE_BASES, NUM_ZERO_BASES>;

  std::array<ScalarType, NUM_BASIS_BLADES> coefficients_{};

  static constexpr size_t DEGENERATE_BASIS_MASK = []() {
    size_t mask{};
    for (size_t i = 0; i < NUM_ZERO_BASES; ++i) {
      mask = mask | (1UL << i);
    }
    return mask;
  }();

  /**
   * Determine if a particular blade includes a basis vector that has a degenerate metric.
   */
  static constexpr bool includes_degenerate_basis(size_t blade_index) {
    return (DEGENERATE_BASIS_MASK & blade_index) != 0;
  }

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

  constexpr ScalarType square_magnitude() const {
    ScalarType result{};
    for (size_t i = 0; i < NUM_BASIS_BLADES; ++i) {
      const auto& cayley_entry{cayley_table_.entry(i, i)};
      result += cayley_entry.structure_constant * coefficients_[i] * coefficients_[i];
    }

    return result;
  }

  // TODO(james): Mark as constexpr if using C++26 or later. std::sqrt() is constexpr starting in
  // that dialect.
  Multivector normalize() const {
    using std::sqrt;
    return divide(sqrt(square_magnitude()));
  }

  constexpr const ScalarType& scalar() const { return coefficients_[SCALAR_BASIS_INDEX]; }
  constexpr void set_scalar(const ScalarType& v) { coefficients_[SCALAR_BASIS_INDEX] = v; }

  template <size_t GRADE>
  constexpr Multivector grade_projection() const {
    static_assert(GRADE < NUM_GRADES,
                  "Requested grade is larger than maximum grade of this multivector");

    Multivector result{};
    for (size_t i = 0; i < NUM_BASIS_BLADES; ++i) {
      if (bit_count(i) == GRADE) {
        result.coefficients_[i] = coefficients_[i];
      }
    }
    return result;
  }

  constexpr Multivector grade_projection(size_t grade) const {
    if (grade >= NUM_GRADES) {
      except<std::domain_error>("Requested grade is larger than maximum grade of this multivector");
    }
    Multivector result{};
    for (size_t i = 0; i < NUM_BASIS_BLADES; ++i) {
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
    for (size_t i = 0; i < NUM_BASIS_BLADES; ++i) {
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
    for (size_t i = 0; i < NUM_BASIS_BLADES; ++i) {
      result.coefficients_[i] -= rhs.coefficients_[i];
    }
    return result;
  }

  constexpr Multivector multiply(const ScalarType& rhs) const {
    Multivector result{*this};
    for (size_t i = 0; i < NUM_BASIS_BLADES; ++i) {
      result.coefficients_[i] *= rhs;
    }
    return result;
  }

  constexpr Multivector multiply(const Multivector& rhs) const {
    Multivector result{};
    for (size_t i = 0; i < NUM_BASIS_BLADES; ++i) {
      for (size_t j = 0; j < NUM_BASIS_BLADES; ++j) {
        const auto& cayley_entry{cayley_table_.entry(i, j)};
        result.coefficients_[cayley_entry.basis_index] +=
            cayley_entry.structure_constant * coefficients_[i] * rhs.coefficients_[j];
      }
    }
    return result;
  }

  constexpr Multivector divide(const ScalarType& rhs) const {
    Multivector result{*this};
    for (size_t i = 0; i < NUM_BASIS_BLADES; ++i) {
      result.coefficients_[i] /= rhs;
    }
    return result;
  }

  // Inner product variations.

  /**
   * Computes the left contraction (A ⌋ B) of this multivector (lhs) onto rhs.
   *
   * The left contraction A ⌋ B can be understood as the projection of A onto B:
   * it extracts the component of B that is "complementary to" A within B.
   * For basis blades, e_I ⌋ e_J is nonzero only when I ⊆ J (as sets of basis
   * vector indices), and the result is the blade e_{J\I} (J with I removed),
   * scaled by the appropriate structure constant from the Cayley table.
   */
  constexpr Multivector left_contraction(const Multivector& rhs) const {
    Multivector result{};
    for (size_t i = 0; i < NUM_BASIS_BLADES; ++i) {
      // The left contraction e_I ⌋ e_J is nonzero if and only if the basis
      // vectors of I are a subset of those of J, expressed as a bitmask
      // containment check. If I ⊄ J, these two basis blades are orthogonal
      // under the left contraction and contribute nothing to the result.
      // When i == 0 (scalar), this condition is always satisfied, so a scalar
      // lhs simply scales the entire rhs.
      // Note the initialization in the for-loop below. All of the bases where the grade of i
      // is less than the grade of j will not contribute to the result. Also, j must include every
      // basis in i otherwise the two bases are orthogonal to each other.
      for (size_t j = 0; j < NUM_BASIS_BLADES; ++j) {
        // When I ⊆ J, j ^ i (equivalently j - i here, since the bits of i
        // are a subset of j) gives the bitmask for the remaining blade e_{J\I}
        // after removing the bases of I from J. The structure constant accounts
        // for any sign change from reordering basis vectors into canonical form.
        if ((i & j) == i) {
          const auto& cayley_entry{cayley_table_.entry(i, j)};
          result.coefficients_[i ^ j] +=
              cayley_entry.structure_constant * coefficients_[i] * rhs.coefficients_[j];
        }
      }
    }
    return result;
  }

  /**
   * Computes the right contraction of this multivector (lhs) onto rhs. This is the opposite of
   * left_contraction(). See that method for more details.
   */
  constexpr Multivector right_contraction(const Multivector& rhs) const {
    Multivector result{};
    for (size_t i = 0; i < NUM_BASIS_BLADES; ++i) {
      for (size_t j = 0; j < NUM_BASIS_BLADES; ++j) {
        // The right contraction e_I ⌊ e_J is nonzero if and only if the basis
        // vectors of J are a subset of those of I, expressed as a bitmask
        // containment check. If J ⊄ I, these two basis blades contribute
        // nothing to the result.
        // When j == 0 (scalar rhs), this condition is always satisfied, so a
        // scalar rhs simply scales the entire lhs.
        if ((i & j) == j) {
          // i ^ j removes the bases of J from I, giving the remaining blade
          // e_{I\J}. The structure constant accounts for any sign change from
          // reordering basis vectors into canonical form.
          const auto& cayley_entry{cayley_table_.entry(j, i)};
          result.coefficients_[i ^ j] +=
              cayley_entry.structure_constant * coefficients_[i] * rhs.coefficients_[j];
        }
      }
    }
    return result;
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
    for (size_t i = 0; i < NUM_BASIS_BLADES; ++i) {
      for (size_t j = 0; j < NUM_BASIS_BLADES; ++j) {
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
            result.coefficients_[j ^ i] +=
                cayley_entry.structure_constant * coefficients_[i] * rhs.coefficients_[j];
          }
        } else {
          if ((i & j) == j) {
            const auto& cayley_entry{cayley_table_.entry(j, i)};
            result.coefficients_[i ^ j] +=
                cayley_entry.structure_constant * coefficients_[i] * rhs.coefficients_[j];
          }
        }
      }
    }
    return result;
  }

  /**
   * The inner product. Note that the inner product is not uniformly defined across geometric
   * algebra texts. In some texts, especially those by Leo Dorst, the inner product is defined as
   * the left_contraction(). The right_contraction() style was used extensively by Lounesto. A third
   * style, the BIDIRECTIONAL innner product, also generates a vector result is implemented here as
   * a form of completeness. All of these approaches are useful or worth study, so the Multivector
   * includes the style of the inner product as part of the type. The inner() method below
   * implements selecting the style based on the type definition. Finally, all three approaches are
   * exposed in the API of this class, so they may be used explicitly as needed. Note that
   * Hestenes's approach is NOT included as it generates a scalar and has some properties that make
   * it difficult to work with.
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
   * Computes the outer product, also known as the wedge operator and the progressive product.
   */
  constexpr Multivector outer(const Multivector& rhs) const {
    Multivector result{};
    for (size_t i = 0; i < NUM_BASIS_BLADES; ++i) {
      // Note the exit condition of this for-loop. We only loop while i+j is less than the number of
      // bases.
      for (size_t j = 0; j < NUM_BASIS_BLADES; ++j) {
        const auto& cayley_entry{cayley_table_.entry(i, j)};
        if (bit_count(cayley_entry.basis_index) == bit_count(i) + bit_count(j)) {
          result.coefficients_[i ^ j] +=
              cayley_entry.structure_constant * coefficients_[i] * rhs.coefficients_[j];
        }
      }
    }
    return result;
  }

  /**
   * Computes the regressive product, the dual counterpart of the outer product. Where the
   * outer product constructs the smallest subspace containing both operands, the regressive
   * product constructs the largest subspace contained in both. Whether this corresponds to
   * a geometric join or meet depends on the embedding conventions of the geometry model
   * in use — that assignment belongs in the model, not here.
   */
  constexpr Multivector regress(const Multivector& rhs) const {
    return (dual().outer(rhs.dual())).undual();
  }

  /**
   * The reverse of this Multivector.
   */
  constexpr Multivector reverse() const {
    Multivector result{*this};
    for (size_t i = 0; i < NUM_BASIS_BLADES; ++i) {
      const auto grade{bit_count(i)};
      if (grade % 4 == 2 || grade % 4 == 3) {
        result.coefficients_[i] *= -1;
      }
    }
    return result;
  }

  /**
   * The Clifford conjugate of this Multivector.
   */
  constexpr Multivector conj() const {
    Multivector result{*this};
    for (size_t i = 0; i < NUM_BASIS_BLADES; ++i) {
      const auto grade{bit_count(i)};
      if (grade % 4 == 1 || grade % 4 == 2) {
        result.coefficients_[i] *= -1;
      }
    }
    return result;
  }

  /**
   * The grade involution of this Multivector.
   *
   * Involution negates all odd-grade components and preserves all even-grade
   * components. It is the unique algebra automorphism that distinguishes the even and odd
   * subspaces: even(X) = (X + involute(X)) / 2 and odd(X) = (X - involute(X)) / 2. This
   * decomposition is fundamental to versor application — for an odd-grade versor V such as a
   * reflector, the correct sandwich product is V * x * involute(V)^{-1}, which differs from the
   * reverse-based form used for even-grade versors such as rotors. Without involute(), correctly
   * transforming vectors by odd-grade versors requires the caller to manually negate the
   * appropriate grades, which is error-prone and obscures the geometric intent of the operation.
   */
  constexpr Multivector involute() const {
    Multivector result{*this};
    for (size_t i = 0; i < NUM_BASIS_BLADES; ++i) {
      const auto grade{bit_count(i)};
      if (grade % 2 == 1) {
        result.coefficients_[i] *= -1;
      }
    }
    return result;
  }

  /**
   * Returns the inverse of this multivector under the geometric product, satisfying X * X⁻¹ = 1.
   *
   * This implementation uses the versor formula X⁻¹ = ~X / (X * ~X), which is correct only when
   * X * ~X is a pure scalar — a property guaranteed for versors (elements that are products of
   * invertible vectors, such as rotors, motors, and reflectors) but not for general multivectors.
   * For a general multivector, X * ~X may contain higher-grade components; discarding them and
   * dividing by only the scalar part produces an incorrect result with no indication of failure.
   *
   * Null elements (nonzero multivectors whose X * ~X = 0, such as ideal points and ideal lines in
   * PGA) have no inverse by definition. This implementation returns zero for such elements, which
   * is algebraically meaningless and will silently corrupt any computation that depends on it.
   *
   * A correct general inverse for arbitrary multivectors requires solving X * Y = 1 as a linear
   * system over the full 2^n coefficient space, where n is the number of basis vectors. This is
   * substantially more expensive and is not implemented here because the overwhelmingly common case
   * in geometric algebra computations is versor inversion. For null elements, no such system has
   * a solution and the correct response is to throw rather than return a sentinel value, as no
   * downstream computation can meaningfully recover from an uninvertible element.
   */
  // TODO(james): This implementation is only correct for versors. Extend to general Multivectors.
  constexpr Multivector inverse() const {
    const auto scale{square_magnitude()};
    if (scale > EPSILON) {
      return reverse() / scale;
    } else {
      // TODO(james): Determine better error handling mechanisms. For now, attempt to divide by
      // zero, just results in the zero Multivector.
      return {};
    }
  }

  /**
   * Duality maps a blade to its orthogonal complement via contraction with the pseudoscalar I
   * (the highest-grade blade of the algebra, representing the entire space). Geometrically,
   * the dual of a subspace is the subspace of all elements perpendicular to it — for example,
   * in 3D the dual of a line through the origin is a plane through the origin.
   *
   * The two operations differ in how they behave when composed: undual(dual(X)) = X exactly,
   * regardless of the algebra's signature. This is necessary because I² (the pseudoscalar
   * squared, which is always a scalar) is +1 in some algebras, -1 in others (e.g. G(3,0),
   * G(4,1)), and 0 in degenerate algebras (e.g. PGA G(3,0,1)). When I² = -1, applying the
   * same duality operation twice returns -X rather than X, so a naive implementation of
   * undual (i.e. undual(X) = X ⌋ I, identical to dual(X)) as a second application of dual would
   * silently introduce a sign error. When I² = 0 the pseudoscalar has no scalar inverse at all,
   * making a correction factor impossible. Using left contraction for dual and right contraction
   * for undual resolves all three cases uniformly, with the sign correction emerging from the
   * asymmetry between the two contractions rather than requiring an explicit case analysis on the
   * signature.
   */
  constexpr Multivector dual() const { return left_contraction(invertible_pseudoscalar()); }
  constexpr Multivector undual() const { return right_contraction(invertible_pseudoscalar()); }

  /**
   * Returns true if this multivector is a versor — that is, a product of invertible vectors.
   * Versors are the elements for which the inverse() formula X⁻¹ = ~X / (X * ~X) is correct.
   *
   * The test has two stages. First, the multivector must be purely even-grade or purely odd-grade;
   * a mixed-grade element cannot be a versor and is rejected cheaply before the more expensive
   * check. Second, the geometric product X * ~X must be a pure scalar — higher-grade components
   * in this product indicate that the versor formula would silently discard algebraically
   * significant terms. The scalar must also be nonzero, as null elements (ideal points and lines
   * in PGA, for example) are not invertible and therefore not versors in the sense required by
   * inverse().
   *
   * The tolerance parameter covers both the near-zero scalar check and the near-zero higher-grade
   * coefficient check in X * ~X. Callers working in algebras with very large or very small
   * coordinate magnitudes should scale this accordingly.
   */
  constexpr bool is_versor(const ScalarType tolerance = EPSILON) const {
    // Note: std::abs() is not constexpr until C++23. This wrapper delegates to std::abs() when
    // using C++23 or later, preserving full IEEE 754 compliance for production use, and falls back
    // to a simple negation in constexpr contexts where special float values are not a concern.
    constexpr auto abs = [](ScalarType v) constexpr -> ScalarType {
#if defined(__cpp_lib_constexpr_cmath) && __cpp_lib_constexpr_cmath >= 202202L
      return std::abs(v);
#else
      return v < ScalarType{0} ? -v : v;
#endif
    };

    // Cheap structural rejection: versors are purely even- or purely odd-grade.
    bool has_even{false};
    bool has_odd{false};
    for (size_t blade = 0; blade < NUM_BASIS_BLADES; ++blade) {
      if (abs(coefficients_[blade]) > tolerance) {
        // The grade of a blade is the number of bits set in its bitmask index (popcount).
        if (bit_count(blade) % 2 == 0) {
          has_even = true;
        } else {
          has_odd = true;
        }
      }
      if (has_even && has_odd) {
        return false;
      }
    }

    // X * ~X must be a nonzero pure scalar for the versor inverse formula to be valid.
    const Multivector product{multiply(reverse())};
    for (size_t blade = 1; blade < NUM_BASIS_BLADES; ++blade) {
      if (abs(product.coefficients_[blade]) > tolerance) {
        return false;
      }
    }
    return abs(product.coefficients_[0]) > tolerance;
  }

  // Operator overloads.

  // Equality. Note that these operators implement exact equality. Use near_equal() below for a more
  // reasonable equality check.
  constexpr bool operator==(const Multivector& rhs) const {
    // Note that std::array::operator==() does not work in a constexpr environment until C++20, so
    // we have to implement this ourselves for earlier versions.
    for (size_t i = 0; i < NUM_BASIS_BLADES; ++i) {
      if (coefficients_[i] != rhs.coefficients_[i]) {
        return false;
      }
    }
    return true;
  }

  constexpr bool operator!=(const Multivector& rhs) const {
    // Note that std::array::operator==() does not work in a constexpr environment until C++20, so
    // we have to implement this ourselves for earlier versions.
    for (size_t i = 0; i < NUM_BASIS_BLADES; ++i) {
      if (coefficients_[i] != rhs.coefficients_[i]) {
        return true;
      }
    }
    return false;
  }

  constexpr bool near_equal(const Multivector& rhs, const ScalarType tolerance = EPSILON) const {
    for (size_t i = 0; i < NUM_BASIS_BLADES; ++i) {
      const ScalarType diff{coefficients_[i] - rhs.coefficients_[i]};
      const ScalarType abs_diff{diff < ScalarType{0} ? -diff : diff};
      if (abs_diff > tolerance) {
        return false;
      }
    }
    return true;
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
    for (size_t i = 0; i < NUM_BASIS_BLADES; ++i) {
      coefficients_[i] += rhs.coefficients_[i];
    }
    return *this;
  }

  constexpr Multivector& operator-=(const Multivector& rhs) {
    for (size_t i = 0; i < NUM_BASIS_BLADES; ++i) {
      coefficients_[i] -= rhs.coefficients_[i];
    }
    return *this;
  }

  static constexpr Multivector pseudoscalar() {
    Multivector result{};
    result.coefficients_[NUM_BASIS_BLADES - 1] = 1;
    return result;
  }

  static constexpr Multivector invertible_pseudoscalar() {
    Multivector result{};
    result.coefficients_[(NUM_BASIS_BLADES - 1) & (~DEGENERATE_BASIS_MASK)] = 1;
    return result;
  }

  // Generate a Multivector of a single vector (grade 1) basis. These can be combined to generate
  // any Multivector. See the tests for examples.
  template <size_t N>
  static constexpr Multivector e() {
    if constexpr (N >= NUM_BASIS_VECTORS) {
      // Note: we use a static_assert here to generate an error message for the user. We could have
      // used a template parameter to restrict which basis creation functions are generated by the
      // compiler, but that approach resulted in a much more cryptic error message from the
      // compiler. For completeness, here is the function signature with the template restriction:
      //   template <size_t N, std::enable_if_t<(N < NUM_BASIS_VECTORS), bool> = true>
      //   static constexpr Multivector e();
      static_assert(
          N < NUM_BASIS_VECTORS,
          "Template parameter to basis creation function is out of range of the number of "
          "vectors (grade 1 bases). Template parameter must be less than the NUM_BASIS_VECTORS.");
    } else {
      Multivector result{};
      result.coefficients_[1UL << N] = 1;
      return result;
    }
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
   * can be error-prone. They should be considered internal implementation details and not part of
   * the API.
   *************************************************************************************************/
  constexpr const ScalarType& coefficient(size_t n) const { return coefficients_.at(n); }
  constexpr void set_coefficient(size_t n, const ScalarType& v) { coefficients_.at(n) = v; }
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
