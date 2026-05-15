#include "gtest/gtest.h"
#include "math/geometries.h"
#include "math/has_embedded_space_test_utils.h"
#include "math/has_point_test_utils.h"

namespace ndyn::math {

using HomogeneousGeometryTypes =
    ::testing::Types<Hga2dGeometry<>, Hga3dGeometry<>, Hga4dGeometry<>>;
INSTANTIATE_TYPED_TEST_SUITE_P(HomogeneousGeometryTest, HasPointTest, HomogeneousGeometryTypes);
INSTANTIATE_TYPED_TEST_SUITE_P(HomogeneousGeometryTest, HasEmbeddedSpaceTest,
                               HomogeneousGeometryTypes);

}  // namespace ndyn::math
