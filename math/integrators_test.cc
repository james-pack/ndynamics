#include "math/integrators.h"

#include "gtest/gtest.h"
#include "math/multivector.h"
#include "math/states.h"

namespace ndyn::math {

TEST(ForwardEuler2StateTest, CanHandleConstantVelocity) {
  using ScalarType = float;
  using StateType = StateT<Vga2dMultivector<ScalarType>, 2>;
  using ValueType = typename StateType::ValueType;

  static constexpr ScalarType delta_t{0.5f};
  static constexpr ScalarType velocity{1.f};
  static constexpr ScalarType initial_position{3.f};

  static constexpr StateType s0{{0.f, initial_position, 0.f}, {0.f, velocity, 0.f}};

  static constexpr ValueType expected_position{0.f, initial_position + delta_t * velocity, 0.f};

  ForwardEuler<ScalarType, ValueType, StateType::size()> stepper{};
  StateType s1 = stepper(delta_t, s0);

  EXPECT_EQ(expected_position, s1.element<0>());
  EXPECT_EQ(s0.element<1>(), s1.element<1>());
}

TEST(ForwardEuler2StateTest, CanUpdateVelocityFromPosition) {
  using ScalarType = float;
  using StateType = StateT<Vga2dMultivector<ScalarType>, 2>;
  using ValueType = typename StateType::ValueType;

  static constexpr ScalarType delta_t{0.5f};
  static constexpr ScalarType initial_velocity{1.f};
  static constexpr ScalarType initial_position{3.f};

  static constexpr StateType s0{{0.f, initial_position, 0.f}, {0.f, initial_velocity, 0.f}};

  static constexpr ValueType expected_position{0.f, initial_position + delta_t * initial_position,
                                               0.f};

  static constexpr ValueType expected_velocity{0.f, initial_position, 0.f};

  ForwardEuler<ScalarType, ValueType, StateType::size()> stepper{
      [](const StateType& s0) -> StateType {
        StateType result{s0};
        result.set_element<1>(result.element<0>());
        return result;
      }};

  StateType s1 = stepper(delta_t, s0);

  EXPECT_EQ(expected_position, s1.element<0>());
  EXPECT_EQ(expected_velocity, s1.element<1>());
}

TEST(ForwardEuler3StateTest, CanHandleConstantVelocity) {
  using ScalarType = float;
  using StateType = State2d<ScalarType>;
  using ValueType = typename StateType::ValueType;

  static constexpr ScalarType delta_t{0.5f};
  static constexpr ScalarType velocity{1.f};
  static constexpr ScalarType initial_position{3.f};

  static constexpr StateType s0{
      {0.f, initial_position, 0.f}, {0.f, velocity, 0.f}, {0.f, 0.f, 0.f}};

  static constexpr ValueType expected_position{0.f, initial_position + delta_t * velocity, 0.f};

  ForwardEuler<ScalarType, ValueType, StateType::size()> stepper{};
  StateType s1 = stepper(delta_t, s0);

  EXPECT_EQ(expected_position, s1.element<0>());
  EXPECT_EQ(s0.element<1>(), s1.element<1>());
  EXPECT_EQ(s0.element<2>(), s1.element<2>());
}

TEST(ForwardEuler3StateTest, CanHandleConstantAcceleration) {
  using ScalarType = float;
  using StateType = State2d<ScalarType>;
  using ValueType = typename StateType::ValueType;

  static constexpr ScalarType delta_t{0.5f};
  static constexpr ScalarType acceleration{5.f};
  static constexpr ScalarType initial_velocity{1.f};
  static constexpr ScalarType initial_position{3.f};

  static constexpr StateType s0{
      {0.f, initial_position, 0.f}, {0.f, initial_velocity, 0.f}, {0.f, acceleration, 0.f}};

  static constexpr ValueType expected_position{
      0.f, initial_position + delta_t * (initial_velocity + delta_t * acceleration), 0.f};

  static constexpr ValueType expected_velocity{0.f, initial_velocity + delta_t * acceleration, 0.f};

  ForwardEuler<ScalarType, ValueType, StateType::size()> stepper{};
  StateType s1 = stepper(delta_t, s0);

  EXPECT_EQ(expected_position, s1.element<0>());
  EXPECT_EQ(expected_velocity, s1.element<1>());
  EXPECT_EQ(s0.element<2>(), s1.element<2>());
}

TEST(ForwardEuler3StateTest, CanUpdateAccelerationFromPosition) {
  using ScalarType = float;
  using StateType = State2d<ScalarType>;
  using ValueType = typename StateType::ValueType;

  static constexpr ScalarType delta_t{0.5f};
  static constexpr ScalarType acceleration{5.f};
  static constexpr ScalarType initial_velocity{1.f};
  static constexpr ScalarType initial_position{3.f};

  static constexpr StateType s0{
      {0.f, initial_position, 0.f}, {0.f, initial_velocity, 0.f}, {0.f, acceleration, 0.f}};

  static constexpr ValueType expected_position{
      0.f, initial_position + delta_t * (initial_velocity + delta_t * initial_position), 0.f};

  static constexpr ValueType expected_velocity{0.f, initial_velocity + delta_t * initial_position,
                                               0.f};
  static constexpr ValueType expected_acceleration{0.f, initial_position, 0.f};

  ForwardEuler<ScalarType, ValueType, StateType::size()> stepper{
      [](const StateType& s0) -> StateType {
        StateType result{s0};
        result.set_element<2>(result.element<0>());
        return result;
      }};

  StateType s1 = stepper(delta_t, s0);

  EXPECT_EQ(expected_position, s1.element<0>());
  EXPECT_EQ(expected_velocity, s1.element<1>());
  EXPECT_EQ(expected_acceleration, s1.element<2>());
}

TEST(ForwardEuler4StateTest, CanHandleConstantVelocity) {
  using ScalarType = float;
  using StateType = StateT<Vga2dMultivector<ScalarType>, 4>;
  using ValueType = typename StateType::ValueType;

  static constexpr ScalarType delta_t{0.5f};
  static constexpr ScalarType velocity{1.f};
  static constexpr ScalarType initial_position{3.f};

  static constexpr StateType s0{
      {0.f, initial_position, 0.f}, {0.f, velocity, 0.f}, {0.f, 0.f, 0.f}};

  static constexpr ValueType expected_position{0.f, initial_position + delta_t * velocity, 0.f};

  ForwardEuler<ScalarType, ValueType, StateType::size()> stepper{};
  StateType s1 = stepper(delta_t, s0);

  EXPECT_EQ(expected_position, s1.element<0>());
  EXPECT_EQ(s0.element<1>(), s1.element<1>());
  EXPECT_EQ(s0.element<2>(), s1.element<2>());
  EXPECT_EQ(s0.element<3>(), s1.element<3>());
}

TEST(ForwardEuler4StateTest, CanHandleConstantAcceleration) {
  using ScalarType = float;
  using StateType = StateT<Vga2dMultivector<ScalarType>, 4>;
  using ValueType = typename StateType::ValueType;

  static constexpr ScalarType delta_t{0.5f};
  static constexpr ScalarType acceleration{5.f};
  static constexpr ScalarType initial_velocity{1.f};
  static constexpr ScalarType initial_position{3.f};

  static constexpr StateType s0{
      {0.f, initial_position, 0.f}, {0.f, initial_velocity, 0.f}, {0.f, acceleration, 0.f}};

  static constexpr ValueType expected_position{
      0.f, initial_position + delta_t * (initial_velocity + delta_t * acceleration), 0.f};

  static constexpr ValueType expected_velocity{0.f, initial_velocity + delta_t * acceleration, 0.f};

  ForwardEuler<ScalarType, ValueType, StateType::size()> stepper{};
  StateType s1 = stepper(delta_t, s0);

  EXPECT_EQ(expected_position, s1.element<0>());
  EXPECT_EQ(expected_velocity, s1.element<1>());
  EXPECT_EQ(s0.element<2>(), s1.element<2>());
  EXPECT_EQ(s0.element<3>(), s1.element<3>());
}

TEST(ForwardEuler4StateTest, CanUpdateAccelerationFromPosition) {
  using ScalarType = float;
  using StateType = StateT<Vga2dMultivector<ScalarType>, 4>;
  using ValueType = typename StateType::ValueType;

  static constexpr ScalarType delta_t{0.5f};
  static constexpr ScalarType acceleration{5.f};
  static constexpr ScalarType initial_velocity{1.f};
  static constexpr ScalarType initial_position{3.f};

  static constexpr StateType s0{
      {0.f, initial_position, 0.f}, {0.f, initial_velocity, 0.f}, {0.f, acceleration, 0.f}};

  static constexpr ValueType expected_position{
      0.f, initial_position + delta_t * (initial_velocity + delta_t * initial_position), 0.f};

  static constexpr ValueType expected_velocity{0.f, initial_velocity + delta_t * initial_position,
                                               0.f};
  static constexpr ValueType expected_acceleration{0.f, initial_position, 0.f};

  ForwardEuler<ScalarType, ValueType, StateType::size()> stepper{
      [](const StateType& s0) -> StateType {
        StateType result{s0};
        result.set_element<2>(result.element<0>());
        return result;
      }};

  StateType s1 = stepper(delta_t, s0);

  EXPECT_EQ(expected_position, s1.element<0>());
  EXPECT_EQ(expected_velocity, s1.element<1>());
  EXPECT_EQ(expected_acceleration, s1.element<2>());
  EXPECT_EQ(s0.element<3>(), s1.element<3>());
}

TEST(ForwardEuler4StateTest, CanHandleConstantJerk) {
  using ScalarType = float;
  using StateType = StateT<Vga2dMultivector<ScalarType>, 4>;
  using ValueType = typename StateType::ValueType;

  static constexpr ScalarType delta_t{0.5f};
  static constexpr ScalarType jerk{7.f};
  static constexpr ScalarType initial_acceleration{5.f};
  static constexpr ScalarType initial_velocity{1.f};
  static constexpr ScalarType initial_position{3.f};

  static constexpr StateType s0{{0.f, initial_position, 0.f},
                                {0.f, initial_velocity, 0.f},
                                {0.f, initial_acceleration, 0.f},
                                {0.f, jerk, 0.f}};

  static constexpr ValueType expected_position{
      0.f,
      initial_position +
          delta_t * (initial_velocity + delta_t * (initial_acceleration + delta_t * jerk)),
      0.f};

  static constexpr ValueType expected_velocity{
      0.f, initial_velocity + delta_t * (initial_acceleration + delta_t * jerk), 0.f};
  static constexpr ValueType expected_acceleration{0.f, initial_acceleration + delta_t * jerk, 0.f};

  ForwardEuler<ScalarType, ValueType, StateType::size()> stepper{};
  StateType s1 = stepper(delta_t, s0);

  EXPECT_EQ(expected_position, s1.element<0>());
  EXPECT_EQ(expected_velocity, s1.element<1>());
  EXPECT_EQ(expected_acceleration, s1.element<2>());
  EXPECT_EQ(s0.element<3>(), s1.element<3>());
}

TEST(ForwardEuler4StateTest, CanUpdateJerk) {
  using ScalarType = float;
  using StateType = StateT<Vga2dMultivector<ScalarType>, 4>;
  using ValueType = typename StateType::ValueType;

  static constexpr ScalarType delta_t{0.5f};
  static constexpr ScalarType jerk{7.f};
  static constexpr ScalarType initial_acceleration{5.f};
  static constexpr ScalarType initial_velocity{1.f};
  static constexpr ScalarType initial_position{3.f};

  static constexpr StateType s0{{0.f, initial_position, 0.f},
                                {0.f, initial_velocity, 0.f},
                                {0.f, initial_acceleration, 0.f},
                                {0.f, jerk, 0.f}};

  static constexpr ValueType expected_position{
      0.f,
      initial_position + delta_t * (initial_velocity +
                                    delta_t * (initial_acceleration + delta_t * initial_position)),
      0.f};

  static constexpr ValueType expected_velocity{
      0.f, initial_velocity + delta_t * (initial_acceleration + delta_t * initial_position), 0.f};
  static constexpr ValueType expected_acceleration{
      0.f, initial_acceleration + delta_t * initial_position, 0.f};
  static constexpr ValueType expected_jerk{0.f, initial_position, 0.f};

  ForwardEuler<ScalarType, ValueType, StateType::size()> stepper{
      [](const StateType& s0) -> StateType {
        StateType result{s0};
        result.set_element<3>(result.element<0>());
        return result;
      }};

  StateType s1 = stepper(delta_t, s0);

  EXPECT_EQ(expected_position, s1.element<0>());
  EXPECT_EQ(expected_velocity, s1.element<1>());
  EXPECT_EQ(expected_acceleration, s1.element<2>());
  EXPECT_EQ(expected_jerk, s1.element<3>());
}

}  // namespace ndyn::math
