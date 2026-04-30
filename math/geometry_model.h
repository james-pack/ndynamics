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

template <typename G>
concept HasGenericBases =
    // Basis vector factory methods for physical dimensions. These are the generic names. Specific
    // geometries may include names tied more directly to particular uses, but these should always
    // be available.
    (G::NUM_PHYSICAL_DIMENSIONS < 1 ||
     requires(const G::Multivector& a) {
       { G::e1() } -> std::same_as<typename G::Multivector>;
       { G::get_e1(a) } -> std::same_as<typename G::Scalar>;
     }) &&  //
    (G::NUM_PHYSICAL_DIMENSIONS < 2 ||
     requires(const G::Multivector& a) {
       { G::e2() } -> std::same_as<typename G::Multivector>;
       { G::get_e2(a) } -> std::same_as<typename G::Scalar>;
     }) &&  //
    (G::NUM_PHYSICAL_DIMENSIONS < 3 ||
     requires(const G::Multivector& a) {
       { G::e3() } -> std::same_as<typename G::Multivector>;
       { G::get_e3(a) } -> std::same_as<typename G::Scalar>;
     }) &&  //
    (G::NUM_PHYSICAL_DIMENSIONS < 4 ||
     requires(const G::Multivector& a) {
       { G::e4() } -> std::same_as<typename G::Multivector>;
       { G::get_e4(a) } -> std::same_as<typename G::Scalar>;
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
    requires(const G::Multivector& a, const G::Multivector& b) {
      { G::meet(a, b) } -> std::same_as<typename G::Multivector>;
      { G::join(a, b) } -> std::same_as<typename G::Multivector>;
    } &&  //
    true;

template <typename G>
concept HasPoint =
    GeometryModel<G> &&  //
    requires { requires G::NUM_PHYSICAL_DIMENSIONS >= 1; } &&
    requires(const G::Multivector& m, typename G::Scalar x, typename G::Scalar& out, size_t count,
             typename G::Scalar* values) {
      { G::origin() } -> std::same_as<typename G::Multivector>;
      { G::make_point(count, values) } -> std::same_as<typename G::Multivector>;
      { G::is_point(m) } -> std::same_as<bool>;
      { G::is_normalized_point(m) } -> std::same_as<bool>;
    } &&
    (G::NUM_PHYSICAL_DIMENSIONS != 1 ||
     requires(const G::Multivector& m, typename G::Scalar x, typename G::Scalar& out) {
       { G::make_point(x) } -> std::same_as<typename G::Multivector>;
       { G::extract_point(m, out) } -> std::same_as<void>;
     }) &&
    (G::NUM_PHYSICAL_DIMENSIONS != 2 ||
     requires(const G::Multivector& m, typename G::Scalar x, typename G::Scalar& out) {
       { G::make_point(x, x) } -> std::same_as<typename G::Multivector>;
       { G::extract_point(m, out, out) } -> std::same_as<void>;
     }) &&
    (G::NUM_PHYSICAL_DIMENSIONS != 3 ||
     requires(const G::Multivector& m, typename G::Scalar x, typename G::Scalar& out) {
       { G::make_point(x, x, x) } -> std::same_as<typename G::Multivector>;
       { G::extract_point(m, out, out, out) } -> std::same_as<void>;
     }) &&
    (G::NUM_PHYSICAL_DIMENSIONS != 4 ||
     requires(const G::Multivector& m, typename G::Scalar x, typename G::Scalar& out) {
       { G::make_point(x, x, x, x) } -> std::same_as<typename G::Multivector>;
       { G::extract_point(m, out, out, out, out) } -> std::same_as<void>;
     });

template <typename G>
concept HasDirection =
    GeometryModel<G> &&  //
    requires { requires G::NUM_PHYSICAL_DIMENSIONS >= 1; } &&
    requires(const G::Multivector& m, typename G::Scalar x, typename G::Scalar& out) {
      { G::make_direction(m) } -> std::same_as<typename G::Multivector>;
      { G::is_direction(m) } -> std::same_as<bool>;
    } &&
    (G::NUM_PHYSICAL_DIMENSIONS < 1 ||
     requires(const G::Multivector& m, typename G::Scalar x, typename G::Scalar& out) {
       { G::make_direction(x) } -> std::same_as<typename G::Multivector>;
       { G::extract_direction(m, out) } -> std::same_as<void>;
     }) &&
    (G::NUM_PHYSICAL_DIMENSIONS < 2 ||
     requires(const G::Multivector& m, typename G::Scalar x, typename G::Scalar& out) {
       { G::make_direction(x, x) } -> std::same_as<typename G::Multivector>;
       { G::extract_direction(m, out, out) } -> std::same_as<void>;
     }) &&
    (G::NUM_PHYSICAL_DIMENSIONS < 3 ||
     requires(const G::Multivector& m, typename G::Scalar x, typename G::Scalar& out) {
       { G::make_direction(x, x, x) } -> std::same_as<typename G::Multivector>;
       { G::extract_direction(m, out, out, out) } -> std::same_as<void>;
     }) &&
    (G::NUM_PHYSICAL_DIMENSIONS < 4 ||
     requires(const G::Multivector& m, typename G::Scalar x, typename G::Scalar& out) {
       { G::make_direction(x, x, x, x) } -> std::same_as<typename G::Multivector>;
       { G::extract_direction(m, out, out, out, out) } -> std::same_as<void>;
     });

template <typename G>
concept HasPointPair =
    GeometryModel<G> &&  //
    requires { requires G::NUM_PHYSICAL_DIMENSIONS >= 1; } &&
    requires(const G::Multivector& m, typename G::Multivector& out) {
      { G::make_point_pair(m, m) } -> std::same_as<typename G::Multivector>;
      { G::is_point_pair(m) } -> std::same_as<bool>;
      { G::extract_point_pair(m, out, out) } -> std::same_as<void>;
    } &&
    (G::NUM_PHYSICAL_DIMENSIONS != 1 ||
     requires(const G::Multivector& m, typename G::Scalar x, typename G::Scalar& out) {
       { G::make_point_pair(x, x) } -> std::same_as<typename G::Multivector>;
       { G::extract_point_pair(m, out, out) } -> std::same_as<void>;
     }) &&
    (G::NUM_PHYSICAL_DIMENSIONS != 2 ||
     requires(const G::Multivector& m, typename G::Scalar x, typename G::Scalar& out) {
       { G::make_point_pair(x, x, x, x) } -> std::same_as<typename G::Multivector>;
       { G::extract_point_pair(m, out, out, out, out) } -> std::same_as<void>;
     }) &&
    (G::NUM_PHYSICAL_DIMENSIONS != 3 ||
     requires(const G::Multivector& m, typename G::Scalar x, typename G::Scalar& out) {
       { G::make_point_pair(x, x, x, x, x, x) } -> std::same_as<typename G::Multivector>;
       { G::extract_point_pair(m, out, out, out, out, out, out) } -> std::same_as<void>;
     }) &&
    (G::NUM_PHYSICAL_DIMENSIONS != 4 ||
     requires(const G::Multivector& m, typename G::Scalar x, typename G::Scalar& out) {
       { G::make_point_pair(x, x, x, x, x, x, x, x) } -> std::same_as<typename G::Multivector>;
       { G::extract_point_pair(m, out, out, out, out, out, out, out, out) } -> std::same_as<void>;
     });

template <typename G>
concept HasLine =
    GeometryModel<G> &&  //
    requires { requires G::NUM_PHYSICAL_DIMENSIONS >= 2; } &&
    requires(const G::Multivector& m, typename G::Scalar x, typename G::Scalar& out) {
      { G::make_line(m, m) } -> std::same_as<typename G::Multivector>;
      { G::is_line(m) } -> std::same_as<bool>;
    } &&
    (G::NUM_PHYSICAL_DIMENSIONS != 2 ||
     requires(const G::Multivector& m, typename G::Scalar x, typename G::Scalar& out) {
       { G::make_line(x, x, x, x) } -> std::same_as<typename G::Multivector>;
       { G::extract_line(m, out, out, out, out) } -> std::same_as<void>;
     }) &&
    (G::NUM_PHYSICAL_DIMENSIONS != 3 ||
     requires(const G::Multivector& m, typename G::Scalar x, typename G::Scalar& out) {
       { G::make_line(x, x, x, x, x, x) } -> std::same_as<typename G::Multivector>;
       { G::extract_line(m, out, out, out, out, out, out) } -> std::same_as<void>;
     }) &&
    (G::NUM_PHYSICAL_DIMENSIONS != 4 ||
     requires(const G::Multivector& m, typename G::Scalar x, typename G::Scalar& out) {
       { G::make_line(x, x, x, x, x, x, x, x) } -> std::same_as<typename G::Multivector>;
       { G::extract_line(m, out, out, out, out, out, out, out, out) } -> std::same_as<void>;
     });

template <typename G>
concept HasPlane =
    GeometryModel<G> &&  //
    requires { requires G::NUM_PHYSICAL_DIMENSIONS >= 2; } &&
    requires(const G::Multivector& m, typename G::Scalar x, typename G::Scalar& out) {
      { G::make_plane(x, x, x) } -> std::same_as<typename G::Multivector>;
      { G::extract_plane(m, out, out, out) } -> std::same_as<void>;
      { G::is_plane(m) } -> std::same_as<bool>;
    } &&
    (G::NUM_PHYSICAL_DIMENSIONS < 3 ||
     requires(const G::Multivector& m, typename G::Scalar x, typename G::Scalar& out) {
       { G::make_plane(x, x, x, x) } -> std::same_as<typename G::Multivector>;
       { G::extract_plane(m, out, out, out, out) } -> std::same_as<void>;
     }) &&
    (G::NUM_PHYSICAL_DIMENSIONS < 4 ||
     requires(const G::Multivector& m, typename G::Scalar x, typename G::Scalar& out) {
       { G::make_plane(x, x, x, x, x) } -> std::same_as<typename G::Multivector>;
       { G::extract_plane(m, out, out, out, out, out) } -> std::same_as<void>;
     });

template <typename G>
concept HasCircle =
    GeometryModel<G> &&  //
    requires { requires G::NUM_PHYSICAL_DIMENSIONS >= 2; } &&
    requires(const G::Multivector& m, typename G::Scalar x, typename G::Multivector& out,
             typename G::Scalar& out_scalar) {
      { G::make_circle(m, m, m) } -> std::same_as<typename G::Multivector>;
      { G::is_circle(m) } -> std::same_as<bool>;
      { G::extract_circle(m, out, out, out_scalar) } -> std::same_as<void>;
      { G::extract_circle(m, out, out, out) } -> std::same_as<void>;
    } &&
    (G::NUM_PHYSICAL_DIMENSIONS != 2 ||
     requires(const G::Multivector& m, typename G::Scalar x, typename G::Scalar& out) {
       { G::make_circle(x, x, x) } -> std::same_as<typename G::Multivector>;
       { G::extract_circle(m, out, out, out) } -> std::same_as<void>;
     });

template <typename G>
concept HasSphere =
    GeometryModel<G> &&  //
    requires { requires G::NUM_PHYSICAL_DIMENSIONS >= 3; } &&
    requires(const G::Multivector& m, typename G::Scalar x, typename G::Scalar& out) {
      { G::make_sphere(x, x, x, x) } -> std::same_as<typename G::Multivector>;
      { G::extract_sphere(m, out, out, out, out) } -> std::same_as<void>;
      { G::is_sphere(m) } -> std::same_as<bool>;
    };

template <typename G>
concept HasHyperplane =
    GeometryModel<G> &&  //
    requires { requires G::NUM_PHYSICAL_DIMENSIONS >= 3; } &&
    requires(const G::Multivector& m, typename G::Scalar x, typename G::Scalar& out) {
      { G::make_hyperplane(x, x, x, x) } -> std::same_as<typename G::Multivector>;
      { G::extract_hyperplane(m, out, out, out, out) } -> std::same_as<void>;
      { G::is_hyperplane(m) } -> std::same_as<bool>;
    } &&
    (G::NUM_PHYSICAL_DIMENSIONS < 4 ||
     requires(const G::Multivector& m, typename G::Scalar x, typename G::Scalar& out) {
       { G::make_hyperplane(x, x, x, x, x) } -> std::same_as<typename G::Multivector>;
       { G::extract_hyperplane(m, out, out, out, out, out) } -> std::same_as<void>;
     });

template <typename G>
concept HasHypersphere =
    GeometryModel<G> &&  //
    requires { requires G::NUM_PHYSICAL_DIMENSIONS >= 4; } &&
    requires(const G::Multivector& m, typename G::Scalar x, typename G::Scalar& out) {
      { G::make_hypersphere(x, x, x, x, x) } -> std::same_as<typename G::Multivector>;
      { G::extract_hypersphere(m, out, out, out, out, out) } -> std::same_as<void>;
      { G::is_hypersphere(m) } -> std::same_as<bool>;
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
      { G::make_rotor(m, x) } -> std::same_as<typename G::Multivector>;
      { G::is_rotor(m) } -> std::same_as<bool>;
    } &&
    (G::NUM_PHYSICAL_DIMENSIONS != 2 ||
     // In 2D, the geometry should support construction of a rotor from an angle.
     requires(const G::Multivector& m, typename G::Scalar x, typename G::Scalar& out) {
       { G::make_rotor(x) } -> std::same_as<typename G::Multivector>;
       { G::extract_rotor(m, out) } -> std::same_as<void>;
     }) &&
    (G::NUM_PHYSICAL_DIMENSIONS != 3 ||
     requires(const G::Multivector& m, typename G::Scalar x, typename G::Scalar& out) {
       // In 3D, the geometry should support construction of a rotor from an axis through the origin
       // and an angle.
       { G::make_rotor(x, x, x, x) } -> std::same_as<typename G::Multivector>;
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
      { G::make_translator(x) } -> std::same_as<typename G::Multivector>;
      { G::make_translator(dir, x) } -> std::same_as<typename G::Multivector>;
      { G::extract_translator(m, out) } -> std::same_as<void>;
      { G::is_translator(m) } -> std::same_as<bool>;
    } &&
    (G::NUM_PHYSICAL_DIMENSIONS < 2 ||
     requires(const G::Multivector& m, const G::Multivector& dir, typename G::Scalar x,
              typename G::Scalar& out) {
       { G::make_translator(x, x) } -> std::same_as<typename G::Multivector>;
       { G::make_translator(dir, x) } -> std::same_as<typename G::Multivector>;
       { G::extract_translator(m, out, out) } -> std::same_as<void>;
     }) &&
    (G::NUM_PHYSICAL_DIMENSIONS < 3 ||
     requires(const G::Multivector& m, const G::Multivector& dir, typename G::Scalar x,
              typename G::Scalar& out) {
       { G::make_translator(x, x, x) } -> std::same_as<typename G::Multivector>;
       { G::make_translator(dir, x) } -> std::same_as<typename G::Multivector>;
       { G::extract_translator(m, out, out, out) } -> std::same_as<void>;
     }) &&
    (G::NUM_PHYSICAL_DIMENSIONS < 4 || requires(const G::Multivector& m, const G::Multivector& dir,
                                                typename G::Scalar x, typename G::Scalar& out) {
      { G::make_translator(x, x, x, x) } -> std::same_as<typename G::Multivector>;
      { G::make_translator(dir, x) } -> std::same_as<typename G::Multivector>;
      { G::extract_translator(m, out, out, out, out) } -> std::same_as<void>;
    });

template <typename G>
concept HasDilator =
    GeometryModel<G> &&  //
    requires(const G::Multivector& m, typename G::Scalar x, typename G::Scalar& out) {
      { G::make_dilator(x) } -> std::same_as<typename G::Multivector>;
      { G::extract_dilator(m, out) } -> std::same_as<void>;
      { G::is_dilator(m) } -> std::same_as<bool>;
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
      { G::x() } -> std::same_as<typename G::Multivector>;
      { G::get_x() } -> std::same_as<typename G::Scalar>;
    } &&
    (G::NUM_PHYSICAL_DIMENSIONS < 2 ||
     requires {
       { G::y() } -> std::same_as<typename G::Multivector>;
       { G::get_y() } -> std::same_as<typename G::Scalar>;
     }) &&
    (G::NUM_PHYSICAL_DIMENSIONS < 3 ||
     requires {
       { G::z() } -> std::same_as<typename G::Multivector>;
       { G::get_z() } -> std::same_as<typename G::Scalar>;
     }) &&
    (G::NUM_PHYSICAL_DIMENSIONS < 4 ||
     requires {
       { G::t() } -> std::same_as<typename G::Multivector>;
       { G::get_t() } -> std::same_as<typename G::Scalar>;
     }) &&

    // Basis bivector factory methods for physical dimensions.
    (G::NUM_PHYSICAL_DIMENSIONS < 2 ||
     requires {
       { G::xy() } -> std::same_as<typename G::Multivector>;
     }) &&
    (G::NUM_PHYSICAL_DIMENSIONS < 3 ||
     requires {
       { G::xz() } -> std::same_as<typename G::Multivector>;
       { G::yz() } -> std::same_as<typename G::Multivector>;
     }) &&
    (G::NUM_PHYSICAL_DIMENSIONS < 4 ||
     requires {
       { G::xt() } -> std::same_as<typename G::Multivector>;
       { G::yt() } -> std::same_as<typename G::Multivector>;
       { G::zt() } -> std::same_as<typename G::Multivector>;
     }) &&  //
    true;

template <typename G>
concept ProjectiveGeometryModel =  //
    GeometryModel<G> &&            //

    // Basis vector factory methods for physical dimensions.
    requires {
      { G::x() } -> std::same_as<typename G::Multivector>;
      { G::get_x() } -> std::same_as<typename G::Scalar>;
    } &&
    (G::NUM_PHYSICAL_DIMENSIONS < 2 ||
     requires {
       { G::y() } -> std::same_as<typename G::Multivector>;
       { G::get_y() } -> std::same_as<typename G::Scalar>;
     }) &&
    (G::NUM_PHYSICAL_DIMENSIONS < 3 ||
     requires {
       { G::z() } -> std::same_as<typename G::Multivector>;
       { G::get_z() } -> std::same_as<typename G::Scalar>;
     }) &&
    (G::NUM_PHYSICAL_DIMENSIONS < 4 ||
     requires {
       { G::t() } -> std::same_as<typename G::Multivector>;
       { G::get_t() } -> std::same_as<typename G::Scalar>;
     }) &&

    // Basis bivector factory methods for physical dimensions.
    (G::NUM_PHYSICAL_DIMENSIONS < 2 ||
     requires {
       { G::xy() } -> std::same_as<typename G::Multivector>;
     }) &&
    (G::NUM_PHYSICAL_DIMENSIONS < 3 ||
     requires {
       { G::xz() } -> std::same_as<typename G::Multivector>;
       { G::yz() } -> std::same_as<typename G::Multivector>;
     }) &&
    (G::NUM_PHYSICAL_DIMENSIONS < 4 ||
     requires {
       { G::xt() } -> std::same_as<typename G::Multivector>;
       { G::yt() } -> std::same_as<typename G::Multivector>;
       { G::zt() } -> std::same_as<typename G::Multivector>;
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
      { G::gamma0() } -> std::same_as<typename G::Multivector>;
      { G::get_gamma0(a) } -> std::same_as<typename G::Scalar>;
    } &&
    (G::NUM_PHYSICAL_DIMENSIONS < 2 ||
     requires(const G::Multivector& a) {
       { G::gamma1() } -> std::same_as<typename G::Multivector>;
       { G::get_gamma1(a) } -> std::same_as<typename G::Scalar>;
     }) &&
    (G::NUM_PHYSICAL_DIMENSIONS < 3 ||
     requires(const G::Multivector& a) {
       { G::gamma2() } -> std::same_as<typename G::Multivector>;
       { G::get_gamma2(a) } -> std::same_as<typename G::Scalar>;
     }) &&
    (G::NUM_PHYSICAL_DIMENSIONS < 4 ||
     requires(const G::Multivector& a) {
       { G::gamma3() } -> std::same_as<typename G::Multivector>;
       { G::get_gamma3(a) } -> std::same_as<typename G::Scalar>;
     }) &&

    // Conformal basis vector factory methods.
    requires {
      { G::e_inf() } -> std::same_as<typename G::Multivector>;
      { G::e_orig() } -> std::same_as<typename G::Multivector>;
    } &&

    // Basis bivector factory methods for physical dimensions.
    (G::NUM_PHYSICAL_DIMENSIONS < 2 ||
     requires {
       { G::gamma01() } -> std::same_as<typename G::Multivector>;
     }) &&
    (G::NUM_PHYSICAL_DIMENSIONS < 3 ||
     requires {
       { G::gamma02() } -> std::same_as<typename G::Multivector>;
       { G::gamma12() } -> std::same_as<typename G::Multivector>;
     }) &&
    (G::NUM_PHYSICAL_DIMENSIONS < 4 ||
     requires {
       { G::gamma03() } -> std::same_as<typename G::Multivector>;
       { G::gamma13() } -> std::same_as<typename G::Multivector>;
       { G::gamma23() } -> std::same_as<typename G::Multivector>;
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
      { G::make_flat(point) } -> std::same_as<typename G::Multivector>;
      // Make a line from two points.
      { G::make_flat(point, point) } -> std::same_as<typename G::Multivector>;
      // Make a plane from three points.
      { G::make_flat(point, point, point) } -> std::same_as<typename G::Multivector>;
      // Make a 3D hyperplane from four points.
      { G::make_flat(point, point, point, point) } -> std::same_as<typename G::Multivector>;
      // Make a 4D hyperplane from five points.
      { G::make_flat(point, point, point, point, point) } -> std::same_as<typename G::Multivector>;

      // Make a point from a point. Kind of stupid, but we are expecting a variadic implementation,
      // so this method should exist.
      { G::make_round(point) } -> std::same_as<typename G::Multivector>;
      // Make a point pair from two points.
      { G::make_round(point, point) } -> std::same_as<typename G::Multivector>;
      // Make a circle from three points.
      { G::make_round(point, point, point) } -> std::same_as<typename G::Multivector>;
      // Make a sphere from four points.
      { G::make_round(point, point, point, point) } -> std::same_as<typename G::Multivector>;
      // Make a hypersphere from five points.
      { G::make_round(point, point, point, point, point) } -> std::same_as<typename G::Multivector>;
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
