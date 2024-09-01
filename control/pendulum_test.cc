#include "control/pendulum.h"

#include "base/pi.h"
#include "glog/logging.h"
#include "gtest/gtest.h"
#include "math/multivector.h"
#include "math/multivector_test_utils.h"

namespace ndyn::control {

TEST(ClassicPendulumTest, StateAlwaysZeroIfNoInitialEnergy) {
  ClassicPendulumConfigurator config{};
  config.set_theta(0);
  auto p{config.create()};
  ASSERT_EQ(0, p.theta());
  p.evolve(1);
  EXPECT_EQ(0, p.theta());
  p.evolve(1);
  EXPECT_EQ(0, p.theta());
}

TEST(ClassicPendulumTest, ApproximatesCanonicalSmallAngleSolution) {
  ClassicPendulumConfigurator config{};
  static constexpr float ANGLE{0.01};
  config.set_theta(ANGLE).set_g(-1);
  auto p{config.create()};
  ASSERT_EQ(ANGLE, p.theta());

  // After one period, the angle should be within 10% of the initial angle.
  p.evolve(2 * pi);
  EXPECT_NEAR(ANGLE, p.theta(), ANGLE / 10);
}

TEST(ClassicPendulumTest, SameAngleAfterHalfPeriod) {
  ClassicPendulumConfigurator config{};
  static constexpr float ANGLE{0.01};
  config.set_theta(ANGLE).set_g(-1);
  auto p{config.create()};

  // After 1/2 period, the angle should be within 10% of the initial angle, but with the opposite
  // sign.
  p.evolve(pi);
  EXPECT_NEAR(-ANGLE, p.theta(), ANGLE / 10);
}

TEST(ClassicPendulumTest, HighDampeningCausesAngleToReduceEachPeriod) {
  ClassicPendulumConfigurator config{};
  static constexpr float ANGLE{1};
  config.set_theta(ANGLE).set_g(-1).set_mu(0.75);
  auto p{config.create()};

  static constexpr float STEP{0.001};
  float previous_half_period_max{ANGLE};
  for (int half_period = 0; half_period < 50 && previous_half_period_max > 0.01 * ANGLE;
       ++half_period) {
    float previous_angular_velocity{0};
    do {
      // When the angular velocity changes sign, we have traversed 1/2 a period.
      previous_angular_velocity = p.theta_dot();
      p.evolve(STEP, STEP);
    } while (previous_angular_velocity == 0 || p.theta_dot() / previous_angular_velocity > 0);

    EXPECT_LT(p.theta(), previous_half_period_max) << "half_period: " << half_period;
    previous_half_period_max = p.theta();
  }
}

TEST(ClassicPendulumTest, DampeningCausesAngleToReduceEachPeriod) {
  ClassicPendulumConfigurator config{};
  static constexpr float ANGLE{1};
  config.set_theta(ANGLE).set_g(-1).set_mu(0.01);
  auto p{config.create()};

  static constexpr float STEP{0.001};
  float previous_half_period_max{ANGLE};
  for (int half_period = 0; half_period < 50 && previous_half_period_max > 0.01 * ANGLE;
       ++half_period) {
    float previous_angular_velocity{0};
    do {
      // When the angular velocity changes sign, we have traversed 1/2 a period.
      previous_angular_velocity = p.theta_dot();
      p.evolve(STEP, STEP);
    } while (previous_angular_velocity == 0 || p.theta_dot() / previous_angular_velocity > 0);

    EXPECT_LT(p.theta(), previous_half_period_max) << "half_period: " << half_period;
    previous_half_period_max = p.theta();
  }
}

TEST(GA2DPendulumTest, LengthSameAfterCreation) {
  using T = math::Multivector<float, 2, 0, 0, math::InnerProduct::LEFT_CONTRACTION>;
  GAPendulumConfigurator<T> config{};
  for (const auto length : {1.f, 2.f, 0.5f}) {
    config.set_length(length);
    auto p{config.create()};
    EXPECT_NEAR(length, p.length(), 0.0001);
  }
}

TEST(GA2DPendulumTest, CorrectPositionAfterCreationThetaZero) {
  using T = math::Multivector<float, 2, 0, 0, math::InnerProduct::LEFT_CONTRACTION>;
  GAPendulumConfigurator<T> config{};
  config.set_theta(0.);
  auto p{config.create()};
  EXPECT_TRUE(math::AreNear(-T::template e<1>(), p.position(), 0.0001));
}

TEST(GA2DPendulumTest, CorrectPositionAfterCreationThetaPiOverTwo) {
  using T = math::Multivector<float, 2, 0, 0, math::InnerProduct::LEFT_CONTRACTION>;
  GAPendulumConfigurator<T> config{};
  config.set_theta(pi / 2.);
  auto p{config.create()};
  EXPECT_TRUE(math::AreNear(T::template e<0>(), p.position(), 0.0001));
}

TEST(GA2DPendulumTest, CorrectPositionAfterCreationThetaNegativePiOverTwo) {
  using T = math::Multivector<float, 2, 0, 0, math::InnerProduct::LEFT_CONTRACTION>;
  GAPendulumConfigurator<T> config{};
  config.set_theta(-pi / 2.);
  auto p{config.create()};
  EXPECT_TRUE(math::AreNear(-T::template e<0>(), p.position(), 0.0001));
}

TEST(GA2DPendulumTest, CorrectPositionAfterCreationThetaPi) {
  using T = math::Multivector<float, 2, 0, 0, math::InnerProduct::LEFT_CONTRACTION>;
  GAPendulumConfigurator<T> config{};
  config.set_theta(pi);
  auto p{config.create()};
  EXPECT_TRUE(math::AreNear(T::template e<1>(), p.position(), 0.0001));
}

TEST(GA2DPendulumTest, CorrectPositionAfterCreationThetaNegativePi) {
  using T = math::Multivector<float, 2, 0, 0, math::InnerProduct::LEFT_CONTRACTION>;
  GAPendulumConfigurator<T> config{};
  config.set_theta(-pi);
  auto p{config.create()};
  EXPECT_TRUE(math::AreNear(T::template e<1>(), p.position(), 0.0001));
}

TEST(GA2DPendulumTest, ThetaSameAfterCreation) {
  using T = math::Multivector<float, 2, 0, 0, math::InnerProduct::LEFT_CONTRACTION>;
  GAPendulumConfigurator<T> config{};
  for (const auto angle :
       {0., pi / 2., pi - 0.01, -pi / 2., -(pi - 0.01), 3. * pi / 4., -3. * pi / 4.}) {
    config.set_theta(angle);
    auto p{config.create()};
    EXPECT_NEAR(angle, p.theta(), 0.0001);
  }
}

TEST(GAPendulumTest, LengthSameAfterCreation) {
  using T = math::Multivector<float, 3, 0, 0, math::InnerProduct::LEFT_CONTRACTION>;
  GAPendulumConfigurator<T> config{};
  for (const auto length : {1.f, 2.f, 0.5f}) {
    config.set_length(length);
    auto p{config.create()};
    EXPECT_NEAR(length, p.length(), 0.0001);
  }
}

TEST(GAPendulumTest, CorrectPositionAfterCreationThetaZero) {
  using T = math::Multivector<float, 3, 0, 0, math::InnerProduct::LEFT_CONTRACTION>;
  GAPendulumConfigurator<T> config{};
  config.set_theta(0.);
  auto p{config.create()};
  EXPECT_TRUE(math::AreNear(-T::template e<1>(), p.position(), 0.0001));
}

TEST(GAPendulumTest, CorrectPositionAfterCreationThetaPiOverTwo) {
  using T = math::Multivector<float, 3, 0, 0, math::InnerProduct::LEFT_CONTRACTION>;
  GAPendulumConfigurator<T> config{};
  config.set_theta(pi / 2.);
  auto p{config.create()};
  EXPECT_TRUE(math::AreNear(T::template e<0>(), p.position(), 0.0001));
}

TEST(GAPendulumTest, CorrectPositionAfterCreationThetaNegativePiOverTwo) {
  using T = math::Multivector<float, 3, 0, 0, math::InnerProduct::LEFT_CONTRACTION>;
  GAPendulumConfigurator<T> config{};
  config.set_theta(-pi / 2.);
  auto p{config.create()};
  EXPECT_TRUE(math::AreNear(-T::template e<0>(), p.position(), 0.0001));
}

TEST(GAPendulumTest, CorrectPositionAfterCreationThetaPi) {
  using T = math::Multivector<float, 3, 0, 0, math::InnerProduct::LEFT_CONTRACTION>;
  GAPendulumConfigurator<T> config{};
  config.set_theta(pi);
  auto p{config.create()};
  EXPECT_TRUE(math::AreNear(T::template e<1>(), p.position(), 0.0001));
}

TEST(GAPendulumTest, CorrectPositionAfterCreationThetaNegativePi) {
  using T = math::Multivector<float, 3, 0, 0, math::InnerProduct::LEFT_CONTRACTION>;
  GAPendulumConfigurator<T> config{};
  config.set_theta(-pi);
  auto p{config.create()};
  EXPECT_TRUE(math::AreNear(T::template e<1>(), p.position(), 0.0001));
}

TEST(GAPendulumTest, ThetaSameAfterCreation) {
  using T = math::Multivector<float, 3, 0, 0, math::InnerProduct::LEFT_CONTRACTION>;
  GAPendulumConfigurator<T> config{};
  for (const auto angle :
       {0., pi / 2., pi - 0.01, -pi / 2., -(pi - 0.01), 3. * pi / 4., -3. * pi / 4.}) {
    config.set_theta(angle);
    auto p{config.create()};
    EXPECT_NEAR(angle, p.theta(), 0.0001);
  }
}

TEST(GA2DPendulumTest, StateAlwaysZeroIfNoInitialEnergy) {
  using T = math::Multivector<float, 2, 0, 0, math::InnerProduct::LEFT_CONTRACTION>;
  GAPendulumConfigurator<T> config{};
  config.set_theta(0);
  auto p{config.create()};
  ASSERT_EQ(0, p.theta());
  p.evolve(1);
  EXPECT_EQ(0, p.theta());
  p.evolve(1);
  EXPECT_EQ(0, p.theta());
}

TEST(GA2DPendulumTest, ApproximatesCanonicalSmallAngleSolution) {
  using T = math::Multivector<float, 2, 0, 0, math::InnerProduct::LEFT_CONTRACTION>;
  GAPendulumConfigurator<T> config{};
  static constexpr float ANGLE{0.01};
  config.set_theta(ANGLE).set_g(1);
  auto p{config.create()};

  // Assuming a small angle approximation, after 1/4 period, the angle should be zero.
  p.evolve(pi / 2);
  EXPECT_NEAR(0, p.theta(), ANGLE / 100);

  // Assuming a small angle approximation, after 1/2 period, the angle should be the initial angle,
  // but with the opposite sign.
  p.evolve(pi / 2);
  EXPECT_NEAR(-ANGLE, p.theta(), ANGLE / 100);

  // Assuming a small angle approximation, after 3/4 period, the angle should be zero.
  p.evolve(pi / 2);
  EXPECT_NEAR(0, p.theta(), ANGLE / 100);

  // Assuming a small angle approximation, after one period, the angle should be the initial angle.
  p.evolve(pi / 2);
  EXPECT_NEAR(ANGLE, p.theta(), ANGLE / 100);
}

TEST(GA2DPendulumTest, DISABLED_ApproximatesSmallAngleSolutionThroughMultiplePeriods) {
  using std::pow;
  using ScalarType = double;
  using T = math::Multivector<ScalarType, 2, 0, 0, math::InnerProduct::LEFT_CONTRACTION>;

  GAPendulumConfigurator<T> config{};

  static constexpr ScalarType ANGLE{0.01};
  config.set_theta(ANGLE).set_g(1);

  auto p{config.create()};

  static constexpr auto QUARTER_PERIOD{pi / 2};
  static constexpr size_t NUM_PERIODS{8};
  static constexpr auto EPSILON{ANGLE * ANGLE};

  static constexpr auto STEP_SIZE{0.0001};
  for (size_t i = 0; i < NUM_PERIODS && !HasFailure(); ++i) {
    LOG(INFO) << "p.current_time(): " << p.current_time() << ", p.theta(): " << p.theta();
    p.evolve(QUARTER_PERIOD, STEP_SIZE);
    EXPECT_NEAR(0, p.theta(), EPSILON);
    p.evolve(QUARTER_PERIOD, STEP_SIZE);
    EXPECT_NEAR(-ANGLE, p.theta(), EPSILON);
    p.evolve(QUARTER_PERIOD, STEP_SIZE);
    EXPECT_NEAR(0, p.theta(), EPSILON);
    p.evolve(QUARTER_PERIOD, STEP_SIZE);
    EXPECT_NEAR(ANGLE, p.theta(), EPSILON);
  }
}

TEST(GA2DPendulumTest,
     DISABLED_ApproximatesSmallAngleSolutionThroughMultiplePeriodsWithCircularErrorAdjustment) {
  using std::pow;
  using ScalarType = double;
  using T = math::Multivector<ScalarType, 2, 0, 0, math::InnerProduct::LEFT_CONTRACTION>;

  GAPendulumConfigurator<T> config{};

  static constexpr ScalarType ANGLE{0.01};
  config.set_theta(ANGLE).set_g(1);

  auto p{config.create()};

  // For more details on circular error, see
  // https://en.wikipedia.org/wiki/Pendulum#Period_of_oscillation
  static constexpr auto QUARTER_PERIOD{pi / 2 *
                                       (1 + pow(ANGLE, 2) / 16 + 11 * pow(ANGLE, 4) / 3072)};
  static constexpr size_t NUM_PERIODS{100};
  static constexpr auto EPSILON{ANGLE * ANGLE};

  static constexpr auto STEP_SIZE{0.0001};
  for (size_t i = 0; i < NUM_PERIODS && !HasFailure(); ++i) {
    LOG(INFO) << "p.current_time(): " << p.current_time() << ", p.theta(): " << p.theta();
    p.evolve(QUARTER_PERIOD, STEP_SIZE);
    EXPECT_NEAR(0, p.theta(), EPSILON);
    p.evolve(QUARTER_PERIOD, STEP_SIZE);
    EXPECT_NEAR(-ANGLE, p.theta(), EPSILON);
    p.evolve(QUARTER_PERIOD, STEP_SIZE);
    EXPECT_NEAR(0, p.theta(), EPSILON);
    p.evolve(QUARTER_PERIOD, STEP_SIZE);
    EXPECT_NEAR(ANGLE, p.theta(), EPSILON);
  }
}

}  // namespace ndyn::control
