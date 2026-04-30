#pragma once

#include <concepts>
#include <cstddef>

namespace ndyn::math {

/**
 * Helper concepts to assert that the given type is a form of the Multivector type on the geometry
 * G.
 */
/**
 * Checks if a type can be treated as a Multivector of Algebra G.
 * This is the most permissive; it allows anything where a G::Multivector can be created from that
 * type, including an implicit constructor, conversion operators, or a type cast.
 *
 * We use this concept most often to allow for flexibility in refactoring and optimizations.
 */
template <typename T, typename G>
concept IsMultivectorLike = std::convertible_to<T, typename G::Multivector>;

/**
 * Checks if the underlying type is the geometry's Multivector, ignoring qualifiers.
 */
template <typename T, typename G>
concept IsMultivector =
    std::is_same_v<std::remove_cvref_t<T>, typename std::remove_cvref_t<G>::Multivector>;

/**
 * Checks for an absolute type match including storage qualifiers (no refs, no const).
 */
template <typename T, typename G>
concept MultivectorExact = std::is_same_v<T, typename std::remove_cvref_t<G>::Multivector>;

/**
 * Similar concepts as above but for the Scalar type.
 */
template <typename T, typename G>
concept ScalarLike = std::convertible_to<T, typename G::Scalar>;

template <typename T, typename G>
concept IsScalar = std::is_same_v<std::remove_cvref_t<T>, typename std::remove_cvref_t<G>::Scalar>;

template <typename T, typename G>
concept ScalarExact = std::is_same_v<T, typename std::remove_cvref_t<G>::Scalar>;

template <typename T>
concept BoolLike = std::convertible_to<T, bool>;

template <typename G>
concept HasGenericBases =
    // Basis vector factory methods for physical dimensions. These are the generic names. Specific
    // geometries may include names tied more directly to particular uses, but these should always
    // be available.
    (G::NUM_PHYSICAL_DIMENSIONS < 1 ||
     requires(const G::Multivector& a) {
       { G::e1() } -> IsMultivectorLike<G>;
       { G::get_e1(a) } -> ScalarLike<G>;
     }) &&  //
    (G::NUM_PHYSICAL_DIMENSIONS < 2 ||
     requires(const G::Multivector& a) {
       { G::e2() } -> IsMultivectorLike<G>;
       { G::get_e2(a) } -> ScalarLike<G>;
     }) &&  //
    (G::NUM_PHYSICAL_DIMENSIONS < 3 ||
     requires(const G::Multivector& a) {
       { G::e3() } -> IsMultivectorLike<G>;
       { G::get_e3(a) } -> ScalarLike<G>;
     }) &&  //
    (G::NUM_PHYSICAL_DIMENSIONS < 4 ||
     requires(const G::Multivector& a) {
       { G::e4() } -> IsMultivectorLike<G>;
       { G::get_e4(a) } -> ScalarLike<G>;
     }) &&  //
    true;

/**
 * Base concept required by all geometry implementations.
 */
template <typename G>
concept GeometryModel =  //
    HasGenericBases<G> &&
    requires {
      typename G::Algebra;
      typename G::Multivector;
      typename G::Scalar;
      { G::NUM_PHYSICAL_DIMENSIONS } -> std::convertible_to<size_t>;
    } &&  //
    requires(const G::Multivector& a) {
      // An empty meet or join is a valid operation. It represents the identity element of the
      // operation.
      { G::meet() } -> IsMultivectorLike<G>;
      { G::join() } -> IsMultivectorLike<G>;
      // We expect the meet() and join() functions to be variadic, but regardless of implementation,
      // the single argument forms should exist.
      { G::meet(a) } -> IsMultivectorLike<G>;
      { G::join(a) } -> IsMultivectorLike<G>;
      { G::meet(a, a) } -> IsMultivectorLike<G>;
      { G::join(a, a) } -> IsMultivectorLike<G>;
      { G::meet(a, a, a) } -> IsMultivectorLike<G>;
      { G::join(a, a, a) } -> IsMultivectorLike<G>;
    } &&  //
    true;

template <typename G>
concept HasPoint =
    GeometryModel<G> &&  //
    requires { requires G::NUM_PHYSICAL_DIMENSIONS >= 1; } &&
    requires(const G::Multivector& m, typename G::Scalar x, typename G::Scalar& out, size_t count,
             typename G::Scalar* values) {
      { G::origin() } -> IsMultivectorLike<G>;
      { G::make_point(count, values) } -> IsMultivectorLike<G>;
      { G::is_point(m) } -> BoolLike;
      { G::is_normalized_point(m) } -> BoolLike;
    } &&
    (G::NUM_PHYSICAL_DIMENSIONS != 1 ||
     requires(const G::Multivector& m, typename G::Scalar x, typename G::Scalar& out) {
       { G::make_point(x) } -> IsMultivectorLike<G>;
       { G::extract_point(m, out) } -> std::same_as<void>;
     }) &&
    (G::NUM_PHYSICAL_DIMENSIONS != 2 ||
     requires(const G::Multivector& m, typename G::Scalar x, typename G::Scalar& out) {
       { G::make_point(x, x) } -> IsMultivectorLike<G>;
       { G::extract_point(m, out, out) } -> std::same_as<void>;
     }) &&
    (G::NUM_PHYSICAL_DIMENSIONS != 3 ||
     requires(const G::Multivector& m, typename G::Scalar x, typename G::Scalar& out) {
       { G::make_point(x, x, x) } -> IsMultivectorLike<G>;
       { G::extract_point(m, out, out, out) } -> std::same_as<void>;
     }) &&
    (G::NUM_PHYSICAL_DIMENSIONS != 4 ||
     requires(const G::Multivector& m, typename G::Scalar x, typename G::Scalar& out) {
       { G::make_point(x, x, x, x) } -> IsMultivectorLike<G>;
       { G::extract_point(m, out, out, out, out) } -> std::same_as<void>;
     });

template <typename G>
concept HasDirection =
    GeometryModel<G> &&  //
    requires { requires G::NUM_PHYSICAL_DIMENSIONS >= 1; } &&
    requires(const G::Multivector& m, typename G::Scalar x, typename G::Scalar& out) {
      { G::make_direction(m) } -> IsMultivectorLike<G>;
      { G::is_direction(m) } -> BoolLike;
    } &&
    (G::NUM_PHYSICAL_DIMENSIONS < 1 ||
     requires(const G::Multivector& m, typename G::Scalar x, typename G::Scalar& out) {
       { G::make_direction(x) } -> IsMultivectorLike<G>;
       { G::extract_direction(m, out) } -> std::same_as<void>;
     }) &&
    (G::NUM_PHYSICAL_DIMENSIONS < 2 ||
     requires(const G::Multivector& m, typename G::Scalar x, typename G::Scalar& out) {
       { G::make_direction(x, x) } -> IsMultivectorLike<G>;
       { G::extract_direction(m, out, out) } -> std::same_as<void>;
     }) &&
    (G::NUM_PHYSICAL_DIMENSIONS < 3 ||
     requires(const G::Multivector& m, typename G::Scalar x, typename G::Scalar& out) {
       { G::make_direction(x, x, x) } -> IsMultivectorLike<G>;
       { G::extract_direction(m, out, out, out) } -> std::same_as<void>;
     }) &&
    (G::NUM_PHYSICAL_DIMENSIONS < 4 ||
     requires(const G::Multivector& m, typename G::Scalar x, typename G::Scalar& out) {
       { G::make_direction(x, x, x, x) } -> IsMultivectorLike<G>;
       { G::extract_direction(m, out, out, out, out) } -> std::same_as<void>;
     });

template <typename G>
concept HasPointPair =
    GeometryModel<G> &&  //
    requires { requires G::NUM_PHYSICAL_DIMENSIONS >= 1; } &&
    requires(const G::Multivector& m, typename G::Multivector& out) {
      { G::make_point_pair(m, m) } -> IsMultivectorLike<G>;
      { G::is_point_pair(m) } -> BoolLike;
      { G::extract_point_pair(m, out, out) } -> std::same_as<void>;
    } &&
    (G::NUM_PHYSICAL_DIMENSIONS != 1 ||
     requires(const G::Multivector& m, typename G::Scalar x, typename G::Scalar& out) {
       { G::make_point_pair(x, x) } -> IsMultivectorLike<G>;
       { G::extract_point_pair(m, out, out) } -> std::same_as<void>;
     }) &&
    (G::NUM_PHYSICAL_DIMENSIONS != 2 ||
     requires(const G::Multivector& m, typename G::Scalar x, typename G::Scalar& out) {
       { G::make_point_pair(x, x, x, x) } -> IsMultivectorLike<G>;
       { G::extract_point_pair(m, out, out, out, out) } -> std::same_as<void>;
     }) &&
    (G::NUM_PHYSICAL_DIMENSIONS != 3 ||
     requires(const G::Multivector& m, typename G::Scalar x, typename G::Scalar& out) {
       { G::make_point_pair(x, x, x, x, x, x) } -> IsMultivectorLike<G>;
       { G::extract_point_pair(m, out, out, out, out, out, out) } -> std::same_as<void>;
     }) &&
    (G::NUM_PHYSICAL_DIMENSIONS != 4 ||
     requires(const G::Multivector& m, typename G::Scalar x, typename G::Scalar& out) {
       { G::make_point_pair(x, x, x, x, x, x, x, x) } -> IsMultivectorLike<G>;
       { G::extract_point_pair(m, out, out, out, out, out, out, out, out) } -> std::same_as<void>;
     });

template <typename G>
concept HasLine =
    GeometryModel<G> &&  //
    requires { requires G::NUM_PHYSICAL_DIMENSIONS >= 2; } &&
    requires(const G::Multivector& m, typename G::Scalar x, typename G::Scalar& out) {
      { G::make_line(m, m) } -> IsMultivectorLike<G>;
      { G::is_line(m) } -> BoolLike;
    } &&
    (G::NUM_PHYSICAL_DIMENSIONS != 2 ||
     requires(const G::Multivector& m, typename G::Scalar x, typename G::Scalar& out) {
       { G::make_line(x, x, x, x) } -> IsMultivectorLike<G>;
       { G::extract_line(m, out, out, out, out) } -> std::same_as<void>;
     }) &&
    (G::NUM_PHYSICAL_DIMENSIONS != 3 ||
     requires(const G::Multivector& m, typename G::Scalar x, typename G::Scalar& out) {
       { G::make_line(x, x, x, x, x, x) } -> IsMultivectorLike<G>;
       { G::extract_line(m, out, out, out, out, out, out) } -> std::same_as<void>;
     }) &&
    (G::NUM_PHYSICAL_DIMENSIONS != 4 ||
     requires(const G::Multivector& m, typename G::Scalar x, typename G::Scalar& out) {
       { G::make_line(x, x, x, x, x, x, x, x) } -> IsMultivectorLike<G>;
       { G::extract_line(m, out, out, out, out, out, out, out, out) } -> std::same_as<void>;
     });

template <typename G>
concept HasPlane =
    GeometryModel<G> &&  //
    requires { requires G::NUM_PHYSICAL_DIMENSIONS >= 2; } &&
    requires(const G::Multivector& m, typename G::Scalar x, typename G::Scalar& out) {
      { G::make_plane(x, x, x) } -> IsMultivectorLike<G>;
      { G::extract_plane(m, out, out, out) } -> std::same_as<void>;
      { G::is_plane(m) } -> BoolLike;
    } &&
    (G::NUM_PHYSICAL_DIMENSIONS < 3 ||
     requires(const G::Multivector& m, typename G::Scalar x, typename G::Scalar& out) {
       { G::make_plane(x, x, x, x) } -> IsMultivectorLike<G>;
       { G::extract_plane(m, out, out, out, out) } -> std::same_as<void>;
     }) &&
    (G::NUM_PHYSICAL_DIMENSIONS < 4 ||
     requires(const G::Multivector& m, typename G::Scalar x, typename G::Scalar& out) {
       { G::make_plane(x, x, x, x, x) } -> IsMultivectorLike<G>;
       { G::extract_plane(m, out, out, out, out, out) } -> std::same_as<void>;
     });

template <typename G>
concept HasCircle =
    GeometryModel<G> &&  //
    requires { requires G::NUM_PHYSICAL_DIMENSIONS >= 2; } &&
    requires(const G::Multivector& m, typename G::Scalar x, typename G::Multivector& out,
             typename G::Scalar& out_scalar) {
      { G::make_circle(m, m, m) } -> IsMultivectorLike<G>;
      { G::is_circle(m) } -> BoolLike;
      { G::extract_circle(m, out, out, out_scalar) } -> std::same_as<void>;
      { G::extract_circle(m, out, out, out) } -> std::same_as<void>;
    } &&
    (G::NUM_PHYSICAL_DIMENSIONS != 2 ||
     requires(const G::Multivector& m, typename G::Scalar x, typename G::Scalar& out) {
       { G::make_circle(x, x, x) } -> IsMultivectorLike<G>;
       { G::extract_circle(m, out, out, out) } -> std::same_as<void>;
     });

template <typename G>
concept HasSphere =
    GeometryModel<G> &&  //
    requires { requires G::NUM_PHYSICAL_DIMENSIONS >= 3; } &&
    requires(const G::Multivector& m, typename G::Scalar x, typename G::Scalar& out) {
      { G::make_sphere(x, x, x, x) } -> IsMultivectorLike<G>;
      { G::extract_sphere(m, out, out, out, out) } -> std::same_as<void>;
      { G::is_sphere(m) } -> BoolLike;
    };

template <typename G>
concept HasHyperplane =
    GeometryModel<G> &&  //
    requires { requires G::NUM_PHYSICAL_DIMENSIONS >= 3; } &&
    requires(const G::Multivector& m, typename G::Scalar x, typename G::Scalar& out) {
      { G::make_hyperplane(x, x, x, x) } -> IsMultivectorLike<G>;
      { G::extract_hyperplane(m, out, out, out, out) } -> std::same_as<void>;
      { G::is_hyperplane(m) } -> BoolLike;
    } &&
    (G::NUM_PHYSICAL_DIMENSIONS < 4 ||
     requires(const G::Multivector& m, typename G::Scalar x, typename G::Scalar& out) {
       { G::make_hyperplane(x, x, x, x, x) } -> IsMultivectorLike<G>;
       { G::extract_hyperplane(m, out, out, out, out, out) } -> std::same_as<void>;
     });

template <typename G>
concept HasHypersphere =
    GeometryModel<G> &&  //
    requires { requires G::NUM_PHYSICAL_DIMENSIONS >= 4; } &&
    requires(const G::Multivector& m, typename G::Scalar x, typename G::Scalar& out) {
      { G::make_hypersphere(x, x, x, x, x) } -> IsMultivectorLike<G>;
      { G::extract_hypersphere(m, out, out, out, out, out) } -> std::same_as<void>;
      { G::is_hypersphere(m) } -> BoolLike;
    };

/**
 * Rotation in a plane about the origin.
 *
 * A rotor is a unit even-grade multivector encoding a rotation in the plane
 * defined by a bivector, by a specified angle, about the origin. It is applied
 * via the sandwich product X -> R X R~. In any dimension, the plane of
 * rotation is specified as a unit bivector multivector paired with an angle.
 *
 * In 3D, an additional overload accepts an axis vector and angle as scalars,
 * since in 3D the plane of rotation is uniquely identified by its normal.
 * This overload is not meaningful in other dimensions: in 2D there is only
 * one plane of rotation, and in 4D and above a plane is not uniquely
 * identified by a normal vector.
 *
 * In 2D, an additional overload accepts a single angle scalar, since the
 * plane of rotation is the space itself and no further specification is needed.
 *
 * The angle parameter represents the full Euclidean angle of rotation in
 * radians. The implementation is responsible for the half-angle adjustment
 * required by the sandwich product convention.
 *
 * Extraction of scalar parameters is supported in 2D (angle only) and 3D
 * (axis and angle). In higher dimensions there is no meaningful Euclidean
 * parameterization to extract into, so no extraction overload is required.
 */
template <typename G>
concept HasRotor =
    GeometryModel<G> &&  //
    requires(const G::Multivector& m, typename G::Scalar x, typename G::Scalar& out) {
      { G::make_rotor(m, x) } -> IsMultivectorLike<G>;
      { G::is_rotor(m) } -> BoolLike;
    } &&
    (G::NUM_PHYSICAL_DIMENSIONS != 2 ||
     // In 2D, the geometry should support construction of a rotor from an angle.
     requires(const G::Multivector& m, typename G::Scalar x, typename G::Scalar& out) {
       { G::make_rotor(x) } -> IsMultivectorLike<G>;
       { G::extract_rotor(m, out) } -> std::same_as<void>;
     }) &&
    (G::NUM_PHYSICAL_DIMENSIONS != 3 ||
     requires(const G::Multivector& m, typename G::Scalar x, typename G::Scalar& out) {
       // In 3D, the geometry should support construction of a rotor from an axis through the origin
       // and an angle.
       { G::make_rotor(x, x, x, x) } -> IsMultivectorLike<G>;
       { G::extract_rotor(m, out, out, out, out) } -> std::same_as<void>;
     });

/**
 * Rigid displacement by a Euclidean vector.
 *
 * A translator is a versor that displaces geometric objects by a fixed vector
 * when applied via the sandwich product X -> M X M~. The displacement is
 * specified either as scalar components or as a direction multivector and a
 * scalar distance. Both forms produce the same displacement; the direction
 * multivector form is provided as a convenience when a direction is already
 * available in multivector form.
 *
 * The displacement parameters represent the full Euclidean displacement
 * applied to positions. The implementation is responsible for any internal
 * adjustment of parameters required by the algebra, such as the factor of
 * one half that appears in the standard translator construction.
 *
 * Extraction retrieves scalar displacement components only. There is no
 * extraction overload for the direction multivector form, as the scalar
 * components are the more fundamental Euclidean representation.
 */
template <typename G>
concept HasTranslator =
    GeometryModel<G> &&  //
    requires { requires G::NUM_PHYSICAL_DIMENSIONS >= 1; } &&
    requires(const G::Multivector& m, const G::Multivector& dir, typename G::Scalar x,
             typename G::Scalar& out) {
      { G::make_translator(x) } -> IsMultivectorLike<G>;
      { G::make_translator(dir, x) } -> IsMultivectorLike<G>;
      { G::extract_translator(m, out) } -> std::same_as<void>;
      { G::is_translator(m) } -> BoolLike;
    } &&
    (G::NUM_PHYSICAL_DIMENSIONS < 2 ||
     requires(const G::Multivector& m, const G::Multivector& dir, typename G::Scalar x,
              typename G::Scalar& out) {
       { G::make_translator(x, x) } -> IsMultivectorLike<G>;
       { G::make_translator(dir, x) } -> IsMultivectorLike<G>;
       { G::extract_translator(m, out, out) } -> std::same_as<void>;
     }) &&
    (G::NUM_PHYSICAL_DIMENSIONS < 3 ||
     requires(const G::Multivector& m, const G::Multivector& dir, typename G::Scalar x,
              typename G::Scalar& out) {
       { G::make_translator(x, x, x) } -> IsMultivectorLike<G>;
       { G::make_translator(dir, x) } -> IsMultivectorLike<G>;
       { G::extract_translator(m, out, out, out) } -> std::same_as<void>;
     }) &&
    (G::NUM_PHYSICAL_DIMENSIONS < 4 || requires(const G::Multivector& m, const G::Multivector& dir,
                                                typename G::Scalar x, typename G::Scalar& out) {
      { G::make_translator(x, x, x, x) } -> IsMultivectorLike<G>;
      { G::make_translator(dir, x) } -> IsMultivectorLike<G>;
      { G::extract_translator(m, out, out, out, out) } -> std::same_as<void>;
    });

template <typename G>
concept HasDilator =
    GeometryModel<G> &&  //
    requires(const G::Multivector& m, typename G::Scalar x, typename G::Scalar& out) {
      { G::make_dilator(x) } -> IsMultivectorLike<G>;
      { G::extract_dilator(m, out) } -> std::same_as<void>;
      { G::is_dilator(m) } -> BoolLike;
    };

template <typename G>
concept VectorspaceGeometryModel =  //
    GeometryModel<G> &&             //

    // Geometric primitives.
    HasPoint<G> &&      //
    HasDirection<G> &&  //

    // Fundamental operations.
    HasRotor<G> &&  //

    // Basis vector factory methods for physical dimensions.
    requires {
      { G::x() } -> IsMultivectorLike<G>;
      { G::get_x() } -> ScalarLike<G>;
    } &&
    (G::NUM_PHYSICAL_DIMENSIONS < 2 ||
     requires {
       { G::y() } -> IsMultivectorLike<G>;
       { G::get_y() } -> ScalarLike<G>;
     }) &&
    (G::NUM_PHYSICAL_DIMENSIONS < 3 ||
     requires {
       { G::z() } -> IsMultivectorLike<G>;
       { G::get_z() } -> ScalarLike<G>;
     }) &&
    (G::NUM_PHYSICAL_DIMENSIONS < 4 ||
     requires {
       { G::t() } -> IsMultivectorLike<G>;
       { G::get_t() } -> ScalarLike<G>;
     }) &&

    // Basis bivector factory methods for physical dimensions.
    (G::NUM_PHYSICAL_DIMENSIONS < 2 ||
     requires {
       { G::xy() } -> IsMultivectorLike<G>;
     }) &&
    (G::NUM_PHYSICAL_DIMENSIONS < 3 ||
     requires {
       { G::xz() } -> IsMultivectorLike<G>;
       { G::yz() } -> IsMultivectorLike<G>;
     }) &&
    (G::NUM_PHYSICAL_DIMENSIONS < 4 ||
     requires {
       { G::xt() } -> IsMultivectorLike<G>;
       { G::yt() } -> IsMultivectorLike<G>;
       { G::zt() } -> IsMultivectorLike<G>;
     }) &&  //
    true;

template <typename G>
concept ProjectiveGeometryModel =  //
    GeometryModel<G> &&            //

    // Basis vector factory methods for physical dimensions.
    requires {
      { G::x() } -> IsMultivectorLike<G>;
      { G::get_x() } -> ScalarLike<G>;
    } &&
    (G::NUM_PHYSICAL_DIMENSIONS < 2 ||
     requires {
       { G::y() } -> IsMultivectorLike<G>;
       { G::get_y() } -> ScalarLike<G>;
     }) &&
    (G::NUM_PHYSICAL_DIMENSIONS < 3 ||
     requires {
       { G::z() } -> IsMultivectorLike<G>;
       { G::get_z() } -> ScalarLike<G>;
     }) &&
    (G::NUM_PHYSICAL_DIMENSIONS < 4 ||
     requires {
       { G::w() } -> IsMultivectorLike<G>;
       { G::get_w() } -> ScalarLike<G>;
     }) &&

    // Basis bivector factory methods for physical dimensions.
    (G::NUM_PHYSICAL_DIMENSIONS < 2 ||
     requires {
       { G::xy() } -> IsMultivectorLike<G>;
     }) &&
    (G::NUM_PHYSICAL_DIMENSIONS < 3 ||
     requires {
       { G::xz() } -> IsMultivectorLike<G>;
       { G::yz() } -> IsMultivectorLike<G>;
     }) &&
    (G::NUM_PHYSICAL_DIMENSIONS < 4 ||
     requires {
       { G::xw() } -> IsMultivectorLike<G>;
       { G::yw() } -> IsMultivectorLike<G>;
       { G::zw() } -> IsMultivectorLike<G>;
     }) &&  //

    // Geometric primitives.
    HasPoint<G> &&                                           //
    HasDirection<G> &&                                       //
    (G::NUM_PHYSICAL_DIMENSIONS < 2 || HasLine<G>) &&        //
    (G::NUM_PHYSICAL_DIMENSIONS < 2 || HasPlane<G>) &&       //
    (G::NUM_PHYSICAL_DIMENSIONS < 3 || HasHyperplane<G>) &&  //

    // Fundamental operations.
    HasRotor<G> &&       //
    HasTranslator<G> &&  //
    true;

template <typename G>
concept HasConformalBases =  //
    GeometryModel<G> &&      //

    // Basis vector factory methods for physical dimensions.
    requires(const G::Multivector& a) {
      { G::gamma0() } -> IsMultivectorLike<G>;
      { G::get_gamma0(a) } -> ScalarLike<G>;
    } &&
    (G::NUM_PHYSICAL_DIMENSIONS < 2 ||
     requires(const G::Multivector& a) {
       { G::gamma1() } -> IsMultivectorLike<G>;
       { G::get_gamma1(a) } -> ScalarLike<G>;
     }) &&
    (G::NUM_PHYSICAL_DIMENSIONS < 3 ||
     requires(const G::Multivector& a) {
       { G::gamma2() } -> IsMultivectorLike<G>;
       { G::get_gamma2(a) } -> ScalarLike<G>;
     }) &&
    (G::NUM_PHYSICAL_DIMENSIONS < 4 ||
     requires(const G::Multivector& a) {
       { G::gamma3() } -> IsMultivectorLike<G>;
       { G::get_gamma3(a) } -> ScalarLike<G>;
     }) &&

    // Conformal basis vector factory methods.
    requires {
      { G::e_inf() } -> IsMultivectorLike<G>;
      { G::e_orig() } -> IsMultivectorLike<G>;
    } &&

    // Basis bivector factory methods for physical dimensions.
    (G::NUM_PHYSICAL_DIMENSIONS < 2 ||
     requires {
       { G::gamma01() } -> IsMultivectorLike<G>;
     }) &&
    (G::NUM_PHYSICAL_DIMENSIONS < 3 ||
     requires {
       { G::gamma02() } -> IsMultivectorLike<G>;
       { G::gamma12() } -> IsMultivectorLike<G>;
     }) &&
    (G::NUM_PHYSICAL_DIMENSIONS < 4 ||
     requires {
       { G::gamma03() } -> IsMultivectorLike<G>;
       { G::gamma13() } -> IsMultivectorLike<G>;
       { G::gamma23() } -> IsMultivectorLike<G>;
     }) &&  //
    true;

template <typename G>
concept ConformalGeometryModel =  //
    GeometryModel<G> &&           //
    HasConformalBases<G> &&       //

    // General conformal primitives as the combination (usually, join) of multiple points.
    requires(const G::Multivector& point) {
      // Make an ideal point from a point. This can be considered a point on the boundary of the
      // conformal manifold.
      { G::make_flat(point) } -> IsMultivectorLike<G>;
      // Make a line from two points.
      { G::make_flat(point, point) } -> IsMultivectorLike<G>;
      // Make a plane from three points.
      { G::make_flat(point, point, point) } -> IsMultivectorLike<G>;
      // Make a 3D hyperplane from four points.
      { G::make_flat(point, point, point, point) } -> IsMultivectorLike<G>;
      // Make a 4D hyperplane from five points.
      { G::make_flat(point, point, point, point, point) } -> IsMultivectorLike<G>;

      // Make a point from a point. Kind of stupid, but we are expecting a variadic implementation,
      // so this method should exist.
      { G::make_round(point) } -> IsMultivectorLike<G>;
      // Make a point pair from two points.
      { G::make_round(point, point) } -> IsMultivectorLike<G>;
      // Make a circle from three points.
      { G::make_round(point, point, point) } -> IsMultivectorLike<G>;
      // Make a sphere from four points.
      { G::make_round(point, point, point, point) } -> IsMultivectorLike<G>;
      // Make a hypersphere from five points.
      { G::make_round(point, point, point, point, point) } -> IsMultivectorLike<G>;
    } &&

    // Geometric primitives.
    HasPoint<G> &&      //
    HasPointPair<G> &&  //
    // HasDirection<G> &&                                        //
    (G::NUM_PHYSICAL_DIMENSIONS < 2 || HasLine<G>) &&  //
    //(G::NUM_PHYSICAL_DIMENSIONS < 2 || HasPlane<G>) &&        //
    //(G::NUM_PHYSICAL_DIMENSIONS < 3 || HasHyperplane<G>) &&   //
    (G::NUM_PHYSICAL_DIMENSIONS < 2 || HasCircle<G>) &&  //
    //(G::NUM_PHYSICAL_DIMENSIONS < 3 || HasSphere<G>) &&       //
    //(G::NUM_PHYSICAL_DIMENSIONS < 4 || HasHypersphere<G>) &&  //

    // Fundamental operations.
    HasRotor<G> &&       //
    HasDilator<G> &&     //
    HasTranslator<G> &&  //
    true;

}  // namespace ndyn::math
