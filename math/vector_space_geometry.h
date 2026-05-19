#pragma once

#include <cmath>
#include <concepts>
#include <iterator>

#include "math/algebra.h"
#include "math/geometry_model.h"
#include "math/multivector.h"

namespace ndyn::math {

/**
 * A GeometryModel implementation on a vector space geometric algebra.
 */
template <size_t NUM_PHYSICAL_DIMENSIONS_VALUE, typename T = DefaultScalarType>
class VectorSpaceGeometryType final {
 public:
  static constexpr size_t NUM_PHYSICAL_DIMENSIONS{NUM_PHYSICAL_DIMENSIONS_VALUE};

  using G = VectorSpaceGeometryType<NUM_PHYSICAL_DIMENSIONS, T>;

  using Algebra = math::Algebra<T, NUM_PHYSICAL_DIMENSIONS, 0, 0>;
  using Multivector = Algebra::VectorType;
  using Scalar = Algebra::ScalarType;

  static constexpr size_t NUM_BASIS_VECTORS{Algebra::NUM_BASIS_VECTORS};
  static constexpr size_t NUM_BASIS_BLADES{Algebra::NUM_BASIS_BLADES};

  static constexpr size_t NUM_POSITIVE_BASES{Algebra::NUM_POSITIVE_BASES};
  static constexpr size_t NUM_NEGATIVE_BASES{Algebra::NUM_NEGATIVE_BASES};
  static constexpr size_t NUM_ZERO_BASES{Algebra::NUM_ZERO_BASES};

  static constexpr Scalar EPSILON{Algebra::EPSILON};

 public:
  [[nodiscard]] static constexpr auto origin() noexcept { return Multivector{}; }

  // Factory methods for basis vectors under the generic names.
  [[nodiscard]] static constexpr auto e0() noexcept
    requires(NUM_PHYSICAL_DIMENSIONS >= 1)
  {
    return Multivector::template e<0>();
  }
  [[nodiscard]] static constexpr auto e1() noexcept
    requires(NUM_PHYSICAL_DIMENSIONS >= 2)
  {
    return Multivector::template e<1>();
  }
  [[nodiscard]] static constexpr auto e2() noexcept
    requires(NUM_PHYSICAL_DIMENSIONS >= 3)
  {
    return Multivector::template e<2>();
  }
  [[nodiscard]] static constexpr auto e3() noexcept
    requires(NUM_PHYSICAL_DIMENSIONS >= 4)
  {
    return Multivector::template e<3>();
  }

  // Getters for the coefficients of the basis vectors under the generic names.
  [[nodiscard]] static constexpr auto get_e0(const IsMultivectorLike<G> auto& mv) noexcept
    requires(NUM_PHYSICAL_DIMENSIONS >= 1)
  {
    return mv.template coefficient<1UL << 0>();
  }
  [[nodiscard]] static constexpr auto get_e1(const IsMultivectorLike<G> auto& mv) noexcept
    requires(NUM_PHYSICAL_DIMENSIONS >= 2)
  {
    return mv.template coefficient<1UL << 1>();
  }
  [[nodiscard]] static constexpr auto get_e2(const IsMultivectorLike<G> auto& mv) noexcept
    requires(NUM_PHYSICAL_DIMENSIONS >= 3)
  {
    return mv.template coefficient<1UL << 2>();
  }
  [[nodiscard]] static constexpr auto get_e3(const IsMultivectorLike<G> auto& mv) noexcept
    requires(NUM_PHYSICAL_DIMENSIONS >= 4)
  {
    return mv.template coefficient<1UL << 3>();
  }

  // // Factory methods for basis bivectors.
  [[nodiscard]] static constexpr auto e01() noexcept { return e0() * e1(); }
  [[nodiscard]] static constexpr auto e02() noexcept { return e0() * e2(); }
  [[nodiscard]] static constexpr auto e03() noexcept { return e0() * e3(); }
  [[nodiscard]] static constexpr auto e12() noexcept { return e1() * e2(); }
  [[nodiscard]] static constexpr auto e13() noexcept { return e1() * e3(); }
  [[nodiscard]] static constexpr auto e23() noexcept { return e2() * e3(); }

  [[nodiscard]] static constexpr auto join() noexcept {
    // Note this result is for an OPNS model. An IPNS model would return the pseudoscalar.
    return Multivector{Scalar{1}};
  }

  template <IsMultivectorLike<G> First, IsMultivectorLike<G>... Rest>
  [[nodiscard]] static constexpr auto join(const First& first, const Rest&... rest) noexcept {
    if constexpr (sizeof...(rest) == 0) {
      return first;
    } else {
      // The join in OPNS is the outer product. The ^ operator is overloaded on the Multivector
      // class to perform the outer product.
      return (first ^ ... ^ rest);
    }
  }

  [[nodiscard]] static constexpr auto meet() noexcept {
    // Note this result is for an OPNS model. An IPNS model would return the scalar 1.
    return Multivector::pseudoscalar();
  }

  template <IsMultivectorLike<G> First, IsMultivectorLike<G>... Rest>
  [[nodiscard]] static constexpr auto meet(const First& first, const Rest&... rest) noexcept {
    if constexpr (sizeof...(rest) == 0) {
      return first;
    } else {
      // The meet in OPNS is the regressive product. The & operator is overloaded on the Multivector
      // class to perform the regressive product.
      return (first & ... & rest);
    }
  }

  /**
   * Embed a Euclidean point as a vector using:
   *   X = px*e0 + py*e2 + pz*e3
   */
  [[nodiscard]] static constexpr auto make_point(const ScalarLike<G> auto& t) noexcept {
    return t * e0();
  }

  [[nodiscard]] static constexpr auto make_point(const ScalarLike<G> auto& t,
                                                 const ScalarLike<G> auto& x) {
    return t * e0() + x * e1();
  }

  [[nodiscard]] static constexpr auto make_point(const ScalarLike<G> auto& t,
                                                 const ScalarLike<G> auto& x,
                                                 const ScalarLike<G> auto& y) {
    return t * e0() + x * e1() + y * e2();
  }

  [[nodiscard]] static constexpr auto make_point(const ScalarLike<G> auto& t,
                                                 const ScalarLike<G> auto& x,
                                                 const ScalarLike<G> auto& y,
                                                 const ScalarLike<G> auto& z) noexcept {
    return t * e0() + x * e1() + y * e2() + z * e3();
  }

  template <std::input_iterator Iter>
  [[nodiscard]] static constexpr auto make_point(Iter begin, Iter end) noexcept {
    Multivector result{};
    size_t i{};
    for (Iter iter = begin; iter != end; ++iter, ++i) {
      result += static_cast<Scalar>(*iter) * Multivector::e(i);
    }
    return result;
  }

  /**
   * Extract Euclidean coordinates from a CGA null point vector under standard normalization.
   */
  static constexpr void extract_point(const IsMultivectorLike<G> auto& point,
                                      Scalar& out_t) noexcept {
    out_t = get_e0(point);
  }

  static constexpr void extract_point(const IsMultivectorLike<G> auto& point, Scalar& out_t,
                                      Scalar& out_x) noexcept {
    out_t = get_e0(point);
    out_x = get_e1(point);
  }

  static constexpr void extract_point(const IsMultivectorLike<G> auto& point, Scalar& out_t,
                                      Scalar& out_x, Scalar& out_y) noexcept {
    out_t = get_e0(point);
    out_x = get_e1(point);
    out_y = get_e2(point);
  }

  static constexpr void extract_point(const IsMultivectorLike<G> auto& point, Scalar& out_t,
                                      Scalar& out_x, Scalar& out_y, Scalar& out_z) noexcept {
    out_t = get_e0(point);
    out_x = get_e1(point);
    out_y = get_e2(point);
    out_z = get_e3(point);
  }

  /**
   * In VGA a finite point is any grade-1 multivector.
   */
  [[nodiscard]] static auto is_point(const IsMultivectorLike<G> auto& mv) noexcept {
    const auto is_grade_1{mv.template is_grade<1>()};
    return is_grade_1;
  }

  /**
   * Embed a Euclidean direction as a vector using:
   *   X = px*e0 + py*e2 + pz*e3
   * Note that a point and a direction are modelled the same.
   */
  [[nodiscard]] static constexpr auto make_direction(const ScalarLike<G> auto& t) noexcept {
    return make_point(t);
  }

  [[nodiscard]] static constexpr auto make_direction(const ScalarLike<G> auto& t,
                                                     const ScalarLike<G> auto& x) {
    return make_point(t, x);
  }

  [[nodiscard]] static constexpr auto make_direction(const ScalarLike<G> auto& t,
                                                     const ScalarLike<G> auto& x,
                                                     const ScalarLike<G> auto& y) {
    return make_point(t, x, y);
  }

  [[nodiscard]] static constexpr auto make_direction(const ScalarLike<G> auto& t,
                                                     const ScalarLike<G> auto& x,
                                                     const ScalarLike<G> auto& y,
                                                     const ScalarLike<G> auto& z) noexcept {
    return make_point(t, x, y, z);
  }

  template <std::input_iterator Iter>
  [[nodiscard]] static constexpr auto make_direction(Iter begin, Iter end) noexcept {
    return make_point(begin, end);
  }

  /**
   * Extract Euclidean coordinates from a CGA null direction vector under standard normalization.
   */
  static constexpr void extract_direction(const IsMultivectorLike<G> auto& direction,
                                          Scalar& out_t) noexcept {
    extract_point(direction, out_t);
  }

  static constexpr void extract_direction(const IsMultivectorLike<G> auto& direction, Scalar& out_t,
                                          Scalar& out_x) noexcept {
    extract_point(direction, out_t, out_x);
  }

  static constexpr void extract_direction(const IsMultivectorLike<G> auto& direction, Scalar& out_t,
                                          Scalar& out_x, Scalar& out_y) noexcept {
    extract_point(direction, out_t, out_x, out_y);
  }

  static constexpr void extract_direction(const IsMultivectorLike<G> auto& direction, Scalar& out_t,
                                          Scalar& out_x, Scalar& out_y, Scalar& out_z) noexcept {
    extract_point(direction, out_t, out_x, out_y, out_z);
  }

  /**
   * In VGA a finite direction is any grade-1 multivector, just like a point.
   */
  [[nodiscard]] static constexpr auto is_direction(const IsMultivectorLike<G> auto& mv) noexcept {
    return is_point(mv);
  }

  [[nodiscard]] static constexpr auto is_versor(const IsMultivectorLike<G> auto& mv) noexcept {
    return mv.is_versor();
  }

  /**
   * Create a rotor that rotates dir1 to dir2.
   */
  [[nodiscard]] static constexpr auto make_rotor(const IsMultivectorLike<G> auto& dir1,
                                                 const IsMultivectorLike<G> auto& dir2) noexcept {
    using std::sqrt;
    const auto result{dir2 * dir1};
    const auto scale{(result * ~result).scalar()};
    if (abs(scale) > EPSILON) {
      return result / sqrt(scale);
    } else {
      return Multivector{Scalar{1}};
    }
  }
};

}  // namespace ndyn::math
