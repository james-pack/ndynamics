#include "simulation/body.h"

#include <cmath>
#include <type_traits>

#include "base/pi.h"
#include "gtest/gtest.h"
#include "math/algebra.h"
#include "math/integrators.h"
#include "math/multivector.h"
#include "math/multivector_test_utils.h"
#include "math/state.h"
#include "simulation/pendulum.h"
#include "simulation/potential.h"

namespace ndyn::simulation {

using FloatT = float;
using AlgebraType = math::Algebra<FloatT, 2, 0, 0>;
using VectorType = typename AlgebraType::VectorType;

static constexpr size_t STATE_DEPTH{2};
using StateType = math::State<VectorType, STATE_DEPTH>;

static constexpr FloatT ACCELERATION_GRAVITY{-9.8};
static constexpr FloatT ALLOWED_ERROR_MULTIPLE{0.05};
static constexpr FloatT ALLOWED_ERROR_ABSOLUTE{0.01};

FloatT compute_step_size(FloatT angle) {
  if (angle < pi / 16) {
    return 0.00001 / angle;
  } else if (angle < pi / 8) {
    return 0.0002 / angle;
  } else if (angle < pi / 2) {
    return 0.00015 / angle;
  } else {
    return 0.0001;
  }
}

// 9.8 in the direction of negative x.
static Potential<VectorType> gravity{ACCELERATION_GRAVITY * VectorType::e<0>()};

math::ComputePartials<StateType> configure_pendulum_partials() {
  return [](const StateType& state) -> StateType {
    using std::sin;
    const VectorType g{gravity.at(state.element<0>())};
    const FloatT length{state.element<0>().r()};
    const FloatT angle{state.element<0>().theta() - g.theta()};
    const FloatT magnitude{g.r() / length * sin(angle)};
    StateType partials{state.shift()};
    partials.set_element<1>(magnitude * VectorType::e<1>());
    return partials;
  };
}

class BodyTest : public ::testing::TestWithParam<FloatT> {};

TEST_P(BodyTest, CanCompile) {
  FloatT INITIAL_ANGLE{GetParam()};
  FloatT LENGTH{1};
  VectorType initial_position{LENGTH * VectorType::e<0>() + INITIAL_ANGLE * VectorType::e<1>()};
  const auto partials{configure_pendulum_partials()};
  Body<StateType> body{StateType{initial_position}, partials};

  EXPECT_EQ(initial_position, body.state().template element<0>());
}

TEST_P(BodyTest, CanEvolveFullPeriodTrivial) {
  FloatT LENGTH{1};
  FloatT INITIAL_ANGLE{GetParam()};
  FloatT PERIOD{compute_period(LENGTH, ACCELERATION_GRAVITY, INITIAL_ANGLE)};

  VectorType initial_position{LENGTH * VectorType::e<0>() + INITIAL_ANGLE * VectorType::e<1>()};
  const auto partials{configure_pendulum_partials()};
  Body<StateType> body{StateType{initial_position}, partials};

  body.evolve(PERIOD, compute_step_size(INITIAL_ANGLE));

  EXPECT_TRUE(math::AreNear(initial_position, body.state().template element<0>(),
                            ALLOWED_ERROR_MULTIPLE * INITIAL_ANGLE + ALLOWED_ERROR_ABSOLUTE))
      << "INITIAL_ANGLE: " << INITIAL_ANGLE;
}

TEST_P(BodyTest, CanEvolveHalfPeriod) {
  FloatT LENGTH{1};
  FloatT INITIAL_ANGLE{GetParam()};
  FloatT PERIOD{compute_period(LENGTH, ACCELERATION_GRAVITY, INITIAL_ANGLE)};

  VectorType initial_position{LENGTH * VectorType::e<0>() + INITIAL_ANGLE * VectorType::e<1>()};
  const auto partials{configure_pendulum_partials()};
  Body<StateType> body{StateType{initial_position}, partials};

  body.evolve(PERIOD / 2, compute_step_size(INITIAL_ANGLE));

  VectorType expected_position{LENGTH * VectorType::e<0>() - INITIAL_ANGLE * VectorType::e<1>()};
  EXPECT_TRUE(math::AreNear(expected_position, body.state().template element<0>(),
                            ALLOWED_ERROR_MULTIPLE * INITIAL_ANGLE + ALLOWED_ERROR_ABSOLUTE))
      << "INITIAL_ANGLE: " << INITIAL_ANGLE;
}

TEST_P(BodyTest, CanEvolveQuarterPeriod) {
  FloatT LENGTH{1};
  FloatT INITIAL_ANGLE{GetParam()};
  FloatT PERIOD{compute_period(LENGTH, ACCELERATION_GRAVITY, INITIAL_ANGLE)};

  VectorType initial_position{LENGTH * VectorType::e<0>() + INITIAL_ANGLE * VectorType::e<1>()};
  const auto partials{configure_pendulum_partials()};
  Body<StateType> body{StateType{initial_position}, partials};

  body.evolve(PERIOD / 4, compute_step_size(INITIAL_ANGLE));

  VectorType expected_position{LENGTH * VectorType::e<0>()};
  EXPECT_TRUE(math::AreNear(expected_position, body.state().template element<0>(),
                            ALLOWED_ERROR_MULTIPLE * INITIAL_ANGLE + ALLOWED_ERROR_ABSOLUTE))
      << "INITIAL_ANGLE: " << INITIAL_ANGLE;
}

TEST_P(BodyTest, CanEvolveFullPeriod) {
  FloatT LENGTH{1};
  FloatT INITIAL_ANGLE{GetParam()};
  FloatT PERIOD{compute_period(LENGTH, ACCELERATION_GRAVITY, INITIAL_ANGLE)};

  VectorType initial_position{LENGTH * VectorType::e<0>() + INITIAL_ANGLE * VectorType::e<1>()};
  const auto partials{configure_pendulum_partials()};
  Body<StateType> body{StateType{initial_position}, partials};

  VectorType quarter_position{LENGTH * VectorType::e<0>()};
  VectorType half_position{LENGTH * VectorType::e<0>() - INITIAL_ANGLE * VectorType::e<1>()};

  body.evolve(PERIOD / 4, compute_step_size(INITIAL_ANGLE));
  EXPECT_TRUE(math::AreNear(quarter_position, body.state().template element<0>(),
                            ALLOWED_ERROR_MULTIPLE * INITIAL_ANGLE + ALLOWED_ERROR_ABSOLUTE))
      << "INITIAL_ANGLE: " << INITIAL_ANGLE;

  body.evolve(PERIOD / 4, compute_step_size(INITIAL_ANGLE));
  EXPECT_TRUE(math::AreNear(half_position, body.state().template element<0>(),
                            ALLOWED_ERROR_MULTIPLE * INITIAL_ANGLE + ALLOWED_ERROR_ABSOLUTE))
      << "INITIAL_ANGLE: " << INITIAL_ANGLE;

  body.evolve(PERIOD / 4, compute_step_size(INITIAL_ANGLE));
  EXPECT_TRUE(math::AreNear(quarter_position, body.state().template element<0>(),
                            ALLOWED_ERROR_MULTIPLE * INITIAL_ANGLE + ALLOWED_ERROR_ABSOLUTE))
      << "INITIAL_ANGLE: " << INITIAL_ANGLE;

  body.evolve(PERIOD / 4, compute_step_size(INITIAL_ANGLE));
  EXPECT_TRUE(math::AreNear(initial_position, body.state().template element<0>(),
                            ALLOWED_ERROR_MULTIPLE * INITIAL_ANGLE + ALLOWED_ERROR_ABSOLUTE))
      << "INITIAL_ANGLE: " << INITIAL_ANGLE;
}

TEST_P(BodyTest, CanEvolveMultiplePeriods) {
  size_t NUM_PERIODS{5};

  FloatT LENGTH{1};
  FloatT INITIAL_ANGLE{GetParam()};
  FloatT PERIOD{compute_period(LENGTH, ACCELERATION_GRAVITY, INITIAL_ANGLE)};

  VectorType initial_position{LENGTH * VectorType::e<0>() + INITIAL_ANGLE * VectorType::e<1>()};
  const auto partials{configure_pendulum_partials()};
  Body<StateType> body{StateType{initial_position}, partials};

  VectorType half_position{LENGTH * VectorType::e<0>() - INITIAL_ANGLE * VectorType::e<1>()};

  for (size_t i = 0; i < NUM_PERIODS; ++i) {
    body.evolve(PERIOD / 2, compute_step_size(INITIAL_ANGLE));
    EXPECT_TRUE(math::AreNear(half_position, body.state().template element<0>(),
                              ALLOWED_ERROR_MULTIPLE * INITIAL_ANGLE + ALLOWED_ERROR_ABSOLUTE))
        << "INITIAL_ANGLE: " << INITIAL_ANGLE << ", i: " << i;

    body.evolve(PERIOD / 2, compute_step_size(INITIAL_ANGLE));
    EXPECT_TRUE(math::AreNear(initial_position, body.state().template element<0>(),
                              ALLOWED_ERROR_MULTIPLE * INITIAL_ANGLE + ALLOWED_ERROR_ABSOLUTE))
        << "INITIAL_ANGLE: " << INITIAL_ANGLE << ", i: " << i;
  }
}

TEST_P(BodyTest, CanEvolveManyPeriods) {
  size_t NUM_PERIODS{10};

  FloatT LENGTH{1};
  FloatT INITIAL_ANGLE{GetParam()};
  FloatT PERIOD{compute_period(LENGTH, ACCELERATION_GRAVITY, INITIAL_ANGLE)};

  VectorType initial_position{LENGTH * VectorType::e<0>() + INITIAL_ANGLE * VectorType::e<1>()};
  const auto partials{configure_pendulum_partials()};
  Body<StateType> body{StateType{initial_position}, partials};

  VectorType half_position{LENGTH * VectorType::e<0>() - INITIAL_ANGLE * VectorType::e<1>()};

  for (size_t i = 0; i < NUM_PERIODS; ++i) {
    body.evolve(PERIOD / 2, compute_step_size(INITIAL_ANGLE));
    EXPECT_TRUE(math::AreNear(half_position, body.state().template element<0>(),
                              ALLOWED_ERROR_MULTIPLE * INITIAL_ANGLE + ALLOWED_ERROR_ABSOLUTE))
        << "INITIAL_ANGLE: " << INITIAL_ANGLE << ", i: " << i;

    body.evolve(PERIOD / 2, compute_step_size(INITIAL_ANGLE));
    EXPECT_TRUE(math::AreNear(initial_position, body.state().template element<0>(),
                              ALLOWED_ERROR_MULTIPLE * INITIAL_ANGLE + ALLOWED_ERROR_ABSOLUTE))
        << "INITIAL_ANGLE: " << INITIAL_ANGLE << ", i: " << i;
  }
}

TEST_P(BodyTest, CanEvolveManyMorePeriods) {
  size_t NUM_PERIODS{25};

  FloatT LENGTH{1};
  FloatT INITIAL_ANGLE{GetParam()};
  FloatT PERIOD{compute_period(LENGTH, ACCELERATION_GRAVITY, INITIAL_ANGLE)};

  VectorType initial_position{LENGTH * VectorType::e<0>() + INITIAL_ANGLE * VectorType::e<1>()};
  const auto partials{configure_pendulum_partials()};
  Body<StateType> body{StateType{initial_position}, partials};

  VectorType half_position{LENGTH * VectorType::e<0>() - INITIAL_ANGLE * VectorType::e<1>()};

  for (size_t i = 0; i < NUM_PERIODS; ++i) {
    body.evolve(PERIOD / 2, compute_step_size(INITIAL_ANGLE));
    EXPECT_TRUE(math::AreNear(half_position, body.state().template element<0>(),
                              ALLOWED_ERROR_MULTIPLE * INITIAL_ANGLE + ALLOWED_ERROR_ABSOLUTE))
        << "INITIAL_ANGLE: " << INITIAL_ANGLE << ", i: " << i;

    body.evolve(PERIOD / 2, compute_step_size(INITIAL_ANGLE));
    EXPECT_TRUE(math::AreNear(initial_position, body.state().template element<0>(),
                              ALLOWED_ERROR_MULTIPLE * INITIAL_ANGLE + ALLOWED_ERROR_ABSOLUTE))
        << "INITIAL_ANGLE: " << INITIAL_ANGLE << ", i: " << i;
  }
}

static constexpr FloatT TINY_ANGLE{ndyn::pi / 256};
static constexpr FloatT SMALL_ANGLE{ndyn::pi / 16};
static constexpr FloatT MODERATE_ANGLE{ndyn::pi / 4};
static constexpr FloatT LARGE_ANGLE{ndyn::pi / 2};

INSTANTIATE_TEST_SUITE_P(PendulumSimulationTest, BodyTest,
                         testing::Values(TINY_ANGLE, SMALL_ANGLE, MODERATE_ANGLE, LARGE_ANGLE));

}  // namespace ndyn::simulation
