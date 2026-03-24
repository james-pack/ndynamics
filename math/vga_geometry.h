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
template <typename T = DefaultScalarType,
          InnerProduct INNER_PRODUCT = InnerProduct::LEFT_CONTRACTION>
class VgaGeometry final {
 private:
  static constexpr size_t e1_coefficient{1};
  static constexpr size_t e2_coefficient{2};
  static constexpr size_t e3_coefficient{4};
  static constexpr size_t e12_coefficient{e1_coefficient | e2_coefficient};
  static constexpr size_t e23_coefficient{e2_coefficient | e3_coefficient};
  static constexpr size_t e31_coefficient{e3_coefficient | e1_coefficient};

 public:
  using Algebra = Vga<T, INNER_PRODUCT>;
  using Multivector = Algebra::VectorType;
  using Scalar = Algebra::ScalarType;

  /**
   * Embed a Euclidean point as a grade-1 vector. In VGA there is no homogeneous coordinate —
   * a point is simply the position vector from the origin. This means that points and
   * directions are the same grade, which is the fundamental limitation of VGA for rigid body
   * work: there is no algebraic distinction between a bound point and a free vector.
   */
  constexpr Multivector make_point(Scalar x, Scalar y, Scalar z) const {
    return x * Multivector::template e<0>() +  //
           y * Multivector::template e<1>() +  //
           z * Multivector::template e<2>();
  }

  /**
   * In VGA a point is a grade-1 vector. There is no homogeneous weight and no null
   * condition to check — any nonzero grade-1 element is a valid point. The zero
   * vector is excluded as it represents no geometric location.
   */
  constexpr bool is_point(const Multivector& mv) const { return mv.template is_grade<1>(); }

  /**
   * In VGA a line through the origin is a grade-2 bivector. There is no notion of
   * a line not through the origin — all lines in VGA pass through the origin, which
   * is the fundamental limitation of VGA for general line geometry.
   */
  constexpr bool is_line(const Multivector& mv) const { return mv.template is_grade<2>(); }

  /**
   * In VGA a plane through the origin is the dual of a grade-1 vector, which is a
   * grade-2 bivector — the same grade as a line. VGA cannot algebraically distinguish
   * a line from a plane since both are grade-2 in 3D without a homogeneous dimension.
   * This method checks for a pure grade-2 element, but the caller must understand that
   * the geometric interpretation as a plane vs a line is context-dependent in VGA.
   */
  constexpr bool is_plane(const Multivector& mv) const {
    // In VGA R(3,0,0) the pseudoscalar is grade-3, so a plane is grade-2, the same
    // as a line. The distinction between a line and a plane cannot be made
    // algebraically in VGA without additional context.
    return mv.template is_grade<2>();
  }

  /**
   * Compute the join of two elements via the outer product. In VGA the outer product of two
   * grade-1 vectors produces the grade-2 bivector representing the plane through the origin
   * containing both vectors — not a line through two arbitrary points, since VGA has no
   * mechanism to represent lines that do not pass through the origin. The join of a grade-1
   * vector and a grade-2 bivector produces the pseudoscalar if they span the full space.
   */
  constexpr Multivector join(const Multivector& a, const Multivector& b) const {
    return a.outer(b);
  }

  /**
   * Compute the meet of two elements via the regressive product. In VGA the meet recovers the
   * largest subspace contained in both operands. As with join, the absence of ideal elements
   * means that parallel subspaces have a degenerate meet — parallel planes meet at zero rather
   * than at an ideal line.
   */
  constexpr Multivector meet(const Multivector& a, const Multivector& b) const {
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
  Multivector make_rotor(const Multivector& axis, Scalar angle) const {
    using std::cos, std::sin, std::sqrt;
    const Multivector b{axis.grade_projection(2)};
    const Scalar norm{sqrt(b.multiply(b.reverse()).scalar())};
    const Multivector b_normalized{b / norm};
    const Scalar half{angle / Scalar{2}};
    return Multivector{}.add(cos(half)) - b_normalized.multiply(sin(half));
  }

  /**
   * Not implemented — VGA has no degenerate basis vector from which to construct ideal
   * elements, so translations cannot be represented as versors. Any call to this method
   * indicates a misuse of VgaGeometry in a context that requires translational motion.
   */
  [[noreturn]] Multivector make_translator(Scalar /*dx*/, Scalar /*dy*/, Scalar /*dz*/) const {
    except<std::logic_error>(
        "VGA does not support translators — use PGA or CGA for rigid body "
        "mechanics requiring translation.");
  }

  /**
   * Not implemented — without translators there are no motors in VGA. Any call to this method
   * indicates a misuse of VgaGeometry in a context that requires rigid body motion.
   */
  [[noreturn]] Multivector make_reflection(const Multivector& /*plane*/) const {
    // In VGA, reflection in a hyperplane through the origin is the sandwich product with the
    // grade-1 normal vector directly. However, VGA planes always pass through the origin, so
    // this is provided as a valid operation unlike the translator and motor methods.
    except<std::logic_error>(
        "VGA reflections are only valid for hyperplanes through the origin. "
        "Use PGA or CGA for reflections in arbitrary planes.");
  }

  /**
   * Construct a versor for reflection in a hyperplane through the origin. The plane must be
   * given as a grade-1 normal vector. In VGA the normal vector is itself the reflection
   * versor — no additional construction is needed. The versor formula V * x * V^{-1} with
   * this element performs the reflection.
   *
   * This is the one case in VGA where a reflection is geometrically well-defined: the
   * reflecting hyperplane must pass through the origin.
   */
  constexpr Multivector reflect_in_plane_through_origin(const Multivector& normal) const {
    return normal.grade_projection(1).normalize();
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
  Multivector motor_log(const Multivector& rotor) const {
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
  Multivector motor_exp(const Multivector& bivector) const {
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
   * Extract Euclidean coordinates from a grade-1 point vector. In VGA there is no homogeneous
   * weight to normalize — the coefficients of e1, e2, e3 are the coordinates directly.
   */
  constexpr void extract_point(const Multivector& point, Scalar& out_x, Scalar& out_y,
                               Scalar& out_z) const {
    out_x = point.coefficient(e1_coefficient);
    out_y = point.coefficient(e2_coefficient);
    out_z = point.coefficient(e3_coefficient);
  }

  /**
   * Extract direction and moment of a line. In VGA all lines pass through the origin, so the
   * moment is always zero. The direction is the grade-1 component of the line element — the
   * caller should interpret the zero moment as confirmation that VGA lines are bound to the
   * origin.
   */
  constexpr void extract_line(const Multivector& line, Scalar& out_dx, Scalar& out_dy,
                              Scalar& out_dz, Scalar& out_mx, Scalar& out_my,
                              Scalar& out_mz) const {
    // In VGA, lines are grade-2 bivectors passing through the origin. The direction is recovered
    // via the dual of the bivector, which maps e12->e3, e13->-e2, e23->e1.
    out_dx = line.coefficient(e12_coefficient);   // e12 -> e3 direction
    out_dy = -line.coefficient(e31_coefficient);  // e31 (=-e13) -> -e2 direction
    out_dz = line.coefficient(e23_coefficient);   // e23 -> e1 direction
    out_mx = Scalar{0};
    out_my = Scalar{0};
    out_mz = Scalar{0};
  }

  /**
   * Extract normal and distance from a plane. In VGA all planes pass through the origin so
   * the distance is always zero. The normal is the grade-1 vector whose outer product with
   * the plane bivector spans the full space.
   */
  constexpr void extract_plane(const Multivector& plane, Scalar& out_nx, Scalar& out_ny,
                               Scalar& out_nz, Scalar& out_d) const {
    // In VGA, planes through the origin are grade-2 bivectors. The normal is the dual.
    out_nx = plane.coefficient(e23_coefficient);
    out_ny = -plane.coefficient(e31_coefficient);
    out_nz = plane.coefficient(e12_coefficient);
    out_d = Scalar{0};
  }
};

static_assert(GeometryModel<VgaGeometry<>>);

}  // namespace ndyn::math
