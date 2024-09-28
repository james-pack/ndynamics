#pragma once

#include <cmath>

#include "gtest/gtest.h"
#include "math/integrators.h"
#include "math/multivector.h"
#include "math/multivector_test_utils.h"

namespace ndyn::math {

::testing::AssertionResult AreNear(float lhs, float rhs, float epsilon) {
  using std::abs;
  if (abs(lhs - rhs) > abs(epsilon)) {
    return ::testing::AssertionFailure()
           << "lhs: " << lhs << ", rhs: " << rhs << ", abs difference: " << abs(lhs - rhs)
           << ", epsilon: " << abs(epsilon);
  }
  return ::testing::AssertionSuccess();
}

template <typename StateType>
ComputePartials<StateType> shift_state() {
  return [](const StateType& state) { return state.shift(); };
}

template <typename Integrator>
::testing::AssertionResult CanIntegrateOverConstantVelocity() {
  using std::sqrt;

  using StateType = typename Integrator::StateType;
  using ValueType = typename StateType::ValueType;
  using ScalarType = typename StateType::ScalarType;

  static constexpr ScalarType delta_t{0.1f};
  static constexpr ScalarType velocity{1.f};
  static constexpr ScalarType initial_position{3.f};

  static constexpr StateType s0{ValueType{0.f, initial_position, 0.f}};
  static constexpr ValueType expected_position{0.f, initial_position + delta_t * velocity, 0.f};

  Integrator integrator{[](const StateType& state) -> StateType {
    StateType result{state.shift()};
    result.template set_element<0>({0.f, velocity, 0.f});
    return result;
  }};

  StateType s1 = integrator(delta_t, s0);

  return AreNear(expected_position, s1.template element<0>(), sqrt(2 * delta_t))
         << " <- element<0>";
}

template <typename Integrator>
::testing::AssertionResult CanIntegrateOverConstantAcceleration() {
  /**
   * This test models an object falling in gravity, but with all signs positive. The analytic
   * solution is:
   *
   * x(t) = 0.5*a*t^2 + v0*t + x0
   * v(t) = a*t + v0
   * a(t) = a0
   *
   * where a0 = acceleration, v0 = initial_velocity, and x0 = initial_position.
   *
   * We test that the given integration method approximately arrives at this solution.
   */

  using std::sqrt;

  using StateType = typename Integrator::StateType;
  using ValueType = typename StateType::ValueType;
  using ScalarType = typename StateType::ScalarType;

  if constexpr (StateType::depth() >= 2) {
    static constexpr ScalarType delta_t{0.1f};
    static constexpr ScalarType acceleration{5.f};
    static constexpr ScalarType initial_velocity{1.f};
    static constexpr ScalarType initial_position{3.f};

    static constexpr StateType s0{{0.f, initial_position, 0.f}, {0.f, initial_velocity, 0.f}};

    static constexpr ValueType expected_position{
        0.f,
        initial_position + delta_t * initial_velocity +
            static_cast<ScalarType>(0.5) * acceleration * delta_t * delta_t,
        0.f};
    static constexpr ValueType expected_velocity{0.f, initial_velocity + delta_t * acceleration,
                                                 0.f};

    Integrator integrator{[](const StateType& state) -> StateType {
      StateType result{state.shift()};
      result.template set_element<1>({0.f, acceleration, 0.f});
      return result;
    }};

    StateType s1 = integrator(delta_t, s0);

    ::testing::AssertionResult result{::testing::AssertionSuccess()};
    result = AreNear(expected_position, s1.template element<0>(), sqrt(2 * delta_t))
             << " <- element<0>";
    if (!result) {
      return result;
    }
    result = AreNear(expected_velocity, s1.template element<1>(), sqrt(2 * delta_t))
             << " <- element<1>";
    return result;
  } else {
    return ::testing::AssertionSuccess();
  }
}

template <typename Integrator>
::testing::AssertionResult CanIntegrateOverConstantJerk() {
  /**
   * This test models an object falling in gravity but with some form of motion control to gradually
   * allow acceleration to reach its full amount. This could be an elevator that allows itself to
   * accelerate under gravity but with limits. The analytic solution is:
   *
   * x(t) = 1/6*j*t^3 + 0.5*a*t^2 + v0*t + x0
   * v(t) = 0.5*j*t^2 + a*t + v0
   * a(t) = j*t + a0
   * j(t) = j0
   *
   * where j0 = jerk, a0 = initial_acceleration, v0 = initial_velocity, and x0 = initial_position.
   *
   * We test that the given integration method approximately arrives at this solution.
   */
  using std::sqrt;

  using StateType = typename Integrator::StateType;
  using ValueType = typename StateType::ValueType;
  using ScalarType = typename StateType::ScalarType;

  if constexpr (StateType::depth() >= 3) {
    static constexpr ScalarType delta_t{0.1f};
    static constexpr ScalarType jerk{7.f};
    static constexpr ScalarType initial_acceleration{5.f};
    static constexpr ScalarType initial_velocity{1.f};
    static constexpr ScalarType initial_position{3.f};

    static constexpr StateType s0{{0.f, initial_position, 0.f},
                                  {0.f, initial_velocity, 0.f},
                                  {0.f, initial_acceleration, 0.f}};

    static constexpr ValueType expected_position{
        0.f,
        initial_position +
            delta_t * (initial_velocity +
                       0.5f * delta_t * (initial_acceleration + 1.f / 3.f * delta_t * jerk)),
        0.f};
    static constexpr ValueType expected_velocity{
        0.f, initial_velocity + delta_t * (initial_acceleration + 0.5f * delta_t * jerk), 0.f};
    static constexpr ValueType expected_acceleration{0.f, initial_acceleration + delta_t * jerk,
                                                     0.f};

    Integrator integrator{[](const StateType& state) -> StateType {
      StateType result{state.shift()};
      result.template set_element<2>({0.f, jerk, 0.f});
      return result;
    }};

    StateType s1 = integrator(delta_t, s0);

    ::testing::AssertionResult result{::testing::AssertionSuccess()};
    result = AreNear(expected_position, s1.template element<0>(), sqrt(2 * delta_t))
             << " <- element<0>";
    if (!result) {
      return result;
    }
    result = AreNear(expected_velocity, s1.template element<1>(), sqrt(2 * delta_t))
             << " <- element<1>";
    if (!result) {
      return result;
    }
    result = AreNear(expected_acceleration, s1.template element<2>(), sqrt(2 * delta_t))
             << " <- element<2>";
    return result;
  } else {
    return ::testing::AssertionSuccess();
  }
}

template <typename Integrator>
::testing::AssertionResult CanIntegrateOverConstantSnap() {
  /**
   * I have no idea what this models, but we have a simple differential equation of a constant 4th
   * derivative (snap) of the position.
   *
   * x(t) = 1/24*s*t^4 + 1/6*j*t^3 + 1/2*a*t^2 + v*t + x0
   * v(t) = 1/6*s*t^3 + 0.5*j*t^2 + a*t + v0
   * a(t) = 0.5*s*t^2 + j*t + a0
   * j(t) = s*t + j0
   * s(t) = s0
   *
   * where s0 = snap, j0 = initial_jerk, a0 = initial_acceleration, v0 = initial_velocity, and x0 =
   * initial_position.
   *
   * We test that the given integration method approximately arrives at this solution.
   */
  using std::sqrt;

  using StateType = typename Integrator::StateType;
  using ValueType = typename StateType::ValueType;
  using ScalarType = typename StateType::ScalarType;

  if constexpr (StateType::depth() >= 4) {
    static constexpr ScalarType delta_t{0.1f};
    static constexpr ScalarType snap{2.f};
    static constexpr ScalarType initial_jerk{7.f};
    static constexpr ScalarType initial_acceleration{5.f};
    static constexpr ScalarType initial_velocity{1.f};
    static constexpr ScalarType initial_position{3.f};

    static constexpr StateType s0{{0.f, initial_position, 0.f},
                                  {0.f, initial_velocity, 0.f},
                                  {0.f, initial_acceleration, 0.f},
                                  {0.f, initial_jerk, 0.f}};

    static constexpr ValueType expected_position{
        0.f,
        initial_position +
            delta_t * (initial_velocity + delta_t / 2 *
                                              (initial_acceleration +
                                               delta_t / 3 * (initial_jerk + delta_t / 4 * snap))),
        0.f};
    static constexpr ValueType expected_velocity{
        0.f,
        initial_velocity +
            delta_t * (initial_acceleration + delta_t / 2 * (initial_jerk + delta_t / 3 * snap)),
        0.f};
    static constexpr ValueType expected_acceleration{
        0.f, initial_acceleration + delta_t * (initial_jerk + delta_t / 2 * snap), 0.f};
    static constexpr ValueType expected_jerk{0.f, initial_jerk + delta_t * snap, 0.f};

    Integrator integrator{[](const StateType& state) -> StateType {
      StateType result{state.shift()};
      result.template set_element<3>({0.f, snap, 0.f});
      return result;
    }};

    StateType s1 = integrator(delta_t, s0);

    ::testing::AssertionResult result{::testing::AssertionSuccess()};
    result = AreNear(expected_position, s1.template element<0>(), sqrt(2 * delta_t))
             << " <- element<0>";
    if (!result) {
      return result;
    }
    result = AreNear(expected_velocity, s1.template element<1>(), sqrt(2 * delta_t))
             << " <- element<1>";
    if (!result) {
      return result;
    }
    result = AreNear(expected_acceleration, s1.template element<2>(), sqrt(2 * delta_t))
             << " <- element<2>";
    if (!result) {
      return result;
    }
    result = AreNear(expected_jerk, s1.template element<3>(), sqrt(2 * delta_t))
             << " <- element<3>";
    return result;
  } else {
    return ::testing::AssertionSuccess();
  }
}

template <typename Integrator>
::testing::AssertionResult CanUpdateVelocityFromPosition() {
  using std::sqrt;

  using StateType = typename Integrator::StateType;
  using ValueType = typename StateType::ValueType;
  using ScalarType = typename StateType::ScalarType;

  static constexpr ScalarType delta_t{0.1f};
  static constexpr ScalarType initial_position{3.f};

  static constexpr StateType s0{ValueType{0.f, initial_position, 0.f}};

  static constexpr ValueType expected_position{0.f, initial_position + delta_t * initial_position,
                                               0.f};

  Integrator integrator{[](const StateType& state) -> StateType {
    StateType result{};
    result.template set_element<0>(state.template element<0>());
    return result;
  }};

  StateType s1 = integrator(delta_t, s0);

  return AreNear(expected_position, s1.template element<0>(), sqrt(2 * delta_t))
         << " <- element<0>";
}

template <typename Integrator>
::testing::AssertionResult CanUpdateAccelerationFromPosition() {
  /**
   * This test is simulating an harmonic oscillator.
   *
   * acceleration = -position
   *
   * The real part of the analytic solution is
   *
   * position = A * cos(t + a)
   * velocity = -A * sin(t + a)
   * acceleration = -A * cos(t + a)
   *
   * where A = initial_position, and a = 0 since we are assuming an initial velocity of zero.
   * We set up the expected state according to this analytic solution.
   */
  using std::cos;
  using std::sin;
  using std::sqrt;

  using StateType = typename Integrator::StateType;
  using ScalarType = typename StateType::ScalarType;

  if constexpr (StateType::depth() >= 2) {
    static constexpr ScalarType delta_t{0.5f};
    static constexpr ScalarType initial_velocity{1.f};
    static constexpr ScalarType initial_position{3.f};

    static constexpr StateType s0{{0.f, initial_position, 0.f}, {0.f, initial_velocity, 0.f}};

    static constexpr StateType expected{{0.f, initial_position * cos(delta_t), 0.f},  //
                                        {0.f, -initial_position * sin(delta_t), 0.f}};

    Integrator integrator{[](const StateType& state) -> StateType {
      StateType result{state.shift()};
      result.template set_element<1>(-state.template element<0>());
      return result;
    }};

    StateType s1 = integrator(delta_t, s0);

    ::testing::AssertionResult result{::testing::AssertionSuccess()};
    result = AreNear(expected.template element<0>(), s1.template element<0>(), sqrt(2 * delta_t))
             << " <- element<0>";
    if (!result) {
      return result;
    }
    result = AreNear(expected.template element<1>(), s1.template element<1>(), sqrt(2 * delta_t))
             << " <- element<1>";
    return result;
  } else {
    return ::testing::AssertionSuccess();
  }
}

template <typename Integrator>
::testing::AssertionResult CanUpdateJerkFromPosition() {
  using std::sqrt;

  using StateType = typename Integrator::StateType;
  using ValueType = typename StateType::ValueType;
  using ScalarType = typename StateType::ScalarType;

  if constexpr (StateType::depth() >= 3) {
    static constexpr ScalarType delta_t{0.1f};
    static constexpr ScalarType initial_acceleration{5.f};
    static constexpr ScalarType initial_velocity{1.f};
    static constexpr ScalarType initial_position{3.f};

    static constexpr StateType s0{{0.f, initial_position, 0.f},
                                  {0.f, initial_velocity, 0.f},
                                  {0.f, initial_acceleration, 0.f}};

    static constexpr ValueType expected_position{0.f, initial_position + delta_t * initial_velocity,
                                                 0.f};
    static constexpr ValueType expected_velocity{
        0.f, initial_velocity + delta_t * initial_acceleration, 0.f};
    static constexpr ValueType expected_acceleration{
        0.f, initial_acceleration + delta_t * initial_position, 0.f};

    Integrator integrator{[](const StateType& state) -> StateType {
      StateType result{state.shift()};
      result.template set_element<2>(state.template element<0>());
      return result;
    }};

    StateType s1 = integrator(delta_t, s0);

    ::testing::AssertionResult result{::testing::AssertionSuccess()};
    result = AreNear(expected_position, s1.template element<0>(), sqrt(2 * delta_t))
             << " <- element<0>";
    if (!result) {
      return result;
    }
    result = AreNear(expected_velocity, s1.template element<1>(), sqrt(2 * delta_t))
             << " <- element<1>";
    if (!result) {
      return result;
    }
    result = AreNear(expected_acceleration, s1.template element<2>(), sqrt(2 * delta_t))
             << " <- element<2>";
    return result;
  } else {
    return ::testing::AssertionSuccess();
  }
}

template <typename Integrator>
::testing::AssertionResult CanUpdateSnap() {
  /**
   * This test is simulating an harmonic oscillator but with an odometer that takes off distance
   * when put in reverse. (See _Ferris Bueller's Day Off_ for details of this odometer design.)
   * The test name includes the concept of snap, since we are changing a 3rd derivative. Normally,
   * the 3rd derivative is the jerk, and its derivative is the snap. Here, we add the integration of
   * the position, the odometer, to make the differential equation and its analytic solution align
   * with common concepts. In that sense, this model is just a harmonic oscillator.
   *
   * Differential equation:
   * acceleration = -position
   *
   * Analytic solution:
   * odometer = A * sin(t + a)
   * position = A * cos(t + a) (<- Note that the position is in element 1, instead of element 0)
   * velocity = -A * sin(t + a)
   * acceleration = -A * cos(t + a)
   *
   * where A = initial_position, and a = 0 since we are assuming an initial velocity of zero.
   * We set up the expected state according to this analytic solution.
   */
  using std::cos;
  using std::sin;
  using std::sqrt;

  using StateType = typename Integrator::StateType;
  using ScalarType = typename StateType::ScalarType;

  if constexpr (StateType::depth() >= 4) {
    static constexpr ScalarType delta_t{0.1f};
    static constexpr ScalarType initial_position{3.f};

    static constexpr StateType s0{{0.f, 0.f, 0.f}, {0.f, initial_position, 0.f}};

    static constexpr StateType expected{
        {0.f, initial_position * sin(delta_t), 0.f},   //
        {0.f, initial_position * cos(delta_t), 0.f},   //
        {0.f, -initial_position * sin(delta_t), 0.f},  //
        // For one step, this value for element 3 is more inline with our expectations. The cos()
        // term won't become dominant for serveral iterations.
        {0.f, -initial_position * delta_t, 0.f}};

    Integrator integrator{[](const StateType& state) -> StateType {
      StateType result{state.shift()};
      // Acceleration is in element 3, and position is in element 1.
      result.template set_element<3>(-state.template element<1>());
      return result;
    }};

    StateType s1 = integrator(delta_t, s0);

    ::testing::AssertionResult result{::testing::AssertionSuccess()};
    result = AreNear(expected.template element<0>(), s1.template element<0>(), sqrt(2 * delta_t))
             << " <- element<0>";
    if (!result) {
      return result;
    }
    result = AreNear(expected.template element<1>(), s1.template element<1>(), sqrt(2 * delta_t))
             << " <- element<1>";
    if (!result) {
      return result;
    }
    result = AreNear(expected.template element<2>(), s1.template element<2>(), sqrt(2 * delta_t))
             << " <- element<2>";
    if (!result) {
      return result;
    }
    result = AreNear(expected.template element<3>(), s1.template element<3>(), sqrt(2 * delta_t))
             << " <- element<3>";
    return result;
  } else {
    return ::testing::AssertionSuccess();
  }
}

template <typename Integrator>
::testing::AssertionResult CanIntegrateOverConstantAccelerationManySteps() {
  /**
   * This test models an object falling in gravity, but with all signs positive. The analytic
   * solution is:
   *
   * x(t) = 0.5*a*t^2 + v0*t + x0
   * v(t) = a*t + v0
   * a(t) = a0
   *
   * where a0 = acceleration, v0 = initial_velocity, and x0 = initial_position.
   *
   * We test that the given integration method approximately arrives at this solution.
   */
  using std::sqrt;

  using StateType = typename Integrator::StateType;
  using ScalarType = typename StateType::ScalarType;
  using ValueType = typename StateType::ValueType;

  if constexpr (StateType::depth() >= 2) {
    static constexpr ScalarType delta_t{0.01f};
    static constexpr ScalarType acceleration{2.f};
    static constexpr ValueType initial_velocity{1.f};
    static constexpr ValueType initial_position{3.f};

    static constexpr StateType s0{initial_position, initial_velocity};

    Integrator integrator{[](const StateType& state) -> StateType {
      StateType result{state.shift()};
      result.template set_element<1>(ValueType{acceleration});
      return result;
    }};

    static constexpr size_t NUM_STEPS{10};
    ::testing::AssertionResult result{::testing::AssertionSuccess()};
    StateType current_state{s0};
    for (size_t i = 0; i < NUM_STEPS; ++i) {
      current_state = integrator(delta_t, current_state);
      ValueType expected_position{initial_position + (i + 1) * delta_t * initial_velocity +
                                  (i + 1) * delta_t * delta_t / 2 * acceleration};
      result =
          AreNear(expected_position, current_state.template element<0>(), acceleration * delta_t)
          << " <- element<0>; i: " << i;
      if (!result) {
        return result;
      }
      ValueType expected_velocity{initial_velocity + (i + 1) * delta_t * acceleration};
      result = AreNear(expected_velocity, current_state.template element<1>(),
                       acceleration * sqrt(delta_t))
               << " <- element<1>; i: " << i;
      if (!result) {
        return result;
      }
    }
    return result;
  } else {
    return ::testing::AssertionSuccess();
  }
}

}  // namespace ndyn::math
