#include "math/integrators.h"

#include <cmath>

#include "gtest/gtest.h"
#include "math/canonical_basis_representation.h"
#include "math/integrators_test_utils.h"
#include "math/multivector.h"
#include "math/vga_geometry.h"

namespace ndyn::math {

template <typename IntegratorT>
class IntegratorTest : public ::testing::Test {};

using IntegratorTypes = ::testing::Types<ForwardEuler<State<VgaGeometry<>, 1>>,  //
                                         ForwardEuler<State<VgaGeometry<>, 2>>,  //
                                         ForwardEuler<State<VgaGeometry<>, 3>>,  //
                                         ForwardEuler<State<VgaGeometry<>, 4>>,  //
                                         RungeKutta2<State<VgaGeometry<>, 1>>,   //
                                         RungeKutta2<State<VgaGeometry<>, 2>>,   //
                                         RungeKutta2<State<VgaGeometry<>, 3>>,   //
                                         RungeKutta2<State<VgaGeometry<>, 4>>,   //
                                         RungeKutta4<State<VgaGeometry<>, 1>>,   //
                                         RungeKutta4<State<VgaGeometry<>, 2>>,   //
                                         RungeKutta4<State<VgaGeometry<>, 3>>,   //
                                         RungeKutta4<State<VgaGeometry<>, 4>>>;
TYPED_TEST_SUITE(IntegratorTest, IntegratorTypes);

TYPED_TEST(IntegratorTest, CanIntegrateOverConstantVelocity) {
  ::testing::AssertionResult result{CanIntegrateOverConstantVelocity<TypeParam>()};
  EXPECT_TRUE(result);
}

TYPED_TEST(IntegratorTest, CanUpdateVelocityFromPosition) {
  ::testing::AssertionResult result{CanUpdateVelocityFromPosition<TypeParam>()};
  EXPECT_TRUE(result);
}

TYPED_TEST(IntegratorTest, CanIntegrateOverConstantAcceleration) {
  ::testing::AssertionResult result{CanIntegrateOverConstantAcceleration<TypeParam>()};
  EXPECT_TRUE(result);
}

TYPED_TEST(IntegratorTest, CanIntegrateOverConstantAccelerationManySteps) {
  ::testing::AssertionResult result{CanIntegrateOverConstantAccelerationManySteps<TypeParam>()};
  EXPECT_TRUE(result);
}

TYPED_TEST(IntegratorTest, CanUpdateAccelerationFromPosition) {
  ::testing::AssertionResult result{CanUpdateAccelerationFromPosition<TypeParam>()};
  EXPECT_TRUE(result);
}

TYPED_TEST(IntegratorTest, CanIntegrateOverConstantJerk) {
  ::testing::AssertionResult result{CanIntegrateOverConstantJerk<TypeParam>()};
  EXPECT_TRUE(result);
}

TYPED_TEST(IntegratorTest, CanUpdateJerkFromPosition) {
  ::testing::AssertionResult result{CanUpdateJerkFromPosition<TypeParam>()};
  EXPECT_TRUE(result);
}

TYPED_TEST(IntegratorTest, CanIntegrateOverConstantSnap) {
  ::testing::AssertionResult result{CanIntegrateOverConstantSnap<TypeParam>()};
  EXPECT_TRUE(result);
}

TYPED_TEST(IntegratorTest, CanUpdateSnap) {
  ::testing::AssertionResult result{CanUpdateSnap<TypeParam>()};
  EXPECT_TRUE(result);
}

}  // namespace ndyn::math
