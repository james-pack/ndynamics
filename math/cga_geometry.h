#pragma once

#include "glog/logging.h"
#include "math/abs.h"
#include "math/algebra.h"
#include "math/geometry_model.h"
#include "math/multivector.h"

namespace ndyn::math {

/**
 * GeometryModel implementation for CGA (Conformal Geometric Algebra, R(4,1)).
 *
 * CGA embeds 3D Euclidean geometry into a 5D space with signature R(4,1) by adding two
 * basis vectors beyond the three Euclidean ones:
 *   e_plus  (e<3>, e_plus^2 = +1): the conformal basis vector
 *   e_minus (e<4>, e_minus^2 = -1): the conformal basis vector
 *
 * These are typically recombined into the null basis:
 *   e0 = (e_minus - e_plus) / 2  — the point at the origin (e<4> - e<3>) / 2
 *   e_inf = e_minus + e_plus     — the point at infinity  (e<4> + e<3>)
 *
 * Point embedding (standard normalization):
 *   X = e0 + p + (1/2)|p|^2 * e_inf
 *   where p = px*e1 + py*e2 + pz*e3
 *
 * This null vector representation satisfies X * X = 0 for any finite Euclidean point p.
 * The e0 coefficient is fixed at 1 under standard normalization, allowing recovery of
 * Euclidean coordinates by dividing the e1, e2, e3 coefficients by the e0 coefficient.
 *
 * Basis convention (5 basis vectors):
 *   e<0> = e1 (x), e<1> = e2 (y), e<2> = e3 (z)
 *   e<3> = e_plus  (e_plus^2  = +1)
 *   e<4> = e_minus (e_minus^2 = -1)
 *
 * The CGA algebra used here is R(4,1) = Algebra<T, 4, 1, 0>.
 */
template <typename T = DefaultScalarType,
          InnerProduct INNER_PRODUCT = InnerProduct::LEFT_CONTRACTION>
class CgaGeometry final {
 public:
  using Algebra = ::ndyn::math::Algebra<T, 4, 1, 0, INNER_PRODUCT>;
  using Multivector = ::ndyn::math::Multivector<Algebra>;
  using Scalar = T;

 private:
  // The null basis vectors e0 and e_inf are the conformal origin and infinity respectively.
  // They are not basis vectors of the algebra directly but are linear combinations of e_plus
  // and e_minus. These helpers keep the construction readable throughout the implementation.
  static constexpr Multivector e_plus() { return Multivector::template e<3>(); }
  static constexpr Multivector e_minus() { return Multivector::template e<4>(); }

  // Assert that the order of the bases is as expected.
  static_assert(e_plus() * e_plus() == Multivector{1});
  static_assert(e_minus() * e_minus() == Multivector{-1});

  static constexpr Multivector e_origin() {
    // e0 = (e_minus - e_plus) / 2
    return (e_minus() - e_plus()) * (Scalar{1} / Scalar{2});
  }

  static constexpr Multivector e_inf() {
    // e_inf = e_minus + e_plus
    return e_minus() + e_plus();
  }

  constexpr Scalar weight(const Multivector& mv) const {
    return -(mv.left_contraction(e_inf()).scalar());
  }

 public:
  /**
   * Embed a Euclidean point as a CGA null vector using standard normalization:
   *   X = e0 + px*e1 + py*e2 + pz*e3 + (1/2)(px^2 + py^2 + pz^2)*e_inf
   *
   * The result satisfies X * X = 0. The e0 coefficient is 1 under this normalization,
   * which is what allows extract_point to recover coordinates by simple division. Under
   * a different normalization the coordinates are recoverable but require an additional
   * division by the e0 coefficient. The caller must not scale the result of this method
   * without accounting for the effect on extract_point.
   */
  constexpr Multivector make_point(Scalar x, Scalar y, Scalar z) const {
    const Scalar half_norm_sq{(x * x + y * y + z * z) / Scalar{2}};
    return e_origin() + x * Multivector::template e<0>() + y * Multivector::template e<1>() +
           z * Multivector::template e<2>() + half_norm_sq * e_inf();
  }

  /**
   * In CGA a finite point is a null vector (X * ~X = 0) at grade-1 with a nonzero inner
   * product with e_inf (nonzero homogeneous weight). The null condition distinguishes
   * points from general grade-1 elements such as spheres (which are non-null). A point
   * at infinity has zero inner product with e_inf and is excluded by the weight check.
   */
  constexpr bool is_point(const Multivector& mv) const {
    if (!mv.template is_grade<1>()) {
      return false;
    }

    // The null condition X * ~X = 0 distinguishes points from spheres.
    const auto self_product{mv.multiply(mv.reverse()).scalar()};
    const bool is_null{abs(self_product) < Algebra::EPSILON};

    // A finite point has nonzero inner product with e_inf — this is the homogeneous
    // weight and is exactly -1 under standard normalization.
    const bool has_weight{abs(weight(mv)) > Algebra::EPSILON};

    return is_null && has_weight;
  }

  /**
   * In CGA a line is a grade-3 element that is null (L * ~L = 0) and contains e_inf
   * as a factor (it is the outer product of two points and e_inf, or equivalently passes
   * through the point at infinity). The null condition distinguishes lines from circles,
   * which are also grade-3 but non-null.
   */
  /*constexpr*/ bool is_line(const Multivector& mv) const {
    if (!mv.template is_grade<2>()) {
      LOG(INFO) << "CgaGeometryModel::is_line() -- not grade 2.";
      return false;
    }

    // The null condition L * ~L = 0 distinguishes lines (null) from circles (non-null).
    const auto self_product{mv.multiply(mv.reverse()).scalar()};
    const bool is_null{abs(self_product) < Algebra::EPSILON};
    LOG(INFO) << "CgaGeometryModel::is_line() -- is_null: " << is_null;

    // A line contains e_inf as a factor — its outer product with e_inf is zero.
    const bool contains_e_inf{mv.outer(e_inf()).near_equal(Multivector{})};
    LOG(INFO) << "CgaGeometryModel::is_line() -- contains_e_inf: " << contains_e_inf;

    return is_null && contains_e_inf;
  }

  /**
   * In CGA a plane is a grade-1 null vector that contains e_inf as a factor — it is a
   * degenerate sphere passing through the point at infinity. The null condition X * ~X = 0
   * distinguishes planes from spheres (non-null grade-1 elements). The e_inf containment
   * condition distinguishes planes from finite points, which are also null grade-1 elements
   * but do not contain e_inf.
   */
  constexpr bool is_plane(const Multivector& mv) const {
    if (!mv.template is_grade<1>()) {
      return false;
    }

    // The null condition distinguishes planes from spheres.
    const auto self_product{mv.multiply(mv.reverse()).scalar()};
    const bool is_null{abs(self_product) < Algebra::EPSILON};

    // A plane contains e_inf as a factor — its outer product with e_inf is zero.
    // This distinguishes a plane (passing through infinity) from a finite point
    // (which is also null but does not contain e_inf).
    const bool contains_e_inf{mv.outer(e_inf()).grade_projection(2).near_equal(Multivector{})};

    return is_null && contains_e_inf;
  }

  /**
   * Compute the join via the outer product. In CGA the outer product of two null point
   * vectors produces the line through them (grade-2 in the conformal sense), and the outer
   * product of three points produces the circle or plane through them. This is the same
   * approach as in PGA with points as grade-1 multivectors. This reflects the conformal
   * embedding's structural similarity to projective geometry at the level of the outer product.
   */
  constexpr Multivector join(const Multivector& a, const Multivector& b) const {
    return a.outer(b);
  }

  /**
   * Compute the meet via the regressive product. In CGA the regressive product recovers
   * the intersection of two elements — the intersection of two spheres is a circle, the
   * intersection of a sphere and a plane is a circle, and so on. The regressive product
   * serves as meet in CGA for the same structural reason as in PGA with grade-1 points.
   */
  constexpr Multivector meet(const Multivector& a, const Multivector& b) const {
    return a.regress(b);
  }

  /**
   * Construct a CGA rotor for rotation about the given axis line by angle radians. The axis
   * is a CGA line element (grade-3 in CGA, encoding both direction and moment via the
   * conformal embedding). The rotor has the same exponential form as in PGA but operates
   * in the larger 5D space — the additional conformal dimensions are unaffected by a pure
   * spatial rotation.
   */
  Multivector make_rotor(const Multivector& axis, Scalar angle) const {
    using std::cos, std::sin, std::sqrt;

    // Extract the Euclidean bivector from the axis — the spatial rotation plane.
    const Multivector b{axis.grade_projection(2)};
    const Scalar norm_sq{-(b.multiply(b)).scalar()};
    if (norm_sq < Algebra::EPSILON) {
      except<std::domain_error>("Degenerate axis: cannot construct rotor from a null axis.");
    }
    const Scalar norm{sqrt(norm_sq)};
    const Multivector b_normalized{b / norm};
    const Scalar half{angle / Scalar{2}};
    return Multivector{}.add(cos(half)) - b_normalized.multiply(sin(half));
  }

  /**
   * Construct a CGA translator for displacement (dx, dy, dz). In CGA translators use e_inf:
   *   T = 1 - (1/2)(dx*e1 + dy*e2 + dz*e3) * e_inf
   *
   * The e_inf factor ensures T * reverse(T) = 1, mirroring the PGA translator's use of e0.
   * The sign convention follows from the CGA rotor exponential where the translation
   * generator is -(1/2) * t * e_inf for displacement t.
   */
  constexpr Multivector make_translator(Scalar dx, Scalar dy, Scalar dz) const {
    const Scalar half{Scalar{1} / Scalar{2}};
    const Multivector t{dx * Multivector::template e<0>() + dy * Multivector::template e<1>() +
                        dz * Multivector::template e<2>()};
    Multivector result{Multivector{}.add(Scalar{1}) - (t * e_inf() * half)};
    return result;
  }

  /**
   * Construct a reflection versor from a geometric element. In CGA a sphere, plane, or
   * other grade-1 element is the reflection versor for inversion in that element. A plane
   * is a specific degenerate sphere (one passing through e_inf), and its reflection versor
   * is the plane element itself normalized. Unlike PGA, where the plane is always the versor
   * directly, CGA elements may require normalization to ensure V * reverse(V) = ±1 before
   * use in the sandwich product.
   */
  Multivector make_reflection(const Multivector& element) const {
    const Scalar scale{element.multiply(element.reverse()).scalar()};
    if (scale < Algebra::EPSILON && scale > -Algebra::EPSILON) {
      except<std::domain_error>("Cannot construct reflection versor from a null element.");
    }
    using std::sqrt;
    return element / sqrt(scale < Scalar{0} ? -scale : scale);
  }

  /**
   * Compute the logarithm of a CGA motor. CGA motors represent rigid body motions and
   * live in the even subalgebra of R(4,1). The logarithm formula is analogous to PGA but
   * operates in the 5D conformal space — the Euclidean bivector components are those not
   * involving e_plus or e_minus, and the translational components involve e_inf.
   */
  Multivector motor_log(const Multivector& motor) const {
    using std::acos, std::sin, std::sqrt;

    const Scalar scalar_part{motor.scalar()};

    // In CGA with basis ordering e1,e2,e3,e_plus,e_minus, the Euclidean rotation
    // bivectors are e12, e13, e23 — those not involving e_plus (index 3) or e_minus (index 4).
    // Bit indices (5 basis vectors): e<0>=bit0, e<1>=bit1, e<2>=bit2, e<3>=bit3, e<4>=bit4
    const Scalar b01{motor.coefficient(0b00011)};  // e12
    const Scalar b02{motor.coefficient(0b00101)};  // e13
    const Scalar b12{motor.coefficient(0b00110)};  // e23
    const Scalar euclidean_norm_sq{b01 * b01 + b02 * b02 + b12 * b12};

    if (euclidean_norm_sq < Algebra::EPSILON) {
      return motor.grade_projection(2) - Multivector{}.add(Scalar{1});
    }

    const Scalar euclidean_norm{sqrt(euclidean_norm_sq)};
    const Scalar angle{Scalar{2} * acos(scalar_part)};
    const Scalar s{sin(angle / Scalar{2})};

    const Multivector euclidean_biv{
        b01 * (Multivector::template e<0>() * Multivector::template e<1>()) +
        b02 * (Multivector::template e<0>() * Multivector::template e<2>()) +
        b12 * (Multivector::template e<1>() * Multivector::template e<2>())};

    // The translational component of the motor lives in the bivectors involving e_inf.
    const Multivector translation_biv{motor.grade_projection(2) - euclidean_biv};

    return (angle / euclidean_norm) * euclidean_biv + (Scalar{1} / s) * translation_biv;
  }

  /**
   * Compute the exponential of a CGA bivector. The Euclidean part generates rotation and
   * the e_inf part generates translation. The formula parallels the PGA case but in the
   * conformal embedding — the translational generator uses e_inf rather than the ideal
   * line elements of PGA.
   */
  Multivector motor_exp(const Multivector& bivector) const {
    using std::cos, std::sin, std::sqrt;

    const Scalar b01{bivector.coefficient(0b00011)};
    const Scalar b02{bivector.coefficient(0b00101)};
    const Scalar b12{bivector.coefficient(0b00110)};
    const Scalar euclidean_norm_sq{b01 * b01 + b02 * b02 + b12 * b12};

    if (euclidean_norm_sq < Algebra::EPSILON) {
      Multivector result{bivector};
      result.set_scalar(Scalar{1});
      return result;
    }

    const Scalar euclidean_norm{sqrt(euclidean_norm_sq)};
    const Scalar c{cos(euclidean_norm)};
    const Scalar s{sin(euclidean_norm) / euclidean_norm};

    const Multivector euclidean_biv{
        b01 * (Multivector::template e<0>() * Multivector::template e<1>()) +
        b02 * (Multivector::template e<0>() * Multivector::template e<2>()) +
        b12 * (Multivector::template e<1>() * Multivector::template e<2>())};
    const Multivector translation_biv{bivector.grade_projection(2) - euclidean_biv};

    Multivector result{s * euclidean_biv + s * translation_biv};
    result.set_scalar(c);
    return result;
  }

  /**
   * Extract Euclidean coordinates from a CGA null point vector under standard normalization.
   * The e0 coefficient encodes the homogeneous weight — under standard normalization this
   * is 1, but after arithmetic operations it may drift. The e1, e2, e3 coefficients are
   * divided by the e0 weight to recover Euclidean coordinates. The e_inf coefficient is
   * discarded — it encodes the norm-squared of the point and is redundant given the
   * coordinates.
   *
   * The e0 null vector is (e_minus - e_plus)/2. Its coefficient in a CGA multivector is
   * recovered via the inner product with e_inf, since e0 * e_inf = -1 under the CGA metric.
   */
  constexpr void extract_point(const Multivector& point, Scalar& out_x, Scalar& out_y,
                               Scalar& out_z) const {
    // The weight under standard normalization is -point * e_inf (scalar part of inner product).
    const Scalar w{-(point.left_contraction(e_inf())).scalar()};
    if (w < Algebra::EPSILON && w > -Algebra::EPSILON) {
      except<std::domain_error>("Cannot extract coordinates from a point at infinity (w = 0).");
    }
    // Bit indices for grade-1 basis vectors in R(4,1): e<0>=0b00001, e<1>=0b00010, e<2>=0b00100
    out_x = point.coefficient(0b00001) / w;
    out_y = point.coefficient(0b00010) / w;
    out_z = point.coefficient(0b00100) / w;
  }

  /**
   * Extract direction and moment from a CGA line element. A CGA line is a grade-3 element
   * (the outer product of two CGA points and e_inf, or equivalently the outer product of
   * three collinear points). The direction is recovered from the components not involving
   * e_inf, and the moment from the cross-terms. The extraction normalizes by the magnitude
   * of the direction part.
   */
  void extract_line(const Multivector& line, Scalar& out_dx, Scalar& out_dy, Scalar& out_dz,
                    Scalar& out_mx, Scalar& out_my, Scalar& out_mz) const {
    using std::sqrt;

    // In CGA a line is grade-3. The direction lives in the e_inf ^ e_i ^ e_j components
    // and the moment in the e_i ^ e_j ^ e_k components not involving e_inf.
    // Bit indices (5 basis vectors, e_inf = e<3>+e<4> direction):
    // The Euclidean bivector of the direction: e12, e13, e23 combined with e_inf.
    // For extraction we use the dual approach: L* = L.dual() gives a grade-2 element
    // whose Euclidean part is the direction and ideal part is the moment.
    const Multivector line_dual{line.dual()};

    out_dx = line_dual.coefficient(0b00110);  // e12 component of dual
    out_dy = line_dual.coefficient(0b01010);  // e13 component of dual
    out_dz = line_dual.coefficient(0b01100);  // e23 component of dual

    out_mx = line_dual.coefficient(0b10001);  // e1 ^ e_inf component
    out_my = line_dual.coefficient(0b10010);  // e2 ^ e_inf component
    out_mz = line_dual.coefficient(0b10100);  // e3 ^ e_inf component
  }

  /**
   * Extract normal and distance from a CGA plane element. In CGA a plane is a null vector
   * at grade-1 that contains e_inf — it is a degenerate sphere passing through infinity.
   * The normal is recovered from the e1, e2, e3 components and the distance from the e0
   * component, after normalizing by the magnitude of the normal part.
   */
  constexpr void extract_plane(const Multivector& plane, Scalar& out_nx, Scalar& out_ny,
                               Scalar& out_nz, Scalar& out_d) const {
    using std::sqrt;
    const Scalar nx{plane.coefficient(0b00001)};
    const Scalar ny{plane.coefficient(0b00010)};
    const Scalar nz{plane.coefficient(0b00100)};
    const Scalar norm{sqrt(nx * nx + ny * ny + nz * nz)};
    if (norm < Algebra::EPSILON) {
      except<std::domain_error>("Cannot extract normal from a degenerate plane.");
    }
    out_nx = nx / norm;
    out_ny = ny / norm;
    out_nz = nz / norm;
    // The distance from the origin is encoded in the e0 component of the plane.
    // e0 = (e_minus - e_plus)/2, so its coefficient is recovered from the e_plus/e_minus parts.
    const Scalar e_plus_coeff{plane.coefficient(0b01000)};   // e<3>
    const Scalar e_minus_coeff{plane.coefficient(0b10000)};  // e<4>
    out_d = (e_minus_coeff - e_plus_coeff) / (Scalar{2} * norm);
  }
};

// CGA uses R(4,1), not the Pga alias — the static_assert instantiates CgaGeometry with
// default template parameters to confirm concept satisfaction.
static_assert(GeometryModel<CgaGeometry<>>);

}  // namespace ndyn::math
