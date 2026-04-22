#include "math/cga_geometry.h"

#include "gtest/gtest.h"
#include "math/geometry_model_test_utils.h"

namespace ndyn::math {

using CgaGeometries = ::testing::Types<Cga2dGeometry<>, Cga3dGeometry<>, Cga4dGeometry<>>;
INSTANTIATE_TYPED_TEST_SUITE_P(CGA, GeometryConceptsTest, CgaGeometries);

}  // namespace ndyn::math
