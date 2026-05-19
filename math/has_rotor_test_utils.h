#pragma once

#include <array>
#include <cmath>
#include <type_traits>
#include <utility>

#include "gtest/gtest.h"
#include "math/geometry_model.h"
#include "math/multivector_test_utils.h"
#include "math/scalar_test_utils.h"

namespace ndyn::math {

/**
 * These tests validate the strict structural and algebraic invariants of rotors.
 */
template <HasRotor G>
class HasRotorTest : public ::testing::Test {
 protected:
  using Algebra = typename G::Algebra;
  using Multivector = typename G::Multivector;
  using Scalar = typename G::Scalar;
};

TYPED_TEST_SUITE_P(HasRotorTest);

// Building a rotor from two identical directions must yield a valid multivector
// representing a zero-rotation. Because a rotor is a specialized versor, the
// output must satisfy the model's generic versor predicate.
TYPED_TEST_P(HasRotorTest, IdenticalDirectionsYieldValidVersor) {
  using Scalar = typename TypeParam::Scalar;
  constexpr std::size_t N{TypeParam::NUM_PHYSICAL_DIMENSIONS};

  constexpr std::array<Scalar, N> coords{
      as_scalars<Scalar, int>(std::make_integer_sequence<int, N>())};

  constexpr auto d{TypeParam::make_direction(coords.cbegin(), coords.cend())};
  constexpr auto r{TypeParam::make_rotor(d, d)};

  EXPECT_TRUE(TypeParam::is_versor(r));
}

// Constructing a rotor from two identical direction vectors represents a zero
// degree displacement, which must analytically equate to the geometry's identity element.
TYPED_TEST_P(HasRotorTest, IdenticalDirectionsYieldIdentityRotor) {
  using Scalar = typename TypeParam::Scalar;
  constexpr std::size_t N{TypeParam::NUM_PHYSICAL_DIMENSIONS};

  constexpr std::array<Scalar, N> coords{
      as_scalars<Scalar, int>(std::make_integer_sequence<int, N>())};
  constexpr auto d{TypeParam::make_direction(coords.cbegin(), coords.cend())};

  constexpr auto r{TypeParam::make_rotor(d, d)};
  EXPECT_TRUE(r.near_one());
}

// Rotors are strictly defined as normalized even-grade versors. The product of
// a rotor and its reversion must equal the scalar identity element (R * ~R = 1).
TYPED_TEST_P(HasRotorTest, RotorIsNormalizedUnderReversion) {
  using Scalar = typename TypeParam::Scalar;
  constexpr std::size_t N{TypeParam::NUM_PHYSICAL_DIMENSIONS};

  // Generate two distinct direction vectors to build a non-trivial rotor
  constexpr std::array<Scalar, N> coords_a{
      as_scalars<Scalar, int>(std::make_integer_sequence<int, N>())};
  std::array<Scalar, N> coords_b = coords_a;
  if (N > 1) {
    coords_b[0] = coords_a[1];
    coords_b[1] = coords_a[0];
  }

  const auto d_a{TypeParam::make_direction(coords_a.cbegin(), coords_a.cend())};
  const auto d_b{TypeParam::make_direction(coords_b.cbegin(), coords_b.cend())};

  const auto r{TypeParam::make_rotor(d_a, d_b)};
  EXPECT_TRUE((r * ~r).near_one()) << "r: " << r << ", r * ~r: " << (r * ~r);
}

// Generating a rotor from two directions separated by pi/4 radians should create
// a rotor that rotates a direction vector by pi/2 radians counterclockwise.
TYPED_TEST_P(HasRotorTest, RotorCanPerformTrivialRotation) {
  using Scalar = typename TypeParam::Scalar;
  using Algebra = typename TypeParam::Algebra;
  constexpr std::size_t N{TypeParam::NUM_PHYSICAL_DIMENSIONS};

  // This test strictly requires at least 2 physical dimensions to define a plane
  // of rotation. Models with fewer dimensions pass trivially.
  if constexpr (N >= 2) {
    // Construct two direction multivectors that are pi/4 radians apart. These will be the Euclidean
    // x-axis, y=x line.
    std::array<Scalar, N> coords_x{};
    std::array<Scalar, N> coords_xy{};
    coords_x[0] = Scalar{1};
    coords_xy[0] = Scalar{1};
    coords_xy[1] = Scalar{1};

    const auto dir_x{TypeParam::make_direction(coords_x.cbegin(), coords_x.cend())};
    const auto dir_xy{TypeParam::make_direction(coords_xy.cbegin(), coords_xy.cend())};

    // Counter-clockwise rotation.
    const auto r{TypeParam::make_rotor(dir_x, dir_xy)};

    const auto dir_rotated{r * dir_x * (~r)};

    std::array<Scalar, N> coords_y{};
    coords_y[1] = Scalar{1};
    const auto dir_expected{TypeParam::make_direction(coords_y.cbegin(), coords_y.cend())};

    EXPECT_TRUE(AreNear<Algebra>(dir_expected, dir_rotated))
        << "Expected: " << dir_expected << ", Actual: " << dir_rotated;
  }
}

TYPED_TEST_P(HasRotorTest, RotorCanPerformTrivialRotationInReverse) {
  using Scalar = typename TypeParam::Scalar;
  using Algebra = typename TypeParam::Algebra;
  constexpr std::size_t N{TypeParam::NUM_PHYSICAL_DIMENSIONS};

  // This test strictly requires at least 2 physical dimensions to define a plane
  // of rotation. Models with fewer dimensions pass trivially.
  if constexpr (N >= 2) {
    // Construct two direction multivectors that are pi/4 radians apart. These will be the Euclidean
    // x-axis, y=x line.
    std::array<Scalar, N> coords_x{};
    std::array<Scalar, N> coords_xy{};
    coords_x[0] = Scalar{1};
    coords_xy[0] = Scalar{1};
    coords_xy[1] = Scalar{1};

    const auto dir_x{TypeParam::make_direction(coords_x.cbegin(), coords_x.cend())};
    const auto dir_xy{TypeParam::make_direction(coords_xy.cbegin(), coords_xy.cend())};

    // Note that we are creating a clockwise rotation.
    const auto r{TypeParam::make_rotor(dir_xy, dir_x)};

    const auto dir_rotated{r * dir_x * (~r)};

    std::array<Scalar, N> coords_y{};
    coords_y[1] = Scalar{1};

    // After a clockwise rotation, we expect the -y axis as the result.
    const auto dir_expected{-1 * TypeParam::make_direction(coords_y.cbegin(), coords_y.cend())};

    EXPECT_TRUE(AreNear<Algebra>(dir_expected, dir_rotated))
        << "Expected: " << dir_expected << ", Actual: " << dir_rotated;
  }
}

TYPED_TEST_P(HasRotorTest, RotorCanPerformBasicRotation) {
  using Scalar = typename TypeParam::Scalar;
  using Algebra = typename TypeParam::Algebra;
  constexpr std::size_t N{TypeParam::NUM_PHYSICAL_DIMENSIONS};

  // This test strictly requires at least 2 physical dimensions to define a plane
  // of rotation. Models with fewer dimensions pass trivially.
  if constexpr (N >= 2) {
    // Construct two direction multivectors that are pi/2 radians apart. These will be the Euclidean
    // x-axis and y-axis.
    std::array<Scalar, N> coords_x{};
    coords_x[0] = Scalar{1};
    std::array<Scalar, N> coords_y{};
    coords_y[1] = Scalar{1};

    const auto dir_x{TypeParam::make_direction(coords_x.cbegin(), coords_x.cend())};
    const auto dir_y{TypeParam::make_direction(coords_y.cbegin(), coords_y.cend())};

    const auto r{TypeParam::make_rotor(dir_x, dir_y)};

    const auto dir_rotated{r * dir_x * (~r)};

    // After a clockwise rotation, we expect the -x axis as the result.
    const auto dir_expected{-1 * dir_x};

    EXPECT_TRUE(AreNear<Algebra>(dir_expected, dir_rotated))
        << "Expected: " << dir_expected << ", Actual: " << dir_rotated;
  }
}

TYPED_TEST_P(HasRotorTest, RotorObeysRotationalSymmetry) {
  using Scalar = typename TypeParam::Scalar;
  using Algebra = typename TypeParam::Algebra;
  constexpr std::size_t N{TypeParam::NUM_PHYSICAL_DIMENSIONS};

  // This test strictly requires at least 2 physical dimensions to define a plane
  // of rotation. Models with fewer dimensions pass trivially.
  if constexpr (N >= 2) {
    // Construct two direction multivectors that are pi/2 radians apart. These will be the Euclidean
    // x-axis and y-axis.
    std::array<Scalar, N> coords_x{};
    coords_x[0] = Scalar{1};
    std::array<Scalar, N> coords_y{};
    coords_y[1] = Scalar{1};

    const auto dir_x{TypeParam::make_direction(coords_x.cbegin(), coords_x.cend())};
    const auto dir_y{TypeParam::make_direction(coords_y.cbegin(), coords_y.cend())};

    const auto ccw_r{TypeParam::make_rotor(dir_x, dir_y)};
    const auto cw_r{TypeParam::make_rotor(dir_y, dir_x)};

    const auto dir_rotated_ccw{ccw_r * dir_x * ~ccw_r};
    const auto dir_rotated_cw{cw_r * dir_x * ~cw_r};

    // Rotating the x-axis by pi and -pi radians should both land at the -x axis.
    EXPECT_TRUE(AreNear<Algebra>(dir_rotated_ccw, dir_rotated_cw));
    EXPECT_TRUE(AreNear<Algebra>(-dir_x, dir_rotated_cw));
  }
}

REGISTER_TYPED_TEST_SUITE_P(HasRotorTest,                             //
                            IdenticalDirectionsYieldValidVersor,      //
                            IdenticalDirectionsYieldIdentityRotor,    //
                            RotorIsNormalizedUnderReversion,          //
                            RotorCanPerformTrivialRotation,           //
                            RotorCanPerformTrivialRotationInReverse,  //
                            RotorCanPerformBasicRotation,             //
                            RotorObeysRotationalSymmetry);

}  // namespace ndyn::math
