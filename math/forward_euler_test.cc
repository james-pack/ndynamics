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
TEST(ForwardEuler_1StateTest, CanIntegrateOverConstantVelocity) {
  using ScalarType = float;
  using StateType = StateT<Vga2dMultivector<ScalarType>, 1>;
  using ValueType = typename StateType::ValueType;

  ::testing::AssertionResult result{
      CanIntegrateOverConstantVelocity<ForwardEuler<ScalarType, ValueType, StateType::depth()>,
                                       ScalarType, ValueType, StateType>()};
  EXPECT_TRUE(result);
}

TEST(ForwardEuler_1StateTest, CanUpdateVelocityFromPosition) {
  using ScalarType = float;
  using StateType = StateT<Vga2dMultivector<ScalarType>, 1>;
  using ValueType = typename StateType::ValueType;

  ::testing::AssertionResult result{
      CanUpdateVelocityFromPosition<ForwardEuler<ScalarType, ValueType, StateType::depth()>,
                                    ScalarType, ValueType, StateType>()};
  EXPECT_TRUE(result);
}

/**
 * Tests for state vectors containing 2 elements.
 */
TEST(ForwardEuler_2StateTest, CanIntegrateOverConstantVelocity) {
  using ScalarType = float;
  using StateType = StateT<Vga2dMultivector<ScalarType>, 2>;
  using ValueType = typename StateType::ValueType;

  ::testing::AssertionResult result{
      CanIntegrateOverConstantVelocity<ForwardEuler<ScalarType, ValueType, StateType::depth()>,
                                       ScalarType, ValueType, StateType>()};
  EXPECT_TRUE(result);
}

TEST(ForwardEuler_2StateTest, CanUpdateVelocityFromPosition) {
  using ScalarType = float;
  using StateType = StateT<Vga2dMultivector<ScalarType>, 2>;
  using ValueType = typename StateType::ValueType;

  ::testing::AssertionResult result{
      CanUpdateVelocityFromPosition<ForwardEuler<ScalarType, ValueType, StateType::depth()>,
                                    ScalarType, ValueType, StateType>()};
  EXPECT_TRUE(result);
}

TEST(ForwardEuler_2StateTest, CanIntegrateOverConstantAcceleration) {
  using ScalarType = float;
  using StateType = StateT<Vga2dMultivector<ScalarType>, 2>;
  using ValueType = typename StateType::ValueType;

  ::testing::AssertionResult result{
      CanIntegrateOverConstantAcceleration<ForwardEuler<ScalarType, ValueType, StateType::depth()>,
                                           ScalarType, ValueType, StateType>()};
  EXPECT_TRUE(result);
}

TEST(ForwardEuler_2StateTest, CanIntegrateOverConstantAccelerationManySteps) {
  using ScalarType = float;
  using StateType = StateT<ScalarType, 2>;

  ::testing::AssertionResult result{CanIntegrateOverConstantAccelerationManySteps<
      ForwardEuler<ScalarType, ScalarType, StateType::depth()>, ScalarType, StateType>()};
  EXPECT_TRUE(result);
}

TEST(ForwardEuler_2StateTest, CanUpdateAccelerationFromPosition) {
  using ScalarType = float;
  using StateType = StateT<Vga2dMultivector<ScalarType>, 2>;
  using ValueType = typename StateType::ValueType;

  ::testing::AssertionResult result{
      CanUpdateAccelerationFromPosition<ForwardEuler<ScalarType, ValueType, StateType::depth()>,
                                        ScalarType, ValueType, StateType>()};
  EXPECT_TRUE(result);
}

/**
 * Tests for state vectors containing 3 elements.
 */
TEST(ForwardEuler_3StateTest, CanIntegrateOverConstantVelocity) {
  using ScalarType = float;
  using StateType = State2d<ScalarType>;
  using ValueType = typename StateType::ValueType;

  ::testing::AssertionResult result{
      CanIntegrateOverConstantVelocity<ForwardEuler<ScalarType, ValueType, StateType::depth()>,
                                       ScalarType, ValueType, StateType>()};
  EXPECT_TRUE(result);
}

TEST(ForwardEuler_3StateTest, CanUpdateVelocityFromPosition) {
  using ScalarType = float;
  using StateType = State2d<ScalarType>;
  using ValueType = typename StateType::ValueType;

  ::testing::AssertionResult result{
      CanUpdateVelocityFromPosition<ForwardEuler<ScalarType, ValueType, StateType::depth()>,
                                    ScalarType, ValueType, StateType>()};
  EXPECT_TRUE(result);
}

TEST(ForwardEuler_3StateTest, CanIntegrateOverConstantAcceleration) {
  using ScalarType = float;
  using StateType = State2d<ScalarType>;
  using ValueType = typename StateType::ValueType;

  ::testing::AssertionResult result{
      CanIntegrateOverConstantAcceleration<ForwardEuler<ScalarType, ValueType, StateType::depth()>,
                                           ScalarType, ValueType, StateType>()};
  EXPECT_TRUE(result);
}

TEST(ForwardEuler_3StateTest, CanIntegrateOverConstantAccelerationManySteps) {
  using ScalarType = float;
  using StateType = StateT<ScalarType, 3>;

  ::testing::AssertionResult result{CanIntegrateOverConstantAccelerationManySteps<
      ForwardEuler<ScalarType, ScalarType, StateType::depth()>, ScalarType, StateType>()};
  EXPECT_TRUE(result);
}

TEST(ForwardEuler_3StateTest, CanUpdateAccelerationFromPosition) {
  using ScalarType = float;
  using StateType = State2d<ScalarType>;
  using ValueType = typename StateType::ValueType;

  ::testing::AssertionResult result{
      CanUpdateAccelerationFromPosition<ForwardEuler<ScalarType, ValueType, StateType::depth()>,
                                        ScalarType, ValueType, StateType>()};
  EXPECT_TRUE(result);
}

TEST(ForwardEuler_3StateTest, CanIntegrateOverConstantJerk) {
  using ScalarType = float;
  using StateType = State2d<ScalarType>;
  using ValueType = typename StateType::ValueType;

  ::testing::AssertionResult result{
      CanIntegrateOverConstantJerk<ForwardEuler<ScalarType, ValueType, StateType::depth()>,
                                   ScalarType, ValueType, StateType>()};
  EXPECT_TRUE(result);
}

TEST(ForwardEuler_3StateTest, CanUpdateJerkFromPosition) {
  using ScalarType = float;
  using StateType = State2d<ScalarType>;
  using ValueType = typename StateType::ValueType;

  ::testing::AssertionResult result{
      CanUpdateJerkFromPosition<ForwardEuler<ScalarType, ValueType, StateType::depth()>, ScalarType,
                                ValueType, StateType>()};
  EXPECT_TRUE(result);
}

/**
 * Tests for state vectors containing 4 elements.
 */
TEST(ForwardEuler_4StateTest, CanIntegrateOverConstantVelocity) {
  using ScalarType = float;
  using StateType = StateT<Vga2dMultivector<ScalarType>, 4>;
  using ValueType = typename StateType::ValueType;

  ::testing::AssertionResult result{
      CanIntegrateOverConstantVelocity<ForwardEuler<ScalarType, ValueType, StateType::depth()>,
                                       ScalarType, ValueType, StateType>()};
  EXPECT_TRUE(result);
}

TEST(ForwardEuler_4StateTest, CanUpdateVelocityFromPosition) {
  using ScalarType = float;
  using StateType = StateT<Vga2dMultivector<ScalarType>, 4>;
  using ValueType = typename StateType::ValueType;

  ::testing::AssertionResult result{
      CanUpdateVelocityFromPosition<ForwardEuler<ScalarType, ValueType, StateType::depth()>,
                                    ScalarType, ValueType, StateType>()};
  EXPECT_TRUE(result);
}

TEST(ForwardEuler_4StateTest, CanIntegrateOverConstantAcceleration) {
  using ScalarType = float;
  using StateType = StateT<Vga2dMultivector<ScalarType>, 4>;
  using ValueType = typename StateType::ValueType;

  ::testing::AssertionResult result{
      CanIntegrateOverConstantAcceleration<ForwardEuler<ScalarType, ValueType, StateType::depth()>,
                                           ScalarType, ValueType, StateType>()};
  EXPECT_TRUE(result);
}

TEST(ForwardEuler_4StateTest, CanIntegrateOverConstantAccelerationManySteps) {
  using ScalarType = float;
  using StateType = StateT<ScalarType, 4>;

  ::testing::AssertionResult result{CanIntegrateOverConstantAccelerationManySteps<
      ForwardEuler<ScalarType, ScalarType, StateType::depth()>, ScalarType, StateType>()};
  EXPECT_TRUE(result);
}

TEST(ForwardEuler_4StateTest, CanUpdateAccelerationFromPosition) {
  using ScalarType = float;
  using StateType = StateT<Vga2dMultivector<ScalarType>, 4>;
  using ValueType = typename StateType::ValueType;

  ::testing::AssertionResult result{
      CanUpdateAccelerationFromPosition<ForwardEuler<ScalarType, ValueType, StateType::depth()>,
                                        ScalarType, ValueType, StateType>()};
  EXPECT_TRUE(result);
}

TEST(ForwardEuler_4StateTest, CanIntegrateOverConstantJerk) {
  using ScalarType = float;
  using StateType = StateT<Vga2dMultivector<ScalarType>, 4>;
  using ValueType = typename StateType::ValueType;

  ::testing::AssertionResult result{
      CanIntegrateOverConstantJerk<ForwardEuler<ScalarType, ValueType, StateType::depth()>,
                                   ScalarType, ValueType, StateType>()};
  EXPECT_TRUE(result);
}

TEST(ForwardEuler_4StateTest, CanUpdateJerkFromPosition) {
  using ScalarType = float;
  using StateType = StateT<Vga2dMultivector<ScalarType>, 4>;
  using ValueType = typename StateType::ValueType;

  ::testing::AssertionResult result{
      CanUpdateJerkFromPosition<ForwardEuler<ScalarType, ValueType, StateType::depth()>, ScalarType,
                                ValueType, StateType>()};
  EXPECT_TRUE(result);
}

TEST(ForwardEuler_4StateTest, CanIntegrateOverConstantSnap) {
  using ScalarType = float;
  using StateType = StateT<Vga2dMultivector<ScalarType>, 4>;
  using ValueType = typename StateType::ValueType;

  ::testing::AssertionResult result{
      CanIntegrateOverConstantSnap<ForwardEuler<ScalarType, ValueType, StateType::depth()>,
                                   ScalarType, ValueType, StateType>()};
  EXPECT_TRUE(result);
}

TEST(ForwardEuler_4StateTest, CanUpdateSnap) {
  using ScalarType = float;
  using StateType = StateT<Vga2dMultivector<ScalarType>, 4>;
  using ValueType = typename StateType::ValueType;

  ::testing::AssertionResult result{
      CanUpdateSnap<ForwardEuler<ScalarType, ValueType, StateType::depth()>, ScalarType, ValueType,
                    StateType>()};
  EXPECT_TRUE(result);
}

}  // namespace ndyn::math
