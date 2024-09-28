#include <cmath>

#include "glog/logging.h"
#include "gtest/gtest.h"
#include "math/integrators.h"
#include "math/integrators_test_utils.h"
#include "math/multivector.h"
#include "math/multivector_test_utils.h"

namespace ndyn::math {

/**
 * Tests for state vectors containing 1 element.
 */
TEST(RungeKutta4_1StateTest, CanIntegrateOverConstantVelocity) {
  using ScalarType = float;
  using StateType = State<Vga2dMultivector<ScalarType>, 1>;

  ::testing::AssertionResult result{CanIntegrateOverConstantVelocity<RungeKutta4<StateType>>()};
  EXPECT_TRUE(result);
}

TEST(RungeKutta4_1StateTest, CanUpdateVelocityFromPosition) {
  using ScalarType = float;
  using StateType = State<Vga2dMultivector<ScalarType>, 1>;

  ::testing::AssertionResult result{CanUpdateVelocityFromPosition<RungeKutta4<StateType>>()};
  EXPECT_TRUE(result);
}

/**
 * Tests for state vectors containing 2 elements.
 */
TEST(RungeKutta4_2StateTest, CanIntegrateOverConstantVelocity) {
  using ScalarType = float;
  using StateType = State<Vga2dMultivector<ScalarType>, 2>;

  ::testing::AssertionResult result{CanIntegrateOverConstantVelocity<RungeKutta4<StateType>>()};
  EXPECT_TRUE(result);
}

TEST(RungeKutta4_2StateTest, CanUpdateVelocityFromPosition) {
  using ScalarType = float;
  using StateType = State<Vga2dMultivector<ScalarType>, 2>;

  ::testing::AssertionResult result{CanUpdateVelocityFromPosition<RungeKutta4<StateType>>()};
  EXPECT_TRUE(result);
}

TEST(RungeKutta4_2StateTest, CanIntegrateOverConstantAcceleration) {
  using ScalarType = float;
  using StateType = State<Vga2dMultivector<ScalarType>, 2>;

  ::testing::AssertionResult result{CanIntegrateOverConstantAcceleration<RungeKutta4<StateType>>()};
  EXPECT_TRUE(result);
}

TEST(RungeKutta4_2StateTest, CanIntegrateOverConstantAccelerationManySteps) {
  using ScalarType = float;
  using StateType = State<ScalarType, 2>;

  ::testing::AssertionResult result{
      CanIntegrateOverConstantAccelerationManySteps<RungeKutta4<StateType>>()};
  EXPECT_TRUE(result);
}

TEST(RungeKutta4_2StateTest, CanUpdateAccelerationFromPosition) {
  using ScalarType = float;
  using StateType = State<Vga2dMultivector<ScalarType>, 2>;

  ::testing::AssertionResult result{CanUpdateAccelerationFromPosition<RungeKutta4<StateType>>()};
  EXPECT_TRUE(result);
}

/**
 * Tests for state vectors containing 3 elements.
 */
TEST(RungeKutta4_3StateTest, CanIntegrateOverConstantVelocity) {
  using ScalarType = float;
  using StateType = State<Vga2dMultivector<ScalarType>, 3>;

  ::testing::AssertionResult result{CanIntegrateOverConstantVelocity<RungeKutta4<StateType>>()};
  EXPECT_TRUE(result);
}

TEST(RungeKutta4_3StateTest, CanUpdateVelocityFromPosition) {
  using ScalarType = float;
  using StateType = State<Vga2dMultivector<ScalarType>, 3>;

  ::testing::AssertionResult result{CanUpdateVelocityFromPosition<RungeKutta4<StateType>>()};
  EXPECT_TRUE(result);
}

TEST(RungeKutta4_3StateTest, CanIntegrateOverConstantAcceleration) {
  using ScalarType = float;
  using StateType = State<Vga2dMultivector<ScalarType>, 3>;

  ::testing::AssertionResult result{CanIntegrateOverConstantAcceleration<RungeKutta4<StateType>>()};
  EXPECT_TRUE(result);
}

TEST(RungeKutta4_3StateTest, CanUpdateAccelerationFromPosition) {
  using ScalarType = float;
  using StateType = State<Vga2dMultivector<ScalarType>, 3>;

  ::testing::AssertionResult result{CanUpdateAccelerationFromPosition<RungeKutta4<StateType>>()};
  EXPECT_TRUE(result);
}

TEST(RungeKutta4_3StateTest, CanIntegrateOverConstantJerk) {
  using ScalarType = float;
  using StateType = State<Vga2dMultivector<ScalarType>, 3>;

  ::testing::AssertionResult result{CanIntegrateOverConstantJerk<RungeKutta4<StateType>>()};
  EXPECT_TRUE(result);
}

TEST(RungeKutta4_3StateTest, CanUpdateJerkFromPosition) {
  using ScalarType = float;
  using StateType = State<Vga2dMultivector<ScalarType>, 3>;

  ::testing::AssertionResult result{CanUpdateJerkFromPosition<RungeKutta4<StateType>>()};
  EXPECT_TRUE(result);
}

/**
 * Tests for state vectors containing 4 elements.
 */
TEST(RungeKutta4_4StateTest, CanIntegrateOverConstantVelocity) {
  using ScalarType = float;
  using StateType = State<Vga2dMultivector<ScalarType>, 4>;

  ::testing::AssertionResult result{CanIntegrateOverConstantVelocity<RungeKutta4<StateType>>()};
  EXPECT_TRUE(result);
}

TEST(RungeKutta4_4StateTest, CanUpdateVelocityFromPosition) {
  using ScalarType = float;
  using StateType = State<Vga2dMultivector<ScalarType>, 4>;

  ::testing::AssertionResult result{CanUpdateVelocityFromPosition<RungeKutta4<StateType>>()};
  EXPECT_TRUE(result);
}

TEST(RungeKutta4_4StateTest, CanIntegrateOverConstantAcceleration) {
  using ScalarType = float;
  using StateType = State<Vga2dMultivector<ScalarType>, 4>;

  ::testing::AssertionResult result{CanIntegrateOverConstantAcceleration<RungeKutta4<StateType>>()};
  EXPECT_TRUE(result);
}

TEST(RungeKutta4_4StateTest, CanUpdateAccelerationFromPosition) {
  using ScalarType = float;
  using StateType = State<Vga2dMultivector<ScalarType>, 4>;

  ::testing::AssertionResult result{CanUpdateAccelerationFromPosition<RungeKutta4<StateType>>()};
  EXPECT_TRUE(result);
}

TEST(RungeKutta4_4StateTest, CanIntegrateOverConstantJerk) {
  using ScalarType = float;
  using StateType = State<Vga2dMultivector<ScalarType>, 4>;

  ::testing::AssertionResult result{CanIntegrateOverConstantJerk<RungeKutta4<StateType>>()};
  EXPECT_TRUE(result);
}

TEST(RungeKutta4_4StateTest, CanUpdateJerkFromPosition) {
  using ScalarType = float;
  using StateType = State<Vga2dMultivector<ScalarType>, 4>;

  ::testing::AssertionResult result{CanUpdateJerkFromPosition<RungeKutta4<StateType>>()};
  EXPECT_TRUE(result);
}

TEST(RungeKutta4_4StateTest, CanIntegrateOverConstantSnap) {
  using ScalarType = float;
  using StateType = State<Vga2dMultivector<ScalarType>, 4>;

  ::testing::AssertionResult result{CanIntegrateOverConstantSnap<RungeKutta4<StateType>>()};
  EXPECT_TRUE(result);
}

TEST(RungeKutta4_4StateTest, CanUpdateSnap) {
  using ScalarType = float;
  using StateType = State<Vga2dMultivector<ScalarType>, 4>;

  ::testing::AssertionResult result{CanUpdateSnap<RungeKutta4<StateType>>()};
  EXPECT_TRUE(result);
}

}  // namespace ndyn::math
