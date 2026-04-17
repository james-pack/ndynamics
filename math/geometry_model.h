#pragma once

#include <concepts>

namespace ndyn::math {

/**
 * GeometryModel details which operators and geometric primitives are available in all geometries.
 *
 * General separation of concerns:
 *   - The Multivector owns algebraic operations (product, reverse, grade, norm, etc.) and exists as
 * part of an algebra.
 *   - The geometry class owns geometric interpretation of the Multivector and specifies how
 * operations and geometric primitives are modelled in Multivectors.
 *   - These GeometryModels provide contracts to make it easier to swap between geometries.
 */
template <typename G>
concept GeometryModel =
    requires {
      typename G::Algebra;
      typename G::Multivector;
      typename G::Scalar;
    } &&

    /**
     * Compute the join (span) of two geometric elements. The join answers
     * "what is the smallest element containing both operands" — point+point
     * gives a line, point+line gives a plane. In PGA the join is the
     * regressive product (the dual of the outer product), while in CGA it
     * is the outer product directly. Exposing join as a named operation
     * shields the caller from which product serves this role in each algebra.
     */
    requires(const G::Multivector& a, const G::Multivector& b) {
      { G::join(a, b) } -> std::same_as<typename G::Multivector>;
    } &&

    /**
     * Compute the meet (intersection) of two geometric elements. The meet
     * answers "what is the largest element contained in both operands" —
     * plane+plane gives a line, plane+line gives a point. In PGA the meet is
     * the outer product, while in CGA it is the regressive product — the
     * inversion of roles between PGA and CGA.
     */
    requires(const G::Multivector& a, const G::Multivector& b) {
      { G::meet(a, b) } -> std::same_as<typename G::Multivector>;
    } &&

    /**
     * Construct a rotor representing a rotation about the given axis element
     * by the given angle in radians. The axis is a line element in the
     * geometry's representation.
     */
    requires(G::Scalar nx, G::Scalar ny, G::Scalar nz, G::Scalar angle) {
      { G::make_rotor(nx, ny, nz, angle) } -> std::same_as<typename G::Multivector>;
    } &&

    /**
     * Compute the logarithm of a motor, returning a bivector in the Lie
     * algebra of the motor group. This is the foundation for both motor
     * interpolation (ScLERP) and velocity integration.
     */
    requires(const G::Multivector& motor) {
      { G::motor_log(motor) } -> std::same_as<typename G::Multivector>;
    } &&

    /**
     * Compute the exponential of a bivector, returning a motor. This is the
     * inverse of motor_log and is used to integrate angular and linear
     * velocities back into a motor representing the resulting rigid body
     * state.
     */
    requires(const G::Multivector& bivector) {
      { G::motor_exp(bivector) } -> std::same_as<typename G::Multivector>;
    } &&

    /**
     * Embed a Euclidean point into the algebra's representation. Primarily, this method should be
     * used to bridge a user's mental model to the geometry.
     */
    requires(G::Scalar x, G::Scalar y, G::Scalar z) {
      { G::make_point(x, y, z) } -> std::same_as<typename G::Multivector>;
    } &&

    /**
     * Extract Euclidean coordinates from a point element. This is the inverse of the make_point()
     * method.
     */
    requires(const G::Multivector& point, G::Scalar& out_x, G::Scalar& out_y, G::Scalar& out_z) {
      G::extract_point(point, out_x, out_y, out_z);
    } &&

    /**
     * Test if a given multivector represents a point in the geometry.
     */
    requires(const G::Multivector& mv) {
      { G::is_point(mv) } -> std::same_as<bool>;
    };

template <typename G>
concept GeometryWithTime =
    GeometryModel<G> &&

    requires(G::Scalar t, G::Scalar x, G::Scalar y, G::Scalar z) {
      { G::make_point(t, x, y, z) } -> std::same_as<typename G::Multivector>;
    } &&

    requires(const G::Multivector& point, G::Scalar& out_t, G::Scalar& out_x, G::Scalar& out_y,
             G::Scalar& out_z) { G::extract_point(point, out_t, out_x, out_y, out_z); };

template <typename G>
concept EuclideanGeometry =  //
    GeometryModel<G> &&

    /**
     * Factory methods to create Multivectors representing the Euclidean basis vectors.
     */
    requires {
      { G::x() } -> std::same_as<typename G::Multivector>;
    } &&

    requires {
      { G::y() } -> std::same_as<typename G::Multivector>;
    } &&

    requires {
      { G::z() } -> std::same_as<typename G::Multivector>;
    };

template <typename G>
concept EuclideanGeometryWithTime =  //
    EuclideanGeometry<G> &&          //
    GeometryWithTime<G> &&           //

    requires {
      { G::t() } -> std::same_as<typename G::Multivector>;
    };

template <typename G>
concept SpatialConformalGeometryModel =  //
    GeometryModel<G> &&                  //

    /**
     * Factory methods to create Multivectors representing general 3-space basis vectors. Note that
     * these bases use the terminology of relativity, even though the standard algebra used in
     * relativity (STA) cannot be used to build a conformal geometry. (Note that you can use the
     * CSTA (Conformal Spacetime Algebra) if you need a conformal geometry with STA.)
     *
     * Traditionally, gamma0 is time (see ConformalGeometryWithTime below), and gammas 1,2,3
     * represent spatial dimensions.
     */
    requires {
      { G::gamma1() } -> std::same_as<typename G::Multivector>;
    } &&

    requires {
      { G::gamma2() } -> std::same_as<typename G::Multivector>;
    } &&

    requires {
      { G::gamma3() } -> std::same_as<typename G::Multivector>;
    } &&

    requires(G::Scalar nx, G::Scalar ny, G::Scalar nz, G::Scalar px, G::Scalar py, G::Scalar pz) {
      { G::make_reflector(nx, ny, nz, px, py, pz) } -> std::same_as<typename G::Multivector>;
    } &&

    requires(G::Scalar cx, G::Scalar cy, G::Scalar cz, G::Scalar scale) {
      { G::make_dilator(cx, cy, cz, scale) } -> std::same_as<typename G::Multivector>;
    } &&

    requires(G::Scalar px, G::Scalar py, G::Scalar pz) {
      { G::make_translator(px, py, pz) } -> std::same_as<typename G::Multivector>;
    } &&

    requires(G::Scalar nx, G::Scalar ny, G::Scalar nz, G::Scalar px, G::Scalar py, G::Scalar pz) {
      { G::make_plane(nx, ny, nz, px, py, pz) } -> std::same_as<typename G::Multivector>;
    } &&

    requires(const G::Multivector& v) {
      { G::is_plane(v) } -> std::same_as<bool>;
    } &&

    requires(G::Scalar px1, G::Scalar py1, G::Scalar pz1, G::Scalar px2, G::Scalar py2,
             G::Scalar pz2) {
      { G::make_line(px1, py1, pz1, px2, py2, pz2) } -> std::same_as<typename G::Multivector>;
    } &&

    requires(const G::Multivector& v) {
      { G::is_line(v) } -> std::same_as<bool>;
    } &&

    requires(G::Scalar cx, G::Scalar cy, G::Scalar cz, G::Scalar nx, G::Scalar ny, G::Scalar nz,
             G::Scalar radius) {
      { G::make_circle(cx, cy, cz, nx, ny, nz, radius) } -> std::same_as<typename G::Multivector>;
    } &&

    requires(const G::Multivector& v) {
      { G::is_circle(v) } -> std::same_as<bool>;
    } &&

    true;

template <typename G>
concept ConformalGeometryModel =         //
    SpatialConformalGeometryModel<G> &&  //
    GeometryWithTime<G> &&               //

    requires {
      { G::gamma0() } -> std::same_as<typename G::Multivector>;
    } &&

    requires(G::Scalar ct, G::Scalar cx, G::Scalar cy, G::Scalar cz, G::Scalar scale) {
      { G::make_dilator(ct, cx, cy, cz, scale) } -> std::same_as<typename G::Multivector>;
    } &&

    requires(G::Scalar nt, G::Scalar nx, G::Scalar ny, G::Scalar nz, G::Scalar pt, G::Scalar px,
             G::Scalar py, G::Scalar pz) {
      {
        G::make_reflector(nt, nx, ny, nz, pt, px, py, pz)
      } -> std::same_as<typename G::Multivector>;
    };

}  // namespace ndyn::math
