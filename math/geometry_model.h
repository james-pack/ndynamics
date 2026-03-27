#pragma once

#include <concepts>

namespace ndyn::math {

/**
 * A GeometryModel encodes the relationship between abstract multivector elements
 * and geometric primitives for a specific geometric algebra and embedding. It is
 * intentionally minimal. It only specifies concepts that are at the intersection
 * of all geometries.
 *
 * The separation of concerns is as follows:
 *   - The multivector owns algebraic operations (product, reverse, grade, norm, etc.)
 *   - The GeometryModel owns geometric interpretation (what a point *is* in this
 *     geometric model, how lines are constructed, which product is join vs meet, etc.)
 */
template <typename G>
concept GeometryModel =
    requires {
      typename G::Algebra;
      typename G::Multivector;
      typename G::Scalar;
    } &&

    /**
     * Embed a Euclidean point into the algebra's representation. The encoding
     * of a point as a multivector is not universal — in PGA a point is a
     * trivector (the meet of three planes), while in CGA a point is a null
     * vector on the hyperboloid. In STA a point carries an implicit timelike
     * component that must be supplied by the geometry model relative to a
     * chosen reference frame — the three scalar coordinates are purely spatial.
     * In DCGA the point is embedded twice into the higher-dimensional space,
     * and the construction is not a simple grade-1 embedding. The caller must
     * treat the result as opaque and never assume a particular grade.
     */
    requires(const G& g, G::Scalar x, G::Scalar y, G::Scalar z) {
      { g.make_point(x, y, z) } -> std::same_as<typename G::Multivector>;
    } &&

    /**
     * Test if a given multivector represents a point.
     */
    requires(G& g, const G::Multivector& mv) {
      { g.is_point(mv) } -> std::same_as<bool>;
    } &&

    /**
     * Test if a given multivector represents a line.
     */
    requires(G& g, const G::Multivector& mv) {
      { g.is_line(mv) } -> std::same_as<bool>;
    } &&

    /**
     * Test if a given multivector represents a circle. Note that circles are not directly
     * representable using a single multivector in all algebras.
     */
    requires(G& g, const G::Multivector& mv) {
      { g.is_circle(mv) } -> std::same_as<bool>;
    } &&

    /**
     * Test if a given multivector represents a plane. Note that planes are not directly
     * representable in all algebras. In VGA, a plane must pass through the origin; the multivector
     * representing the plane is purely the direction of the normal.
     */
    requires(G& g, const G::Multivector& mv) {
      { g.is_plane(mv) } -> std::same_as<bool>;
    } &&

    /**
     * Compute the join (span) of two geometric elements. The join answers
     * "what is the smallest element containing both operands" — point+point
     * gives a line, point+line gives a plane. In PGA the join is the
     * regressive product (the dual of the outer product), while in CGA it
     * is the outer product directly. In STA the join produces elements that
     * may have mixed spacetime character — the result of joining two spatial
     * points depends on the chosen frame. In DCGA the join operates in the
     * higher-dimensional space and the result encodes surface geometry beyond
     * what is expressible in PGA or CGA. Exposing join as a named operation
     * shields the caller from which product serves this role in each algebra.
     */
    requires(const G& g, const G::Multivector& a, const G::Multivector& b) {
      { g.join(a, b) } -> std::same_as<typename G::Multivector>;
    } &&

    /**
     * Compute the meet (intersection) of two geometric elements. The meet
     * answers "what is the largest element contained in both operands" —
     * plane+plane gives a line, plane+line gives a point. In PGA the meet is
     * the outer product, while in CGA it is the regressive product — the
     * inversion of roles between PGA and CGA is precisely why this must be
     * part of the geometry model. In STA the meet of two hyperplanes yields a
     * line whose character (spacelike, timelike, or null) depends on the
     * metric signature and the orientations of the planes. In DCGA the meet
     * can recover intersection curves of higher-order surfaces, which have no
     * equivalent in lower-dimensional algebras.
     */
    requires(const G& g, const G::Multivector& a, const G::Multivector& b) {
      { g.meet(a, b) } -> std::same_as<typename G::Multivector>;
    } &&

    /**
     * Construct a rotor representing a rotation about the given axis element
     * by the given angle in radians. The axis is a line element in the
     * algebra's representation — not a bare direction vector — because in
     * PGA rotation is about a line in space rather than a direction through
     * the origin. In CGA the axis line is a different grade object but the
     * exponential map structure is preserved. In STA a spatial rotation must
     * be constructed within a chosen reference frame — the axis line element
     * must be purely spacelike, and the geometry model is responsible for
     * enforcing this constraint relative to the frame it encodes. In DCGA
     * the rotor acts on the doubly-embedded space and must preserve the
     * double-embedding structure, requiring a construction step that differs
     * from a naive bivector exponential.
     */
    requires(const G& g, const G::Multivector& axis, G::Scalar angle) {
      { g.make_rotor(axis, angle) } -> std::same_as<typename G::Multivector>;
    } &&

    /**
     * Compute the logarithm of a motor, returning a bivector in the Lie
     * algebra of the motor group. This is the foundation for both motor
     * interpolation (ScLERP) and velocity integration. In PGA and CGA the
     * motor group is isomorphic to SE(3) and its Lie algebra is well-defined
     * in terms of the even subalgebra. In STA the motor group is a subgroup
     * of the Lorentz group and the Lie algebra bivectors encode both spatial
     * rotations and boosts — the logarithm must correctly decompose these,
     * which requires knowledge of the metric signature. In DCGA the motor
     * group acts on the doubly-embedded space and the logarithm must respect
     * that structure, producing a bivector that generates motions consistent
     * with the double embedding.
     */
    requires(const G& g, const G::Multivector& motor) {
      { g.motor_log(motor) } -> std::same_as<typename G::Multivector>;
    } &&

    /**
     * Compute the exponential of a bivector, returning a motor. This is the
     * inverse of motor_log and is used to integrate angular and linear
     * velocities back into a motor representing the resulting rigid body
     * state. In STA the bivector may encode a boost as well as a spatial
     * rotation, and the exponential must produce a valid Lorentz motor rather
     * than a Euclidean one — the geometry model determines which components
     * of the bivector are interpreted as rotational vs boost generators. In
     * DCGA the exponential must produce a motor that acts consistently on
     * both embedded copies of the geometry.
     */
    requires(const G& g, const G::Multivector& bivector) {
      { g.motor_exp(bivector) } -> std::same_as<typename G::Multivector>;
    } &&

    /**
     * Extract Euclidean coordinates from a point element. Normalization of
     * the homogeneous weight is GA-specific — in PGA the weight is the
     * coefficient of e123, in CGA the normalization involves the inner product
     * with the point at infinity. In STA the spatial coordinates must be
     * extracted relative to the reference frame encoded by the geometry model
     * — the timelike component is consumed by the extraction and not surfaced
     * to the caller, as it is frame metadata rather than spatial position. In
     * DCGA the two embedded copies of the point must agree after
     * dehomogenization, and the extraction must verify or enforce this
     * consistency before returning coordinates. The caller must not attempt to
     * read coordinates directly from the multivector components.
     */
    requires {
      requires requires(G& g, const G::Multivector& point, G::Scalar& out_x, G::Scalar& out_y,
                        G::Scalar& out_z) { g.extract_point(point, out_x, out_y, out_z); };
    } &&

    /**
     * Extract the direction and moment of a line element as Euclidean vectors.
     * In PGA these correspond directly to the two three-component parts of the
     * bivector. In CGA the line is a different grade object and the direction
     * and moment must be recovered via projection onto the appropriate
     * subspace. In STA a line has an additional timelike component whose
     * extraction is frame-dependent — the geometry model consumes the timelike
     * part and surfaces only the spatial direction and moment relative to its
     * reference frame. In DCGA the line exists in the doubly-embedded space
     * and the direction and moment must be recovered from the higher-dimensional
     * representation, with the double-embedding redundancy resolved before
     * returning. The caller must not assume any correspondence between
     * multivector components and the returned direction and moment.
     */
    requires {
      requires requires(G& g, const G::Multivector& line, G::Scalar& out_direction_x,
                        G::Scalar& out_direction_y, G::Scalar& out_direction_z,
                        G::Scalar& out_moment_x, G::Scalar& out_moment_y, G::Scalar& out_moment_z) {
        g.extract_line(line, out_direction_x, out_direction_y, out_direction_z, out_moment_x,
                       out_moment_y, out_moment_z);
      };
    };

}  // namespace ndyn::math
