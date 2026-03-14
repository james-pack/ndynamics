#pragma once

#include <concepts>

namespace ndyn::math {

/**
 * Concept defining the interface for a geometric algebra geometry model.
 *
 * A geometry model encodes the conventions for embedding geometric primitives into a specific
 * Clifford algebra. The algebra's metric signature determines which primitives are representable
 * and how versors act on them. For example, points are grade-1 in VGA and CGA but grade-3 in
 * PGA; translations are not representable as versors in VGA at all.
 *
 * Implementing types should be final structs or classes with no virtual dispatch overhead.
 * Default implementations for operations that reduce to standard multivector arithmetic
 * (join, meet, motor, apply, apply_unit) may be provided conventionally in each concrete
 * model rather than centrally here.
 */
template <typename ModelT>
concept GeometryModel =
    requires(const ModelT model, typename ModelT::Scalar s, const typename ModelT::Multivector mv) {
      // ---------------------------------------------------------------------------
      // Associated types
      //
      // Algebra is the Clifford algebra traits type (e.g. Algebra<double, 3, 0, 1>
      // for PGA). Scalar and Multivector are derived from it for convenience.
      // ---------------------------------------------------------------------------

      typename ModelT::Algebra;
      typename ModelT::Scalar;
      typename ModelT::Multivector;

      // ---------------------------------------------------------------------------
      // Geometric primitive constructors
      //
      // These methods encode the model's embedding conventions. Grade and structure
      // of the returned multivector are model-dependent and are the primary reason
      // this concept exists — the same geometric intent maps to different algebraic
      // representations across models.
      // ---------------------------------------------------------------------------

      /**
       * Constructs a finite point from Euclidean coordinates (x, y, z). The grade
       * and homogeneous embedding of the result are model-dependent: grade-1 in VGA
       * and CGA, grade-3 in PGA.
       */
      { model.point(s, s, s) } -> std::same_as<typename ModelT::Multivector>;

      /**
       * Constructs an ideal point (point at infinity) from direction components
       * (x, y, z). Ideal points represent pure directions with no finite location.
       * In PGA the homogeneous weight component is zero; in CGA the e_inf component
       * is nonzero while the e_0 component is zero.
       */
      { model.ideal_point(s, s, s) } -> std::same_as<typename ModelT::Multivector>;

      /**
       * Constructs the line passing through two finite points. The grade and
       * factorization of the result are model-dependent: a bivector in PGA (via
       * outer product of the two point trivectors), a trivector in CGA.
       */
      { model.line(mv, mv) } -> std::same_as<typename ModelT::Multivector>;

      /**
       * Constructs the plane passing through three finite points. As with line(),
       * the grade of the result depends on the model's point representation.
       */
      { model.plane(mv, mv, mv) } -> std::same_as<typename ModelT::Multivector>;

      // ---------------------------------------------------------------------------
      // Versor constructors
      //
      // Versors are the invertible elements of the algebra that act on geometric
      // primitives via the sandwich product. Their construction requires knowing the
      // model's line and translation representations.
      // ---------------------------------------------------------------------------

      /**
       * Constructs a rotor encoding rotation by angle theta (radians) about the
       * given bivector axis B. B must be a unit bivector under the model's norm
       * convention. The rotor takes the form R = cos(theta/2) - sin(theta/2) * B
       * and encodes a rotation about a line through the model's origin.
       */
      { model.rotor(mv, s) } -> std::same_as<typename ModelT::Multivector>;

      /**
       * Constructs a translator encoding a rigid displacement along the direction
       * and magnitude encoded in t. Only well-defined in models with a degenerate
       * or null basis (PGA, CGA); models that do not support translation as a versor
       * (e.g. VGA) should signal an error rather than return a meaningless result.
       */
      { model.translator(mv) } -> std::same_as<typename ModelT::Multivector>;

      /**
       * Constructs a motor (rigid body displacement) from a rotor R and translator
       * T. A motor encodes a general screw motion — simultaneous rotation and
       * translation along an axis — and is the composition T * R. Motors are the
       * most general versors in PGA and CGA rigid body geometry.
       */
      { model.motor(mv, mv) } -> std::same_as<typename ModelT::Multivector>;

      // ---------------------------------------------------------------------------
      // Versor application
      // ---------------------------------------------------------------------------

      /**
       * Applies a unit versor V to multivector X via V * X * ~V. Valid only when
       * V * ~V = 1; using this on a non-unit versor silently produces incorrect
       * results.
       */
      { model.apply(mv, mv) } -> std::same_as<typename ModelT::Multivector>;

      // ---------------------------------------------------------------------------
      // Join and meet
      //
      // While join and meet reduce to the outer and regressive products, their
      // geometric interpretation — span and intersection of primitives — is only
      // meaningful in the context of a model's embedding conventions. The default
      // implementation via outer() and regress() is correct for most models.
      // ---------------------------------------------------------------------------

      /**
       * Returns the join (projective span) of two geometric primitives. For two
       * points this is the line through them; for a point and a line this is the
       * plane containing them. Implemented via the outer product in most models.
       */
      { model.join(mv, mv) } -> std::same_as<typename ModelT::Multivector>;

      /**
       * Returns the meet (intersection) of two geometric primitives. For two planes
       * this is their line of intersection; for a plane and a line this is their
       * point of intersection. Implemented via the regressive product in most models.
       */
      { model.meet(mv, mv) } -> std::same_as<typename ModelT::Multivector>;
    };

}  // namespace ndyn::math
