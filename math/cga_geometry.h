#pragma once

#include <limits>

#include "glog/logging.h"
#include "math/abs.h"
#include "math/algebra.h"
#include "math/geometry_model.h"
#include "math/multivector.h"

namespace ndyn::math {

/**
 * A GeometryModel implementation on a conformal geometric algebra.
 *
 * Base CGA embeds 3D Euclidean geometry into a 5D space with signature Cl(4,1) by adding two
 * basis vectors beyond the three Euclidean ones:
 *   e_plus  (e_plus^2 = +1)
 *   e_minus (e_minus^2 = -1)
 *
 * These are typically recombined as:
 *   e_orig = (e_minus - e_plus) / 2  — the point at the origin
 *   e_inf = e_minus + e_plus     — the point at infinity
 *
 * When extended with a time basis, the algebra is Cl(5,1) known here as Time-extended CGA (TCGA).

 * Point embedding (standard normalization):
 *   X = e_orig + p + (1/2)|p|^2 * e_inf
 *   where p = pt * e0 + px * e1 + py * e2 + pz * e3
 *
 * This vector representation satisfies X * X = 0 for any finite Euclidean point p.
 * The e_orig coefficient is fixed at 1 under standard normalization, allowing recovery of
 * Euclidean coordinates by dividing the e0, e1, e2, e3 coefficients by the e_orig coefficient.
 *
 * Basis convention (non-time-extended):
 *   e<0> = x, e<1> = y, e<2> = z
 *   e<3> = e_plus
 *   e<4> = e_minus
 *
 * Basis convention (time-extended):
 *   e<0> = t, e<1> = x, e<2> = y, e<3> = z
 *   e<4> = e_plus
 *   e<5> = e_minus
 */
template <bool TIME_EXTENDED, typename T = DefaultScalarType>
class ConformalGeometryType final {
 public:
  static constexpr size_t NUM_EUCLIDEAN_DIMENSIONS{TIME_EXTENDED ? 4 : 3};

  using Algebra = math::Algebra<T, NUM_EUCLIDEAN_DIMENSIONS + 1, 1, 0>;
  using Multivector = Algebra::VectorType;
  using Scalar = Algebra::ScalarType;

 private:
  static constexpr size_t TIME_VECTOR_INDEX{TIME_EXTENDED ? 0 : std::numeric_limits<size_t>::max()};
  static constexpr size_t X_VECTOR_INDEX{TIME_EXTENDED ? 1 : 0};
  static constexpr size_t Y_VECTOR_INDEX{X_VECTOR_INDEX + 1};
  static constexpr size_t Z_VECTOR_INDEX{Y_VECTOR_INDEX + 1};

  static constexpr size_t TIME_BASIS_COEFFICIENT{1UL << TIME_VECTOR_INDEX};
  static constexpr size_t X_BASIS_COEFFICIENT{1UL << X_VECTOR_INDEX};
  static constexpr size_t Y_BASIS_COEFFICIENT{1UL << Y_VECTOR_INDEX};
  static constexpr size_t Z_BASIS_COEFFICIENT{1UL << Z_VECTOR_INDEX};

  static constexpr Scalar get_t(const Multivector& mv) {
    return mv.template coefficient<TIME_BASIS_COEFFICIENT>();
  }
  static constexpr Scalar get_x(const Multivector& mv) {
    return mv.template coefficient<X_BASIS_COEFFICIENT>();
  }
  static constexpr Scalar get_y(const Multivector& mv) {
    return mv.template coefficient<Y_BASIS_COEFFICIENT>();
  }
  static constexpr Scalar get_z(const Multivector& mv) {
    return mv.template coefficient<Z_BASIS_COEFFICIENT>();
  }

  static constexpr Scalar get_tx(const Multivector& mv) {
    return mv.template coefficient<TIME_BASIS_COEFFICIENT | X_BASIS_COEFFICIENT>();
  }
  static constexpr Scalar get_ty(const Multivector& mv) {
    return mv.template coefficient<TIME_BASIS_COEFFICIENT | Y_BASIS_COEFFICIENT>();
  }
  static constexpr Scalar get_tz(const Multivector& mv) {
    return mv.template coefficient<TIME_BASIS_COEFFICIENT | Z_BASIS_COEFFICIENT>();
  }
  static constexpr Scalar get_xy(const Multivector& mv) {
    return mv.template coefficient<X_BASIS_COEFFICIENT | Y_BASIS_COEFFICIENT>();
  }
  static constexpr Scalar get_xz(const Multivector& mv) {
    return mv.template coefficient<X_BASIS_COEFFICIENT | Z_BASIS_COEFFICIENT>();
  }
  static constexpr Scalar get_yz(const Multivector& mv) {
    return mv.template coefficient<Y_BASIS_COEFFICIENT | Z_BASIS_COEFFICIENT>();
  }

  // The null basis vectors e0 and e_inf are the conformal origin and infinity respectively.
  // They are not basis vectors of the algebra directly but are linear combinations of e_plus
  // and e_minus. These helpers keep the construction readable throughout the implementation.
  static constexpr Multivector e_plus() { return Multivector::template e<Z_VECTOR_INDEX + 1>(); }
  static constexpr Multivector e_minus() { return Multivector::template e<Z_VECTOR_INDEX + 2>(); }

  // Assert that the order of the bases is as expected.
  static_assert(e_plus() * e_plus() == Multivector{1});
  static_assert(e_minus() * e_minus() == Multivector{-1});

  static constexpr Multivector e_origin() {
    return (Scalar{1} / Scalar{2}) * (e_minus() - e_plus());
  }

  static constexpr Multivector e_inf() { return e_minus() + e_plus(); }

  static constexpr Scalar weight(const Multivector& mv) {
    return -(mv.left_contraction(e_inf()).scalar());
  }

 public:
  // Factory methods for basis vectors.
  static constexpr Multivector gamma0()
    requires TIME_EXTENDED
  {
    return Multivector::template e<TIME_VECTOR_INDEX>();
  }
  static constexpr Multivector gamma1() { return Multivector::template e<X_VECTOR_INDEX>(); }
  static constexpr Multivector gamma2() { return Multivector::template e<Y_VECTOR_INDEX>(); }
  static constexpr Multivector gamma3() { return Multivector::template e<Z_VECTOR_INDEX>(); }

  // Factory methods for basis bivectors.
  static constexpr Multivector gamma01()
    requires TIME_EXTENDED
  {
    return gamma0() * gamma1();
  }
  static constexpr Multivector gamma02()
    requires TIME_EXTENDED
  {
    return gamma0() * gamma2();
  }
  static constexpr Multivector gamma03()
    requires TIME_EXTENDED
  {
    return gamma0() * gamma3();
  }
  static constexpr Multivector gamma12() { return gamma1() * gamma2(); }
  static constexpr Multivector gamma13() { return gamma1() * gamma3(); }
  static constexpr Multivector gamma23() { return gamma2() * gamma3(); }

  /**
   * Compute the join via the outer product. In CGA the outer product of two null point
   * vectors produces the line through them (grade-2 in the conformal sense), and the outer
   * product of three points produces the circle or plane through them. This is the same
   * approach as in PGA with points as grade-1 multivectors. This reflects the conformal
   * embedding's structural similarity to projective geometry at the level of the outer product.
   */
  static constexpr Multivector join(const Multivector& a, const Multivector& b) noexcept {
    return a.outer(b);
  }

  /**
   * Compute the meet via the regressive product. In CGA the regressive product recovers
   * the intersection of two elements — the intersection of two spheres is a circle, the
   * intersection of a sphere and a plane is a circle, and so on. The regressive product
   * serves as meet in CGA for the same structural reason as in PGA with grade-1 points.
   */
  static constexpr Multivector meet(const Multivector& a, const Multivector& b) noexcept {
    return a.regress(b);
  }

  static constexpr Multivector make_reflector(Scalar nx, Scalar ny, Scalar nz, Scalar px, Scalar py,
                                              Scalar pz) noexcept {
    const Scalar norm{hypot(nx, ny, nz)};
    // Normalized components.
    const Scalar ux = nx / norm;
    const Scalar uy = ny / norm;
    const Scalar uz = nz / norm;
    // Distance of point to plane through the origin. Or, displacement of the plane through the
    // origin to make it contain the given point.
    const Scalar dist{px * ux + py * uy + pz * uz};

    return {ux * gamma1() + uy * gamma2() + uz * gamma3() + dist * e_inf()};
  }

  static constexpr Multivector make_reflector(Scalar nt, Scalar nx, Scalar ny, Scalar nz, Scalar pt,
                                              Scalar px, Scalar py, Scalar pz) noexcept
    requires TIME_EXTENDED
  {
    const Scalar norm{hypot(nt, nx, ny, nz)};
    // Normalized components.
    const Scalar ut = nt / norm;
    const Scalar ux = nx / norm;
    const Scalar uy = ny / norm;
    const Scalar uz = nz / norm;
    // Distance of point to plane through the origin. Or, displacement of the plane through the
    // origin to make it contain the given point.
    const Scalar dist{pt * ut + px * ux + py * uy + pz * uz};

    return {ut * gamma0() + ux * gamma1() + uy * gamma2() + uz * gamma3() + dist * e_inf()};
  }

  /**
   * Construct a CGA rotor in the given plane to rotate by angle radians. The plane of rotation is
   * given by its normal.
   */
  static Multivector make_rotor(Scalar nx, Scalar ny, Scalar nz, Scalar angle) {
    using std::cos, std::sin;

    const Scalar norm{hypot(nx, ny, nz)};
    if (norm < Algebra::EPSILON) {
      except<std::domain_error>("Degenerate axis: cannot construct rotor from a null axis.");
    }

    const Scalar half_angle{angle / Scalar{2}};
    const Multivector rotation_plane{nx / norm * gamma23()    //
                                     - ny / norm * gamma13()  //
                                     + nz / norm * gamma12()};
    return {cos(half_angle) - sin(half_angle) * rotation_plane};
  }

  static Multivector make_rotor(const Multivector& rotation_plane, Scalar angle) {
    if (!rotation_plane.is_grade(2)) {
      except<std::invalid_argument>(
          "In all geometric algebras, versors, such as rotors, should be bivectors.");
    }

    using std::cos, std::sin, std::sqrt;

    const Scalar norm{sqrt(abs(rotation_plane.square_magnitude()))};
    if (norm < Algebra::EPSILON) {
      except<std::domain_error>(
          "Degenerate plane: cannot construct rotor from a null rotation plane.");
    }

    const Scalar half_angle{angle / Scalar{2}};
    return {cos(half_angle) - sin(half_angle) / norm * rotation_plane};
  }

  /**
   * Construct a CGA translator for displacement (dx, dy, dz). In CGA translators use e_inf:
   *   T = 1 - (1/2)(dx*e1 + dy*e2 + dz*e3) * e_inf
   *
   * The e_inf factor ensures T * reverse(T) = 1, mirroring the PGA translator's use of e0.
   * The sign convention follows from the CGA rotor exponential where the translation
   * generator is -(1/2) * t * e_inf for displacement t.
   */
  static constexpr Multivector make_translator(Scalar dx, Scalar dy, Scalar dz) {
    const Multivector displacement{dx * gamma1() + dy * gamma2() + dz * gamma3()};
    Multivector result{Scalar{1} - (displacement * e_inf() / Scalar{2})};
    return result;
  }

  static constexpr Multivector make_translator(Scalar dt, Scalar dx, Scalar dy, Scalar dz)
    requires TIME_EXTENDED
  {
    const Multivector displacement{dt * gamma0() + dx * gamma1() + dy * gamma2() + dz * gamma3()};
    Multivector result{Scalar{1} - (displacement * e_inf() / Scalar{2})};
    return result;
  }

  /**
   * Constructs a dilator (scaling versor) centered at a given point.
   * A dilator scales space by the given factor relative to the center (cx, cy, cz).
   */
  static Multivector make_dilator(Scalar cx, Scalar cy, Scalar cz, Scalar scale) {
    using std::log, std::cosh, std::sinh;

    if (scale <= Scalar{0}) {
      except<std::domain_error>("Scale factor must be positive.");
    }

    // The generator of dilation at the origin is the Minkowski plane E = e_inf ^ e_origin.
    // In our basis, E = e_inf ^ e_origin() has the property E^2 = 1.
    const Multivector E = e_inf() ^ e_origin();

    // For a scale factor k, the dilation is exp(ln(k)/2 * E).
    // Using the identity exp(phi * E) = cosh(phi) + E * sinh(phi) for E^2 = 1.
    const Scalar phi = log(scale) / Scalar{2};
    const Multivector origin_dilator = cosh(phi) + (E * sinh(phi));

    // Translate the origin-centered dilator to the target center point.
    const Multivector translate = make_translator(cx, cy, cz);

    return translate * origin_dilator * ~translate;
  }

  static Multivector make_dilator(Scalar ct, Scalar cx, Scalar cy, Scalar cz, Scalar scale)
    requires TIME_EXTENDED
  {
    using std::log, std::cosh, std::sinh;

    if (scale <= Scalar{0}) {
      except<std::domain_error>("Scale factor must be positive.");
    }

    // The generator of dilation at the origin is the Minkowski plane E = e_inf ^ e_origin.
    // In our basis, E = e_inf ^ e_origin() has the property E^2 = 1.
    const Multivector E = e_inf() ^ e_origin();

    // For a scale factor k, the dilation is exp(ln(k)/2 * E).
    // Using the identity exp(phi * E) = cosh(phi) + E * sinh(phi) for E^2 = 1.
    const Scalar phi = log(scale) / Scalar{2};
    const Multivector origin_dilator = cosh(phi) + (E * sinh(phi));

    // Translate the origin-centered dilator to the target center point.
    const Multivector translate = make_translator(ct, cx, cy, cz);

    return translate * origin_dilator * ~translate;
  }

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
  static constexpr Multivector make_point(Scalar x, Scalar y, Scalar z) {
    const Scalar half_norm_sq{(x * x + y * y + z * z) / Scalar{2}};
    return e_origin() + x * gamma1() + y * gamma2() + z * gamma3() + half_norm_sq * e_inf();
  }

  static constexpr Multivector make_point(Scalar t, Scalar x, Scalar y, Scalar z)
    requires TIME_EXTENDED
  {
    const Scalar half_norm_sq{(t * t + x * x + y * y + z * z) / Scalar{2}};
    return e_origin() + t * gamma0() + x * gamma1() + y * gamma2() + z * gamma3() +
           half_norm_sq * e_inf();
  }

  /**
   * Extract Euclidean coordinates from a CGA null point vector under standard normalization.
   */
  static constexpr void extract_point(const Multivector& point, Scalar& out_x, Scalar& out_y,
                                      Scalar& out_z) {
    const Scalar w{weight(point)};
    if (abs(w) < Algebra::EPSILON) {
      except<std::domain_error>("Cannot extract coordinates from a point at infinity (w = 0).");
    }

    out_x = get_x(point) / w;
    out_y = get_y(point) / w;
    out_z = get_z(point) / w;
  }

  static constexpr void extract_point(const Multivector& point, Scalar& out_t, Scalar& out_x,
                                      Scalar& out_y, Scalar& out_z)
    requires TIME_EXTENDED
  {
    const Scalar w{weight(point)};
    if (abs(w) < Algebra::EPSILON) {
      except<std::domain_error>("Cannot extract coordinates from a point at infinity (w = 0).");
    }

    out_t = get_t(point) / w;
    out_x = get_x(point) / w;
    out_y = get_y(point) / w;
    out_z = get_z(point) / w;
  }

  /**
   * In CGA a finite point is a null vector (X * ~X = 0) at grade-1 with a nonzero inner
   * product with e_inf (nonzero homogeneous weight). The null condition distinguishes
   * points from general grade-1 elements such as spheres (which are non-null). A point
   * at infinity has zero inner product with e_inf and is excluded by the weight check.
   *
   * Note that this approach works in both time-extended and purely spatial geometries.
   */
  static constexpr bool is_point(const Multivector& mv) {
    if (!mv.template is_grade<1>()) {
      return false;
    }

    // The null condition X * ~X = 0 distinguishes points from spheres.
    const auto self_product{mv.multiply(mv.reverse()).scalar()};
    const bool is_null{abs(self_product) < Algebra::EPSILON};

    // A finite point has nonzero weight — should be exactly -1 under perfect normalization.
    const bool has_weight{abs(weight(mv)) > Algebra::EPSILON};

    return is_null && has_weight;
  }
  static_assert(is_point(make_point(1, 1, 1)),
                "Check that the definitions of is_point() and make_point() agree.");

  /**
   * Constructs a grade-4 multivector representing a plane.
   *
   * This bridges Euclidean intuition (normal-defined planes) into a grade-4
   * multivector.
   */
  static constexpr Multivector make_plane(Scalar nx, Scalar ny, Scalar nz, Scalar px, Scalar py,
                                          Scalar pz) {
    const Scalar norm{hypot(nx, ny, nz)};
    if (norm < Algebra::EPSILON) {
      except<std::domain_error>("Degenerate normal: cannot construct plane from null vector.");
    }

    // Normalize direction components
    const Scalar ux = nx / norm;
    const Scalar uy = ny / norm;
    const Scalar uz = nz / norm;

    // The 3D spatial bivector representing the orientation of the plane.
    // This is the dual of the normal vector within the 3D Euclidean subspace.
    const Multivector spatial_bivector{ux * gamma23() - uy * gamma13() + uz * gamma12()};

    const Multivector p{make_point(px, py, pz)};

    // A Plane in OPNS is the wedge of a point, the direction bivectors, and infinity.
    // Resulting Grade: 1 (point) + 2 (bivector) + 1 (e_inf) = 4.
    return join(join(p, spatial_bivector), e_inf());
  }

  static constexpr bool is_plane(const Multivector& mv) noexcept {
    if (!mv.template is_grade<4>()) {
      return false;
    }

    // The null condition L * ~L = 0 distinguishes lines (null) from circles (non-null).
    const auto self_product{(mv * ~mv).scalar()};
    const bool is_null{abs(self_product) < Algebra::EPSILON};

    // A line contains e_inf as a factor — its outer product with e_inf is zero.
    const bool contains_e_inf{(mv ^ e_inf()).near_equal(Multivector{})};

    return is_null and contains_e_inf;
  }

  /**
   * Constructs a 3-blade line passing through two 3D Euclidean points.
   * In conformal geometries, a flat line is the wedge of two conformal points and the point at
   * infinity.
   */
  static constexpr Multivector make_line(Scalar px1, Scalar py1, Scalar pz1, Scalar px2, Scalar py2,
                                         Scalar pz2) {
    const Multivector p1{make_point(px1, py1, pz1)};
    const Multivector p2{make_point(px2, py2, pz2)};

    return join(join(p1, p2), e_inf());
  }

  /**
   * In conformal geometric algebras, a line is a grade-3 element that is null (L * ~L = 0) and
   * contains e_inf as a factor (it is the outer product of two points and e_inf, or equivalently
   * passes through the point at infinity). The null condition distinguishes lines from circles,
   * which are also grade-3 but non-null and without conformal basis blades.
   */
  static constexpr bool is_line(const Multivector& mv) noexcept {
    if (!mv.template is_grade<3>()) {
      return false;
    }

    // The null condition L * ~L = 0 distinguishes lines (null) from circles (non-null).
    const auto self_product{(mv * ~mv).scalar()};
    const bool is_null{abs(self_product) < Algebra::EPSILON};

    // A line contains e_inf as a factor — its outer product with e_inf is zero.
    const bool contains_e_inf{(mv ^ e_inf()).near_equal(Multivector{})};

    return is_null and contains_e_inf;
  }

  /**
   * Constructs a 3-blade circle from a center point, a normal vector defining the
   * plane of the circle, and a radius.
   *
   * This is implemented by intersecting a sphere with a plane.
   */
  static constexpr Multivector make_circle(Scalar cx, Scalar cy, Scalar cz, Scalar nx, Scalar ny,
                                           Scalar nz, Scalar radius) {
    const Multivector center{make_point(cx, cy, cz)};

    // Create the dual sphere: P - r^2 / 2 * e_inf
    const Multivector sphere{center - radius * radius / Scalar{2} * e_inf()};

    // Create the dual plane passing through the center with the given normal.
    const Multivector plane{make_plane(nx, ny, nz, cx, cy, cz)};

    // The intersection of a dual sphere and a dual plane is the dual circle.
    return meet(sphere, plane);
  }

  /**
   * Circles and lines are almost the same entities in CGA. A line is just a circle with infinite
   * radius.
   */
  static constexpr bool is_circle(const Multivector& mv) {
    if (!mv.template is_grade<3>()) {
      return false;
    }

    // The null condition L * ~L = 0 distinguishes lines (null) from circles (non-null).
    const auto self_product{mv.multiply(mv.reverse()).scalar()};
    const bool is_null{abs(self_product) < Algebra::EPSILON};

    // A line contains e_inf as a factor — its outer product with e_inf is zero. A circle does not
    // have e_inf as a factor.
    const bool contains_e_inf{mv.outer(e_inf()).near_equal(Multivector{})};

    return !is_null && !contains_e_inf;
  }

  /**
   * Compute the logarithm of a CGA motor. CGA motors represent rigid body motions and
   * live in the even subalgebra of R(4,1). The logarithm formula is analogous to PGA but
   * operates in the 5D conformal space — the Euclidean bivector components are those not
   * involving e_plus or e_minus, and the translational components involve e_inf.
   */
  static Multivector motor_log(const Multivector& motor) {
    using std::acos, std::sin, std::sqrt;

    const Scalar scalar_part{motor.scalar()};

    const Scalar e01{motor.coefficient(0b00011)};
    const Scalar e02{motor.coefficient(0b00101)};
    const Scalar e12{motor.coefficient(0b00110)};
    const Scalar euclidean_norm_sq{e01 * e01 + e02 * e02 + e12 * e12};

    if (euclidean_norm_sq < Algebra::EPSILON) {
      return motor.grade_projection(2) - Multivector{}.add(Scalar{1});
    }

    const Scalar euclidean_norm{sqrt(euclidean_norm_sq)};
    const Scalar angle{Scalar{2} * acos(scalar_part)};
    const Scalar s{sin(angle / Scalar{2})};

    const Multivector euclidean_biv{
        e01 * (Multivector::template e<0>() * Multivector::template e<1>()) +
        e02 * (Multivector::template e<0>() * Multivector::template e<2>()) +
        e12 * (Multivector::template e<1>() * Multivector::template e<2>())};

    // The translational component of the motor lives in the bivectors involving e_inf.
    const Multivector translation_biv{motor.template grade_projection<2>() - euclidean_biv};

    return (angle / euclidean_norm) * euclidean_biv + (Scalar{1} / s) * translation_biv;
  }

  /**
   * Compute the exponential of a CGA bivector. The Euclidean part generates rotation and
   * the e_inf part generates translation. The formula parallels the PGA case but in the
   * conformal embedding — the translational generator uses e_inf rather than the ideal
   * line elements of PGA.
   */
  static Multivector motor_exp(const Multivector& bivector) {
    using std::cos, std::sin, std::sqrt;

    const Scalar e01{bivector.coefficient(0b0011)};
    const Scalar e02{bivector.coefficient(0b0101)};
    const Scalar e12{bivector.coefficient(0b0110)};
    const Scalar euclidean_norm_sq{e01 * e01 + e02 * e02 + e12 * e12};

    if (euclidean_norm_sq < Algebra::EPSILON) {
      Multivector result{bivector};
      result.set_scalar(Scalar{1});
      return result;
    }

    const Scalar euclidean_norm{sqrt(euclidean_norm_sq)};
    const Scalar c{cos(euclidean_norm)};
    const Scalar s{sin(euclidean_norm) / euclidean_norm};

    Multivector result{s * bivector.template grade_projection<2>() + c};
    return result;
  }
};

template <typename Scalar = DefaultScalarType>
using SpatialConformalGeometry = ConformalGeometryType</* Not time-extended */ false, Scalar>;

template <typename Scalar = DefaultScalarType>
using ConformalGeometry = ConformalGeometryType</* Time-extended */ true, Scalar>;

static_assert(GeometryModel<SpatialConformalGeometry<>>);
static_assert(SpatialConformalGeometryModel<SpatialConformalGeometry<>>);
static_assert(ConformalGeometryModel<ConformalGeometry<>>);

}  // namespace ndyn::math
