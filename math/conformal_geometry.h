#pragma once

#include <concepts>
#include <iterator>

#include "glog/logging.h"
#include "math/abs.h"
#include "math/algebra.h"
#include "math/geometry_model.h"
#include "math/matrix.h"
#include "math/multivector.h"
#include "math/representation.h"

namespace ndyn::math {

template <GeometryModel EmbeddedSpaceType>
class ConformalAtlas final {
 public:
  using EmbeddedSpace = EmbeddedSpaceType;
  using Scalar = typename EmbeddedSpace::Scalar;

  template <GeometryModel ContainingSpace>
  [[nodiscard]] static constexpr auto lift(IsMultivectorLike<EmbeddedSpace> auto&& v) noexcept {
    typename ContainingSpace::Multivector result{};
    for (size_t i = 0; i < EmbeddedSpace::NUM_BASIS_BLADES; ++i) {
      result.set_coefficient(i, v.coefficient(i));
    }
    const auto norm_sq{v.square_magnitude()};
    result += (norm_sq - Scalar{1}) / Scalar{2} * ContainingSpace::e_plus() +
              (norm_sq + Scalar{1}) / Scalar{2} * ContainingSpace::e_minus();
    return result;
  }

  template <GeometryModel ContainingSpace>
  [[nodiscard]] static constexpr auto lower(IsMultivectorLike<ContainingSpace> auto&& v) noexcept {
    typename EmbeddedSpace::Multivector result{};
    for (size_t i = 0; i < EmbeddedSpace::NUM_BASIS_BLADES; ++i) {
      result.set_coefficient(i, v.coefficient(i));
    }
    return result;
  }
};

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
template <GeometryModel EmbeddedSpaceType, IsAtlas AtlasType, typename T = DefaultScalarType>
class ConformalGeometryType final {
 public:
  using EmbeddedSpace = EmbeddedSpaceType;
  static constexpr size_t NUM_PHYSICAL_DIMENSIONS{EmbeddedSpace::NUM_BASIS_VECTORS};

  using G = ConformalGeometryType<EmbeddedSpace, AtlasType, T>;

  using Atlas = AtlasType;
  static_assert(std::is_same_v<typename Atlas::EmbeddedSpace, EmbeddedSpace>);

  using Algebra =
      math::Algebra<T, EmbeddedSpace::NUM_POSITIVE_BASES + 1, EmbeddedSpace::NUM_NEGATIVE_BASES + 1,
                    EmbeddedSpace::NUM_ZERO_BASES>;
  using Multivector = Algebra::VectorType;
  using Scalar = Algebra::ScalarType;

  static constexpr size_t NUM_BASIS_VECTORS{Algebra::NUM_BASIS_VECTORS};
  static constexpr size_t NUM_BASIS_BLADES{Algebra::NUM_BASIS_BLADES};

  static constexpr size_t NUM_POSITIVE_BASES{Algebra::NUM_POSITIVE_BASES};
  static constexpr size_t NUM_NEGATIVE_BASES{Algebra::NUM_NEGATIVE_BASES};
  static constexpr size_t NUM_ZERO_BASES{Algebra::NUM_ZERO_BASES};

  static constexpr Scalar EPSILON{Algebra::EPSILON};

 private:
  [[nodiscard]] static constexpr auto mask_conformal_bases(IsMultivectorLike<G> auto&& mv) {
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

 public:
  // The null basis vectors e0 and e_inf are the conformal origin and infinity respectively.
  // They are not basis vectors of the algebra directly but are linear combinations of e_plus
  // and e_minus. These helpers keep the construction readable throughout the implementation.
  static constexpr auto e_plus() { return Multivector::template e<NUM_PHYSICAL_DIMENSIONS>(); }
  static constexpr auto e_minus() { return Multivector::template e<NUM_PHYSICAL_DIMENSIONS + 1>(); }

  // Assert that the order of the bases is as expected.
  static_assert((e_plus() * e_plus()).scalar() == Scalar{1});
  static_assert((e_minus() * e_minus()).scalar() == Scalar{-1});

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
  [[nodiscard]] static constexpr auto get_e0(IsMultivectorLike<G> auto&& mv) noexcept
    requires(NUM_PHYSICAL_DIMENSIONS >= 1)
  {
    return mv.template coefficient<1UL << 0>();
  }
  [[nodiscard]] static constexpr auto get_e1(IsMultivectorLike<G> auto&& mv) noexcept
    requires(NUM_PHYSICAL_DIMENSIONS >= 2)
  {
    return mv.template coefficient<1UL << 1>();
  }
  [[nodiscard]] static constexpr auto get_e2(IsMultivectorLike<G> auto&& mv) noexcept
    requires(NUM_PHYSICAL_DIMENSIONS >= 3)
  {
    return mv.template coefficient<1UL << 2>();
  }
  [[nodiscard]] static constexpr auto get_e3(IsMultivectorLike<G> auto&& mv) noexcept
    requires(NUM_PHYSICAL_DIMENSIONS >= 4)
  {
    return mv.template coefficient<1UL << 3>();
  }

  // Factory methods for basis bivectors.
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

  [[nodiscard]] static constexpr auto lift(IsMultivectorLike<EmbeddedSpace> auto&& v) noexcept {
    return Atlas::template lift<G>(v);
  }

  [[nodiscard]] static constexpr auto lower(IsMultivectorLike<G> auto&& v) noexcept {
    return Atlas::template lower<G>(v);
  }

  template <std::input_iterator Iter>
  [[nodiscard]] static constexpr auto make_point(Iter begin, Iter end) noexcept {
    Multivector result{};
    size_t i{};
    Scalar norm_sq{};
    for (Iter iter = begin; iter != end; ++iter, ++i) {
      const auto v{static_cast<Scalar>(*iter)};
      norm_sq += v * v;
      result += v * Multivector::e(i);
    }
    result += (norm_sq - Scalar{1}) / Scalar{2} * e_plus() +
              (norm_sq + Scalar{1}) / Scalar{2} * e_minus();
    return result;
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

    const auto ratio{abs(physical_mag) > EPSILON ? conformal_mag / physical_mag : Scalar{1}};
    // const bool is_null{(physical_mag > EPSILON) ? (abs(ratio - Scalar{1}) < EPSILON)
    //                                             : (abs(conformal_mag) < EPSILON)};
    bool is_null;

    if (abs(physical_mag) > EPSILON) {
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
};

}  // namespace ndyn::math
