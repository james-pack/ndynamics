#pragma once

#include <array>
#include <type_traits>
#include <utility>

#include "gtest/gtest.h"
#include "math/geometry_model.h"
#include "math/scalar_test_utils.h"

namespace ndyn::math {

/**
 * These tests verify that lift() and lower() are mutual inverses up to the tolerance of the scalar
 * type, and that points constructed in the embedded space remain points after lifting into the
 * outer geometry.
 *
 * The outer geometry G must satisfy HasEmbeddedSpace. The type of the embedded space is accessed as
 * G::EmbeddedSpace.
 */
template <HasEmbeddedSpace G>
class HasEmbeddedSpaceTest : public ::testing::Test {
 protected:
  using Scalar = typename G::Scalar;

  // Build a non-degenerate point in the embedded space using the iterator
  // API. The coordinates are 0, 1, 2, …, N-1.
  static constexpr auto make_embed_point() {
    using Embed = typename G::EmbeddedSpace;
    constexpr std::size_t N{Embed::NUM_PHYSICAL_DIMENSIONS};
    constexpr std::array<Scalar, N> coords{
        as_scalars<Scalar, int>(std::make_integer_sequence<int, N>())};
    return Embed::make_point(coords.begin(), coords.end());
  }
};

TYPED_TEST_SUITE_P(HasEmbeddedSpaceTest);

// Lifting a point and lowering it back must recover a point in
// the embedded space.
TYPED_TEST_P(HasEmbeddedSpaceTest, RoundtripPoint) {
  constexpr auto embed_pt{TestFixture::make_embed_point()};
  constexpr auto lifted_pt{TypeParam::lift(embed_pt)};
  constexpr auto recovered{TypeParam::lower(lifted_pt)};

  EXPECT_TRUE(TypeParam::is_point(lifted_pt));
  EXPECT_TRUE(TypeParam::EmbeddedSpace::is_point(recovered));
}

// Lifting the origin point and lowering it back must recover a point in
// the embedded space.
TYPED_TEST_P(HasEmbeddedSpaceTest, RoundtripOrigin) {
  constexpr auto embed_pt{TypeParam::EmbeddedSpace::origin()};
  constexpr auto lifted_pt{TypeParam::lift(embed_pt)};
  constexpr auto recovered{TypeParam::lower(lifted_pt)};

  EXPECT_TRUE(TypeParam::is_point(lifted_pt));
  EXPECT_TRUE(TypeParam::EmbeddedSpace::is_point(recovered));
}

// The origin in the geometry must also be a point on the embedded space.
TYPED_TEST_P(HasEmbeddedSpaceTest, OriginLowersToPointOnEmbeddedSpace) {
  constexpr auto origin_pt{TypeParam::origin()};
  constexpr auto embed_pt{TypeParam::lower(origin_pt)};

  EXPECT_TRUE(TypeParam::EmbeddedSpace::is_point(embed_pt));
}

REGISTER_TYPED_TEST_SUITE_P(HasEmbeddedSpaceTest,  //
                            RoundtripPoint,        //
                            RoundtripOrigin,       //
                            OriginLowersToPointOnEmbeddedSpace);

}  // namespace ndyn::math
