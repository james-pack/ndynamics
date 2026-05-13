#include "gtest/gtest.h"
#include "math/geometries.h"
#include "math/has_embedded_space_test_utils.h"
#include "math/has_point_test_utils.h"

namespace ndyn::math {

using ProjectiveGeometryTypes = ::testing::Types<Pga2dGeometry<>, Pga3dGeometry<>, Pga4dGeometry<>>;
INSTANTIATE_TYPED_TEST_SUITE_P(ProjectiveGeometryTest, HasPointTest, ProjectiveGeometryTypes);
INSTANTIATE_TYPED_TEST_SUITE_P(ProjectiveGeometryTest, HasEmbeddedSpaceTest,
                               ProjectiveGeometryTypes);

}  // namespace ndyn::math
