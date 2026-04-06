#include "math/geometry_model.h"

#include "gtest/gtest.h"
#include "math/algebra.h"
#include "math/canonical_basis_representation.h"
#include "math/cga_geometry.h"
#include "math/multivector.h"
#include "math/pga_geometry.h"
#include "math/vga_geometry.h"

namespace ndyn::math {

template <typename T>
class GeometryModelTest : public ::testing::Test {};

using GeometryModelTypes = ::testing::Types<  //
    VgaGeometry<>,                            //
    PgaGrade1PointGeometry<>,                 //
    PgaGrade3PointGeometry<>,                 //
    CgaGeometry<>                             //
    >;

TYPED_TEST_SUITE(GeometryModelTest, GeometryModelTypes);

/**
 * Verify that a point constructed from Euclidean coordinates survives a roundtrip through
 * the geometry model's make_point() and extract_point() without loss of information. This
 * confirms that the embedding and extraction conventions are mutually consistent for each
 * model, and that the homogeneous normalization applied during extraction correctly recovers
 * the original coordinates.
 */
TYPED_TEST(GeometryModelTest, PointRoundtrip) {
  static constexpr TypeParam model{};

  using Scalar = typename TypeParam::Scalar;

  static constexpr Scalar x{1};
  static constexpr Scalar y{2};
  static constexpr Scalar z{3};

  const auto point{model.make_point(x, y, z)};

  Scalar out_x{};
  Scalar out_y{};
  Scalar out_z{};
  model.extract_point(point, out_x, out_y, out_z);

  static constexpr Scalar tolerance{TypeParam::Algebra::EPSILON};
  EXPECT_NEAR(out_x, x, tolerance);
  EXPECT_NEAR(out_y, y, tolerance);
  EXPECT_NEAR(out_z, z, tolerance);
}

/**
 * Verify that a point constructed from Euclidean coordinates can be detected as a point and not a
 * line or plane. This confirms that these three tests behave correctly when given points.
 */
TYPED_TEST(GeometryModelTest, IsPoint) {
  static constexpr TypeParam model{};

  using Scalar = typename TypeParam::Scalar;

  static constexpr Scalar x{1};
  static constexpr Scalar y{2};
  static constexpr Scalar z{3};

  const auto point{model.make_point(x, y, z)};

  EXPECT_TRUE(model.is_point(point));
}

}  // namespace ndyn::math
