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
TEST(RungeKutta2_1StateTest, CanIntegrateOverConstantVelocity) {
  using ScalarType = float;
  using StateType = StateT<Vga2dMultivector<ScalarType>, 1>;
  using ValueType = typename StateType::ValueType;

  ::testing::AssertionResult result{
      CanIntegrateOverConstantVelocity<RungeKutta2<ScalarType, ValueType, StateType::depth()>,
                                       ScalarType, ValueType, StateType>()};
  EXPECT_TRUE(result);
}

TEST(RungeKutta2_1StateTest, CanUpdateVelocityFromPosition) {
  using ScalarType = float;
  using StateType = StateT<Vga2dMultivector<ScalarType>, 1>;
  using ValueType = typename StateType::ValueType;

  ::testing::AssertionResult result{
      CanUpdateVelocityFromPosition<RungeKutta2<ScalarType, ValueType, StateType::depth()>,
                                    ScalarType, ValueType, StateType>()};
  EXPECT_TRUE(result);
}

/**
 * Tests for state vectors containing 2 elements.
 */
TEST(RungeKutta2_2StateTest, CanIntegrateOverConstantVelocity) {
  using ScalarType = float;
  using StateType = StateT<Vga2dMultivector<ScalarType>, 2>;
  using ValueType = typename StateType::ValueType;

  ::testing::AssertionResult result{
      CanIntegrateOverConstantVelocity<RungeKutta2<ScalarType, ValueType, StateType::depth()>,
                                       ScalarType, ValueType, StateType>()};
  EXPECT_TRUE(result);
}

TEST(RungeKutta2_2StateTest, CanUpdateVelocityFromPosition) {
  using ScalarType = float;
  using StateType = StateT<Vga2dMultivector<ScalarType>, 2>;
  using ValueType = typename StateType::ValueType;

  ::testing::AssertionResult result{
      CanUpdateVelocityFromPosition<RungeKutta2<ScalarType, ValueType, StateType::depth()>,
                                    ScalarType, ValueType, StateType>()};
  EXPECT_TRUE(result);
}

TEST(RungeKutta2_2StateTest, CanIntegrateOverConstantAcceleration) {
  using ScalarType = float;
  using StateType = StateT<Vga2dMultivector<ScalarType>, 2>;
  using ValueType = typename StateType::ValueType;

  ::testing::AssertionResult result{
      CanIntegrateOverConstantAcceleration<RungeKutta2<ScalarType, ValueType, StateType::depth()>,
                                           ScalarType, ValueType, StateType>()};
  EXPECT_TRUE(result);
}

TEST(RungeKutta2_2StateTest, CanIntegrateOverConstantAccelerationManySteps) {
  using ScalarType = float;
  using StateType = StateT<ScalarType, 2>;

  ::testing::AssertionResult result{CanIntegrateOverConstantAccelerationManySteps<
      RungeKutta2<ScalarType, ScalarType, StateType::depth()>, ScalarType, StateType>()};
  EXPECT_TRUE(result);
}

TEST(RungeKutta2_2StateTest, CanUpdateAccelerationFromPosition) {
  using ScalarType = float;
  using StateType = StateT<Vga2dMultivector<ScalarType>, 2>;
  using ValueType = typename StateType::ValueType;

  ::testing::AssertionResult result{
      CanUpdateAccelerationFromPosition<RungeKutta2<ScalarType, ValueType, StateType::depth()>,
                                        ScalarType, ValueType, StateType>()};
  EXPECT_TRUE(result);
}

/**
 * Tests for state vectors containing 3 elements.
 */
TEST(RungeKutta2_3StateTest, CanIntegrateOverConstantVelocity) {
  using ScalarType = float;
  using StateType = State2d<ScalarType>;
  using ValueType = typename StateType::ValueType;

  ::testing::AssertionResult result{
      CanIntegrateOverConstantVelocity<RungeKutta2<ScalarType, ValueType, StateType::depth()>,
                                       ScalarType, ValueType, StateType>()};
  EXPECT_TRUE(result);
}

TEST(RungeKutta2_3StateTest, CanUpdateVelocityFromPosition) {
  using ScalarType = float;
  using StateType = State2d<ScalarType>;
  using ValueType = typename StateType::ValueType;

  ::testing::AssertionResult result{
      CanUpdateVelocityFromPosition<RungeKutta2<ScalarType, ValueType, StateType::depth()>,
                                    ScalarType, ValueType, StateType>()};
  EXPECT_TRUE(result);
}

TEST(RungeKutta2_3StateTest, CanIntegrateOverConstantAcceleration) {
  using ScalarType = float;
  using StateType = State2d<ScalarType>;
  using ValueType = typename StateType::ValueType;

  ::testing::AssertionResult result{
      CanIntegrateOverConstantAcceleration<RungeKutta2<ScalarType, ValueType, StateType::depth()>,
                                           ScalarType, ValueType, StateType>()};
  EXPECT_TRUE(result);
}

TEST(RungeKutta2_3StateTest, CanUpdateAccelerationFromPosition) {
  using ScalarType = float;
  using StateType = State2d<ScalarType>;
  using ValueType = typename StateType::ValueType;

  ::testing::AssertionResult result{
      CanUpdateAccelerationFromPosition<RungeKutta2<ScalarType, ValueType, StateType::depth()>,
                                        ScalarType, ValueType, StateType>()};
  EXPECT_TRUE(result);
}

TEST(RungeKutta2_3StateTest, CanIntegrateOverConstantJerk) {
  using ScalarType = float;
  using StateType = State2d<ScalarType>;
  using ValueType = typename StateType::ValueType;

  ::testing::AssertionResult result{
      CanIntegrateOverConstantJerk<RungeKutta2<ScalarType, ValueType, StateType::depth()>,
                                   ScalarType, ValueType, StateType>()};
  EXPECT_TRUE(result);
}

TEST(RungeKutta2_3StateTest, CanUpdateJerkFromPosition) {
  using ScalarType = float;
  using StateType = State2d<ScalarType>;
  using ValueType = typename StateType::ValueType;

  ::testing::AssertionResult result{
      CanUpdateJerkFromPosition<RungeKutta2<ScalarType, ValueType, StateType::depth()>, ScalarType,
                                ValueType, StateType>()};
  EXPECT_TRUE(result);
}

/**
 * Tests for state vectors containing 4 elements.
 */
TEST(RungeKutta2_4StateTest, CanIntegrateOverConstantVelocity) {
  using ScalarType = float;
  using StateType = StateT<Vga2dMultivector<ScalarType>, 4>;
  using ValueType = typename StateType::ValueType;

  ::testing::AssertionResult result{
      CanIntegrateOverConstantVelocity<RungeKutta2<ScalarType, ValueType, StateType::depth()>,
                                       ScalarType, ValueType, StateType>()};
  EXPECT_TRUE(result);
}

TEST(RungeKutta2_4StateTest, CanUpdateVelocityFromPosition) {
  using ScalarType = float;
  using StateType = StateT<Vga2dMultivector<ScalarType>, 4>;
  using ValueType = typename StateType::ValueType;

  ::testing::AssertionResult result{
      CanUpdateVelocityFromPosition<RungeKutta2<ScalarType, ValueType, StateType::depth()>,
                                    ScalarType, ValueType, StateType>()};
  EXPECT_TRUE(result);
}

TEST(RungeKutta2_4StateTest, CanIntegrateOverConstantAcceleration) {
  using ScalarType = float;
  using StateType = StateT<Vga2dMultivector<ScalarType>, 4>;
  using ValueType = typename StateType::ValueType;

  ::testing::AssertionResult result{
      CanIntegrateOverConstantAcceleration<RungeKutta2<ScalarType, ValueType, StateType::depth()>,
                                           ScalarType, ValueType, StateType>()};
  EXPECT_TRUE(result);
}

TEST(RungeKutta2_4StateTest, CanUpdateAccelerationFromPosition) {
  using ScalarType = float;
  using StateType = StateT<Vga2dMultivector<ScalarType>, 4>;
  using ValueType = typename StateType::ValueType;

  ::testing::AssertionResult result{
      CanUpdateAccelerationFromPosition<RungeKutta2<ScalarType, ValueType, StateType::depth()>,
                                        ScalarType, ValueType, StateType>()};
  EXPECT_TRUE(result);
}

TEST(RungeKutta2_4StateTest, CanIntegrateOverConstantJerk) {
  using ScalarType = float;
  using StateType = StateT<Vga2dMultivector<ScalarType>, 4>;
  using ValueType = typename StateType::ValueType;

  ::testing::AssertionResult result{
      CanIntegrateOverConstantJerk<RungeKutta2<ScalarType, ValueType, StateType::depth()>,
                                   ScalarType, ValueType, StateType>()};
  EXPECT_TRUE(result);
}

TEST(RungeKutta2_4StateTest, CanUpdateJerkFromPosition) {
  using ScalarType = float;
  using StateType = StateT<Vga2dMultivector<ScalarType>, 4>;
  using ValueType = typename StateType::ValueType;

  ::testing::AssertionResult result{
      CanUpdateJerkFromPosition<RungeKutta2<ScalarType, ValueType, StateType::depth()>, ScalarType,
                                ValueType, StateType>()};
  EXPECT_TRUE(result);
}

TEST(RungeKutta2_4StateTest, CanIntegrateOverConstantSnap) {
  using ScalarType = float;
  using StateType = StateT<Vga2dMultivector<ScalarType>, 4>;
  using ValueType = typename StateType::ValueType;

  ::testing::AssertionResult result{
      CanIntegrateOverConstantSnap<RungeKutta2<ScalarType, ValueType, StateType::depth()>,
                                   ScalarType, ValueType, StateType>()};
  EXPECT_TRUE(result);
}

TEST(RungeKutta2_4StateTest, CanUpdateSnap) {
  using ScalarType = float;
  using StateType = StateT<Vga2dMultivector<ScalarType>, 4>;
  using ValueType = typename StateType::ValueType;

  ::testing::AssertionResult result{
      CanUpdateSnap<RungeKutta2<ScalarType, ValueType, StateType::depth()>, ScalarType, ValueType,
                    StateType>()};
  EXPECT_TRUE(result);
}

}  // namespace ndyn::math
