#include "control/pendulum.h"

#include "glog/logging.h"
#include "gtest/gtest.h"

namespace ndyn::control {

static constexpr double PI{3.14159265359};

TEST(PendulumTest, StateAlwaysZeroIfNoInitialEnergy) {
  PendulumConfigurator config{};
  config.set_theta(0).set_theta_dot(0);
  Pendulum p{config.create()};
  ASSERT_EQ(0, p.theta());
  p.evolve(1);
  EXPECT_EQ(0, p.theta());
  p.evolve(1);
  EXPECT_EQ(0, p.theta());
}

TEST(PendulumTest, ApproximatesCanonicalSmallAngleSolution) {
  PendulumConfigurator config{};
  static constexpr float ANGLE{0.01};
  config.set_theta(ANGLE).set_theta_dot(0).set_g(-1);
  Pendulum p{config.create()};
  ASSERT_EQ(ANGLE, p.theta());

  // After one period, the angle should be within 10% of the initial angle.
  p.evolve(2 * PI);
  EXPECT_NEAR(ANGLE, p.theta(), ANGLE / 10);
}

TEST(PendulumTest, SameAngleAfterHalfPeriod) {
  PendulumConfigurator config{};
  static constexpr float ANGLE{0.01};
  config.set_theta(ANGLE).set_theta_dot(0).set_g(-1);
  Pendulum p{config.create()};

  // After 1/2 period, the angle should be within 10% of the initial angle, but with the opposite
  // sign.
  p.evolve(PI);
  EXPECT_NEAR(-ANGLE, p.theta(), ANGLE / 10);
}

TEST(PendulumTest, HighDampeningCausesAngleToReduceEachPeriod) {
  PendulumConfigurator config{};
  static constexpr float ANGLE{1};
  config.set_theta(ANGLE).set_theta_dot(0).set_g(-1).set_mu(0.75);
  Pendulum p{config.create()};

  static constexpr float STEP{0.001};
  float previous_half_period_max{ANGLE};
  for (int half_period = 0; half_period < 50 && previous_half_period_max > 0.01 * ANGLE;
       ++half_period) {
    float previous_angular_velocity{0};
    do {
      LOG(INFO) << "p.theta(): " << p.theta() << ", p.theta_dot(): " << p.theta_dot();
      LOG(INFO) << "previous_angular_velocity: " << previous_angular_velocity;

      // When the angular velocity changes sign, we have traversed 1/2 a period.
      previous_angular_velocity = p.theta_dot();
      p.evolve(STEP, STEP);
    } while (previous_angular_velocity == 0 || p.theta_dot() / previous_angular_velocity > 0);

    LOG(INFO) << "previous_half_period_max: " << previous_half_period_max
              << ", p.theta(): " << p.theta() << ", p.theta_dot(): " << p.theta_dot();
    EXPECT_LT(p.theta(), previous_half_period_max) << "half_period: " << half_period;
    previous_half_period_max = p.theta();
  }
}

TEST(PendulumTest, DampeningCausesAngleToReduceEachPeriod) {
  PendulumConfigurator config{};
  static constexpr float ANGLE{1};
  config.set_theta(ANGLE).set_theta_dot(0).set_g(-1).set_mu(0.01);
  Pendulum p{config.create()};

  static constexpr float STEP{0.001};
  float previous_half_period_max{ANGLE};
  for (int half_period = 0; half_period < 50 && previous_half_period_max > 0.01 * ANGLE;
       ++half_period) {
    float previous_angular_velocity{0};
    do {
      LOG(INFO) << "p.theta(): " << p.theta() << ", p.theta_dot(): " << p.theta_dot();
      LOG(INFO) << "previous_angular_velocity: " << previous_angular_velocity;

      // When the angular velocity changes sign, we have traversed 1/2 a period.
      previous_angular_velocity = p.theta_dot();
      p.evolve(STEP, STEP);
    } while (previous_angular_velocity == 0 || p.theta_dot() / previous_angular_velocity > 0);

    LOG(INFO) << "previous_half_period_max: " << previous_half_period_max
              << ", p.theta(): " << p.theta() << ", p.theta_dot(): " << p.theta_dot();
    EXPECT_LT(p.theta(), previous_half_period_max) << "half_period: " << half_period;
    previous_half_period_max = p.theta();
  }
}

}  // namespace ndyn::control
