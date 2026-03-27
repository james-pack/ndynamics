#pragma once

#include "math/abs.h"
#include "math/algebra.h"
#include "math/geometry_model.h"
#include "math/multivector.h"

namespace ndyn::math {

/**
 * GeometryModel implementation for PGA (Projective Geometric Algebra, R(3,0,1)) using the
 * point-based convention, where points are grade-1 vectors and planes are grade-3 trivectors.
 *
 * This is the dual of the more common plane-based PGA convention. In this convention:
 *   - Points are grade-1 vectors:   e0 + x*e1 + y*e2 + z*e3
 *   - Lines are grade-2 bivectors
 *   - Planes are grade-3 trivectors
 *   - The join of two points is their outer product (grade-2 line)
 *   - The meet of two planes is their regressive product (grade-2 line)
 *
 * Basis convention:
 *   e<0> = e0 (degenerate/ideal basis, e0^2 = 0)
 *   e<1> = e1 (x), e<2> = e2 (y), e<3> = e3 (z)
 *
 * The degenerate basis vector e0 is at bit index 1 (the lowest bit) per the algebra's
 * zero-bases-first ordering convention.
 */
template <typename T = DefaultScalarType,
          InnerProduct INNER_PRODUCT = InnerProduct::LEFT_CONTRACTION>
class PgaGrade1PointGeometry final {
 public:
  using Algebra = Pga<T, INNER_PRODUCT>;
  using Multivector = ::ndyn::math::Multivector<Algebra>;
  using Scalar = T;

 private:
  static constexpr size_t e0_coefficient{0b0001};
  static constexpr size_t e1_coefficient{0b0010};
  static constexpr size_t e2_coefficient{0b0100};
  static constexpr size_t e3_coefficient{0b1000};
  static constexpr size_t e01_coefficient{e0_coefficient | e1_coefficient};
  static constexpr size_t e02_coefficient{e0_coefficient | e2_coefficient};
  static constexpr size_t e03_coefficient{e0_coefficient | e3_coefficient};
  static constexpr size_t e12_coefficient{e1_coefficient | e2_coefficient};
  static constexpr size_t e23_coefficient{e2_coefficient | e3_coefficient};
  static constexpr size_t e13_coefficient{e1_coefficient | e3_coefficient};

  static constexpr size_t e012_coefficient{e0_coefficient | e1_coefficient | e2_coefficient};
  static constexpr size_t e013_coefficient{e0_coefficient | e1_coefficient | e3_coefficient};
  static constexpr size_t e023_coefficient{e0_coefficient | e2_coefficient | e3_coefficient};
  static constexpr size_t e123_coefficient{e1_coefficient | e2_coefficient | e3_coefficient};
  static constexpr size_t e0123_coefficient{e0_coefficient | e1_coefficient | e2_coefficient |
                                            e3_coefficient};
  static constexpr Multivector e0{Multivector::template e<0>()};
  static constexpr Multivector e1{Multivector::template e<1>()};
  static constexpr Multivector e2{Multivector::template e<2>()};
  static constexpr Multivector e3{Multivector::template e<3>()};
  static_assert((e0 * e0).scalar() == 0);
  static_assert((e1 * e1).scalar() == 1);
  static_assert((e2 * e2).scalar() == 1);
  static_assert((e3 * e3).scalar() == 1);

 public:
  /**
   * Embed a Euclidean point as a grade-1 vector: e0 + x*e1 + y*e2 + z*e3. The e0 component
   * is the homogeneous weight, set to 1 for a finite point. Ideal points (at infinity) have
   * e0 = 0 and are not constructible via this method.
   */
  constexpr Multivector make_point(Scalar x, Scalar y, Scalar z) const {
    return e0 + x * e1 + y * e2 + z * e3;
  }

  /**
   * In the grade-1 point convention a finite point is a grade-1 vector whose e0
   * component (the homogeneous weight) is nonzero. A pure grade-1 element with zero
   * e0 coefficient is an ideal point (at infinity) and is not considered a finite
   * point by this check. The null condition X*~X = 0 is automatically satisfied by
   * any grade-1 element in PGA due to the degenerate metric and need not be checked
   * separately.
   */
  constexpr bool is_point(const Multivector& mv) const {
    const bool is_grade_1{mv.template is_grade<1>()};
    const bool has_weight{abs(mv.coefficient(e0_coefficient)) > Algebra::EPSILON};
    return is_grade_1 && has_weight;
  }

  /**
   * In the grade-1 point convention a line is a grade-2 bivector with both a nonzero
   * Euclidean part (the direction) and consistency between the Euclidean and ideal
   * parts (the Plücker condition: d · m = 0, where d is the direction and m is the
   * moment). A grade-2 element that fails the Plücker condition does not correspond
   * to a real line in space.
   */
  constexpr bool is_line(const Multivector& mv) const {
    if (!mv.template is_grade<2>()) {
      return false;
    }

    const auto d_x{mv.coefficient(e23_coefficient)};
    const auto d_y{-mv.coefficient(e13_coefficient)};
    const auto d_z{mv.coefficient(e12_coefficient)};

    const auto m_x{mv.coefficient(e01_coefficient)};
    const auto m_y{mv.coefficient(e02_coefficient)};
    const auto m_z{mv.coefficient(e03_coefficient)};

    const auto direction_norm{d_x * d_x + d_y * d_y + d_z * d_z};
    const auto moment_norm{m_x * m_x + m_y * m_y + m_z * m_z};
    const auto scale{sqrt(direction_norm * moment_norm)};
    const auto plucker_product{d_x * m_x + d_y * m_y + d_z * m_z};

    // A nonzero direction is required — ideal-only bivectors are not lines.
    const bool has_direction{d_x * d_x + d_y * d_y + d_z * d_z > Algebra::EPSILON};

    // The Plücker condition d · m = 0 must hold for the bivector to represent a
    // real line rather than a degenerate or imaginary one.
    const bool plucker_satisfied{scale < Algebra::EPSILON ||
                                 abs(plucker_product / scale) < Algebra::EPSILON};

    return has_direction && plucker_satisfied;
  }

  /**
   * Circles are not directly representable in a single multivector in PGA.
   */
  constexpr bool is_circle(const Multivector& mv) const { return false; }

  /**
   * In the grade-1 point convention a plane is a grade-3 trivector with a nonzero
   * e123 component (the homogeneous weight). A grade-3 element with zero e123 is an
   * ideal plane (the plane at infinity) and is not considered a finite plane by this
   * check.
   */
  constexpr bool is_plane(const Multivector& mv) const {
    const bool is_grade_3{mv.template is_grade<3>()};
    // e123 is the homogeneous weight of a finite plane.
    const bool has_weight{abs(mv.coefficient(e123_coefficient)) > Algebra::EPSILON};
    return is_grade_3 && has_weight;
  }

  /**
   * Compute the join via the outer product. In the grade-1 point convention, the outer product
   * of two points yields the line through them (grade-2), and the outer product of three points
   * or a point and a line yields the plane through them (grade-3). This is the defining
   * characteristic that distinguishes this convention from the plane-based convention, where
   * the regressive product serves as join.
   */
  constexpr Multivector join(const Multivector& a, const Multivector& b) const {
    return a.outer(b);
  }

  /**
   * Compute the meet via the regressive product. In the grade-1 point convention, the
   * regressive product of two planes yields the line of intersection (grade-2), and the
   * regressive product of a plane and a line yields their intersection point (grade-1). Note
   * that join and meet swap their product implementations relative to the plane-based
   * convention — this is the algebraic signature of the duality between the two conventions.
   */
  constexpr Multivector meet(const Multivector& a, const Multivector& b) const {
    return a.regress(b);
  }

  /**
   * Construct a rotor for rotation about the given axis line by angle radians. The axis is a
   * grade-2 bivector representing a line through the origin in the grade-1 point convention.
   * The rotor is the exponential of half the normalized axis bivector, following the standard
   * PGA rotor formula: R = cos(angle/2) - sin(angle/2) * B_normalized.
   *
   * The e0 components of the bivector (the ideal part) contribute to the translational
   * component and must be zero for a pure rotation. A non-zero ideal part in the axis
   * indicates a screw motion rather than a pure rotation.
   */
  Multivector make_rotor(const Multivector& axis, Scalar angle) const {
    using std::cos, std::sin, std::sqrt;
    const Multivector b{axis.grade_projection(2)};

    // The Euclidean part of the bivector is the spatial direction of the rotation axis.
    // We normalize using only the Euclidean part to avoid the degenerate e0 metric.
    const Multivector euclidean_part{
        b.coefficient(e12_coefficient) *
            (Multivector::template e<1>() * Multivector::template e<2>()) +
        b.coefficient(e13_coefficient) *
            (Multivector::template e<1>() * Multivector::template e<3>()) +
        b.coefficient(e23_coefficient) *
            (Multivector::template e<2>() * Multivector::template e<3>())};
    const Scalar norm{sqrt(euclidean_part.multiply(euclidean_part.reverse()).scalar())};
    const Multivector b_normalized{b / norm};
    const Scalar half{angle / Scalar{2}};
    return Multivector{cos(half)} - b_normalized.multiply(sin(half));
  }

  /**
   * Construct a translator for displacement (dx, dy, dz). In PGA, translators are built from
   * ideal bivectors (those containing e0) and have the form:
   *   T = 1 + (1/2)(dx*e01 + dy*e02 + dz*e03)
   * The translator always satisfies T * reverse(T) = 1 regardless of the displacement
   * magnitude, a consequence of the degenerate metric on e0.
   */
  constexpr Multivector make_translator(Scalar dx, Scalar dy, Scalar dz) const {
    const Scalar half{Scalar{1} / Scalar{2}};
    Multivector result{};
    result.set_scalar(Scalar{1});
    result.set_coefficient(e01_coefficient, half * dx);
    result.set_coefficient(e02_coefficient, half * dy);
    result.set_coefficient(e03_coefficient, half * dz);
    return result;
  }

  /**
   * Construct a reflection versor from a plane element. In PGA with the grade-1 point
   * convention, planes are grade-3 trivectors. The plane element is itself the reflection
   * versor — no additional construction is needed. The versor formula V * x * involute(V)^{-1}
   * performs the reflection. The plane must be normalized before use as a versor.
   */
  constexpr Multivector make_reflection(const Multivector& plane) const {
    return plane.grade_projection(3).normalize();
  }

  /**
   * Compute the logarithm of a PGA motor, returning a grade-2 bivector in the Lie algebra.
   * A PGA motor M = R + epsilon*T decomposes into a scalar+bivector part. The logarithm
   * recovers the screw axis bivector B such that motor_exp(B) = M.
   *
   * The formula handles two cases:
   *   - Pure translation (scalar part = 1, no rotation): log = ideal part of motor
   *   - General screw motion: log via the standard PGA motor logarithm formula
   */
  Multivector motor_log(const Multivector& motor) const {
    using std::acos, std::sin, std::sqrt;

    const Scalar scalar_part{motor.scalar()};

    // Extract the Euclidean bivector part (non-ideal grade-2 components).
    const Scalar b23{motor.coefficient(e12_coefficient)};
    const Scalar b13{motor.coefficient(e13_coefficient)};
    const Scalar b12{motor.coefficient(e23_coefficient)};
    const Scalar euclidean_norm_sq{b23 * b23 + b13 * b13 + b12 * b12};

    if (euclidean_norm_sq < Algebra::EPSILON) {
      // Pure translation: the motor has no rotational component. The logarithm is
      // just the ideal bivector part (the translation generator).
      return motor.grade_projection(2) - Multivector{}.add(Scalar{1});
    }

    const Scalar euclidean_norm{sqrt(euclidean_norm_sq)};
    const Scalar angle{Scalar{2} * acos(scalar_part)};
    const Scalar inv_sin_half{Scalar{1} / sin(angle / Scalar{2})};

    // The full bivector = angle * (Euclidean part) / euclidean_norm, plus the ideal
    // part scaled to recover the translation component of the screw.
    const Multivector euclidean_biv{
        b23 * (Multivector::template e<1>() * Multivector::template e<2>()) +
        b13 * (Multivector::template e<1>() * Multivector::template e<3>()) +
        b12 * (Multivector::template e<2>() * Multivector::template e<3>())};

    // Ideal bivector part encodes the translation component of the screw motion.
    const Multivector ideal_biv{//
                                motor.coefficient(e01_coefficient) * Multivector::template e<0>() *
                                    Multivector::template e<1>() +  //
                                motor.coefficient(e02_coefficient) * Multivector::template e<0>() *
                                    Multivector::template e<2>() +  //
                                motor.coefficient(e03_coefficient) * Multivector::template e<0>() *
                                    Multivector::template e<3>()};

    return (angle / euclidean_norm) * euclidean_biv + (inv_sin_half)*ideal_biv;
  }

  /**
   * Compute the exponential of a grade-2 bivector, returning a PGA motor. The bivector
   * encodes a screw motion — its Euclidean part encodes the rotation axis and angle,
   * and its ideal part encodes the translational component.
   *
   * The formula handles two cases:
   *   - Pure translation (zero Euclidean part): exp = 1 + ideal part
   *   - General screw: exp via the standard PGA bivector exponential
   */
  Multivector motor_exp(const Multivector& bivector) const {
    using std::cos, std::sin, std::sqrt;

    const Scalar b23{bivector.coefficient(e12_coefficient)};
    const Scalar b13{bivector.coefficient(e13_coefficient)};
    const Scalar b12{bivector.coefficient(e23_coefficient)};
    const Scalar euclidean_norm_sq{b23 * b23 + b13 * b13 + b12 * b12};

    if (euclidean_norm_sq < Algebra::EPSILON) {
      // Pure translation generator: exp(T) = 1 + T.
      Multivector result{bivector};
      result.set_scalar(Scalar{1});
      return result;
    }

    const Scalar euclidean_norm{sqrt(euclidean_norm_sq)};
    const Scalar c{cos(euclidean_norm)};
    const Scalar s{sin(euclidean_norm) / euclidean_norm};

    // The ideal part scales differently from the Euclidean part under the exponential — it
    // contributes the translational component of the resulting screw motor.
    const Multivector euclidean_biv{
        b23 * (Multivector::template e<1>() * Multivector::template e<2>()) +
        b13 * (Multivector::template e<1>() * Multivector::template e<3>()) +
        b12 * (Multivector::template e<2>() * Multivector::template e<3>())};
    const Multivector ideal_biv{
        //
        bivector.coefficient(e01_coefficient) * Multivector::template e<0>() *
            Multivector::template e<1>() +  //
        bivector.coefficient(e02_coefficient) * Multivector::template e<0>() *
            Multivector::template e<2>() +  //
        bivector.coefficient(e03_coefficient) * Multivector::template e<0>() *
            Multivector::template e<3>()};

    Multivector result{s * euclidean_biv + s * ideal_biv};
    result.set_scalar(c);
    return result;
  }

  /**
   * Extract Euclidean coordinates from a grade-1 point. The e0 coefficient is the homogeneous
   * weight w. Finite points have w != 0, and the Euclidean coordinates are recovered by dividing
   * the e1, e2, e3 coefficients by w.
   */
  constexpr void extract_point(const Multivector& point, Scalar& out_x, Scalar& out_y,
                               Scalar& out_z) const {
    const Scalar w{point.coefficient(e0_coefficient)};
    if (abs(w) < Algebra::EPSILON) {
      except<std::domain_error>("Cannot extract coordinates from an ideal point (w = 0).");
    }
    out_x = point.coefficient(e1_coefficient) / w;
    out_y = point.coefficient(e2_coefficient) / w;
    out_z = point.coefficient(e3_coefficient) / w;
  }

  /**
   * Extract direction and moment from a grade-2 line bivector. In the grade-1 point convention
   * the Euclidean bivector components (e12, e13, e23) encode the line direction, and the ideal
   * bivector components (e01, e02, e03) encode the moment (the translational offset of the
   * line from the origin).
   */
  constexpr void extract_line(const Multivector& line, Scalar& out_dx, Scalar& out_dy,
                              Scalar& out_dz, Scalar& out_mx, Scalar& out_my,
                              Scalar& out_mz) const {
    out_dx = line.coefficient(e23_coefficient);
    out_dy = line.coefficient(e13_coefficient);
    out_dz = line.coefficient(e12_coefficient);
    out_mx = line.coefficient(e01_coefficient);
    out_my = line.coefficient(e02_coefficient);
    out_mz = line.coefficient(e03_coefficient);
  }

  /**
   * Extract normal and distance from a grade-3 plane trivector. In the grade-1 point
   * convention, planes are trivectors. The e123 component is the homogeneous weight. The
   * normal components are recovered from the e023, e013, e012 coefficients, normalized by
   * the e123 weight. The signed distance from the origin is the e0123 component divided by
   * the e123 weight.
   */
  constexpr void extract_plane(const Multivector& plane, Scalar& out_nx, Scalar& out_ny,
                               Scalar& out_nz, Scalar& out_d) const {
    const Scalar w{plane.coefficient(e123_coefficient)};
    if (abs(w) < Algebra::EPSILON) {
      except<std::domain_error>("Cannot extract plane parameters from a degenerate plane.");
    }
    out_nx = plane.coefficient(e023_coefficient) / w;
    out_ny = plane.coefficient(e013_coefficient) / w;
    out_nz = plane.coefficient(e012_coefficient) / w;
    out_d = plane.coefficient(e0123_coefficient) / w;
  }
};

static_assert(GeometryModel<PgaGrade1PointGeometry<>>);

/**
 * GeometryModel implementation for PGA (Projective Geometric Algebra, R(3,0,1)) using the
 * plane-based convention, where planes are grade-1 vectors and points are grade-3 trivectors.
 *
 * This is the more common PGA convention in the literature (Gunn, De Keninck, et al.). In
 * this convention:
 *   - Planes are grade-1 vectors:   a*e0 + b*e1 + c*e2 + d*e3
 *   - Lines are grade-2 bivectors
 *   - Points are grade-3 trivectors: w*e123 + x*e032 + y*e013 + z*e021
 *   - The join of two points is their regressive product (grade-2 line)
 *   - The meet of two planes is their outer product (grade-2 line)
 *
 * Basis convention:
 *   e<0> = e0 (degenerate/ideal basis, e0^2 = 0)
 *   e<1> = e1 (x), e<2> = e2 (y), e<3> = e3 (z)
 *
 * Note that join uses the regressive product and meet uses the outer product — the opposite
 * of PgaGrade1PointGeometry. This swap is the defining algebraic signature of the duality
 * between the two PGA point conventions.
 */
template <typename T = DefaultScalarType,
          InnerProduct INNER_PRODUCT = InnerProduct::LEFT_CONTRACTION>
class PgaGrade3PointGeometry final {
 public:
  using Algebra = Pga<T, INNER_PRODUCT>;
  using Multivector = ::ndyn::math::Multivector<Algebra>;
  using Scalar = T;

 private:
  static constexpr size_t e0_coefficient{1};
  static constexpr size_t e1_coefficient{2};
  static constexpr size_t e2_coefficient{4};
  static constexpr size_t e3_coefficient{8};
  static constexpr size_t e01_coefficient{e0_coefficient | e1_coefficient};
  static constexpr size_t e02_coefficient{e0_coefficient | e2_coefficient};
  static constexpr size_t e03_coefficient{e0_coefficient | e3_coefficient};
  static constexpr size_t e12_coefficient{e1_coefficient | e2_coefficient};
  static constexpr size_t e23_coefficient{e2_coefficient | e3_coefficient};
  static constexpr size_t e13_coefficient{e1_coefficient | e3_coefficient};

  static constexpr size_t e012_coefficient{e0_coefficient | e1_coefficient | e2_coefficient};
  static constexpr size_t e013_coefficient{e0_coefficient | e1_coefficient | e3_coefficient};
  static constexpr size_t e023_coefficient{e0_coefficient | e2_coefficient | e3_coefficient};
  static constexpr size_t e123_coefficient{e1_coefficient | e2_coefficient | e3_coefficient};
  static constexpr size_t e0123_coefficient{e0_coefficient | e1_coefficient | e2_coefficient |
                                            e3_coefficient};
  static constexpr Multivector e0{Multivector::template e<0>()};
  static constexpr Multivector e1{Multivector::template e<1>()};
  static constexpr Multivector e2{Multivector::template e<2>()};
  static constexpr Multivector e3{Multivector::template e<3>()};
  static_assert((e0 * e0).scalar() == 0);
  static_assert((e1 * e1).scalar() == 1);
  static_assert((e2 * e2).scalar() == 1);
  static_assert((e3 * e3).scalar() == 1);

 public:
  /**
   * Embed a Euclidean point as a grade-3 trivector: w*e123 + x*e032 + y*e013 + z*e021.
   * The weight w is set to 1 for a finite point. The sign conventions on the components
   * follow from the standard plane-based PGA embedding where the point (x,y,z) satisfies
   * the three planes x=const, y=const, z=const.
   */
  constexpr Multivector make_point(Scalar x, Scalar y, Scalar z) const {
    Multivector result{};
    result.set_coefficient(e123_coefficient, Scalar{1});  // w*e123
    result.set_coefficient(e023_coefficient, x);          // x*e032
    result.set_coefficient(e013_coefficient, y);          // y*e013
    result.set_coefficient(e012_coefficient, z);          // z*e021
    return result;
  }

  /**
   * In the plane-based PGA convention a finite point is a grade-3 trivector with a
   * nonzero e123 component (the homogeneous weight). A grade-3 element with zero e123
   * is an ideal point and is not a finite point.
   */
  constexpr bool is_point(const Multivector& mv) const {
    const bool is_grade_3{mv.template is_grade<3>()};
    // e123 is the homogeneous weight of a finite point in this convention.
    const bool has_weight{abs(mv.coefficient(e123_coefficient)) > Algebra::EPSILON};
    return is_grade_3 && has_weight;
  }

  /**
   * In the plane-based PGA convention a line is a grade-2 bivector satisfying the
   * Plücker condition. The Euclidean and ideal components swap their geometric roles
   * relative to the grade-1 point convention, but the Plücker condition d · m = 0
   * is the same algebraic constraint on the same six bivector coefficients.
   */
  constexpr bool is_line(const Multivector& mv) const {
    if (!mv.template is_grade<2>()) {
      return false;
    }

    const auto d_x{mv.coefficient(e23_coefficient)};
    const auto d_y{-mv.coefficient(e13_coefficient)};
    const auto d_z{mv.coefficient(e12_coefficient)};
    const auto m_x{mv.coefficient(e01_coefficient)};
    const auto m_y{mv.coefficient(e02_coefficient)};
    const auto m_z{mv.coefficient(e03_coefficient)};

    const auto direction_norm{d_x * d_x + d_y * d_y + d_z * d_z};
    const auto moment_norm{m_x * m_x + m_y * m_y + m_z * m_z};
    const auto scale{sqrt(direction_norm * moment_norm)};
    const auto plucker_product{d_x * m_x + d_y * m_y + d_z * m_z};

    const bool has_direction{direction_norm > Algebra::EPSILON};

    const bool plucker_satisfied{scale < Algebra::EPSILON ||
                                 abs(plucker_product / scale) < Algebra::EPSILON};

    return has_direction && plucker_satisfied;
  }

  /**
   * Circles are not directly representable in a single multivector in PGA.
   */
  constexpr bool is_circle(const Multivector& mv) const { return false; }

  /**
   * In the plane-based PGA convention a finite plane is a grade-1 vector with a nonzero
   * Euclidean part (nonzero e1, e2, or e3 component). A pure e0 vector is the plane at
   * infinity and is not a finite plane. The e0 component encodes the signed distance from
   * the origin and may be zero for planes passing through the origin.
   */
  constexpr bool is_plane(const Multivector& mv) const {
    if (!mv.template is_grade<1>()) {
      return false;
    }
    // A finite plane must have a nonzero normal (Euclidean) direction.
    const auto nx{mv.coefficient(e1_coefficient)};
    const auto ny{mv.coefficient(e2_coefficient)};
    const auto nz{mv.coefficient(e3_coefficient)};
    return nx * nx + ny * ny + nz * nz > Algebra::EPSILON;
  }

  /**
   * Compute the join via the regressive product. In the plane-based PGA convention, the
   * regressive product of two grade-3 points produces the grade-2 line through them, and
   * the regressive product of a point and a line produces the plane through them. This is
   * the opposite product role from the grade-1 point convention, and is the correct choice
   * here because the pseudoscalar complement of a grade-3 element is grade-1, matching the
   * structure of the outer product in the dual convention.
   */
  constexpr Multivector join(const Multivector& a, const Multivector& b) const {
    return a.regress(b);
  }

  /**
   * Compute the meet via the outer product. In the plane-based PGA convention, the outer
   * product of two grade-1 planes produces the grade-2 line of intersection, and the outer
   * product of a plane and a line produces their intersection point (grade-3). The outer
   * product serves as meet here for the same duality reason that it serves as join in the
   * grade-1 point convention.
   */
  constexpr Multivector meet(const Multivector& a, const Multivector& b) const {
    return a.outer(b);
  }

  /**
   * Construct a rotor for rotation about the given axis line by angle radians. The axis is a
   * grade-2 bivector. The Euclidean part of the bivector (e12, e13, e23 components) defines
   * the spatial direction of the rotation axis. The construction is identical to the grade-1
   * point convention — the rotor lives in the even subalgebra and is independent of the
   * point grade convention.
   */
  Multivector make_rotor(const Multivector& axis, Scalar angle) const {
    using std::cos, std::sin, std::sqrt;
    const Multivector b{axis.grade_projection(2)};

    // Normalize using only the Euclidean (non-ideal) bivector components.
    const Multivector euclidean_part{
        b.coefficient(0b0110) * (Multivector::template e<1>() * Multivector::template e<2>()) +
        b.coefficient(0b1010) * (Multivector::template e<1>() * Multivector::template e<3>()) +
        b.coefficient(0b1100) * (Multivector::template e<2>() * Multivector::template e<3>())};
    const Scalar norm{sqrt(euclidean_part.multiply(euclidean_part.reverse()).scalar())};
    const Multivector b_normalized{b / norm};
    const Scalar half{angle / Scalar{2}};
    return Multivector{}.add(cos(half)) - b_normalized.multiply(sin(half));
  }

  /**
   * Construct a translator for displacement (dx, dy, dz). The translator construction is
   * identical to the grade-1 point convention — translators live in the even subalgebra
   * and are not affected by the choice of point grade convention.
   *   T = 1 + (1/2)(dx*e01 + dy*e02 + dz*e03)
   */
  constexpr Multivector make_translator(Scalar dx, Scalar dy, Scalar dz) const {
    const Scalar half{Scalar{1} / Scalar{2}};
    Multivector result{};
    result.set_scalar(Scalar{1});
    result.set_coefficient(0b0011, half * dx);  // e01
    result.set_coefficient(0b0101, half * dy);  // e02
    result.set_coefficient(0b1001, half * dz);  // e03
    return result;
  }

  /**
   * Construct a reflection versor from a plane element. In the plane-based PGA convention,
   * planes are grade-1 vectors and a plane element is itself the reflection versor. This is
   * the most direct case: the plane vector can be used immediately in the sandwich product
   * V * x * involute(V)^{-1} to reflect any element. The plane must be normalized.
   */
  constexpr Multivector make_reflection(const Multivector& plane) const {
    return plane.grade_projection(1).normalize();
  }

  /**
   * Compute the logarithm of a PGA motor. Identical in formula to PgaGrade1PointGeometry
   * since motors live in the even subalgebra, which is independent of the point grade
   * convention. See PgaGrade1PointGeometry::motor_log for the full derivation.
   */
  Multivector motor_log(const Multivector& motor) const {
    using std::acos, std::sin, std::sqrt;

    const Scalar scalar_part{motor.scalar()};
    const Scalar b12{motor.coefficient(e12_coefficient)};
    const Scalar b13{motor.coefficient(e13_coefficient)};
    const Scalar b23{motor.coefficient(e23_coefficient)};

    const Scalar b01{motor.coefficient(e01_coefficient)};
    const Scalar b02{motor.coefficient(e02_coefficient)};
    const Scalar b03{motor.coefficient(e03_coefficient)};

    const Scalar euclidean_norm_sq{b23 * b23 + b13 * b13 + b12 * b12};
    if (euclidean_norm_sq < Algebra::EPSILON) {
      return motor.grade_projection(2) - Multivector{}.add(Scalar{1});
    }

    const Scalar euclidean_norm{sqrt(euclidean_norm_sq)};
    const Scalar angle{Scalar{2} * acos(scalar_part)};
    const Scalar inv_sin_half{Scalar{1} / sin(angle / Scalar{2})};

    Multivector result{};
    result.set_coefficient(e12_coefficient, (angle / euclidean_norm) * b12);
    result.set_coefficient(e13_coefficient, (angle / euclidean_norm) * b13);
    result.set_coefficient(e23_coefficient, (angle / euclidean_norm) * b23);

    result.set_coefficient(e01_coefficient, inv_sin_half * b01);
    result.set_coefficient(e02_coefficient, inv_sin_half * b02);
    result.set_coefficient(e03_coefficient, inv_sin_half * b03);
    return result;
  }

  /**
   * Compute the exponential of a grade-2 bivector. Identical in formula to
   * PgaGrade1PointGeometry since motors live in the even subalgebra, which is independent
   * of the point grade convention. See PgaGrade1PointGeometry::motor_exp for the full
   * derivation.
   */
  Multivector motor_exp(const Multivector& bivector) const {
    using std::cos, std::sin, std::sqrt;

    const Scalar b12{bivector.coefficient(e12_coefficient)};
    const Scalar b13{bivector.coefficient(e13_coefficient)};
    const Scalar b23{bivector.coefficient(e23_coefficient)};

    const Scalar b01{bivector.coefficient(e01_coefficient)};
    const Scalar b02{bivector.coefficient(e02_coefficient)};
    const Scalar b03{bivector.coefficient(e03_coefficient)};

    const Scalar euclidean_norm_sq{b23 * b23 + b13 * b13 + b12 * b12};
    if (euclidean_norm_sq < Algebra::EPSILON) {
      Multivector result{bivector};
      result.set_scalar(Scalar{1});
      return result;
    }

    const Scalar euclidean_norm{sqrt(euclidean_norm_sq)};
    const Scalar c{cos(euclidean_norm)};
    const Scalar s{sin(euclidean_norm) / euclidean_norm};

    Multivector result{c};
    result.set_coefficient(e12_coefficient, s * b12);
    result.set_coefficient(e13_coefficient, s * b13);
    result.set_coefficient(e23_coefficient, s * b23);

    result.set_coefficient(e01_coefficient, s * b01);
    result.set_coefficient(e02_coefficient, s * b02);
    result.set_coefficient(e03_coefficient, s * b03);
    return result;
  }

  /**
   * Extract Euclidean coordinates from a grade-3 point trivector. The e123 coefficient is
   * the homogeneous weight w. The spatial coordinates are in e032, e013, e021, normalized
   * by w. Ideal points have w = 0 and have no finite Euclidean coordinates.
   */
  constexpr void extract_point(const Multivector& point, Scalar& out_x, Scalar& out_y,
                               Scalar& out_z) const {
    const Scalar w{point.coefficient(e123_coefficient)};
    if (abs(w) < Algebra::EPSILON) {
      except<std::domain_error>("Cannot extract coordinates from an ideal point (w = 0).");
    }
    out_x = point.coefficient(e023_coefficient) / w;
    out_y = point.coefficient(e013_coefficient) / w;
    out_z = point.coefficient(e012_coefficient) / w;
  }

  /**
   * Extract direction and moment from a grade-2 line bivector. The extraction is identical
   * to PgaGrade1PointGeometry — line bivectors have the same structure regardless of the
   * point grade convention. The Euclidean components encode direction; the ideal components
   * encode moment.
   */
  constexpr void extract_line(const Multivector& line, Scalar& out_dx, Scalar& out_dy,
                              Scalar& out_dz, Scalar& out_mx, Scalar& out_my,
                              Scalar& out_mz) const {
    out_dx = line.coefficient(e23_coefficient);
    out_dy = line.coefficient(e13_coefficient);
    out_dz = line.coefficient(e12_coefficient);
    out_mx = line.coefficient(e01_coefficient);
    out_my = line.coefficient(e02_coefficient);
    out_mz = line.coefficient(e03_coefficient);
  }

  /**
   * Extract normal and distance from a grade-1 plane vector. In the plane-based convention
   * planes are grade-1 vectors: a*e0 + b*e1 + c*e2 + d*e3. The e0 coefficient is the
   * signed distance from the origin (the homogeneous offset). The e1, e2, e3 coefficients
   * are the normal direction, normalized by the norm of (e1, e2, e3).
   */
  constexpr void extract_plane(const Multivector& plane, Scalar& out_nx, Scalar& out_ny,
                               Scalar& out_nz, Scalar& out_d) const {
    using std::sqrt;
    const Scalar nx{plane.coefficient(e1_coefficient)};
    const Scalar ny{plane.coefficient(e2_coefficient)};
    const Scalar nz{plane.coefficient(e3_coefficient)};
    const Scalar norm{sqrt(nx * nx + ny * ny + nz * nz)};
    if (norm < Algebra::EPSILON) {
      except<std::domain_error>("Cannot extract normal from a degenerate plane.");
    }
    out_nx = nx / norm;
    out_ny = ny / norm;
    out_nz = nz / norm;
    out_d = plane.coefficient(e0_coefficient) / norm;
  }
};

static_assert(GeometryModel<PgaGrade3PointGeometry<>>);

}  // namespace ndyn::math
