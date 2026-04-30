#pragma once

#include <cmath>
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
 * When extended with a time basis, the algebra is Cl(5,1).

 * Point embedding:
 *   X = e_orig + p + (1/2)|p|^2 * e_inf
 *   where p = pt * e0 + px * e1 + py * e2 + pz * e3
 *
 * This vector representation satisfies X * X = 0 for any finite Euclidean point p.
 * The e_orig coefficient is fixed at 1 under standard normalization, allowing recovery of
 * Euclidean coordinates by dividing the e0, e1, e2, e3 coefficients by the e_orig coefficient.
 *
 * Basis convention for three physical dimensions:
 *   e<0> = x, e<1> = y, e<2> = z
 *   e<3> = e_plus
 *   e<4> = e_minus
 */
template <size_t NUM_PHYSICAL_DIMENSIONS_VALUE, typename T = DefaultScalarType>
class CgaGeometryType final {
 public:
  static constexpr size_t NUM_PHYSICAL_DIMENSIONS{NUM_PHYSICAL_DIMENSIONS_VALUE};
  using G = CgaGeometryType<NUM_PHYSICAL_DIMENSIONS, T>;
  using Algebra = math::Algebra<T, NUM_PHYSICAL_DIMENSIONS + 1, 1, 0>;
  using Multivector = Algebra::VectorType;
  using Scalar = Algebra::ScalarType;

  static constexpr Scalar EPSILON{Algebra::EPSILON};

 private:
  // The null basis vectors e0 and e_inf are the conformal origin and infinity respectively.
  // They are not basis vectors of the algebra directly but are linear combinations of e_plus
  // and e_minus. These helpers keep the construction readable throughout the implementation.
  static constexpr auto e_plus() { return Multivector::template e<NUM_PHYSICAL_DIMENSIONS>(); }
  static constexpr auto e_minus() { return Multivector::template e<NUM_PHYSICAL_DIMENSIONS + 1>(); }

  // Assert that the order of the bases is as expected.
  static_assert((e_plus() * e_plus()).scalar() == Scalar{1});
  static_assert((e_minus() * e_minus()).scalar() == Scalar{-1});

  [[nodiscard]] static constexpr auto mask_conformal_bases(IsMultivectorLike<G> auto&& mv) {
    return mv.template mask_bases<NUM_PHYSICAL_DIMENSIONS, NUM_PHYSICAL_DIMENSIONS + 1>();
  }

 public:
  /**
   * Standard weight calculation.
   * The weight is a measure of the scale of the space.
   *
   * Direct mathematical calculation would yield:
   * -(mv.left_contraction(e_inf()).scalar());
   *
   * But we use an equivalent calculation based on the coefficients of e_plus and e_minus.
   */
  [[nodiscard]] static constexpr auto weight(IsMultivectorLike<G> auto&& mv) noexcept {
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
  [[nodiscard]] static constexpr auto weight_origin(IsMultivectorLike<G> auto&& mv) noexcept {
    return (mv.template coefficient<1UL << (NUM_PHYSICAL_DIMENSIONS + 1)>() +
            mv.template coefficient<1UL << NUM_PHYSICAL_DIMENSIONS>()) /
           Scalar{2};
  }

  [[nodiscard]] static constexpr auto e_inf() noexcept { return e_minus() + e_plus(); }
  [[nodiscard]] static constexpr auto e_orig() noexcept {
    return (Scalar{1} / Scalar{2}) * (e_minus() - e_plus());
  }
  static_assert((e_inf() * e_inf()).scalar() == Scalar{0});
  static_assert((e_orig() * e_orig()).scalar() == Scalar{0});
  static_assert((e_inf() * e_orig()).scalar() == Scalar{-1});
  static_assert((e_orig() * e_inf()).scalar() == Scalar{-1});

  [[nodiscard]] static constexpr auto origin() noexcept { return e_orig(); }

  // Factory methods for basis vectors under the generic names.
  [[nodiscard]] static constexpr auto e1() noexcept
    requires(NUM_PHYSICAL_DIMENSIONS >= 1)
  {
    return Multivector::template e<0>();
  }
  [[nodiscard]] static constexpr auto e2() noexcept
    requires(NUM_PHYSICAL_DIMENSIONS >= 2)
  {
    return Multivector::template e<1>();
  }
  [[nodiscard]] static constexpr auto e3() noexcept
    requires(NUM_PHYSICAL_DIMENSIONS >= 3)
  {
    return Multivector::template e<2>();
  }
  [[nodiscard]] static constexpr auto e4() noexcept
    requires(NUM_PHYSICAL_DIMENSIONS >= 4)
  {
    return Multivector::template e<3>();
  }

  // Getters for the coefficients of the basis vectors under the generic names.
  [[nodiscard]] static constexpr auto get_e1(IsMultivectorLike<G> auto&& mv) noexcept
    requires(NUM_PHYSICAL_DIMENSIONS >= 1)
  {
    return mv.template coefficient<1UL << 0>();
  }
  [[nodiscard]] static constexpr auto get_e2(IsMultivectorLike<G> auto&& mv) noexcept
    requires(NUM_PHYSICAL_DIMENSIONS >= 2)
  {
    return mv.template coefficient<1UL << 1>();
  }
  [[nodiscard]] static constexpr auto get_e3(IsMultivectorLike<G> auto&& mv) noexcept
    requires(NUM_PHYSICAL_DIMENSIONS >= 3)
  {
    return mv.template coefficient<1UL << 2>();
  }
  [[nodiscard]] static constexpr auto get_e4(IsMultivectorLike<G> auto&& mv) noexcept
    requires(NUM_PHYSICAL_DIMENSIONS >= 4)
  {
    return mv.template coefficient<1UL << 3>();
  }

  // Factory methods for physical basis vectors using names more specific to conformal geometry.
  [[nodiscard]] static constexpr auto gamma0() noexcept
    requires(NUM_PHYSICAL_DIMENSIONS >= 1)
  {
    static_assert((e1() * e1()).scalar() == Scalar{1});
    return e1();
  }
  [[nodiscard]] static constexpr auto gamma1() noexcept
    requires(NUM_PHYSICAL_DIMENSIONS >= 2)
  {
    static_assert((e2() * e2()).scalar() == Scalar{1});
    return e2();
  }
  [[nodiscard]] static constexpr auto gamma2() noexcept
    requires(NUM_PHYSICAL_DIMENSIONS >= 3)
  {
    static_assert((e3() * e3()).scalar() == Scalar{1});
    return e3();
  }
  [[nodiscard]] static constexpr auto gamma3() noexcept
    requires(NUM_PHYSICAL_DIMENSIONS >= 4)
  {
    static_assert((e4() * e4()).scalar() == Scalar{1});
    return e4();
  }

  // Getters for the coefficients of the basis vectors under the generic names.
  [[nodiscard]] static constexpr auto get_gamma0(IsMultivectorLike<G> auto&& mv) noexcept
    requires(NUM_PHYSICAL_DIMENSIONS >= 1)
  {
    return get_e1(mv);
  }
  [[nodiscard]] static constexpr auto get_gamma1(IsMultivectorLike<G> auto&& mv) noexcept
    requires(NUM_PHYSICAL_DIMENSIONS >= 2)
  {
    return get_e2(mv);
  }
  [[nodiscard]] static constexpr auto get_gamma2(IsMultivectorLike<G> auto&& mv) noexcept
    requires(NUM_PHYSICAL_DIMENSIONS >= 3)
  {
    return get_e3(mv);
  }
  [[nodiscard]] static constexpr auto get_gamma3(IsMultivectorLike<G> auto&& mv) noexcept
    requires(NUM_PHYSICAL_DIMENSIONS >= 4)
  {
    return get_e4(mv);
  }

  // Factory methods for basis bivectors.
  [[nodiscard]] static constexpr auto gamma01() noexcept { return gamma0() * gamma1(); }
  [[nodiscard]] static constexpr auto gamma02() noexcept { return gamma0() * gamma2(); }
  [[nodiscard]] static constexpr auto gamma03() noexcept { return gamma0() * gamma3(); }
  [[nodiscard]] static constexpr auto gamma12() noexcept { return gamma1() * gamma2(); }
  [[nodiscard]] static constexpr auto gamma13() noexcept { return gamma1() * gamma3(); }
  [[nodiscard]] static constexpr auto gamma23() noexcept { return gamma2() * gamma3(); }

  /**
   * Construct a rotor around the origin of the plane. Note that this is only meaningful with two
   * physical dimensions.
   */
  [[nodiscard]] static auto make_rotor(ScalarLike<G> auto&& angle) noexcept
    requires(NUM_PHYSICAL_DIMENSIONS == 2)
  {
    using std::cos, std::sin;

    const auto half_angle{angle / Scalar{2}};
    static constexpr const auto rotation_plane{gamma01()};
    return cos(half_angle) - sin(half_angle) * rotation_plane;
  }

  /**
   * Construct a rotor around an axis through the origin. Note that this is only meaningful with
   * three physical dimensions. In higher dimensions, a single axis does not uniquely determine a
   * rotation.
   */
  [[nodiscard]] static auto make_rotor(ScalarLike<G> auto&& nx, ScalarLike<G> auto&& ny,
                                       ScalarLike<G> auto&& nz, ScalarLike<G> auto&& angle)
    requires(NUM_PHYSICAL_DIMENSIONS == 3)
  {
    using std::cos, std::sin;

    const auto norm{hypot(nx, ny, nz)};
    if (norm < Algebra::EPSILON) {
      except<std::domain_error>("Degenerate axis: cannot construct rotor from a null axis.");
    }

    const auto half_angle{angle / Scalar{2}};
    const auto rotation_plane{nx / norm * gamma12() - ny / norm * gamma02() +
                              nz / norm * gamma01()};
    return cos(half_angle) - sin(half_angle) * rotation_plane;
  }

  /**
   * Construct a CGA rotor in the given plane to rotate by angle radians. The plane of rotation is
   * given by its normal.
   */
  [[nodiscard]] static auto make_rotor(IsMultivectorLike<G> auto&& rotation_plane,
                                       ScalarLike<G> auto&& angle) noexcept {
    if (!rotation_plane.is_grade(2)) {
      except<std::invalid_argument>(
          "In all geometric algebras, versors, such as rotors, should be bivectors.");
    }

    using std::cos, std::sin;

    const auto half_angle{angle / Scalar{2}};
    return cos(half_angle) - sin(half_angle) * rotation_plane.normalize();
  }

  static void extract_rotor(IsMultivectorLike<G> auto&& mv, ScalarLike<G> auto&& angle) noexcept {
    using std::acos;

    angle = Scalar{2} * acos(mv.scalar());
  }

  static void extract_rotor(IsMultivectorLike<G> auto&& mv, ScalarLike<G> auto&& nx,
                            ScalarLike<G> auto&& ny, ScalarLike<G> auto&& nz,
                            ScalarLike<G> auto&& angle) noexcept
    requires(NUM_PHYSICAL_DIMENSIONS >= 3)
  {
    using std::acos, std::sin;
    // A normalized rotor has the form R = cos(θ/2) - sin(θ/2)(nx e01 + ny e02 + nz e12).
    // The scalar part is cos(θ/2) and the bivector part has magnitude sin(θ/2).
    // Recover the full rotation angle from the scalar component.
    const auto half_angle = acos(mv.scalar());
    angle = half_angle * Scalar{2};

    // sin(θ/2) is the magnitude of the bivector part. If it is zero the rotor
    // is the identity and the axis is undefined; return a canonical axis.
    const auto sin_half_angle = sin(half_angle);
    if (sin_half_angle == Scalar{0}) {
      nx = Scalar{0};
      ny = Scalar{0};
      nz = Scalar{1};
    } else {
      // Extract the axis by dividing out sin(θ/2) from each bivector component.
      // The sign convention matches make_rotor(): R = cos(θ/2) - sin(θ/2)(n * B).
      const auto inv_sin = Scalar{1} / sin_half_angle;
      nx = -get_gamma01(mv) * inv_sin;
      ny = -get_gamma02(mv) * inv_sin;
      nz = -get_gamma12(mv) * inv_sin;
    }
  }

  [[nodiscard]] static constexpr auto is_rotor(IsMultivectorLike<G> auto&& mv) noexcept {
    // A normalized rotor has the form R = cos(θ/2) - sin(θ/2) * rotation_plane. The scalar part is
    // cos(θ/2) and the rotation_plane (a bivector) has magnitude sin(θ/2). For this method, we
    // check the value of the scalar is in the range of the cos() function. We check that the
    // multivector is in the form of a bivector plus a scalar. We verify that the square magnitude
    // of the bivector plus the square of the scalar is one.
    const auto scalar_value{mv.scalar()};
    if (abs(scalar_value) - Scalar{1} > EPSILON) {
      return false;
    }

    const auto bivector{mv.template grade_projection<2>()};
    if (!mv.near_equal(bivector + scalar_value)) {
      return false;
    }

    return abs(bivector.square_magnitude() + scalar_value * scalar_value - Scalar{1}) < EPSILON;
  }

  /**
   * Constructs a dilator (scaling versor) centered at the origin.
   * A dilator scales space by the given factor.
   */
  [[nodiscard]] static constexpr auto make_dilator(ScalarLike<G> auto&& scale) noexcept {
    using std::log, std::cosh, std::sinh;

    if (scale <= Scalar{0}) {
      except<std::domain_error>("Scale factor must be positive.");
    }

    // The generator of dilation at the origin is the Minkowski plane E = e_inf ^ e_orig.
    // In our basis, E = e_inf ^ e_orig() has the property E^2 = 1.
    constexpr auto E{e_inf() ^ e_orig()};
    static_assert(abs((E * E).scalar() - Scalar{1}) < EPSILON);

    // For a scale factor k, the dilation is exp(ln(k)/2 * E).
    // Using the identity exp(phi * E) = cosh(phi) + E * sinh(phi) for E^2 = 1.
    const auto phi{log(scale) / Scalar{2}};
    return cosh(phi) + sinh(phi) * E;
  }

  static constexpr void extract_dilator(IsMultivectorLike<G> auto&& mv,
                                        ScalarLike<G> auto&& scale) noexcept {
    // Transform the 'origin' and see how its weight changes.
    // In CGA, D * e_orig * ~D = (1/scale) * e_orig, where D is a dilator.
    const auto transformed_null_basis{mv * e_orig() * mv.reverse()};

    // The scale is the inverse of the resulting e_inf coefficient.
    scale = Scalar{1} / weight(transformed_null_basis);
  }

  [[nodiscard]] static constexpr auto is_dilator(IsMultivectorLike<G> auto&& mv) noexcept {
    // Transform the 'origin' and see how its weight changes.
    // In CGA, D * e_orig * ~D = (1/scale) * e_orig, where D is a dilator.
    const auto transformed_null_basis{mv * e_orig() * mv.reverse()};

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
  [[nodiscard]] static constexpr auto make_translator(ScalarLike<G> auto&& dt,
                                                      ScalarLike<G> auto&& dx,
                                                      ScalarLike<G> auto&& dy,
                                                      ScalarLike<G> auto&& dz) noexcept {
    const auto displacement{dt * gamma0() + dx * gamma1() + dy * gamma2() + dz * gamma3()};
    return Scalar{1} - displacement * e_inf() / Scalar{2};
  }

  [[nodiscard]] static constexpr auto make_translator(ScalarLike<G> auto&& dt,
                                                      ScalarLike<G> auto&& dx,
                                                      ScalarLike<G> auto&& dy) noexcept {
    const auto displacement{dt * gamma0() + dx * gamma1() + dy * gamma2()};
    return Scalar{1} - displacement * e_inf() / Scalar{2};
  }

  [[nodiscard]] static constexpr auto make_translator(ScalarLike<G> auto&& dt,
                                                      ScalarLike<G> auto&& dx) noexcept {
    const auto displacement{dt * gamma0() + dx * gamma1()};
    return Scalar{1} - displacement * e_inf() / Scalar{2};
  }

  [[nodiscard]] static constexpr auto make_translator(ScalarLike<G> auto&& dt) noexcept {
    const auto displacement{dt * gamma0()};
    return Scalar{1} - displacement * e_inf() / Scalar{2};
  }

  [[nodiscard]] static constexpr auto make_translator(IsMultivectorLike<G> auto&& direction,
                                                      ScalarLike<G> auto&& dt) noexcept {
    const auto displacement{dt * direction};
    return Scalar{1} - displacement * e_inf() / Scalar{2};
  }

  static constexpr void extract_translator(IsMultivectorLike<G> auto&& mv, Scalar& dt, Scalar& dx,
                                           Scalar& dy, Scalar& dz) noexcept {
    dt = get_gamm0_e_minus(mv);
    dx = get_gamm1_e_minus(mv);
    dy = get_gamm2_e_minus(mv);
    dz = get_gamm3_e_minus(mv);
  }

  static constexpr void extract_translator(IsMultivectorLike<G> auto&& mv, Scalar& dt, Scalar& dx,
                                           Scalar& dy) noexcept {
    dt = get_gamm0_e_minus(mv);
    dx = get_gamm1_e_minus(mv);
    dy = get_gamm2_e_minus(mv);
  }

  static constexpr void extract_translator(IsMultivectorLike<G> auto&& mv, Scalar& dt,
                                           Scalar& dx) noexcept {
    dt = get_gamm0_e_minus(mv);
    dx = get_gamm1_e_minus(mv);
  }

  static constexpr void extract_translator(IsMultivectorLike<G> auto&& mv, Scalar& dt) noexcept {
    dt = get_gamm0_e_minus(mv);
  }

  [[nodiscard]] static constexpr auto is_translator(IsMultivectorLike<G> auto&& mv) noexcept {
    const auto scalar_value{mv.scalar()};
    if (abs(scalar_value - Scalar{1}) > EPSILON) {
      return false;
    }

    const auto bivector{mv.template grade_projection<2>()};
    if (!bivector.near_equal(mv - scalar_value)) {
      return false;
    }

    return (bivector * e_orig()).template is_grade<1>();
  }

  /**
   * The join is the operation that determines the smallest common subspace containing two
   * or more geometric entities. Its algebraic implementation depends entirely on the representation
   * of the geometry: the Outer Product Null Space (OPNS) or the Inner Product Null Space
   * (IPNS). In OPNS, the join of two disjoint objects is their outer product,
   * which builds higher-grade blades from lower-grade ones. Conversely, in IPNS, where objects are
   * represented by their duals (the "normal" or "null" space they exclude), the join is performed
   * via the regressive product. While the outer product expands the "span," the regressive
   * product effectively performs a "dual-meet," allowing the algebra to resolve the union of
   * constraints.
   *
   * In the OPNS framework, an object is defined by the points it contains. Here, the identity
   * element for the join is the scalar 1, representing the "empty set" or a vacuous lack of
   * constraint. Joining three points in OPNS creates a circle (a round), as the wedge product
   * captures the unique 3-blade defining that circular path. To create a "flat" such as a
   * plane, one would join three points with the point at infinity (e_inf). In this mode,
   * the join is constructive and grade-increasing: it takes the "nothingness" of the scalar
   * identity and adds dimensions of containment.
   *
   * In the IPNS framework, the logic is inverted: an object is defined by its relationship to
   * the vectors normal to it. Here, the identity element for the join is the pseudoscalar
   * (I), representing the "total space." In IPNS, the join is a grade-decreasing operation
   * (using the regressive product) that removes constraints. While an OPNS join adds "what is
   * there," an IPNS join removes the "limitations" of what is not. This distinction is vital; the
   * scalar 1 is the "nothing" from which we build (Join in OPNS), while the pseudoscalar I is the
   * "everything" from which we carve (Meet in OPNS).
   *
   * This dual nature allows the join to function as the operation of an algebraic monoid
   * structure. (A monoid can be thought of as a group without inverse elements.) By treating the
   * join as a formal mapping within the Grassmannian, the algebra distinguishes between the linear
   * span (the flat or round extent) and the metric properties (distance and angle). The choice
   * between an outer product join and a regressive product join determines whether the join
   * aggregates points into a body (OPNS) or merges the bounding constraints of two separate
   * manifolds (IPNS).
   */
  [[nodiscard]] static constexpr auto join() noexcept {
    // Note this result is for an OPNS model. An IPNS model would return the pseudoscalar.
    return Multivector{Scalar{1}};
  }

  template <IsMultivectorLike<G> First, IsMultivectorLike<G>... Rest>
  [[nodiscard]] static constexpr auto join(First&& first, Rest&&... rest) noexcept {
    if constexpr (sizeof...(rest) == 0) {
      return std::forward<First>(first);
    } else {
      // The join in OPNS is the outer product. The ^ operator is overloaded on the Multivector
      // class to perform the outer product.
      return (std::forward<First>(first) ^ ... ^ std::forward<Rest>(rest));
    }
  }

  [[nodiscard]] static constexpr auto meet() noexcept {
    // Note this result is for an OPNS model. An IPNS model would return the scalar 1.
    return Multivector::pseudoscalar();
  }

  template <IsMultivectorLike<G> First, IsMultivectorLike<G>... Rest>
  [[nodiscard]] static constexpr auto meet(First&& first, Rest&&... rest) noexcept {
    if constexpr (sizeof...(rest) == 0) {
      return std::forward<First>(first);
    } else {
      // The meet in OPNS is the regressive product. The & operator is overloaded on the Multivector
      // class to perform the regressive product.
      return (std::forward<First>(first) & ... & std::forward<Rest>(rest));
    }
  }

  template <IsMultivectorLike<G>... Points>
  [[nodiscard]] static constexpr auto make_round(Points&&... points) noexcept {
    return join(std::forward<Points>(points)...);
  }

  template <IsMultivectorLike<G>... Points>
  [[nodiscard]] static constexpr auto make_flat(Points&&... points) noexcept {
    return join(e_inf(), std::forward<Points>(points)...);
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
  [[nodiscard]] static constexpr auto make_point(ScalarLike<G> auto&& t) noexcept {
    const auto half_norm_sq{(t * t) / Scalar{2}};
    return t * gamma0() + (half_norm_sq - Scalar{1} / Scalar{2}) * e_plus() +
           (half_norm_sq + Scalar{1} / Scalar{2}) * e_minus();
  }

  [[nodiscard]] static constexpr auto make_point(ScalarLike<G> auto&& t, ScalarLike<G> auto&& x) {
    const auto half_norm_sq{(t * t + x * x) / Scalar{2}};
    return t * gamma0() + x * gamma1() + (half_norm_sq - Scalar{1} / Scalar{2}) * e_plus() +
           (half_norm_sq + Scalar{1} / Scalar{2}) * e_minus();
  }

  [[nodiscard]] static constexpr auto make_point(ScalarLike<G> auto&& t, ScalarLike<G> auto&& x,
                                                 ScalarLike<G> auto&& y) {
    const auto half_norm_sq{(t * t + x * x + y * y) / Scalar{2}};
    return t * gamma0() + x * gamma1() + y * gamma2() +
           (half_norm_sq - Scalar{1} / Scalar{2}) * e_plus() +
           (half_norm_sq + Scalar{1} / Scalar{2}) * e_minus();
  }

  [[nodiscard]] static constexpr auto make_point(ScalarLike<G> auto&& t, ScalarLike<G> auto&& x,
                                                 ScalarLike<G> auto&& y,
                                                 ScalarLike<G> auto&& z) noexcept {
    const auto half_norm_sq{(t * t + x * x + y * y + z * z) / Scalar{2}};
    return t * gamma0() + x * gamma1() + y * gamma2() + z * gamma3() +
           (half_norm_sq - Scalar{1} / Scalar{2}) * e_plus() +
           (half_norm_sq + Scalar{1} / Scalar{2}) * e_minus();
  }

  [[nodiscard]] static constexpr auto make_point(size_t count,
                                                 const ScalarLike<G> auto* values) noexcept {
    Multivector result{};
    Scalar norm_sq{};
    for (size_t i = 0; i < count and i < NUM_PHYSICAL_DIMENSIONS; ++i) {
      norm_sq += values[i] * values[i];
      result += values[i] * Multivector::e(i);
    }

    result += ((norm_sq - Scalar{1}) * e_plus() + (norm_sq + Scalar{1}) * e_minus()) / Scalar{2};
    return result;
  }

  /**
   * Extract Euclidean coordinates from a CGA null point vector under standard normalization.
   */
  static constexpr void extract_point(IsMultivectorLike<G> auto&& point, Scalar& out_t) noexcept {
    const auto w{weight(point)};
    if (abs(w) < Algebra::EPSILON) {
      except<std::domain_error>("Cannot extract coordinates from a point at infinity (w = 0).");
    }

    out_t = get_gamma0(point) / w;
  }

  static constexpr void extract_point(IsMultivectorLike<G> auto&& point, Scalar& out_t,
                                      Scalar& out_x) noexcept {
    const auto w{weight(point)};
    if (abs(w) < Algebra::EPSILON) {
      except<std::domain_error>("Cannot extract coordinates from a point at infinity (w = 0).");
    }

    out_t = get_gamma0(point) / w;
    out_x = get_gamma1(point) / w;
  }

  static constexpr void extract_point(IsMultivectorLike<G> auto&& point, Scalar& out_t,
                                      Scalar& out_x, Scalar& out_y) noexcept {
    const auto w{weight(point)};
    if (abs(w) < Algebra::EPSILON) {
      except<std::domain_error>("Cannot extract coordinates from a point at infinity (w = 0).");
    }

    out_t = get_gamma0(point) / w;
    out_x = get_gamma1(point) / w;
    out_y = get_gamma2(point) / w;
  }

  static constexpr void extract_point(IsMultivectorLike<G> auto&& point, Scalar& out_t,
                                      Scalar& out_x, Scalar& out_y, Scalar& out_z) noexcept {
    const auto w{weight(point)};
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
  [[nodiscard]] static auto is_point(IsMultivectorLike<G> auto&& mv) noexcept {
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
    const auto physical_aspect{mask_conformal_bases(mv)};
    const auto physical_mag{physical_aspect.square_magnitude()};

    // Compute the magnitude of the conformal bases.
    const auto w_inf{weight(mv)};
    const auto w_orig{weight_origin(mv)};
    const auto conformal_mag{Scalar{2} * w_inf * w_orig};

    const auto ratio{physical_mag > EPSILON ? conformal_mag / physical_mag : Scalar{1}};
    // const bool is_null{(physical_mag > EPSILON) ? (abs(ratio - Scalar{1}) < EPSILON)
    //                                             : (abs(conformal_mag) < EPSILON)};
    bool is_null;

    if (physical_mag > EPSILON) {
      // TODO(james): Fix the numeric instability and remove the multiple of the acceptable
      // inaccuracy.
      is_null = abs(ratio - Scalar{1}) < 100 * EPSILON;
      DLOG(INFO) << "primary -- is_null: " << is_null << ", ratio: " << ratio
                 << ", ratio - Scalar{1}: " << (ratio - Scalar{1});
    } else {
      is_null = abs(conformal_mag) < EPSILON;
      DLOG(INFO) << "secondary -- is_null: " << is_null;
    }

    // A finite point has nonzero weight — should be exactly 1 under perfect normalization.
    const auto has_weight{abs(w_inf) > Algebra::EPSILON};

    DLOG(INFO) << "is_point() -- mv: " << mv << ", physical_aspect: " << physical_aspect
               << ", ratio:" << ratio << ", physical_mag: " << physical_mag
               << ", conformal_mag: " << conformal_mag << ", w_inf: " << w_inf
               << ", w_orig: " << w_orig << ", is_null: " << is_null
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
  [[nodiscard]] static constexpr auto is_normalized_point(IsMultivectorLike<G> auto&& mv) noexcept {
    return is_point(mv) && (abs(weight(mv) - Scalar{1}) < Algebra::EPSILON);
  }

  /**
   * In OPNS, a point pair is a pure grade-2 object.
   *
   * A bivector B can only be a blade if B ^ B = 0. This is the Plücker Relation.
   *
   * A bivector is a simple blade (and thus a geometric primitive) only if its square is a positive
   * scalar.
   * Real point pairs in CGA square to a positive value.
   * Negative squares represent rotations/circles; zero represents tangents.
   */
  [[nodiscard]] static constexpr auto is_point_pair(IsMultivectorLike<G> auto&& mv) noexcept {
    const auto normalized{mv / mv.square_magnitude()};
    const auto is_grade_2{normalized.template is_grade<2>()};
    const auto square{normalized * normalized};
    const auto square_is_grade_0{square.template is_grade<0>()};
    const auto square_scalar_is_positive{[&square, &mv]() {
      if (square.scalar() > EPSILON) {
        return true;
      } else {
        const auto unnormalized_square{mv * mv};
        return unnormalized_square.scalar() > EPSILON;
      }
    }()};
    const auto square_is_nearly_grade_0{[&square, square_is_grade_0]() {
      if (!square_is_grade_0) {
        const auto adjusted_square{square / square.scalar()};
        return adjusted_square.template is_grade<0>();
      } else {
        return true;
      }
    }()};
    DLOG(INFO) << "is_point_pair() -- mv: " << mv << ", normalized: " << normalized
               << ", is_grade_2: " << is_grade_2 << ", square: " << square
               << ", square_is_grade_0: " << square_is_grade_0
               << ", square_is_nearly_grade_0: " << square_is_nearly_grade_0
               << ", square.scalar(): " << square.scalar()
               << ", square_scalar_is_positive: " << square_scalar_is_positive;

    return is_grade_2 and square_is_nearly_grade_0 and square_scalar_is_positive;
  }

  [[nodiscard]] static constexpr auto make_point_pair(IsMultivectorLike<G> auto&& p1,
                                                      IsMultivectorLike<G> auto&& p2) noexcept {
    return join(p1, p2);
  }

  [[nodiscard]] static constexpr auto make_point_pair(ScalarLike<G> auto&& t1,
                                                      ScalarLike<G> auto&& x1,
                                                      ScalarLike<G> auto&& t2,
                                                      ScalarLike<G> auto&& x2) noexcept
    requires(NUM_PHYSICAL_DIMENSIONS == 2)
  {
    const auto p1{make_point(t1, x1)};
    const auto p2{make_point(t2, x2)};
    return make_point_pair(p1, p2);
  }

  [[nodiscard]] static constexpr auto make_point_pair(
      ScalarLike<G> auto&& t1, ScalarLike<G> auto&& x1, ScalarLike<G> auto&& y1,
      ScalarLike<G> auto&& t2, ScalarLike<G> auto&& x2, ScalarLike<G> auto&& y2) noexcept
    requires(NUM_PHYSICAL_DIMENSIONS == 3)
  {
    const auto p1{make_point(t1, x1, y1)};
    const auto p2{make_point(t2, x2, y2)};
    return make_point_pair(p1, p2);
  }

  [[nodiscard]] static constexpr auto make_point_pair(
      ScalarLike<G> auto&& t1, ScalarLike<G> auto&& x1, ScalarLike<G> auto&& y1,
      ScalarLike<G> auto&& z1, ScalarLike<G> auto&& t2, ScalarLike<G> auto&& x2,
      ScalarLike<G> auto&& y2, ScalarLike<G> auto&& z2) noexcept
    requires(NUM_PHYSICAL_DIMENSIONS == 4)
  {
    const auto p1{make_point(t1, x1, y1, z1)};
    const auto p2{make_point(t2, x2, y2, z2)};
    return make_point_pair(p1, p2);
  }

  static constexpr void extract_point_pair(IsMultivectorLike<G> auto&& point_pair, Multivector& p1,
                                           Multivector& p2) noexcept {
    using std::sqrt;

    const auto magnitude{sqrt(abs(point_pair.square_magnitude()))};

    const auto dir{e_inf().left_contraction(point_pair)};
    const auto dir_magnitude{dir.square_magnitude()};
    DLOG(INFO) << "extract_point_pair() -- point_pair: " << point_pair
               << ", dir_magnitude: " << dir_magnitude << ", magnitude: " << magnitude
               << ", dir: " << dir;
    if (abs(dir_magnitude) > EPSILON) {
      p1 = (point_pair + magnitude) * dir;
      p2 = (point_pair - magnitude) * dir;

      // Scale the points so that they align to the null cone (e_inf . P = -1)
      // This step also suppresses some numeric instabilities whereby some non-zero coefficients
      // leak to some basis blades of other grades. Without this normalization, these points are
      // likely to have coefficients in higher grade bases and appear to not be points.
      DLOG(INFO) << "extract_point_pair() -- weight(p1): " << weight(p1);
      DLOG(INFO) << "extract_point_pair() -- weight(p2): " << weight(p2);
      p1 = -p1 / weight(p1);
      p2 = -p2 / weight(p2);
    } else {
      // One point is e_inf, the other is derived from the remaining gradient.
      // For a line dual, the 'finite' part is found by contracting
      // out the Minkowski plane component.
      p1 = (point_pair.dual()).left_contraction(e_orig());

      // Normalize if it's a finite point
      const auto w{weight(p1)};
      if (abs(w) > EPSILON) {
        p1 = p1 / w;
      }

      p2 = e_inf();
    }
  }

  static constexpr void extract_point_pair(IsMultivectorLike<G> auto&& pair, Scalar& pt1,
                                           Scalar& px1, Scalar& pt2, Scalar& px2) noexcept
    requires(NUM_PHYSICAL_DIMENSIONS == 2)
  {
    Multivector p1{};
    Multivector p2{};
    extract_point_pair(pair, p1, p2);
    extract_point(p1, pt1, px1);
    extract_point(p2, pt2, px2);
  }

  static constexpr void extract_point_pair(IsMultivectorLike<G> auto&& pair, Scalar& pt1,
                                           Scalar& px1, Scalar& py1, Scalar& pt2, Scalar& px2,
                                           Scalar& py2) noexcept
    requires(NUM_PHYSICAL_DIMENSIONS == 3)
  {
    Multivector p1{};
    Multivector p2{};
    extract_point_pair(pair, p1, p2);
    extract_point(p1, pt1, px1, py1);
    extract_point(p2, pt2, px2, py2);
  }

  static constexpr void extract_point_pair(IsMultivectorLike<G> auto&& pair, Scalar& pt1,
                                           Scalar& px1, Scalar& py1, Scalar& pz1, Scalar& pt2,
                                           Scalar& px2, Scalar& py2, Scalar& pz2) noexcept
    requires(NUM_PHYSICAL_DIMENSIONS == 4)
  {
    Multivector p1{};
    Multivector p2{};
    extract_point_pair(pair, p1, p2);
    extract_point(p1, pt1, px1, py1, pz1);
    extract_point(p2, pt2, px2, py2, pz2);
  }
  /**
   * Constructs a line passing through two Euclidean points.
   * In conformal geometries, a flat line is the wedge of two conformal points and the point at
   * infinity.
   */
  [[nodiscard]] static constexpr auto make_line(IsMultivectorLike<G> auto&& p1,
                                                IsMultivectorLike<G> auto&& p2) {
    return join(join(p1, p2), e_inf());
  }

  [[nodiscard]] static constexpr auto make_line(Scalar pt1, Scalar px1, Scalar pt2, Scalar px2) {
    const auto p1{make_point(pt1, px1)};
    const auto p2{make_point(pt2, px2)};

    return make_line(p1, p2);
  }

  [[nodiscard]] static constexpr auto make_line(Scalar pt1, Scalar px1, Scalar py1, Scalar pt2,
                                                Scalar px2, Scalar py2) noexcept
    requires(NUM_PHYSICAL_DIMENSIONS == 3)
  {
    const auto p1{make_point(pt1, px1, py1)};
    const auto p2{make_point(pt2, px2, py2)};

    return make_line(p1, p2);
  }

  [[nodiscard]] static constexpr auto make_line(Scalar pt1, Scalar px1, Scalar py1, Scalar pz1,
                                                Scalar pt2, Scalar px2, Scalar py2,
                                                Scalar pz2) noexcept
    requires(NUM_PHYSICAL_DIMENSIONS == 4)
  {
    const auto p1{make_point(pt1, px1, py1, pz1)};
    const auto p2{make_point(pt2, px2, py2, pz2)};

    return make_line(p1, p2);
  }

  static constexpr void extract_line(IsMultivectorLike<G> auto&& line, Scalar& pt1, Scalar& px1,
                                     Scalar& pt2, Scalar& px2) noexcept
    requires(NUM_PHYSICAL_DIMENSIONS == 2)
  {
    using std::clamp;

    constexpr Scalar WEIGHT_THRESHOLD{1};
    Scalar first_weight{};
    Multivector first_point{};
    Scalar second_weight{};
    Multivector second_point{};
    Scalar radius{1};
    while (radius < std::numeric_limits<Scalar>::max() and abs(second_weight) < WEIGHT_THRESHOLD) {
      const auto circle{make_circle(Scalar{0}, Scalar{0}, radius)};
      const auto intersection{meet(line, circle)};
      Multivector intersection_point_1{};
      Multivector intersection_point_2{};
      extract_point_pair(intersection, intersection_point_1, intersection_point_2);
      for (const auto& p : {intersection_point_1, intersection_point_2}) {
        const auto w{weight(p)};
        if (abs(w) > abs(first_weight)) {
          second_weight = first_weight;
          second_point = first_point;
          first_weight = w;
          first_point = p;
        } else if (abs(w) > abs(second_weight)) {
          second_weight = w;
          second_point = p;
        }
      }

      radius *= Scalar{2};
    }

    if (abs(first_weight) > EPSILON) {
      pt1 = get_gamma0(first_point) / first_weight;
      px1 = get_gamma1(first_point) / first_weight;
    } else {
      DLOG(INFO) << "extract_line() -- No first point found.";
      pt1 = std::numeric_limits<Scalar>::max();
      px1 = std::numeric_limits<Scalar>::max();
    }
    if (abs(second_weight) > EPSILON) {
      pt2 = get_gamma0(second_point) / second_weight;
      px2 = get_gamma1(second_point) / second_weight;
    } else {
      DLOG(INFO) << "extract_line() -- No second point found.";
      pt2 = std::numeric_limits<Scalar>::max();
      px2 = std::numeric_limits<Scalar>::max();
    }
  }

  static constexpr void extract_line(IsMultivectorLike<G> auto&& line, Scalar& pt1, Scalar& px1,
                                     Scalar& py1, Scalar& pt2, Scalar& px2, Scalar& py2) noexcept
    requires(NUM_PHYSICAL_DIMENSIONS == 3)
  {
    using std::clamp;

    constexpr Scalar WEIGHT_THRESHOLD{1};
    Scalar first_weight{};
    Multivector first_point{};
    Scalar second_weight{};
    Multivector second_point{};
    Scalar radius{1};
    while (radius < std::numeric_limits<Scalar>::max() and abs(second_weight) < WEIGHT_THRESHOLD) {
      const auto sphere{make_sphere(Scalar{0}, Scalar{0}, Scalar{0}, radius)};
      const auto intersection{meet(line, sphere)};
      Multivector intersection_point_1{};
      Multivector intersection_point_2{};
      extract_point_pair(intersection, intersection_point_1, intersection_point_2);
      for (const auto& p : {intersection_point_1, intersection_point_2}) {
        const auto w{weight(p)};
        if (abs(w) > abs(first_weight)) {
          second_weight = first_weight;
          second_point = first_point;
          first_weight = w;
          first_point = p;
        } else if (abs(w) > abs(second_weight)) {
          second_weight = w;
          second_point = p;
        }
      }

      radius *= Scalar{2};
    }

    if (abs(first_weight) > EPSILON) {
      pt1 = get_gamma0(first_point) / first_weight;
      px1 = get_gamma1(first_point) / first_weight;
      py1 = get_gamma2(first_point) / first_weight;
    } else {
      DLOG(INFO) << "extract_line() -- No first point found.";
      pt1 = std::numeric_limits<Scalar>::max();
      px1 = std::numeric_limits<Scalar>::max();
      py1 = std::numeric_limits<Scalar>::max();
    }
    if (abs(second_weight) > EPSILON) {
      pt2 = get_gamma0(second_point) / second_weight;
      px2 = get_gamma1(second_point) / second_weight;
      py2 = get_gamma2(second_point) / second_weight;
    } else {
      DLOG(INFO) << "extract_line() -- No second point found.";
      pt2 = std::numeric_limits<Scalar>::max();
      px2 = std::numeric_limits<Scalar>::max();
      py2 = std::numeric_limits<Scalar>::max();
    }
  }

  static constexpr void extract_line(IsMultivectorLike<G> auto&& line, Scalar& pt1, Scalar& px1,
                                     Scalar& py1, Scalar& pz1, Scalar& pt2, Scalar& px2,
                                     Scalar& py2, Scalar& pz2) noexcept
    requires(NUM_PHYSICAL_DIMENSIONS == 4)
  {
    using std::clamp;

    constexpr Scalar WEIGHT_THRESHOLD{1};
    Scalar first_weight{};
    Multivector first_point{};
    Scalar second_weight{};
    Multivector second_point{};
    Scalar radius{1};
    while (radius < std::numeric_limits<Scalar>::max() and abs(second_weight) < WEIGHT_THRESHOLD) {
      const auto hypersphere{make_hypersphere(Scalar{0}, Scalar{0}, Scalar{0}, Scalar{0}, radius)};
      const auto intersection{meet(line, hypersphere)};
      Multivector intersection_point_1{};
      Multivector intersection_point_2{};
      extract_point_pair(intersection, intersection_point_1, intersection_point_2);
      for (const auto& p : {intersection_point_1, intersection_point_2}) {
        const auto w{weight(p)};
        if (abs(w) > abs(first_weight)) {
          second_weight = first_weight;
          second_point = first_point;
          first_weight = w;
          first_point = p;
        } else if (abs(w) > abs(second_weight)) {
          second_weight = w;
          second_point = p;
        }
      }

      radius *= Scalar{2};
    }

    if (abs(first_weight) > EPSILON) {
      pt1 = get_gamma0(first_point) / first_weight;
      px1 = get_gamma1(first_point) / first_weight;
      py1 = get_gamma2(first_point) / first_weight;
      pz1 = get_gamma3(first_point) / first_weight;
    } else {
      DLOG(INFO) << "extract_line() -- No first point found.";
      pt1 = std::numeric_limits<Scalar>::max();
      px1 = std::numeric_limits<Scalar>::max();
      py1 = std::numeric_limits<Scalar>::max();
      pz1 = std::numeric_limits<Scalar>::max();
    }
    if (abs(second_weight) > EPSILON) {
      pt2 = get_gamma0(second_point) / second_weight;
      px2 = get_gamma1(second_point) / second_weight;
      py2 = get_gamma2(second_point) / second_weight;
      pz2 = get_gamma3(second_point) / second_weight;
    } else {
      DLOG(INFO) << "extract_line() -- No second point found.";
      pt2 = std::numeric_limits<Scalar>::max();
      px2 = std::numeric_limits<Scalar>::max();
      py2 = std::numeric_limits<Scalar>::max();
      pz2 = std::numeric_limits<Scalar>::max();
    }
  }

  /**
   * In conformal geometric algebras, a line is a grade-3 element that is null (L * ~L = 0) and
   * contains e_inf as a factor (it is the outer product of two points and e_inf, or
   * equivalently passes through the point at infinity). The null condition distinguishes lines
   * from circles, which are also grade-3 but non-null and without conformal basis blades.
   */
  static constexpr bool is_line(IsMultivectorLike<G> auto&& mv) noexcept {
    const auto is_grade_3{mv.template is_grade<3>()};
    const auto is_nonzero{!mv.near_zero()};
    const auto contains_e_inf{mv.outer(e_inf()).near_zero()};

    DLOG(INFO) << "is_line() -- mv: " << mv << ", is_grade_3: " << is_grade_3
               << ", is_nonzero: " << is_nonzero << ", contains_e_inf: " << contains_e_inf;
    return is_grade_3 and is_nonzero and contains_e_inf;
  }

  [[nodiscard]] static constexpr auto make_circle(IsMultivectorLike<G> auto&& p1,
                                                  IsMultivectorLike<G> auto&& p2,
                                                  IsMultivectorLike<G> auto&& p3) noexcept {
    return join(join(p1, p2), p3);
  }

  /*
   * Constructs a circle as a 3-blade representing a collection of points.
   * The attitude is a 2-blade defining the circle's spatial orientation and plane.
   * For example, in a 2D simulation, this is typically e1 ^ e2. In a 3D
   * simulation, a circle in the XZ plane would be constructed using e1 ^ e3.
   */
  [[nodiscard]] static constexpr auto make_circle(IsMultivectorLike<G> auto&& center,
                                                  IsMultivectorLike<G> auto&& attitude,
                                                  ScalarLike<G> auto&& radius) noexcept {
    static_assert(NUM_PHYSICAL_DIMENSIONS >= 2);
    LOG_IF(FATAL, !center.template is_grade<1>()) << "Center must be a grade-1 point.";
    LOG_IF(FATAL, !attitude.template is_grade<2>()) << "Attitude must be a grade-2 bivector.";

    // Construct the round primitive representing the points at the given radius.
    const auto sphere{center - (0.5 * radius * radius * e_inf())};

    // Derive the carrier plane from the attitude and center point.
    // The attitude is dualized within the spatial manifold to find the normal direction.
    const auto normal{(attitude << Multivector::inverse_pseudoscalar()).mask_conformal_bases()};

    // Assemble the flat primitive that constrains the circle to the specified plane.
    const auto plane{normal + ((center << normal) * e_inf())};

    // The circle is the intersection (meet) of the sphere and the carrier plane.
    return (sphere ^ plane).dual();
  }

  [[nodiscard]] static constexpr auto make_circle(ScalarLike<G> auto&& c1, ScalarLike<G> auto&& c2,
                                                  ScalarLike<G> auto&& radius) noexcept
    requires(NUM_PHYSICAL_DIMENSIONS == 2)
  {
    const auto p1{make_point(c1 + radius, c2)};
    const auto p2{make_point(c1, c2 + radius)};
    const auto p3{make_point(c1 - radius, c2)};
    return make_circle(p1, p2, p3);
  }

  [[nodiscard]] static constexpr auto make_circle(ScalarLike<G> auto&& c1, ScalarLike<G> auto&& c2,
                                                  ScalarLike<G> auto&& c3, ScalarLike<G> auto&& nx,
                                                  ScalarLike<G> auto&& ny, ScalarLike<G> auto&& nz,
                                                  ScalarLike<G> auto&& radius) noexcept
    requires(NUM_PHYSICAL_DIMENSIONS == 3)
  {
    using std::hypot;
    using ScalarType = decltype(radius);

    // Find one in-plane vector of length radius by zeroing the smallest normal component
    // and constructing a perpendicular from the other two.
    const auto [ax, ay, az] = [nx, ny, nz,
                               radius]() -> std::tuple<ScalarType, ScalarType, ScalarType> {
      if (abs(nx) < abs(ny) and abs(nx) < abs(nz)) {
        const auto scale{radius / hypot(ny, nz)};
        return std::make_tuple(0, -nz * scale, ny * scale);
      } else if (abs(ny) < abs(nz)) {
        const auto scale{radius / hypot(nx, nz)};
        return std::make_tuple(nz * scale, 0, -nx * scale);
      } else {
        const auto scale{radius / hypot(nx, ny)};
        return std::make_tuple(-ny * scale, nx * scale, 0);
      }
    }();

    // Compute a second in-plane vector b orthogonal to a via n × a.
    // Normalize b so that |a| = |b| = radius.
    const auto n_len{hypot(nx, ny, nz)};
    const auto bx{(ny * az - nz * ay) / n_len};
    const auto by{(nz * ax - nx * az) / n_len};
    const auto bz{(nx * ay - ny * ax) / n_len};

    // Precompute sqrt(3)/2, the value of sin(60°).
    constexpr Scalar sqrt3_2{0.8660254037844386};

    // Place three points at 120° intervals using a and b as the in-plane basis:
    // p = center + a*cos(theta) + b*sin(theta) for theta = 0°, 120°, 240°
    const auto p1{make_point(c1 + ax, c2 + ay, c3 + az)};
    const auto p2{make_point(c1 - ax / 2 + bx * sqrt3_2, c2 - ay / 2 + by * sqrt3_2,
                             c3 - az / 2 + bz * sqrt3_2)};
    const auto p3{make_point(c1 - ax / 2 - bx * sqrt3_2, c2 - ay / 2 - by * sqrt3_2,
                             c3 - az / 2 - bz * sqrt3_2)};

    const auto circle{make_circle(p1, p2, p3)};
    LOG(INFO) << "p1: " << p1 << ", p2: " << p2 << ", p3: " << p3 << ", circle: " << circle;
    return circle;
  }

  static constexpr void extract_circle(IsMultivectorLike<G> auto&& circle, Multivector& center,
                                       Multivector& attitude, Scalar& radius) noexcept {
    using std::sqrt;

    LOG(INFO) << "extract_circle() -- circle: " << circle;

    // The 1-blade plane (IPNS).
    const auto pi{(circle ^ e_inf()).dual().normalize()};
    LOG(INFO) << "extract_circle() -- pi: " << pi;
    if constexpr (NUM_PHYSICAL_DIMENSIONS == 2) {
      // In 2 physical dimensions, the IPNS plane is just a scalar.
      LOG_IF(FATAL, !pi.template is_grade<0>())  //
          << "Expect the computed plane pi (IPNS) in extract_circle() to be a scalar in two "
          << "physical dimensions. pi: " << pi;
    }

    // The parent sphere S (IPNS).
    const auto S{(pi << circle.dual()) / pi.square_magnitude()};
    LOG(INFO) << "extract_circle() -- S: " << S;
    LOG(INFO) << "extract_circle() -- is_point(S): " << is_point(S) << ", weight(S): " << weight(S);

    // The center point.
    // As a vector, the sphere is its own reverse, so S*e_inf*~S = S*e_inf*S.
    center = S * e_inf() * S;
    LOG(INFO) << "extract_circle() -- center: " << center;
    center = center / weight(center);
    LOG(INFO) << "extract_circle() -- center: " << center << " (normalized)";

    // The 2-blade spatial orientation (OPNS) of the plane containing the circle.
    attitude = e_inf() << circle;
    LOG(INFO) << "extract_circle() -- attitude: " << attitude;

    radius = sqrt(abs(S.square_magnitude())) / abs(weight(S));
    LOG(INFO) << "extract_circle() -- radius: " << radius;
  }

  static constexpr void extract_circle(IsMultivectorLike<G> auto&& circle, Multivector& p1,
                                       Multivector& p2, Multivector& p3) noexcept {
    using std::sin, std::cos, std::numbers::pi;

    Multivector center{};
    Multivector B{};
    Scalar radius{};
    extract_circle(circle, center, B, radius);
    LOG(INFO) << "circle: " << circle << ", center: " << center << ", B: " << B
              << ", radius: " << radius;

    // Form basis vectors u and v in the plane of the circle via contraction.
    // TODO: Update this for more dimensions. It is possible both gamma0 and gamma1 are parallel to
    // B.
    auto u = (gamma0() << B);
    LOG(INFO) << "u: " << u << ", u.square_magnitude(): " << u.square_magnitude();

    if (u.square_magnitude() < EPSILON) {
      u = (gamma1() << B);
      LOG(INFO) << "u: " << u << ", u.square_magnitude(): " << u.square_magnitude();
    }
    u = u.normalize();
    LOG(INFO) << "u: " << u << ", u.square_magnitude(): " << u.square_magnitude();
    auto v = (u << B).normalize();
    LOG(INFO) << "v: " << v << ", v.square_magnitude(): " << v.square_magnitude();

    // Create a 120-degree rotor
    const auto angle{Scalar{pi} / Scalar{3}};
    const auto R{cos(angle) + sin(angle) * B};
    const auto R_rev{R.reverse()};

    p1 = center + radius * u;
    p2 = R * p1 * R_rev;
    p3 = R * p2 * R_rev;
  }

  static constexpr void extract_circle(IsMultivectorLike<G> auto&& circle, Scalar& center_x,
                                       Scalar& center_y, Scalar& radius) noexcept
    requires(NUM_PHYSICAL_DIMENSIONS == 2)
  {
    using std::sin, std::cos, std::sqrt, std::numbers::pi;

    Multivector center{};
    Multivector B{};
    extract_circle(circle, center, B, radius);

    extract_point(center, center_x, center_y);
  }

  static constexpr void extract_circle(IsMultivectorLike<G> auto&& circle, Scalar& center_x,
                                       Scalar& center_y, Scalar& center_z, Scalar& normal_x,
                                       Scalar& normal_y, Scalar& normal_z, Scalar& radius) noexcept
    requires(NUM_PHYSICAL_DIMENSIONS == 3)
  {
    using std::sin, std::cos, std::sqrt, std::numbers::pi;

    Multivector center{};
    Multivector B{};
    extract_circle(circle, center, B, radius);

    // TODO(james): Add extraction of the normal vector to B.
    extract_point(center, center_x, center_y, center_z);
  }

  [[nodiscard]] static constexpr auto make_sphere(IsMultivectorLike<G> auto&& p1,
                                                  IsMultivectorLike<G> auto&& p2,
                                                  IsMultivectorLike<G> auto&& p3,
                                                  IsMultivectorLike<G> auto&& p4) noexcept {
    return join(join(join(p1, p2), p3), p4);
  }

  [[nodiscard]] static constexpr auto make_hypersphere(IsMultivectorLike<G> auto&& p1,
                                                       IsMultivectorLike<G> auto&& p2,
                                                       IsMultivectorLike<G> auto&& p3,
                                                       IsMultivectorLike<G> auto&& p4,
                                                       IsMultivectorLike<G> auto&& p5) noexcept {
    return join(join(join(join(p1, p2), p3), p4), p5);
  }

  [[nodiscard]] static constexpr auto make_sphere(ScalarLike<G> auto&& c1, ScalarLike<G> auto&& c2,
                                                  ScalarLike<G> auto&& c3,
                                                  ScalarLike<G> auto&& radius) noexcept
    requires(NUM_PHYSICAL_DIMENSIONS == 3)
  {
    const auto p1{make_point(c1 + radius, c2, c3)};
    const auto p2{make_point(c1, c2 + radius, c3)};
    const auto p3{make_point(c1 - radius, c2, c3)};
    const auto p4{make_point(c1, c2, c3 + radius)};
    return make_sphere(p1, p2, p3, p4);
  }

  [[nodiscard]] static constexpr auto make_hypersphere(ScalarLike<G> auto&& c1,
                                                       ScalarLike<G> auto&& c2,
                                                       ScalarLike<G> auto&& c3,
                                                       ScalarLike<G> auto&& c4,
                                                       ScalarLike<G> auto&& radius) noexcept
    requires(NUM_PHYSICAL_DIMENSIONS == 4)
  {
    const auto p1{make_point(c1 + radius, c2, c3, c4)};
    const auto p2{make_point(c1, c2 + radius, c3, c4)};
    const auto p3{make_point(c1 - radius, c2, c3, c4)};
    const auto p4{make_point(c1, c2, c3 + radius, c4)};
    const auto p5{make_point(c1, c2, c3, c4 + radius)};
    return make_hypersphere(p1, p2, p3, p4, p5);
  }

  /**
   * Constructs a multivector representing a plane in a conformal space.
   *
   * This bridges Euclidean coordinate-based intuition (normal and point) into the algebraic
   * framework of Conformal Geometric Algebra (CGA). This construction is appropriate in any
   * conformal geometry provided the underlying algebra provides enough physical dimensions.
   */
  [[nodiscard]] static constexpr auto make_plane(ScalarLike<G> auto&& nx, ScalarLike<G> auto&& ny,
                                                 ScalarLike<G> auto&& nz, ScalarLike<G> auto&& px,
                                                 ScalarLike<G> auto&& py, ScalarLike<G> auto&& pz) {
    using std::hypot;
    const auto norm{hypot(nx, ny, nz)};
    if (norm < Algebra::EPSILON) {
      except<std::domain_error>("Degenerate normal: cannot construct plane from null vector.");
    }

    // Normalize direction components
    const auto ux = nx / norm;
    const auto uy = ny / norm;
    const auto uz = nz / norm;

    // The 3D spatial bivector representing the orientation of the plane.
    // This is the dual of the normal vector within the 3D Euclidean subspace.
    const auto spatial_bivector{ux * gamma12() - uy * gamma02() + uz * gamma01()};

    const auto p{make_point(px, py, pz)};

    return join(join(p, spatial_bivector), e_inf());
  }

  /**
   * Constructs a multivector representing a hyperplane in a time-extended conformal space.
   *
   * This bridges higher-dimensional coordinate-based intuition into the algebraic framework
   * of Conformal Geometric Algebra (CGA). This construction is appropriate in any conformal
   * geometry provided the underlying algebra provides enough physical dimensions.
   */
  [[nodiscard]] static constexpr auto make_plane(ScalarLike<G> auto&& nt, ScalarLike<G> auto&& nx,
                                                 ScalarLike<G> auto&& ny, ScalarLike<G> auto&& nz,
                                                 ScalarLike<G> auto&& pt, ScalarLike<G> auto&& px,
                                                 ScalarLike<G> auto&& py,
                                                 ScalarLike<G> auto&& pz) noexcept {
    const auto norm{hypot(nt, nx, ny, nz)};
    if (norm < Algebra::EPSILON) {
      except<std::domain_error>("Degenerate normal: cannot construct plane from null vector.");
    }

    const auto ut = nt / norm;
    const auto ux = nx / norm;
    const auto uy = ny / norm;
    const auto uz = nz / norm;

    const auto spatial_trivector{ut * (gamma12() * gamma3()) - ux * (gamma02() * gamma3()) +
                                 uy * (gamma01() * gamma3()) - uz * (gamma01() * gamma2())};

    const auto p{make_point(pt, px, py, pz)};

    return join(join(p, spatial_trivector), e_inf());
  }

  static constexpr bool is_plane(IsMultivectorLike<G> auto&& mv) noexcept {
    if (!mv.template is_grade<5>()) return false;

    // The null condition L * ~L = 0 distinguishes lines (null) from circles (non-null).
    const auto self_product{(mv * ~mv).scalar()};
    const auto is_null{abs(self_product) < Algebra::EPSILON};

    // A line contains e_inf as a factor — its outer product with e_inf is zero.
    const auto contains_e_inf{(mv ^ e_inf()).near_equal(Multivector{})};

    return is_null and contains_e_inf;
  }

  /**
   * Constructs a 3-blade circle from a center point, a normal vector defining the
   * plane of the circle, and a radius.
   *
   * This is implemented by intersecting a sphere with a plane.
   */
  [[nodiscard]] static constexpr auto make_circle(ScalarLike<G> auto&& cx, ScalarLike<G> auto&& cy,
                                                  ScalarLike<G> auto&& cz, ScalarLike<G> auto&& nx,
                                                  ScalarLike<G> auto&& ny, ScalarLike<G> auto&& nz,
                                                  ScalarLike<G> auto&& radius) noexcept {
    const auto center{make_point(cx, cy, cz)};

    // Create the dual sphere: P - r^2 / 2 * e_inf
    const auto sphere{center - radius * radius / Scalar{2} * e_inf()};

    // Create the dual plane passing through the center with the given normal.
    const auto plane{make_plane(nx, ny, nz, cx, cy, cz)};

    // The intersection of a dual sphere and a dual plane is the dual circle.
    return meet(sphere, plane);
  }

  [[nodiscard]] static constexpr auto make_circle(ScalarLike<G> auto&& ct, ScalarLike<G> auto&& cx,
                                                  ScalarLike<G> auto&& cy, ScalarLike<G> auto&& cz,
                                                  ScalarLike<G> auto&& nt, ScalarLike<G> auto&& nx,
                                                  ScalarLike<G> auto&& ny, ScalarLike<G> auto&& nz,
                                                  ScalarLike<G> auto&& radius) noexcept {
    const auto center{make_point(ct, cx, cy, cz)};

    // Create the dual sphere.
    const auto sphere{center - radius * radius / Scalar{2} * e_inf()};

    // Create the dual hyperplane passing through the center with the given normal.
    const auto plane{make_plane(nt, nx, ny, nz, ct, cx, cy, cz)};

    // The intersection of a dual sphere and a dual hyperplane is the dual circle.
    return meet(sphere, plane);
  }

  /**
   * Circles and lines are almost the same entities in CGA. A line is just a circle with
   * infinite radius.
   */
  [[nodiscard]] static constexpr auto is_circle(IsMultivectorLike<G> auto&& mv) noexcept {
    if (!mv.template is_grade<3>()) {
      return false;
    }

    // The null condition L * ~L = 0 distinguishes lines (null) from circles (non-null).
    const auto self_product{mv.multiply(mv.reverse()).scalar()};
    const auto is_null{abs(self_product) < Algebra::EPSILON};

    // A line contains e_inf as a factor — its outer product with e_inf is zero. A circle does
    // not have e_inf as a factor.
    const auto contains_e_inf{mv.outer(e_inf()).near_equal(Multivector{})};

    return !is_null && !contains_e_inf;
  }

  /**
   * Compute the logarithm of a CGA motor. CGA motors represent rigid body motions and
   * live in the even subalgebra of R(4,1). The logarithm formula is analogous to PGA but
   * operates in the 5D conformal space — the Euclidean bivector components are those not
   * involving e_plus or e_minus, and the translational components involve e_inf.
   */
  [[nodiscard]] static auto motor_log(IsMultivectorLike<G> auto&& motor) noexcept {
    using std::acos, std::sin, std::sqrt;

    const auto scalar_part{motor.scalar()};

    const auto exy{get_xy(motor)};
    const auto exz{get_xz(motor)};
    const auto eyz{get_yz(motor)};
    const auto euclidean_norm_sq{exy * exy + exz * exz + eyz * eyz};

    if (euclidean_norm_sq < Algebra::EPSILON) {
      // For pure translations, the log is simply the bivector part.
      return motor.template grade_projection<2>();
    }

    const auto euclidean_norm{sqrt(euclidean_norm_sq)};
    const auto angle{Scalar{2} * acos(scalar_part)};
    const auto s{sin(angle / Scalar{2})};

    const auto euclidean_biv{exy * gamma12() + exz * gamma13() + eyz * gamma23()};

    // The translational component of the motor lives in the bivectors involving e_inf.
    const auto translation_biv{motor.template grade_projection<2>() - euclidean_biv};

    return angle / euclidean_norm * euclidean_biv + Scalar{1} / s * translation_biv;
  }

  /**
   * Compute the exponential of a CGA bivector. The Euclidean part generates rotation and
   * the e_inf part generates translation. The formula parallels the PGA case but in the
   * conformal embedding — the translational generator uses e_inf rather than the ideal
   * line elements of PGA.
   */
  [[nodiscard]] static auto motor_exp(IsMultivectorLike<G> auto&& bivector) noexcept {
    using std::cos, std::sin, std::sqrt;

    const auto exy{get_xy(bivector)};
    const auto exz{get_xz(bivector)};
    const auto eyz{get_yz(bivector)};
    const auto euclidean_norm_sq{exy * exy + exz * exz + eyz * eyz};

    if (euclidean_norm_sq < Algebra::EPSILON) {
      auto result{bivector.template grade_projection<2>()};
      result.set_scalar(Scalar{1});
      return result;
    }

    const auto euclidean_norm{sqrt(euclidean_norm_sq)};
    const auto c{cos(euclidean_norm)};
    const auto s{sin(euclidean_norm) / euclidean_norm};

    return s * bivector.template grade_projection<2>() + c;
  }
};

template <typename Scalar = DefaultScalarType>
using Cga2dGeometry = CgaGeometryType<2, Scalar>;

template <typename Scalar = DefaultScalarType>
using Cga3dGeometry = CgaGeometryType<3, Scalar>;

template <typename Scalar = DefaultScalarType>
using Cga4dGeometry = CgaGeometryType<4, Scalar>;

static_assert(ConformalGeometryModel<Cga2dGeometry<>>);
static_assert(ConformalGeometryModel<Cga3dGeometry<>>);
static_assert(ConformalGeometryModel<Cga4dGeometry<>>);

}  // namespace ndyn::math
