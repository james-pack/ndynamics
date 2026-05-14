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
    // Basis vector factory methods. These are the generic names and mirror
    // the factory methods on Multivectors. Specific geometries may include names tied more directly
    // to particular uses, but these should always be available.
    (G::NUM_PHYSICAL_DIMENSIONS < 1 ||
     requires(const G::Multivector& a) {
       { G::e0() } -> IsMultivectorLike<G>;
       { G::get_e0(a) } -> ScalarLike<G>;
     }) and  //
    (G::NUM_PHYSICAL_DIMENSIONS < 2 ||
     requires(const G::Multivector& a) {
       { G::e1() } -> IsMultivectorLike<G>;
       { G::get_e1(a) } -> ScalarLike<G>;
     }) and  //
    (G::NUM_PHYSICAL_DIMENSIONS < 3 ||
     requires(const G::Multivector& a) {
       { G::e2() } -> IsMultivectorLike<G>;
       //{ G::get_e2(a) } -> ScalarLike<G>;
     }) and  //
    (G::NUM_PHYSICAL_DIMENSIONS < 4 ||
     requires(const G::Multivector& a) {
       { G::e3() } -> IsMultivectorLike<G>;
       { G::get_e3(a) } -> ScalarLike<G>;
     }) and  //
    (G::NUM_PHYSICAL_DIMENSIONS < 5 ||
     requires(const G::Multivector& a) {
       { G::e4() } -> IsMultivectorLike<G>;
       { G::get_e4(a) } -> ScalarLike<G>;
     }) and  //
    (G::NUM_PHYSICAL_DIMENSIONS < 6 ||
     requires(const G::Multivector& a) {
       { G::e5() } -> IsMultivectorLike<G>;
       { G::get_e5(a) } -> ScalarLike<G>;
     }) and  //
    (G::NUM_PHYSICAL_DIMENSIONS < 7 ||
     requires(const G::Multivector& a) {
       { G::e6() } -> IsMultivectorLike<G>;
       { G::get_e6(a) } -> ScalarLike<G>;
     }) and  //
    (G::NUM_PHYSICAL_DIMENSIONS < 8 ||
     requires(const G::Multivector& a) {
       { G::e7() } -> IsMultivectorLike<G>;
       { G::get_e7(a) } -> ScalarLike<G>;
     }) and  //
    true;

/**
 * Base concept required by all geometry implementations.
 */
template <typename G>
concept GeometryModel =     //
    HasGenericBases<G> and  //
    requires {
      typename G::Algebra;
      typename G::Multivector;
      typename G::Scalar;
      { G::NUM_PHYSICAL_DIMENSIONS } -> std::convertible_to<size_t>;
      { G::NUM_POSITIVE_BASES } -> std::convertible_to<size_t>;
      { G::NUM_NEGATIVE_BASES } -> std::convertible_to<size_t>;
      { G::NUM_ZERO_BASES } -> std::convertible_to<size_t>;
    } and  //
    requires(const G::Multivector& a) {
      // An empty meet or join is a valid operation. Its result represents the identity element of
      // the operation.
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

      // Factor a multivector into a number of points. Note that this function will only give
      // meaningful results if a was the join of one or more points.
      //{ G::factor(a) } -> std::same_as<std::vector<typename G::Multivector>>;
    } and  //
    true;

/**
 * Every non-vector space geometry must express what space it embeds. The main purpose of
 * expressing this embedding is to bridge construction and deconstruction primitives and operators
 * in conformal and projective GAs. This embedded space defines how the API for primitives and
 * operators should bridge from an intuitive vector space GA to a conformal or projective
 * representation.
 *
 * The embedded space itself is just another geometry, in the sense of GeometryModel, meaning
 * that this definition is potentially recursive. In most cases, the embedded space will be a
 * Euclidean vector space or potentially a Minkowski vector space.
 *
 * The scalar type of the embedded space must be the same as the scalar type of the geometry.
 * This restriction simplifies implementation and seems reasonable at this time. The restriction
 * could be lifted if necessary, but it would require specifying a mapping from one scalar type to
 * another.
 *
 * See the IsAtlas concept below as the typical way to provide the functionality in this concept is
 * to use an Atlas.
 */
template <typename G>
concept HasEmbeddedSpace =                                                                   //
    requires { typename G::EmbeddedSpace; } and                                              //
    GeometryModel<typename G::EmbeddedSpace> and                                             //
    requires { std::is_same_v<typename G::Scalar, typename G::EmbeddedSpace::Scalar>; } and  //
    requires(const G::Multivector& g, const typename G::EmbeddedSpace::Multivector& embed) {
      { G::lower(g) } -> IsMultivectorLike<typename G::EmbeddedSpace>;
      { G::lift(embed) } -> IsMultivectorLike<G>;
    } and  //
    true;

/**
 * An Atlas provides a map for multivectors between a containing space and an embedded space. Note
 * that there is no expectation that this map is consistent. In general, lower(lift(p)) != p, but in
 * practice, each atlas should provide some guarantee of consistency.
 *
 * Note on terminology: the term Atlas comes from differential geometry. The idea is that a manifold
 * is usually embedded in a higher dimensional space. For any point on the manifold, there exists at
 * least one mapping function, called a chart, that maps a point on the manifold to a lower
 * dimensional space. The set of all of these charts is called an atlas.
 *
 * We extend this notion of mapping points to mapping multivectors. All of the geometries modelled
 * here can express a point as a single multivector. But, multivectors can express more than points.
 * Each Atlas must achieve a useful consistency for the purpose at hand.
 */
template <typename Atlas>
concept IsAtlas =                                     //
    requires { typename Atlas::EmbeddedSpace; } and   //
    GeometryModel<typename Atlas::EmbeddedSpace> and  //
    requires {
      []<typename ContainingSpace>(const typename ContainingSpace::Multivector& contained,
                                   const typename Atlas::EmbeddedSpace::Multivector& embed)
        requires requires {
          {
            // lower() takes a Multivector in the ContainingSpace and maps it to a Multivector in
            // the EmbeddedSpace.
            Atlas::template lower<ContainingSpace>(contained)
          } -> IsMultivectorLike<ContainingSpace>;
          {
            // lift() takes a Multivector in the EmbeddedSpace and maps it to a Multivector in
            // the ContainingSpace.
            Atlas::template lift<ContainingSpace>(embed)
          } -> IsMultivectorLike<ContainingSpace>;
        }
      {};
    } and  //
    true;

/**
 * Expresses how a geometry models points in terms of the embedded space.
 *
 * The overall pattern is to construct a point in the embedded space and then use the
 * lift() method from the HasEmbeddedSpace concept to transform it into a point in this geometry.
 * Vector space geometries have a simple method for creating points from scalars, much like the API
 * you might expect for a normal vector in the style of vector analysis from Gibbs and Heaviside.
 */
template <typename G>
concept HasPoint =        //
    GeometryModel<G> and  //
    requires { requires G::NUM_PHYSICAL_DIMENSIONS >= 1; } and
    requires(const G::Multivector& m) {
      { G::origin() } -> IsMultivectorLike<G>;
      { G::is_point(m) } -> BoolLike;
    } and  //
    true;

template <typename G>
concept VectorSpaceGeometryModel =  //
    GeometryModel<G> and            //

    // Geometric primitives.
    HasPoint<G> and  //

    requires(typename G::Scalar* begin_iter, typename G::Scalar* end_iter) {
      { G::make_point(begin_iter, end_iter) } -> IsMultivectorLike<G>;
    } and  //

    (G::NUM_PHYSICAL_DIMENSIONS != 1 ||
     requires(const G::Scalar& x) {
       { G::make_point(x) } -> IsMultivectorLike<G>;
     }) and  //

    (G::NUM_PHYSICAL_DIMENSIONS != 2 ||
     requires(const G::Scalar& x) {
       { G::make_point(x, x) } -> IsMultivectorLike<G>;
     }) and  //

    (G::NUM_PHYSICAL_DIMENSIONS != 3 ||
     requires(const G::Scalar& x) {
       { G::make_point(x, x, x) } -> IsMultivectorLike<G>;
     }) and  //

    (G::NUM_PHYSICAL_DIMENSIONS != 4 ||
     requires(const G::Scalar& x) {
       { G::make_point(x, x, x, x) } -> IsMultivectorLike<G>;
     }) and  //

    // Fundamental operations.
    // HasRotor<G> and  //

    true;

template <typename G>
concept HasProjectiveBases =  //
    requires {
      { G::e_inf() } -> IsMultivectorLike<G>;
    };

template <typename G>
concept ProjectiveGeometryModel =  //
    GeometryModel<G> and           //
    HasProjectiveBases<G> and      //
    HasEmbeddedSpace<G> and        //

    // Geometric primitives.
    HasPoint<G> and  //
    // HasLine<G> and   //
    // HasPlane<G> and  //
    // Fundamental operations.
    // HasRotor<G> and       //
    // HasTranslator<G> and  //

    true;

template <typename G>
concept HasConformalBases =  //
    requires {
      { G::e_inf() } -> IsMultivectorLike<G>;
      { G::e_orig() } -> IsMultivectorLike<G>;

      { G::e_plus() } -> IsMultivectorLike<G>;
      { G::e_minus() } -> IsMultivectorLike<G>;
    };

template <typename G>
concept ConformalGeometryModel =  //
    GeometryModel<G> and          //
    HasConformalBases<G> and      //
    HasEmbeddedSpace<G> and       //

    // Geometric primitives.
    HasPoint<G> and
    // HasPointPair<G> and //
    // HasDirection<G> and                                        //
    // (G::NUM_PHYSICAL_DIMENSIONS < 2 || HasLine<G>) and  //
    // (G::NUM_PHYSICAL_DIMENSIONS < 2 || HasPlane<G>) and        //
    // (G::NUM_PHYSICAL_DIMENSIONS < 3 || HasHyperplane<G>) and   //
    // (G::NUM_PHYSICAL_DIMENSIONS < 2 || HasCircle<G>) and  //
    // (G::NUM_PHYSICAL_DIMENSIONS < 3 || HasSphere<G>) and       //
    // (G::NUM_PHYSICAL_DIMENSIONS < 4 || HasHypersphere<G>) and  //

    // Fundamental operations.
    // HasRotor<G> and       //
    // HasDilator<G> and     //
    // HasTranslator<G> and  //
    true;

}  // namespace ndyn::math
