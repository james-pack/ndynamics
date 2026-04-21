#pragma once

#include <concepts>
#include <cstddef>

namespace ndyn::math {

/**
 * Base concept required by all geometry implementations.
 */
template <typename G>
concept GeometryModel = requires {
  typename G::Algebra;
  typename G::Multivector;
  typename G::Scalar;
  { G::NUM_PHYSICAL_DIMENSIONS } -> std::convertible_to<size_t>;
} && requires(G g, typename G::Multivector const& a, typename G::Multivector const& b) {
  { g.meet(a, b) } -> std::same_as<typename G::Multivector>;
  { g.join(a, b) } -> std::same_as<typename G::Multivector>;
};

template <typename G>
concept HasPoint =
    GeometryModel<G> &&  //
    requires { requires G::NUM_PHYSICAL_DIMENSIONS >= 1; } &&
    requires(G g, typename G::Multivector const& m, typename G::Scalar x, typename G::Scalar& out) {
      { g.make_point(x) } -> std::same_as<typename G::Multivector>;
      { g.extract_point(m, out) } -> std::same_as<void>;
      { g.is_point(m) } -> std::same_as<bool>;
    } &&
    (G::NUM_PHYSICAL_DIMENSIONS < 2 ||
     requires(G g, typename G::Multivector const& m, typename G::Scalar x,
              typename G::Scalar& out) {
       { g.make_point(x, x) } -> std::same_as<typename G::Multivector>;
       { g.extract_point(m, out, out) } -> std::same_as<void>;
     }) &&
    (G::NUM_PHYSICAL_DIMENSIONS < 3 ||
     requires(G g, typename G::Multivector const& m, typename G::Scalar x,
              typename G::Scalar& out) {
       { g.make_point(x, x, x) } -> std::same_as<typename G::Multivector>;
       { g.extract_point(m, out, out, out) } -> std::same_as<void>;
     }) &&
    (G::NUM_PHYSICAL_DIMENSIONS < 4 || requires(G g, typename G::Multivector const& m,
                                                typename G::Scalar x, typename G::Scalar& out) {
      { g.make_point(x, x, x, x) } -> std::same_as<typename G::Multivector>;
      { g.extract_point(m, out, out, out, out) } -> std::same_as<void>;
    });

template <typename G>
concept HasDirection =
    GeometryModel<G> &&  //
    requires { requires G::NUM_PHYSICAL_DIMENSIONS >= 1; } &&
    requires(G g, typename G::Multivector const& m, typename G::Scalar x, typename G::Scalar& out) {
      { g.make_direction(x) } -> std::same_as<typename G::Multivector>;
      { g.extract_direction(m, out) } -> std::same_as<void>;
      { g.is_direction(m) } -> std::same_as<bool>;
    } &&
    (G::NUM_PHYSICAL_DIMENSIONS < 2 ||
     requires(G g, typename G::Multivector const& m, typename G::Scalar x,
              typename G::Scalar& out) {
       { g.make_direction(x, x) } -> std::same_as<typename G::Multivector>;
       { g.extract_direction(m, out, out) } -> std::same_as<void>;
     }) &&
    (G::NUM_PHYSICAL_DIMENSIONS < 3 ||
     requires(G g, typename G::Multivector const& m, typename G::Scalar x,
              typename G::Scalar& out) {
       { g.make_direction(x, x, x) } -> std::same_as<typename G::Multivector>;
       { g.extract_direction(m, out, out, out) } -> std::same_as<void>;
     }) &&
    (G::NUM_PHYSICAL_DIMENSIONS < 4 || requires(G g, typename G::Multivector const& m,
                                                typename G::Scalar x, typename G::Scalar& out) {
      { g.make_direction(x, x, x, x) } -> std::same_as<typename G::Multivector>;
      { g.extract_direction(m, out, out, out, out) } -> std::same_as<void>;
    });

template <typename G>
concept HasPointPair =
    GeometryModel<G> &&  //
    requires { requires G::NUM_PHYSICAL_DIMENSIONS >= 2; } &&
    requires(G g, typename G::Multivector const& m, typename G::Scalar x, typename G::Scalar& out) {
      { g.make_point_pair(x, x, x, x) } -> std::same_as<typename G::Multivector>;
      { g.extract_point_pair(m, out, out, out, out) } -> std::same_as<void>;
      { g.is_point_pair(m) } -> std::same_as<bool>;
    } &&
    (G::NUM_PHYSICAL_DIMENSIONS < 3 ||
     requires(G g, typename G::Multivector const& m, typename G::Scalar x,
              typename G::Scalar& out) {
       { g.make_point_pair(x, x, x, x, x, x) } -> std::same_as<typename G::Multivector>;
       { g.extract_point_pair(m, out, out, out, out, out, out) } -> std::same_as<void>;
     }) &&
    (G::NUM_PHYSICAL_DIMENSIONS < 4 || requires(G g, typename G::Multivector const& m,
                                                typename G::Scalar x, typename G::Scalar& out) {
      { g.make_point_pair(x, x, x, x, x, x, x, x) } -> std::same_as<typename G::Multivector>;
      { g.extract_point_pair(m, out, out, out, out, out, out, out, out) } -> std::same_as<void>;
    });

template <typename G>
concept HasLine =
    GeometryModel<G> &&  //
    requires { requires G::NUM_PHYSICAL_DIMENSIONS >= 2; } &&
    requires(G g, typename G::Multivector const& m, typename G::Scalar x, typename G::Scalar& out) {
      { g.make_line(x, x, x, x) } -> std::same_as<typename G::Multivector>;
      { g.extract_line(m, out, out, out, out) } -> std::same_as<void>;
      { g.is_line(m) } -> std::same_as<bool>;
    } &&
    (G::NUM_PHYSICAL_DIMENSIONS < 3 ||
     requires(G g, typename G::Multivector const& m, typename G::Scalar x,
              typename G::Scalar& out) {
       { g.make_line(x, x, x, x, x, x) } -> std::same_as<typename G::Multivector>;
       { g.extract_line(m, out, out, out, out, out, out) } -> std::same_as<void>;
     }) &&
    (G::NUM_PHYSICAL_DIMENSIONS < 4 || requires(G g, typename G::Multivector const& m,
                                                typename G::Scalar x, typename G::Scalar& out) {
      { g.make_line(x, x, x, x, x, x, x, x) } -> std::same_as<typename G::Multivector>;
      { g.extract_line(m, out, out, out, out, out, out, out, out) } -> std::same_as<void>;
    });

template <typename G>
concept HasPlane =
    GeometryModel<G> &&  //
    requires { requires G::NUM_PHYSICAL_DIMENSIONS >= 2; } &&
    requires(G g, typename G::Multivector const& m, typename G::Scalar x, typename G::Scalar& out) {
      { g.make_plane(x, x, x) } -> std::same_as<typename G::Multivector>;
      { g.extract_plane(m, out, out, out) } -> std::same_as<void>;
      { g.is_plane(m) } -> std::same_as<bool>;
    } &&
    (G::NUM_PHYSICAL_DIMENSIONS < 3 ||
     requires(G g, typename G::Multivector const& m, typename G::Scalar x,
              typename G::Scalar& out) {
       { g.make_plane(x, x, x, x) } -> std::same_as<typename G::Multivector>;
       { g.extract_plane(m, out, out, out, out) } -> std::same_as<void>;
     }) &&
    (G::NUM_PHYSICAL_DIMENSIONS < 4 || requires(G g, typename G::Multivector const& m,
                                                typename G::Scalar x, typename G::Scalar& out) {
      { g.make_plane(x, x, x, x, x) } -> std::same_as<typename G::Multivector>;
      { g.extract_plane(m, out, out, out, out, out) } -> std::same_as<void>;
    });

template <typename G>
concept HasCircle =
    GeometryModel<G> &&  //
    requires { requires G::NUM_PHYSICAL_DIMENSIONS >= 2; } &&
    requires(G g, typename G::Multivector const& m, typename G::Scalar x, typename G::Scalar& out) {
      { g.make_circle(x, x, x) } -> std::same_as<typename G::Multivector>;
      { g.extract_circle(m, out, out, out) } -> std::same_as<void>;
      { g.is_circle(m) } -> std::same_as<bool>;
    } &&
    (G::NUM_PHYSICAL_DIMENSIONS < 3 ||
     requires(G g, typename G::Multivector const& m, typename G::Scalar x,
              typename G::Scalar& out) {
       { g.make_circle(x, x, x, x) } -> std::same_as<typename G::Multivector>;
       { g.extract_circle(m, out, out, out, out) } -> std::same_as<void>;
     }) &&
    (G::NUM_PHYSICAL_DIMENSIONS < 4 || requires(G g, typename G::Multivector const& m,
                                                typename G::Scalar x, typename G::Scalar& out) {
      { g.make_circle(x, x, x, x, x) } -> std::same_as<typename G::Multivector>;
      { g.extract_circle(m, out, out, out, out, out) } -> std::same_as<void>;
    });

template <typename G>
concept HasSphere =
    GeometryModel<G> &&  //
    requires { requires G::NUM_PHYSICAL_DIMENSIONS >= 3; } &&
    requires(G g, typename G::Multivector const& m, typename G::Scalar x, typename G::Scalar& out) {
      { g.make_sphere(x, x, x, x) } -> std::same_as<typename G::Multivector>;
      { g.extract_sphere(m, out, out, out, out) } -> std::same_as<void>;
      { g.is_sphere(m) } -> std::same_as<bool>;
    };

template <typename G>
concept HasHyperplane =
    GeometryModel<G> &&  //
    requires { requires G::NUM_PHYSICAL_DIMENSIONS >= 3; } &&
    requires(G g, typename G::Multivector const& m, typename G::Scalar x, typename G::Scalar& out) {
      { g.make_hyperplane(x, x, x, x) } -> std::same_as<typename G::Multivector>;
      { g.extract_hyperplane(m, out, out, out, out) } -> std::same_as<void>;
      { g.is_hyperplane(m) } -> std::same_as<bool>;
    } &&
    (G::NUM_PHYSICAL_DIMENSIONS < 4 || requires(G g, typename G::Multivector const& m,
                                                typename G::Scalar x, typename G::Scalar& out) {
      { g.make_hyperplane(x, x, x, x, x) } -> std::same_as<typename G::Multivector>;
      { g.extract_hyperplane(m, out, out, out, out, out) } -> std::same_as<void>;
    });

template <typename G>
concept HasHypersphere =
    GeometryModel<G> &&  //
    requires { requires G::NUM_PHYSICAL_DIMENSIONS >= 4; } &&
    requires(G g, typename G::Multivector const& m, typename G::Scalar x, typename G::Scalar& out) {
      { g.make_hypersphere(x, x, x, x, x) } -> std::same_as<typename G::Multivector>;
      { g.extract_hypersphere(m, out, out, out, out, out) } -> std::same_as<void>;
      { g.is_hypersphere(m) } -> std::same_as<bool>;
    };

template <typename G>
concept VectorspaceGeometry =  //
    GeometryModel<G> &&        //
    HasPoint<G> &&             //
    HasDirection<G>;

template <typename G>
concept ProjectiveGeometry =                            //
    GeometryModel<G> &&                                 //
    HasPoint<G> &&                                      //
    HasDirection<G> &&                                  //
    (G::NUM_PHYSICAL_DIMENSIONS < 2 || HasLine<G>) &&   //
    (G::NUM_PHYSICAL_DIMENSIONS < 2 || HasPlane<G>) &&  //
    (G::NUM_PHYSICAL_DIMENSIONS < 3 || HasHyperplane<G>);

template <typename G>
concept ConformalGeometry =                                   //
    GeometryModel<G> &&                                       //
    HasPoint<G> &&                                            //
    HasPointPair<G> &&                                        //
    HasDirection<G> &&                                        //
    (G::NUM_PHYSICAL_DIMENSIONS < 2 || HasLine<G>) &&         //
    (G::NUM_PHYSICAL_DIMENSIONS < 2 || HasPlane<G>) &&        //
    (G::NUM_PHYSICAL_DIMENSIONS < 3 || HasHyperplane<G>) &&   //
    (G::NUM_PHYSICAL_DIMENSIONS < 2 || HasCircle<G>) &&       //
    (G::NUM_PHYSICAL_DIMENSIONS < 3 || HasSphere<G>) &&       //
    (G::NUM_PHYSICAL_DIMENSIONS < 4 || HasHypersphere<G>) &&  //

    // Basis vector factory methods for physical dimensions.
    requires {
      { G::gamma0() } -> std::same_as<typename G::Multivector>;
    } &&
    (G::NUM_PHYSICAL_DIMENSIONS < 2 ||
     requires {
       { G::gamma1() } -> std::same_as<typename G::Multivector>;
     }) &&
    (G::NUM_PHYSICAL_DIMENSIONS < 3 ||
     requires {
       { G::gamma2() } -> std::same_as<typename G::Multivector>;
     }) &&
    (G::NUM_PHYSICAL_DIMENSIONS < 4 ||
     requires {
       { G::gamma3() } -> std::same_as<typename G::Multivector>;
     }) &&

    // Conformal basis vector factory methods.
    requires {
      { G::e_inf() } -> std::same_as<typename G::Multivector>;
      { G::e_orig() } -> std::same_as<typename G::Multivector>;
    } &&

    // Basis bivector factory methods for physical dimensions.
    (G::NUM_PHYSICAL_DIMENSIONS < 2 || requires {
      { G::gamma01() } -> std::same_as<typename G::Multivector>;
    }) && (G::NUM_PHYSICAL_DIMENSIONS < 3 || requires {
      { G::gamma02() } -> std::same_as<typename G::Multivector>;
      { G::gamma12() } -> std::same_as<typename G::Multivector>;
    }) && (G::NUM_PHYSICAL_DIMENSIONS < 4 || requires {
      { G::gamma03() } -> std::same_as<typename G::Multivector>;
      { G::gamma13() } -> std::same_as<typename G::Multivector>;
      { G::gamma23() } -> std::same_as<typename G::Multivector>;
    });

}  // namespace ndyn::math
