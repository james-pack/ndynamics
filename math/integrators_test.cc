#include "math/integrators.h"

#include "gtest/gtest.h"
#include "math/multivector.h"
#include "math/multivector_test_utils.h"
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

TEST(RungeKutta22StateTest, CanHandleConstantVelocity) {
  using ScalarType = float;
  using StateType = StateT<Vga2dMultivector<ScalarType>, 2>;
  using ValueType = typename StateType::ValueType;

  static constexpr ScalarType delta_t{0.5f};
  static constexpr ScalarType velocity{1.f};
  static constexpr ScalarType initial_position{3.f};

  static constexpr StateType s0{{0.f, initial_position, 0.f}, {0.f, velocity, 0.f}};

  static constexpr ValueType expected_position{0.f, initial_position + delta_t * velocity, 0.f};

  RungeKutta2<ScalarType, ValueType, StateType::size()> stepper{};
  StateType s1 = stepper(delta_t, s0);

  EXPECT_EQ(expected_position, s1.element<0>());
  EXPECT_EQ(s0.element<1>(), s1.element<1>());
}

TEST(RungeKutta22StateTest, CanUpdateVelocityFromPosition) {
  using std::exp;

  using ScalarType = float;
  using StateType = StateT<Vga2dMultivector<ScalarType>, 2>;
  using ValueType = typename StateType::ValueType;

  static constexpr ScalarType delta_t{0.01f};
  static constexpr ScalarType initial_position{3.f};

  static constexpr StateType s0{{0.f, initial_position, 0.f}};

  /**
   * This test is simulating an exponential. The analytic solution is
   *
   * position = A * e^(-t)
   * velocity = -A * e^(-t)
   *
   * where A = initial_position. We set up the expected state according to this analytic solution.
   */

  static constexpr StateType expected{{0.f, initial_position * exp(-delta_t), 0.f},  //
                                      {0.f, -initial_position * exp(-delta_t), 0.f}};

  RungeKutta2<ScalarType, ValueType, StateType::size()> stepper{
      [](const StateType& s0) -> StateType {
        StateType result{s0};
        result.set_element<1>(-result.element<0>());
        return result;
      }};

  StateType s1 = stepper(delta_t, s0);

  EXPECT_TRUE(AreNear(expected.element<0>(), s1.element<0>(), 0.05f));
  EXPECT_TRUE(AreNear(expected.element<1>(), s1.element<1>(), 0.05f));
}

TEST(RungeKutta23StateTest, CanHandleConstantVelocity) {
  using ScalarType = float;
  using StateType = State2d<ScalarType>;
  using ValueType = typename StateType::ValueType;

  static constexpr ScalarType delta_t{0.5f};
  static constexpr ScalarType velocity{1.f};
  static constexpr ScalarType initial_position{3.f};

  static constexpr StateType s0{
      {0.f, initial_position, 0.f}, {0.f, velocity, 0.f}, {0.f, 0.f, 0.f}};

  static constexpr ValueType expected_position{0.f, initial_position + delta_t * velocity, 0.f};

  RungeKutta2<ScalarType, ValueType, StateType::size()> stepper{};
  StateType s1 = stepper(delta_t, s0);

  EXPECT_EQ(expected_position, s1.element<0>());
  EXPECT_EQ(s0.element<1>(), s1.element<1>());
  EXPECT_EQ(s0.element<2>(), s1.element<2>());
}

TEST(RungeKutta23StateTest, CanHandleConstantAcceleration) {
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

  RungeKutta2<ScalarType, ValueType, StateType::size()> stepper{};
  StateType s1 = stepper(delta_t, s0);

  EXPECT_EQ(expected_position, s1.element<0>());
  EXPECT_EQ(expected_velocity, s1.element<1>());
  EXPECT_EQ(s0.element<2>(), s1.element<2>());
}

TEST(RungeKutta23StateTest, CanUpdateAccelerationFromPosition) {
  using std::cos;
  using std::sin;

  using ScalarType = float;
  using StateType = State2d<ScalarType>;
  using ValueType = typename StateType::ValueType;

  static constexpr ScalarType delta_t{0.1f};
  static constexpr ScalarType initial_position{3.f};

  static constexpr StateType s0{{0.f, initial_position, 0.f}};

  /**
   * This test is simulating an harmonic oscillator. The analytic solution is
   *
   * position = A * cos(t + a)
   * velocity = -A * sin(t + a)
   * acceleration = -A * cos(t + a)
   *
   * where A = initial_position, and a = 0 since we are assuming an initial velocity of zero.
   * We set up the expected state according to this analytic solution.
   */

  static constexpr StateType expected{{0.f, initial_position * cos(delta_t), 0.f},   //
                                      {0.f, -initial_position * sin(delta_t), 0.f},  //
                                      {0.f, -initial_position * cos(delta_t), 0.f}};

  RungeKutta2<ScalarType, ValueType, StateType::size()> stepper{
      [](const StateType& s0) -> StateType {
        StateType result{s0};
        result.set_element<2>(-result.element<0>());
        return result;
      }};

  StateType s1 = stepper(delta_t, s0);

  EXPECT_TRUE(AreNear(expected.element<0>(), s1.element<0>(), 0.1f));
  EXPECT_TRUE(AreNear(expected.element<1>(), s1.element<1>(), 0.1f));
  EXPECT_TRUE(AreNear(expected.element<2>(), s1.element<2>(), 0.1f));
}

TEST(RungeKutta24StateTest, CanHandleConstantVelocity) {
  using ScalarType = float;
  using StateType = StateT<Vga2dMultivector<ScalarType>, 4>;
  using ValueType = typename StateType::ValueType;

  static constexpr ScalarType delta_t{0.5f};
  static constexpr ScalarType velocity{1.f};
  static constexpr ScalarType initial_position{3.f};

  static constexpr StateType s0{
      {0.f, initial_position, 0.f}, {0.f, velocity, 0.f}, {0.f, 0.f, 0.f}};

  static constexpr ValueType expected_position{0.f, initial_position + delta_t * velocity, 0.f};

  RungeKutta2<ScalarType, ValueType, StateType::size()> stepper{};
  StateType s1 = stepper(delta_t, s0);

  EXPECT_EQ(expected_position, s1.element<0>());
  EXPECT_EQ(s0.element<1>(), s1.element<1>());
  EXPECT_EQ(s0.element<2>(), s1.element<2>());
  EXPECT_EQ(s0.element<3>(), s1.element<3>());
}

TEST(RungeKutta24StateTest, CanHandleConstantAcceleration) {
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

  RungeKutta2<ScalarType, ValueType, StateType::size()> stepper{};
  StateType s1 = stepper(delta_t, s0);

  EXPECT_EQ(expected_position, s1.element<0>());
  EXPECT_EQ(expected_velocity, s1.element<1>());
  EXPECT_EQ(s0.element<2>(), s1.element<2>());
  EXPECT_EQ(s0.element<3>(), s1.element<3>());
}

TEST(RungeKutta24StateTest, CanUpdateAccelerationFromPosition) {
  using std::cos;
  using std::sin;

  using ScalarType = float;
  using StateType = StateT<Vga2dMultivector<ScalarType>, 4>;
  using ValueType = typename StateType::ValueType;

  static constexpr ScalarType delta_t{0.1f};
  static constexpr ScalarType initial_position{3.f};

  static constexpr StateType s0{{0.f, initial_position, 0.f}};

  /**
   * This test is simulating an harmonic oscillator. The analytic solution is
   *
   * position = A * cos(t + a)
   * velocity = -A * sin(t + a)
   * acceleration = -A * cos(t + a)
   *
   * where A = initial_position, and a = 0 since we are assuming an initial velocity of zero.
   * We set up the expected state according to this analytic solution.
   */

  static constexpr StateType expected{{0.f, initial_position * cos(delta_t), 0.f},   //
                                      {0.f, -initial_position * sin(delta_t), 0.f},  //
                                      {0.f, -initial_position * cos(delta_t), 0.f}};

  RungeKutta2<ScalarType, ValueType, StateType::size()> stepper{
      [](const StateType& s0) -> StateType {
        StateType result{s0};
        result.set_element<2>(-result.element<0>());
        return result;
      }};

  StateType s1 = stepper(delta_t, s0);

  LOG(INFO) << "s1: " << s1;

  EXPECT_TRUE(AreNear(expected.element<0>(), s1.element<0>(), 0.1f));
  EXPECT_TRUE(AreNear(expected.element<1>(), s1.element<1>(), 0.1f));
  EXPECT_TRUE(AreNear(expected.element<2>(), s1.element<2>(), 0.1f));
  EXPECT_TRUE(AreNear(expected.element<3>(), s1.element<3>(), 0.1f));
}

TEST(RungeKutta24StateTest, CanHandleConstantJerk) {
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

  RungeKutta2<ScalarType, ValueType, StateType::size()> stepper{};
  StateType s1 = stepper(delta_t, s0);

  EXPECT_EQ(expected_position, s1.element<0>());
  EXPECT_EQ(expected_velocity, s1.element<1>());
  EXPECT_EQ(expected_acceleration, s1.element<2>());
  EXPECT_EQ(s0.element<3>(), s1.element<3>());
}

TEST(RungeKutta24StateTest, CanUpdateJerk) {
  using std::cos;
  using std::sin;

  using ScalarType = float;
  using StateType = StateT<Vga2dMultivector<ScalarType>, 4>;
  using ValueType = typename StateType::ValueType;

  static constexpr ScalarType delta_t{0.05f};
  static constexpr ScalarType initial_position{3.f};

  static constexpr StateType s0{{0.f, 0.f, 0.f}, {0.f, initial_position, 0.f}};

  /**
   * This test is simulating an harmonic oscillator but with an odometer that takes off distance
   * when put in reverse. (See _Ferris Bueller's Day Off_ for details of this odometer design.) The
   * test name includes the concept of jerk, since we are dealing with a 3rd derivative in some
   * sense.
   *
   * odometer = A * sin(t + a)
   * position = A * cos(t + a)
   * velocity = -A * sin(t + a)
   * acceleration = -A * cos(t + a)
   *
   * where A = initial_position, and a = 0 since we are assuming an initial velocity of zero.
   * We set up the expected state according to this analytic solution.
   */

  static constexpr StateType expected{{0.f, initial_position * sin(delta_t), 0.f},   //
                                      {0.f, initial_position * cos(delta_t), 0.f},   //
                                      {0.f, -initial_position * sin(delta_t), 0.f},  //
                                      {0.f, -initial_position * cos(delta_t), 0.f}};

  RungeKutta2<ScalarType, ValueType, StateType::size()> stepper{
      [](const StateType& s0) -> StateType {
        StateType result{s0};
        result.set_element<3>(-result.element<1>());
        return result;
      }};

  StateType s1 = stepper(delta_t, s0);

  EXPECT_TRUE(AreNear(expected.element<0>(), s1.element<0>(), 0.01f));
  EXPECT_TRUE(AreNear(expected.element<1>(), s1.element<1>(), 0.01f));
  EXPECT_TRUE(AreNear(expected.element<2>(), s1.element<2>(), 0.01f));
  EXPECT_TRUE(AreNear(expected.element<3>(), s1.element<3>(), 0.01f));
}

}  // namespace ndyn::math
