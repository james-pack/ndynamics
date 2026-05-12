#include "gtest/gtest.h"
#include "math/geometries.h"
#include "math/has_embedded_manifold_test_utils.h"
#include "math/has_point_test_utils.h"

namespace ndyn::math {

using ConformalGeometryTypes = ::testing::Types<Cga2dGeometry<>, Cga3dGeometry<>, Cga4dGeometry<>>;
INSTANTIATE_TYPED_TEST_SUITE_P(ConformalGeometryTest, HasPointTest, ConformalGeometryTypes);
INSTANTIATE_TYPED_TEST_SUITE_P(ConformalGeometryTest, HasEmbeddedManifoldTest,
                               ConformalGeometryTypes);

}  // namespace ndyn::math
