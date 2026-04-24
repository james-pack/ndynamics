#pragma once

#include <limits>

#include "glog/logging.h"
#include "math/abs.h"
#include "math/algebra.h"
#include "math/canonical_basis_representation.h"
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
template <size_t NUM_PHYSICAL_DIMENSIONS_VALUE, typename T = DefaultScalarType>
class CgaGeometryType final {
 public:
  static constexpr size_t NUM_PHYSICAL_DIMENSIONS{NUM_PHYSICAL_DIMENSIONS_VALUE};
  using Algebra = math::Algebra<T, NUM_PHYSICAL_DIMENSIONS + 1, 1, 0>;
  using Multivector = Algebra::VectorType;
  using Scalar = Algebra::ScalarType;
  static constexpr Scalar EPSILON{Algebra::EPSILON};

 private:
  // The null basis vectors e0 and e_inf are the conformal origin and infinity respectively.
  // They are not basis vectors of the algebra directly but are linear combinations of e_plus
  // and e_minus. These helpers keep the construction readable throughout the implementation.
  static constexpr Multivector e_plus() {
    return Multivector::template e<NUM_PHYSICAL_DIMENSIONS>();
  }
  static constexpr Multivector e_minus() {
    return Multivector::template e<NUM_PHYSICAL_DIMENSIONS + 1>();
  }

  // Assert that the order of the bases is as expected.
  static_assert((e_plus() * e_plus()).scalar() == Scalar{1});
  static_assert((e_minus() * e_minus()).scalar() == Scalar{-1});

  static constexpr Multivector mask_conformal_bases(const Multivector& mv) {
    return mv.template mask_bases<NUM_PHYSICAL_DIMENSIONS, NUM_PHYSICAL_DIMENSIONS + 1>();
  }

  /**
   * Standard weight calculation.
   * The weight is a measure of the scale of the space.
   *
   * Direct mathematical calculation would yield:
   * -(mv.left_contraction(e_inf()).scalar());
   *
   * But we use an equivalent calculation based on the coefficients of e_plus and e_minus.
   */
  static constexpr Scalar weight(const Multivector& mv) {
    return mv.template coefficient<1UL << (NUM_PHYSICAL_DIMENSIONS + 1)>() -
           mv.template coefficient<1UL << NUM_PHYSICAL_DIMENSIONS>();
  }

  /**
   * Weight of the origin conformal basis.
   *
   * Direct mathematical calculation would yield:
   * -(mv.left_contraction(e_orig()).scalar());
   *
   * But we use an equivalent calculation based on the coefficients of e_plus and e_minus.
   */
  static constexpr Scalar weight_origin(const Multivector& mv) {
    return (mv.template coefficient<1UL << (NUM_PHYSICAL_DIMENSIONS + 1)>() +
            mv.template coefficient<1UL << NUM_PHYSICAL_DIMENSIONS>()) /
           Scalar{2};
  }

 public:
  static constexpr Multivector e_orig() { return (Scalar{1} / Scalar{2}) * (e_minus() - e_plus()); }

  static constexpr Multivector e_inf() { return e_minus() + e_plus(); }

  // Factory methods for basis vectors under the generic names.
  static constexpr Multivector e1()
    requires(NUM_PHYSICAL_DIMENSIONS >= 1)
  {
    return Multivector::template e<0>();
  }
  static constexpr Multivector e2()
    requires(NUM_PHYSICAL_DIMENSIONS >= 2)
  {
    return Multivector::template e<1>();
  }
  static constexpr Multivector e3()
    requires(NUM_PHYSICAL_DIMENSIONS >= 3)
  {
    return Multivector::template e<2>();
  }
  static constexpr Multivector e4()
    requires(NUM_PHYSICAL_DIMENSIONS >= 4)
  {
    return Multivector::template e<3>();
  }

  // Getters for the coefficients of the basis vectors under the generic names.
  static constexpr Scalar get_e1(const Multivector& mv)
    requires(NUM_PHYSICAL_DIMENSIONS >= 1)
  {
    return mv.template coefficient<1UL << 0>();
  }
  static constexpr Scalar get_e2(const Multivector& mv)
    requires(NUM_PHYSICAL_DIMENSIONS >= 2)
  {
    return mv.template coefficient<1UL << 1>();
  }
  static constexpr Scalar get_e3(const Multivector& mv)
    requires(NUM_PHYSICAL_DIMENSIONS >= 3)
  {
    return mv.template coefficient<1UL << 2>();
  }
  static constexpr Scalar get_e4(const Multivector& mv)
    requires(NUM_PHYSICAL_DIMENSIONS >= 4)
  {
    return mv.template coefficient<1UL << 3>();
  }

  // Factory methods for physical basis vectors using names more specific to conformal geometry.
  static constexpr Multivector gamma0()
    requires(NUM_PHYSICAL_DIMENSIONS >= 1)
  {
    static_assert((e1() * e1()).scalar() == Scalar{1});
    return e1();
  }
  static constexpr Multivector gamma1()
    requires(NUM_PHYSICAL_DIMENSIONS >= 2)
  {
    static_assert((e2() * e2()).scalar() == Scalar{1});
    return e2();
  }
  static constexpr Multivector gamma2()
    requires(NUM_PHYSICAL_DIMENSIONS >= 3)
  {
    static_assert((e3() * e3()).scalar() == Scalar{1});
    return e3();
  }
  static constexpr Multivector gamma3()
    requires(NUM_PHYSICAL_DIMENSIONS >= 4)
  {
    static_assert((e4() * e4()).scalar() == Scalar{1});
    return e4();
  }

  // Getters for the coefficients of the basis vectors under the generic names.
  static constexpr Scalar get_gamma0(const Multivector& mv)
    requires(NUM_PHYSICAL_DIMENSIONS >= 1)
  {
    return get_e1(mv);
  }
  static constexpr Scalar get_gamma1(const Multivector& mv)
    requires(NUM_PHYSICAL_DIMENSIONS >= 2)
  {
    return get_e2(mv);
  }
  static constexpr Scalar get_gamma2(const Multivector& mv)
    requires(NUM_PHYSICAL_DIMENSIONS >= 3)
  {
    return get_e3(mv);
  }
  static constexpr Scalar get_gamma3(const Multivector& mv)
    requires(NUM_PHYSICAL_DIMENSIONS >= 4)
  {
    return get_e4(mv);
  }

  // Factory methods for basis bivectors.
  static constexpr Multivector gamma01() { return gamma0() * gamma1(); }
  static constexpr Multivector gamma02() { return gamma0() * gamma2(); }
  static constexpr Multivector gamma03() { return gamma0() * gamma3(); }
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

  /**
   * Construct a rotor around the origin of the plane. Note that this is only meaningful with two
   * physical dimensions.
   */
  static Multivector make_rotor(Scalar angle)
    requires(NUM_PHYSICAL_DIMENSIONS == 2)
  {
    using std::cos, std::sin;

    const Scalar half_angle{angle / Scalar{2}};
    static constexpr const Multivector rotation_plane{gamma01()};
    return cos(half_angle) - sin(half_angle) * rotation_plane;
  }

  /**
   * Construct a rotor around an axis. Note that this is only meaningful with three
   * physical dimensions. In higher dimensions, a single axis does not uniquely determine a
   * rotation.
   */
  static Multivector make_rotor(Scalar nx, Scalar ny, Scalar nz, Scalar angle)
    requires(NUM_PHYSICAL_DIMENSIONS == 3)
  {
    using std::cos, std::sin;

    const Scalar norm{hypot(nx, ny, nz)};
    if (norm < Algebra::EPSILON) {
      except<std::domain_error>("Degenerate axis: cannot construct rotor from a null axis.");
    }

    const Scalar half_angle{angle / Scalar{2}};
    const Multivector rotation_plane{nx / norm * gamma12() - ny / norm * gamma02() +
                                     nz / norm * gamma01()};
    return cos(half_angle) - sin(half_angle) * rotation_plane;
  }

  /**
   * Construct a CGA rotor in the given plane to rotate by angle radians. The plane of rotation is
   * given by its normal.
   */
  static Multivector make_rotor(const Multivector& rotation_plane, Scalar angle) {
    if (!rotation_plane.is_grade(2)) {
      except<std::invalid_argument>(
          "In all geometric algebras, versors, such as rotors, should be bivectors.");
    }

    using std::cos, std::sin;

    const Scalar half_angle{angle / Scalar{2}};
    return cos(half_angle) - sin(half_angle) * rotation_plane.normalize();
  }

  static void extract_rotor(const Multivector& mv, Scalar& angle) {
    using std::acos;

    angle = Scalar{2} * acos(mv.scalar());
  }

  static void extract_rotor(const Multivector& mv, Scalar& nx, Scalar& ny, Scalar& nz,
                            Scalar& angle)
    requires(NUM_PHYSICAL_DIMENSIONS >= 3)
  {
    using std::acos, std::sin;
    // A normalized rotor has the form R = cos(θ/2) - sin(θ/2)(nx e01 + ny e02 + nz e12).
    // The scalar part is cos(θ/2) and the bivector part has magnitude sin(θ/2).
    // Recover the full rotation angle from the scalar component.
    const Scalar half_angle = acos(mv.scalar());
    angle = half_angle * 2;

    // sin(θ/2) is the magnitude of the bivector part. If it is zero the rotor
    // is the identity and the axis is undefined; return a canonical axis.
    const Scalar sin_half_angle = sin(half_angle);
    if (sin_half_angle == Scalar{0}) {
      nx = Scalar{0};
      ny = Scalar{0};
      nz = Scalar{1};
    } else {
      // Extract the axis by dividing out sin(θ/2) from each bivector component.
      // The sign convention matches make_rotor(): R = cos(θ/2) - sin(θ/2)(n * B).
      const Scalar inv_sin = Scalar{1} / sin_half_angle;
      nx = -get_gamma01(mv) * inv_sin;
      ny = -get_gamma02(mv) * inv_sin;
      nz = -get_gamma12(mv) * inv_sin;
    }
  }

  static bool is_rotor(const Multivector& mv) {
    // A normalized rotor has the form R = cos(θ/2) - sin(θ/2) * rotation_plane. The scalar part is
    // cos(θ/2) and the rotation_plane (a bivector) has magnitude sin(θ/2). For this method, we
    // check the value of the scalar is in the range of the cos() function. We check that the
    // multivector is in the form of a bivector plus a scalar. We verify that the square magnitude
    // of the bivector plus the square of the scalar is one.
    const Scalar scalar_value{mv.scalar()};
    if (abs(scalar_value) - Scalar{1} > EPSILON) {
      return false;
    }

    const Multivector bivector{mv.template grade_projection<2>()};
    if (!mv.is_near(bivector + scalar_value)) {
      return false;
    }

    return abs(bivector.square_magnitude() + scalar_value * scalar_value - Scalar{1}) < EPSILON;
  }

  /**
   * Constructs a dilator (scaling versor) centered at the origin.
   * A dilator scales space by the given factor.
   */
  static Multivector make_dilator(Scalar scale) {
    using std::log, std::cosh, std::sinh;

    if (scale <= Scalar{0}) {
      except<std::domain_error>("Scale factor must be positive.");
    }

    // The generator of dilation at the origin is the Minkowski plane E = e_inf ^ e_orig.
    // In our basis, E = e_inf ^ e_orig() has the property E^2 = 1.
    static constexpr const Multivector E{e_inf() ^ e_orig()};
    static_assert(abs((E * E).scalar() - Scalar{1}) < EPSILON);

    // For a scale factor k, the dilation is exp(ln(k)/2 * E).
    // Using the identity exp(phi * E) = cosh(phi) + E * sinh(phi) for E^2 = 1.
    const Scalar phi{log(scale) / Scalar{2}};
    const Multivector dilator{cosh(phi) + sinh(phi) * E};
    return dilator;
  }

  static void extract_dilator(const Multivector& mv, Scalar& scale) {
    // Transform the 'origin' and see how its weight changes.
    // In CGA, D * e_orig * ~D = (1/scale) * e_orig, where D is a dilator.
    const Multivector transformed_null_basis{mv * e_orig() * mv.reverse()};

    // The scale is the inverse of the resulting e_inf coefficient.
    scale = Scalar{1} / weight(transformed_null_basis);
  }

  static bool is_dilator(const Multivector& mv) {
    // Transform the 'origin' and see how its weight changes.
    // In CGA, D * e_orig * ~D = (1/scale) * e_orig, where D is a dilator.
    const Multivector transformed_null_basis{mv * e_orig() * mv.reverse()};

    // The scale factor is effectively the inverse of the weight of this transformed basis. A weight
    // of 1 means no scaling.
    return abs(weight(transformed_null_basis) - Scalar{1}) > EPSILON;
  }

  /**
   * Construct a CGA translator for a displacement (dt, dx, dy, dz). In CGA translators use
   * e_inf: T = 1 - (1/2)(dt*gamma0 + dx*gamma1 + dy*gamma2 + dz*gamma3) * e_inf
   *
   * The e_inf factor ensures T * reverse(T) = 1, mirroring the PGA translator's use of e0.
   * The sign convention follows from the CGA rotor exponential where the translation
   * generator is -(1/2) * t * e_inf for displacement t.
   */
  static constexpr Multivector make_translator(Scalar dt, Scalar dx, Scalar dy, Scalar dz) {
    const Multivector displacement{dt * gamma0() + dx * gamma1() + dy * gamma2() + dz * gamma3()};
    return Scalar{1} - displacement * e_inf() / Scalar{2};
  }

  static constexpr Multivector make_translator(Scalar dt, Scalar dx, Scalar dy) {
    const Multivector displacement{dt * gamma0() + dx * gamma1() + dy * gamma2()};
    return Scalar{1} - displacement * e_inf() / Scalar{2};
  }

  static constexpr Multivector make_translator(Scalar dt, Scalar dx) {
    const Multivector displacement{dt * gamma0() + dx * gamma1()};
    return Scalar{1} - displacement * e_inf() / Scalar{2};
  }

  static constexpr Multivector make_translator(Scalar dt) {
    const Multivector displacement{dt * gamma0()};
    return Scalar{1} - displacement * e_inf() / Scalar{2};
  }

  static constexpr Multivector make_translator(const Multivector& direction, Scalar dt) {
    const Multivector displacement{dt * direction};
    return Scalar{1} - displacement * e_inf() / Scalar{2};
  }

  static constexpr void extract_translator(const Multivector& mv, Scalar& dt, Scalar& dx,
                                           Scalar& dy, Scalar& dz) {
    dt = get_gamm0_e_minus(mv);
    dx = get_gamm1_e_minus(mv);
    dy = get_gamm2_e_minus(mv);
    dz = get_gamm3_e_minus(mv);
  }

  static constexpr void extract_translator(const Multivector& mv, Scalar& dt, Scalar& dx,
                                           Scalar& dy) {
    dt = get_gamm0_e_minus(mv);
    dx = get_gamm1_e_minus(mv);
    dy = get_gamm2_e_minus(mv);
  }

  static constexpr void extract_translator(const Multivector& mv, Scalar& dt, Scalar& dx) {
    dt = get_gamm0_e_minus(mv);
    dx = get_gamm1_e_minus(mv);
  }

  static constexpr void extract_translator(const Multivector& mv, Scalar& dt) {
    dt = get_gamm0_e_minus(mv);
  }

  static constexpr bool is_translator(const Multivector& mv) {
    const Scalar scalar_value{mv.scalar()};
    if (abs(scalar_value - Scalar{1}) > EPSILON) {
      return false;
    }

    const Multivector bivector{mv.template grade_projection<2>()};
    if (!bivector.is_near(mv - scalar_value)) {
      return false;
    }

    return (bivector * e_orig).template is_grade<1>();
  }

  /**
   * Embed a Euclidean point as a CGA null vector using standard normalization:
   *   X = e_orig + px*e1 + py*e2 + pz*e3 + (1/2)(px^2 + py^2 + pz^2)*e_inf
   *
   * The result satisfies X * ~X = 0. The weight is 1 under this normalization,
   * which is what allows extract_point to recover coordinates by simple division. Under
   * a different normalization the coordinates are recoverable but require an additional
   * division by the weight.
   *
   * Note that we recombine the e_orig and e_inf terms into a form that is more numerically stable
   * based directly on e_plus and e_minus:
   *
   *   (half_norm_sq - Scalar{1} / Scalar{2}) * e_plus() +
   *   (half_norm_sq + Scalar{1} / Scalar{2}) * e_minus()
   *
   */
  static constexpr Multivector make_point(Scalar t) {
    const Scalar half_norm_sq{(t * t) / Scalar{2}};
    return t * gamma0() + (half_norm_sq - Scalar{1} / Scalar{2}) * e_plus() +
           (half_norm_sq + Scalar{1} / Scalar{2}) * e_minus();
  }

  static constexpr Multivector make_point(Scalar t, Scalar x) {
    const Scalar half_norm_sq{(t * t + x * x) / Scalar{2}};
    return t * gamma0() + x * gamma1() + (half_norm_sq - Scalar{1} / Scalar{2}) * e_plus() +
           (half_norm_sq + Scalar{1} / Scalar{2}) * e_minus();
  }

  static constexpr Multivector make_point(Scalar t, Scalar x, Scalar y) {
    const Scalar half_norm_sq{(t * t + x * x + y * y) / Scalar{2}};
    return t * gamma0() + x * gamma1() + y * gamma2() +
           (half_norm_sq - Scalar{1} / Scalar{2}) * e_plus() +
           (half_norm_sq + Scalar{1} / Scalar{2}) * e_minus();
  }

  static constexpr Multivector make_point(Scalar t, Scalar x, Scalar y, Scalar z) {
    const Scalar half_norm_sq{(t * t + x * x + y * y + z * z) / Scalar{2}};
    return t * gamma0() + x * gamma1() + y * gamma2() + z * gamma3() +
           (half_norm_sq - Scalar{1} / Scalar{2}) * e_plus() +
           (half_norm_sq + Scalar{1} / Scalar{2}) * e_minus();
  }

  /**
   * Extract Euclidean coordinates from a CGA null point vector under standard normalization.
   */
  static constexpr void extract_point(const Multivector& point, Scalar& out_t) {
    const Scalar w{weight(point)};
    if (abs(w) < Algebra::EPSILON) {
      except<std::domain_error>("Cannot extract coordinates from a point at infinity (w = 0).");
    }

    out_t = get_gamma0(point) / w;
  }

  static constexpr void extract_point(const Multivector& point, Scalar& out_t, Scalar& out_x) {
    const Scalar w{weight(point)};
    if (abs(w) < Algebra::EPSILON) {
      except<std::domain_error>("Cannot extract coordinates from a point at infinity (w = 0).");
    }

    out_t = get_gamma0(point) / w;
    out_x = get_gamma1(point) / w;
  }

  static constexpr void extract_point(const Multivector& point, Scalar& out_t, Scalar& out_x,
                                      Scalar& out_y) {
    const Scalar w{weight(point)};
    if (abs(w) < Algebra::EPSILON) {
      except<std::domain_error>("Cannot extract coordinates from a point at infinity (w = 0).");
    }

    out_t = get_gamma0(point) / w;
    out_x = get_gamma1(point) / w;
    out_y = get_gamma2(point) / w;
  }

  static constexpr void extract_point(const Multivector& point, Scalar& out_t, Scalar& out_x,
                                      Scalar& out_y, Scalar& out_z) {
    const Scalar w{weight(point)};
    if (abs(w) < Algebra::EPSILON) {
      except<std::domain_error>("Cannot extract coordinates from a point at infinity (w = 0).");
    }

    out_t = get_gamma0(point) / w;
    out_x = get_gamma1(point) / w;
    out_y = get_gamma2(point) / w;
    out_z = get_gamma3(point) / w;
  }

  /**
   * In CGA a finite point is a null vector (X * ~X = 0) at grade-1 that has a nonzero inner
   * product with e_inf (nonzero homogeneous weight). The null condition distinguishes
   * points from general grade-1 elements such as spheres (which are non-null). A point
   * at infinity has a zero weight and is excluded by the weight check.
   */
  static constexpr bool is_point(const Multivector& mv) {
    if (!mv.template is_grade<1>()) {
      return false;
    }

    // The null condition X * ~X = 0 distinguishes points from spheres.
    // The square_magnitude() method calculates this value, but gives numerically unstable results.
    // In this approach, calculate the square magnitude of the physical bases and conformal bases
    // separately. The condition above is equivalent to the physical and conformal square magnitudes
    // being equal.
    // physical_mag = conformal_mag
    // where physical_mag is the square magnitude of the physical bases and conformal magnitude is
    // twice the weight of the infinity basis times the weight of the origin basis.
    // For numerical stability, we compare the ratio of these two magnitudes to one.

    // Compute the square magnitude of the physical dimensions.
    const Multivector physical_aspect{mask_conformal_bases(mv)};
    const Scalar physical_mag{physical_aspect.square_magnitude()};

    // Compute the magnitude of the conformal bases.
    const Scalar w_inf{weight(mv)};
    const Scalar conformal_mag{Scalar{2} * w_inf * weight_origin(mv)};

    const Scalar ratio{physical_mag > EPSILON ? conformal_mag / physical_mag : Scalar{1}};
    const bool is_null{(physical_mag > EPSILON) ? (abs(ratio - Scalar{1}) < EPSILON)
                                                : (abs(conformal_mag) < EPSILON)};

    // A finite point has nonzero weight — should be exactly 1 under perfect normalization.
    const bool has_weight{w_inf > Algebra::EPSILON};

    DLOG(INFO) << "is_point() -- mv: " << mv << ", physical_aspect: " << physical_aspect
               << ", ratio:" << ratio << ", physical_mag: " << physical_mag
               << ", conformal_mag: " << conformal_mag << ", is_null: " << is_null
               << ", has_weight: " << has_weight;
    return is_null && has_weight;
  }

  /**
   * Verifies that a point is normalized, which conceptually aligns the multivector
   * with a "Standard Observer" where the local scale of space and time is unity.
   * In the conformal manifold, a normalized weight of 1 ensures the point sits
   * at the "vertex" of its spacetime hyperbola, effectively setting the gauge
   * so that coordinate time matches proper time. From a Euclidean perspective,
   * this normalization factor acts as the homogeneous divisor that maps the
   * higher-dimensional conformal representation back to a unique, measurable
   * coordinate in flat 3D space. See the division by the weight in extract_point() methods.
   */
  static constexpr bool is_normalized_point(const Multivector& mv) {
    return is_point(mv) && (abs(weight(mv) - Scalar{1}) < Algebra::EPSILON);
  }

  /**
   * Constructs a multivector representing a plane in a conformal space.
   *
   * This bridges Euclidean coordinate-based intuition (normal and point) into the algebraic
   * framework of Conformal Geometric Algebra (CGA). This construction is appropriate in any
   * conformal geometry provided the underlying algebra provides enough physical dimensions.
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

    return join(join(p, spatial_bivector), e_inf());
  }

  /**
   * Constructs a multivector representing a hyperplane in a time-extended conformal space.
   *
   * This bridges higher-dimensional coordinate-based intuition into the algebraic framework
   * of Conformal Geometric Algebra (CGA). This construction is appropriate in any conformal
   * geometry provided the underlying algebra provides enough physical dimensions.
   */
  static constexpr Multivector make_plane(Scalar nt, Scalar nx, Scalar ny, Scalar nz, Scalar pt,
                                          Scalar px, Scalar py, Scalar pz)
    requires TIME_EXTENDED
  {
    const Scalar norm{hypot(nt, nx, ny, nz)};
    if (norm < Algebra::EPSILON) {
      except<std::domain_error>("Degenerate normal: cannot construct plane from null vector.");
    }

    const Scalar ut = nt / norm;
    const Scalar ux = nx / norm;
    const Scalar uy = ny / norm;
    const Scalar uz = nz / norm;

    const Multivector spatial_trivector{ut * (gamma12() * gamma3()) - ux * (gamma02() * gamma3()) +
                                        uy * (gamma01() * gamma3()) - uz * (gamma01() * gamma2())};

    const Multivector p{make_point(pt, px, py, pz)};

    return join(join(p, spatial_trivector), e_inf());
  }

  static constexpr bool is_plane(const Multivector& mv) noexcept {
    if (!mv.template is_grade<5>()) return false;

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

  static constexpr Multivector make_line(Scalar pt1, Scalar px1, Scalar py1, Scalar pz1, Scalar pt2,
                                         Scalar px2, Scalar py2, Scalar pz2) {
    const Multivector p1{make_point(pt1, px1, py1, pz1)};
    const Multivector p2{make_point(pt2, px2, py2, pz2)};

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

  static constexpr Multivector make_circle(Scalar ct, Scalar cx, Scalar cy, Scalar cz, Scalar nt,
                                           Scalar nx, Scalar ny, Scalar nz, Scalar radius)
    requires TIME_EXTENDED
  {
    const Multivector center{make_point(ct, cx, cy, cz)};

    // Create the dual sphere.
    const Multivector sphere{center - radius * radius / Scalar{2} * e_inf()};

    // Create the dual hyperplane passing through the center with the given normal.
    const Multivector plane{make_plane(nt, nx, ny, nz, ct, cx, cy, cz)};

    // The intersection of a dual sphere and a dual hyperplane is the dual circle.
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

    const Scalar exy{get_xy(motor)};
    const Scalar exz{get_xz(motor)};
    const Scalar eyz{get_yz(motor)};
    const Scalar euclidean_norm_sq{exy * exy + exz * exz + eyz * eyz};

    if (euclidean_norm_sq < Algebra::EPSILON) {
      // For pure translations, the log is simply the bivector part.
      return motor.template grade_projection<2>();
    }

    const Scalar euclidean_norm{sqrt(euclidean_norm_sq)};
    const Scalar angle{Scalar{2} * acos(scalar_part)};
    const Scalar s{sin(angle / Scalar{2})};

    const Multivector euclidean_biv{exy * gamma12() + exz * gamma13() + eyz * gamma23()};

    // The translational component of the motor lives in the bivectors involving e_inf.
    const Multivector translation_biv{motor.template grade_projection<2>() - euclidean_biv};

    return angle / euclidean_norm * euclidean_biv + Scalar{1} / s * translation_biv;
  }

  /**
   * Compute the exponential of a CGA bivector. The Euclidean part generates rotation and
   * the e_inf part generates translation. The formula parallels the PGA case but in the
   * conformal embedding — the translational generator uses e_inf rather than the ideal
   * line elements of PGA.
   */
  static Multivector motor_exp(const Multivector& bivector) {
    using std::cos, std::sin, std::sqrt;

    const Scalar exy{get_xy(bivector)};
    const Scalar exz{get_xz(bivector)};
    const Scalar eyz{get_yz(bivector)};
    const Scalar euclidean_norm_sq{exy * exy + exz * exz + eyz * eyz};

    if (euclidean_norm_sq < Algebra::EPSILON) {
      Multivector result{bivector.template grade_projection<2>()};
      result.set_scalar(Scalar{1});
      return result;
    }

    const Scalar euclidean_norm{sqrt(euclidean_norm_sq)};
    const Scalar c{cos(euclidean_norm)};
    const Scalar s{sin(euclidean_norm) / euclidean_norm};

    return s * bivector.template grade_projection<2>() + c;
  }
};

template <typename Scalar = DefaultScalarType>
using Cga2dGeometry = CgaGeometryType<2, Scalar>;

template <typename Scalar = DefaultScalarType>
using Cga3dGeometry = CgaGeometryType<3, Scalar>;

template <typename Scalar = DefaultScalarType>
using Cga4dGeometry = CgaGeometryType<4, Scalar>;

static_assert(GeometryModel<Cga2dGeometry<>>);
static_assert(GeometryModel<Cga3dGeometry<>>);
static_assert(GeometryModel<Cga4dGeometry<>>);
static_assert(ConformalGeometryModel<Cga2dGeometry<>>);
static_assert(ConformalGeometryModel<Cga3dGeometry<>>);
static_assert(ConformalGeometryModel<Cga4dGeometry<>>);

}  // namespace ndyn::math
