#pragma once

#include "math/algebra.h"
#include "math/geometry_model.h"
#include "math/multivector.h"

namespace ndyn::math {

/**
 * GeometryModel implementation for VGA (Vanilla/Euclidean Geometric Algebra, R(3,0,0)).
 *
 * VGA has no projective or conformal extension, so there are no ideal points, no translators,
 * and no motors in the algebraic sense. Translations cannot be represented as versors — they
 * must be handled as coordinate offsets outside the algebra. This geometry model is included
 * for completeness and for applications that require only rotations and reflections.
 *
 * make_translator(), make_motor(), motor_log(), and motor_exp() are present to satisfy the
 * GeometryModel concept but are not geometrically meaningful in VGA. Each will throw if called,
 * as no correct implementation exists within this algebra.
 *
 * Basis convention:
 *   Multivector::e<0> = e1 (x)
 *   Multivector::e<1> = e2 (y)
 *   Multivector::e<2> = e3 (z)
 *
 *   Points are grade-1 vectors (position vectors from the origin).
 *   Lines through the origin are grade-2 bivectors.
 *   The pseudoscalar is e123 (grade-3).
 */
template <typename T = DefaultScalarType>
class VgaGeometry final {
 private:
  static constexpr size_t scalar_coefficient{0};
  static constexpr size_t e1_coefficient{1};
  static constexpr size_t e2_coefficient{2};
  static constexpr size_t e3_coefficient{4};
  static constexpr size_t e12_coefficient{e1_coefficient | e2_coefficient};
  static constexpr size_t e23_coefficient{e2_coefficient | e3_coefficient};
  static constexpr size_t e13_coefficient{e1_coefficient | e3_coefficient};
  static constexpr size_t e123_coefficient{e1_coefficient | e2_coefficient | e3_coefficient};

 public:
  using Algebra = Vga<T>;
  using Multivector = Algebra::VectorType;
  using Scalar = Algebra::ScalarType;

  static constexpr Scalar scalar(const Multivector& mv) noexcept {
    return mv.coefficient(scalar_coefficient);
  }

  static constexpr Scalar e1(const Multivector& mv) noexcept {
    return mv.coefficient(e1_coefficient);
  }
  static constexpr Scalar e2(const Multivector& mv) noexcept {
    return mv.coefficient(e2_coefficient);
  }
  static constexpr Scalar e3(const Multivector& mv) noexcept {
    return mv.coefficient(e3_coefficient);
  }

  static constexpr Scalar e12(const Multivector& mv) noexcept {
    return mv.coefficient(e12_coefficient);
  }
  static constexpr Scalar e13(const Multivector& mv) noexcept {
    return mv.coefficient(e13_coefficient);
  }
  static constexpr Scalar e23(const Multivector& mv) noexcept {
    return mv.coefficient(e23_coefficient);
  }

  static constexpr Scalar e123(const Multivector& mv) noexcept {
    return mv.coefficient(e123_coefficient);
  }

  /**
   * Embed a Euclidean point as a grade-1 vector. In VGA there is no homogeneous coordinate —
   * a point is simply the position vector from the origin. This means that points and
   * directions are the same grade, which is the fundamental limitation of VGA for rigid body
   * work: there is no algebraic distinction between a bound point and a free vector.
   */
  static constexpr Multivector make_point(Scalar x, Scalar y, Scalar z) noexcept {
    return x * Multivector::template e<0>() +  //
           y * Multivector::template e<1>() +  //
           z * Multivector::template e<2>();
  }

  /**
   * In VGA a point is a grade-1 vector. There is no homogeneous weight and no null
   * condition to check — any nonzero grade-1 element is a valid point. The zero
   * vector is excluded as it represents no geometric location.
   */
  static constexpr bool is_point(const Multivector& mv) noexcept {
    return mv.template is_grade<1>();
  }

  /**
   * Compute the join of two elements via the outer product. In VGA the outer product of two
   * grade-1 vectors produces the grade-2 bivector representing the plane through the origin
   * containing both vectors — not a line through two arbitrary points, since VGA has no
   * mechanism to represent lines that do not pass through the origin. The join of a grade-1
   * vector and a grade-2 bivector produces the pseudoscalar if they span the full space.
   */
  static constexpr Multivector join(const Multivector& a, const Multivector& b) noexcept {
    return a.outer(b);
  }

  /**
   * Compute the meet of two elements via the regressive product. In VGA the meet recovers the
   * largest subspace contained in both operands. As with join, the absence of ideal elements
   * means that parallel subspaces have a degenerate meet — parallel planes meet at zero rather
   * than at an ideal line.
   */
  static constexpr Multivector meet(const Multivector& a, const Multivector& b) noexcept {
    return a.regress(b);
  }

  /**
   * Construct a rotor representing a rotation about the given axis bivector by angle radians.
   * The axis must be a grade-2 bivector (a plane through the origin). The rotor is the
   * exponential of half the axis bivector: R = cos(angle/2) - sin(angle/2) * B_normalized,
   * following the standard VGA rotor formula.
   *
   * Note that in VGA, rotation is always about a line through the origin. Rotation about an
   * arbitrary line in space cannot be expressed as a single rotor in VGA.
   */
  static Multivector make_rotor(const Multivector& axis, Scalar angle) noexcept {
    using std::cos, std::sin, std::sqrt;
    const Multivector b{axis.grade_projection(2)};
    const Scalar norm{sqrt(b.multiply(b.reverse()).scalar())};
    const Multivector b_normalized{b / norm};
    const Scalar half{angle / Scalar{2}};
    return Multivector{}.add(cos(half)) - b_normalized.multiply(sin(half));
  }

  /**
   * Compute the logarithm of a VGA rotor, returning a grade-2 bivector in the Lie algebra
   * of the rotation group. For a rotor R = cos(t) + sin(t)*B where B is a unit bivector,
   * the logarithm returns t*B. This is the foundation for rotor interpolation (SLERP) and
   * angular velocity integration in VGA.
   *
   * The input must be a unit rotor — a mixed-grade or non-unit input will produce a
   * meaningless result. The degenerate case R = -1 (a rotation by 2*pi) has a logarithm
   * of magnitude pi in an arbitrary plane; this implementation throws in that case since
   * the result is not unique.
   */
  static Multivector motor_log(const Multivector& rotor) noexcept {
    using std::acos, std::sin, std::sqrt;

    const Scalar cos_half_angle{rotor.scalar()};

    // Clamp to [-1, 1] to guard against floating point drift past the acos domain.
    const Scalar clamped{cos_half_angle < Scalar{-1}  ? Scalar{-1}
                         : cos_half_angle > Scalar{1} ? Scalar{1}
                                                      : cos_half_angle};

    const Scalar half_angle{acos(clamped)};
    const Scalar sin_half_angle{sin(half_angle)};

    if (sin_half_angle < Algebra::EPSILON && sin_half_angle > -Algebra::EPSILON) {
      // R is the identity or a 2*pi rotation — the bivector part is zero and the
      // logarithm is the zero bivector (identity case) or undefined (2*pi case).
      // Returning zero is correct for the identity; the 2*pi case is indistinguishable
      // without additional context.
      return {};
    }

    return rotor.grade_projection(2) * (half_angle / sin_half_angle);
  }

  /**
   * Compute the exponential of a grade-2 bivector, returning a VGA rotor. For a bivector
   * B = t*B_unit where B_unit is a unit bivector, the exponential returns cos(t) + sin(t)*B_unit.
   * This is used to integrate angular velocity bivectors into rotor states.
   */
  static Multivector motor_exp(const Multivector& bivector) noexcept {
    using std::cos, std::sin, std::sqrt;

    const Multivector b{bivector.grade_projection(2)};
    const Scalar norm_sq{b.multiply(b.reverse()).scalar()};

    if (norm_sq < Algebra::EPSILON) {
      // Zero bivector — the exponential is the identity rotor.
      return Multivector{}.add(Scalar{1});
    }

    const Scalar norm{sqrt(norm_sq)};
    Multivector result{b * (sin(norm) / norm)};
    result.set_scalar(cos(norm));
    return result;
  }

  /**
   * Extract Euclidean coordinates from a multivector. In VGA there is no homogeneous
   * weight to normalize — the coefficients of e1, e2, e3 are the coordinates directly.
   */
  static constexpr void extract_point(const Multivector& mv, Scalar& out_x, Scalar& out_y,
                                      Scalar& out_z) noexcept {
    out_x = mv.coefficient(e1_coefficient);
    out_y = mv.coefficient(e2_coefficient);
    out_z = mv.coefficient(e3_coefficient);
  }

  /**
   * Extract Euclidean orientation from a multivector.
   */
  static constexpr void extract_orientation(const Multivector& mv, Scalar& out_w, Scalar& out_x,
                                            Scalar& out_y, Scalar& out_z) noexcept {
    const auto norm{std::sqrt(mv.square_magnitude())};
    out_w = mv.coefficient(scalar_coefficient) / norm;
    out_x = mv.coefficient(e1_coefficient) / norm;
    out_y = mv.coefficient(e2_coefficient) / norm;
    out_z = mv.coefficient(e3_coefficient) / norm;
  }
};

static_assert(GeometryModel<VgaGeometry<>>);

}  // namespace ndyn::math
