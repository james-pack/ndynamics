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
class ProjectiveAtlas final {
 public:
  using EmbeddedSpace = EmbeddedSpaceType;
  using Scalar = typename EmbeddedSpace::Scalar;

  template <GeometryModel ContainingSpace>
  [[nodiscard]] static constexpr auto lift(IsMultivectorLike<EmbeddedSpace> auto&& v) noexcept {
    typename ContainingSpace::Multivector result{ContainingSpace::e_inf()};
    for (size_t i = 0; i < EmbeddedSpace::NUM_BASIS_BLADES; ++i) {
      result.set_coefficient(i, v.coefficient(i));
    }
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

template <GeometryModel EmbeddedSpaceType, IsAtlas AtlasType, typename T = DefaultScalarType>
class ProjectiveGeometryType final {
 public:
  using EmbeddedSpace = EmbeddedSpaceType;
  static constexpr size_t NUM_PHYSICAL_DIMENSIONS{EmbeddedSpace::NUM_BASIS_VECTORS};

  using G = ProjectiveGeometryType<EmbeddedSpace, AtlasType, T>;

  using Atlas = AtlasType;
  static_assert(std::is_same_v<typename Atlas::EmbeddedSpace, EmbeddedSpace>);

  using Algebra =
      math::Algebra<T, EmbeddedSpace::NUM_POSITIVE_BASES, EmbeddedSpace::NUM_NEGATIVE_BASES,
                    EmbeddedSpace::NUM_ZERO_BASES + 1>;
  using Multivector = Algebra::VectorType;
  using Scalar = Algebra::ScalarType;

  static constexpr size_t NUM_BASIS_VECTORS{Algebra::NUM_BASIS_VECTORS};
  static constexpr size_t NUM_BASIS_BLADES{Algebra::NUM_BASIS_BLADES};

  static constexpr size_t NUM_POSITIVE_BASES{Algebra::NUM_POSITIVE_BASES};
  static constexpr size_t NUM_NEGATIVE_BASES{Algebra::NUM_NEGATIVE_BASES};
  static constexpr size_t NUM_ZERO_BASES{Algebra::NUM_ZERO_BASES};

  static constexpr Scalar EPSILON{Algebra::EPSILON};

 private:
  [[nodiscard]] static constexpr auto mask_projective_bases(IsMultivectorLike<G> auto&& mv) {
    return mv.template mask_bases<NUM_PHYSICAL_DIMENSIONS, NUM_PHYSICAL_DIMENSIONS>();
  }

  /**
   * Standard weight calculation.
   * The weight is a measure of the scale of the space.
   */
  [[nodiscard]] static constexpr auto weight(IsMultivectorLike<G> auto&& mv) noexcept {
    return mv.template coefficient<1UL << NUM_PHYSICAL_DIMENSIONS>();
  }

 public:
  static constexpr auto e_inf() { return Multivector::template e<NUM_PHYSICAL_DIMENSIONS>(); }
  // Assert that the order of the bases is as expected.
  static_assert((e_inf() * e_inf()).scalar() == Scalar{0});

  [[nodiscard]] static constexpr auto origin() noexcept { return e_inf(); }

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
    Multivector result{e_inf()};
    size_t i{};
    for (Iter iter = begin; iter != end; ++iter, ++i) {
      const auto v{static_cast<Scalar>(*iter)};
      result += v * Multivector::e(i);
    }
    return result;
  }

  [[nodiscard]] static auto is_point(IsMultivectorLike<G> auto&& mv) noexcept {
    const bool is_grade_1{mv.template is_grade<1>()};
    const bool has_nonzero_weight{abs(weight(mv)) > EPSILON};
    return is_grade_1 and has_nonzero_weight;
  }
};

}  // namespace ndyn::math
