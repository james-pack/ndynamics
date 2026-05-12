#include "gtest/gtest.h"
#include "math/geometries.h"
#include "math/has_point_test_utils.h"

namespace ndyn::math {

using VectorSpaceGeometryTypes =
    ::testing::Types<Vga2dGeometry<>, Vga3dGeometry<>, Vga4dGeometry<>>;
INSTANTIATE_TYPED_TEST_SUITE_P(VectorSpaceGeometryTest, HasPointTest, VectorSpaceGeometryTypes);

}  // namespace ndyn::math
