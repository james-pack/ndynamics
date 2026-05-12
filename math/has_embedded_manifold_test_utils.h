#pragma once

#include <array>
#include <type_traits>
#include <utility>

#include "gtest/gtest.h"
#include "math/geometry_model.h"
#include "math/scalar_test_utils.h"

namespace ndyn::math {

/**
 * These tests verify that lift() and lower() are mutual inverses up to the
 * tolerance of the scalar type, and that points constructed in the embedded
 * manifold remain points after lifting into the outer geometry.
 *
 * The outer geometry G must satisfy HasEmbeddedManifold. The embedded manifold
 * is accessed as G::EmbeddedManifold.
 */
template <HasEmbeddedManifold G>
class HasEmbeddedManifoldTest : public ::testing::Test {
 protected:
  using Scalar = typename G::Scalar;

  // Build a non-degenerate point in the embedded manifold using the iterator
  // API. The coordinates are 0, 1, 2, …, N-1.
  static constexpr auto make_embed_point() {
    using Embed = typename G::EmbeddedManifold;
    constexpr std::size_t N{Embed::NUM_PHYSICAL_DIMENSIONS};
    constexpr std::array<Scalar, N> coords{
        as_scalars<Scalar, int>(std::make_integer_sequence<int, N>())};
    return Embed::make_point(coords.begin(), coords.end());
  }
};

TYPED_TEST_SUITE_P(HasEmbeddedManifoldTest);

// Lifting a point and lowering it back must recover a point in
// the embedded manifold.
TYPED_TEST_P(HasEmbeddedManifoldTest, RoundtripPoint) {
  constexpr auto embed_pt{TestFixture::make_embed_point()};
  constexpr auto lifted_pt{TypeParam::lift(embed_pt)};
  constexpr auto recovered{TypeParam::lower(lifted_pt)};

  EXPECT_TRUE(TypeParam::is_point(lifted_pt));
  EXPECT_TRUE(TypeParam::EmbeddedManifold::is_point(recovered));
}

// Lifting the origin point and lowering it back must recover a point in
// the embedded manifold.
TYPED_TEST_P(HasEmbeddedManifoldTest, RoundtripOrigin) {
  constexpr auto embed_pt{TypeParam::EmbeddedManifold::origin()};
  constexpr auto lifted_pt{TypeParam::lift(embed_pt)};
  constexpr auto recovered{TypeParam::lower(lifted_pt)};

  EXPECT_TRUE(TypeParam::is_point(lifted_pt));
  EXPECT_TRUE(TypeParam::EmbeddedManifold::is_point(recovered));
}

// The origin in the geometry must also be a point on the embedded manifold.
TYPED_TEST_P(HasEmbeddedManifoldTest, OriginLowersToPointOnEmbeddedManifold) {
  constexpr auto origin_pt{TypeParam::origin()};
  constexpr auto embed_pt{TypeParam::lower(origin_pt)};

  EXPECT_TRUE(TypeParam::EmbeddedManifold::is_point(embed_pt));
}

REGISTER_TYPED_TEST_SUITE_P(HasEmbeddedManifoldTest,  //
                            RoundtripPoint,           //
                            RoundtripOrigin,          //
                            OriginLowersToPointOnEmbeddedManifold);

}  // namespace ndyn::math
