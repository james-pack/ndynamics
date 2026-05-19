#include "gtest/gtest.h"
#include "math/geometries.h"
#include "math/has_direction_test_utils.h"
#include "math/has_point_test_utils.h"
#include "math/has_rotor_test_utils.h"
#include "math/has_versor_test_utils.h"

namespace ndyn::math {

using VectorSpaceGeometryTypes =
    ::testing::Types<Vga2dGeometry<>, Vga3dGeometry<>, Vga4dGeometry<>>;
INSTANTIATE_TYPED_TEST_SUITE_P(VectorSpaceGeometryTest, HasPointTest, VectorSpaceGeometryTypes);
INSTANTIATE_TYPED_TEST_SUITE_P(VectorSpaceGeometryTest, HasDirectionTest, VectorSpaceGeometryTypes);
INSTANTIATE_TYPED_TEST_SUITE_P(VectorSpaceGeometryTest, HasVersorTest, VectorSpaceGeometryTypes);
INSTANTIATE_TYPED_TEST_SUITE_P(VectorSpaceGeometryTest, HasRotorTest, VectorSpaceGeometryTypes);

}  // namespace ndyn::math
