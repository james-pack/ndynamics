#include "simulation/pendulum.h"

#include "base/pi.h"
#include "glog/logging.h"
#include "gtest/gtest.h"
#include "math/multivector.h"
#include "math/multivector_test_utils.h"

namespace ndyn::simulation {

using FloatT = float;

static constexpr size_t ONE_PERIOD{1};
static constexpr size_t MULTIPLE_PERIODS{5};
static constexpr size_t MANY_PERIODS{10};
static constexpr size_t MANY_MORE_PERIODS{25};

static constexpr FloatT SMALL_ANGLE{0.01};
static constexpr FloatT MODERATE_ANGLE{pi / 16};
static constexpr FloatT LARGE_ANGLE{pi / 4};

template <typename T>
::testing::AssertionResult is_near(T lhs, T rhs, T epsilon) {
  using std::abs;
  if (abs(lhs - rhs) < abs(epsilon)) {
    return ::testing::AssertionSuccess();
  } else {
    return ::testing::AssertionFailure() << "lhs: " << lhs                           //
                                         << ", rhs: " << rhs                         //
                                         << ", abs(difference): " << abs(lhs - rhs)  //
                                         << ", abs(epsilon): " << abs(epsilon);
  }
}

template <typename PendulumT, typename ScalarType>
::testing::AssertionResult IsAccurate(PendulumT pendulum, size_t num_periods, ScalarType angle) {
  ::testing::AssertionResult result{::testing::AssertionSuccess()};

  // Calculate the expected period including a correction term for the circular error.
  const ScalarType quarter_period = compute_period(pendulum.length(), pendulum.g(), angle) / 4;

  // Compare the expected and computed values to be within a percentage of the given angle.
  const ScalarType EPSILON{static_cast<ScalarType>(0.05) * angle};

  static constexpr ScalarType ZERO_ANGLE{0};

  // Use a smaller step size as the number of periods is greater. This helps offset the accumulated
  // error.
  ScalarType STEP_SIZE{static_cast<ScalarType>(0.001)};
  for (size_t i = 0; i < num_periods; ++i) {
    pendulum.goto_time(4 * quarter_period * i, STEP_SIZE);

    pendulum.evolve(quarter_period, STEP_SIZE);
    LOG(INFO) << "pendulum.current_time(): " << pendulum.current_time()
              << ", calculated time: " << 4 * quarter_period * i + quarter_period
              << ", theta(): " << pendulum.theta() << " (expected " << ZERO_ANGLE << ")";
    result = is_near(ZERO_ANGLE, pendulum.theta(), EPSILON);
    if (!result) {
      return result << ", pendulum.current_time(): " << pendulum.current_time();
    }

    pendulum.evolve(quarter_period, STEP_SIZE);
    LOG(INFO) << "pendulum.current_time(): " << pendulum.current_time()
              << ", calculated time: " << 4 * quarter_period * i + 2 * quarter_period
              << ", theta(): " << pendulum.theta() << " (expected " << -angle << ")";
    result = is_near(-angle, pendulum.theta(), EPSILON);
    if (!result) {
      return result << ", pendulum.current_time(): " << pendulum.current_time();
    }

    pendulum.evolve(quarter_period, STEP_SIZE);
    LOG(INFO) << "pendulum.current_time(): " << pendulum.current_time()
              << ", calculated time: " << 4 * quarter_period * i + 3 * quarter_period
              << ", theta(): " << pendulum.theta() << " (expected " << ZERO_ANGLE << ")";
    result = is_near(ZERO_ANGLE, pendulum.theta(), EPSILON);
    if (!result) {
      return result << ", pendulum.current_time(): " << pendulum.current_time();
    }

    pendulum.evolve(quarter_period, STEP_SIZE);
    LOG(INFO) << "pendulum.current_time(): " << pendulum.current_time()
              << ", calculated time: " << 4 * quarter_period * i + 4 * quarter_period
              << ", theta(): " << pendulum.theta() << " (expected " << angle << ")";
    result = is_near(angle, pendulum.theta(), EPSILON);
    if (!result) {
      return result << ", pendulum.current_time(): " << pendulum.current_time();
    }
  }
  return result;
}

TEST(PendulumTest, LengthSameAfterCreation) {
  using T = math::Multivector<FloatT, 3, 0, 0, math::InnerProduct::LEFT_CONTRACTION>;
  PendulumConfigurator<T> config{};
  for (const auto length : {1., 2., 0.5}) {
    config.set_length(length);
    auto p{config.create()};
    EXPECT_NEAR(length, p.length(), 0.0001);
  }
}

TEST(PendulumTest, CorrectPositionAfterCreationThetaZero) {
  using T = math::Multivector<FloatT, 3, 0, 0, math::InnerProduct::LEFT_CONTRACTION>;
  PendulumConfigurator<T> config{};
  config.set_theta(0);
  auto p{config.create()};
  EXPECT_TRUE(math::AreNear(-T::template e<1>(), p.position(), 0.0001));
}

TEST(PendulumTest, CorrectPositionAfterCreationThetaPiOverTwo) {
  using T = math::Multivector<FloatT, 3, 0, 0, math::InnerProduct::LEFT_CONTRACTION>;
  PendulumConfigurator<T> config{};
  config.set_theta(pi / 2);
  auto p{config.create()};
  EXPECT_TRUE(math::AreNear(T::template e<0>(), p.position(), 0.0001));
}

TEST(PendulumTest, CorrectPositionAfterCreationThetaNegativePiOverTwo) {
  using T = math::Multivector<FloatT, 3, 0, 0, math::InnerProduct::LEFT_CONTRACTION>;
  PendulumConfigurator<T> config{};
  config.set_theta(-pi / 2);
  auto p{config.create()};
  EXPECT_TRUE(math::AreNear(-T::template e<0>(), p.position(), 0.0001));
}

TEST(PendulumTest, CorrectPositionAfterCreationThetaPi) {
  using T = math::Multivector<FloatT, 3, 0, 0, math::InnerProduct::LEFT_CONTRACTION>;
  PendulumConfigurator<T> config{};
  config.set_theta(pi);
  auto p{config.create()};
  EXPECT_TRUE(math::AreNear(T::template e<1>(), p.position(), 0.0001));
}

TEST(PendulumTest, CorrectPositionAfterCreationThetaNegativePi) {
  using T = math::Multivector<FloatT, 3, 0, 0, math::InnerProduct::LEFT_CONTRACTION>;
  PendulumConfigurator<T> config{};
  config.set_theta(-pi);
  auto p{config.create()};
  EXPECT_TRUE(math::AreNear(T::template e<1>(), p.position(), 0.0001));
}

TEST(PendulumTest, ThetaSameAfterCreation) {
  using T = math::Multivector<FloatT, 3, 0, 0, math::InnerProduct::LEFT_CONTRACTION>;
  PendulumConfigurator<T> config{};
  for (const auto angle : {0., pi / 2, pi - 0.01, -pi / 2, -(pi - 0.01), 3 * pi / 4, -3 * pi / 4}) {
    config.set_theta(angle);
    auto p{config.create()};
    EXPECT_NEAR(angle, p.theta(), 0.0001);
  }
}

TEST(PendulumTest, StateAlwaysZeroIfNoInitialEnergy) {
  using T = math::Multivector<FloatT, 3, 0, 0, math::InnerProduct::LEFT_CONTRACTION>;
  PendulumConfigurator<T> config{};
  config.set_theta(0);
  auto p{config.create()};
  ASSERT_EQ(0, p.theta());
  p.evolve(1);
  EXPECT_EQ(0, p.theta());
  p.evolve(1);
  EXPECT_EQ(0, p.theta());
}

TEST(PendulumTest, ApproximatesCanonicalSmallAngleSolution) {
  using T = math::Multivector<FloatT, 3, 0, 0, math::InnerProduct::LEFT_CONTRACTION>;
  PendulumConfigurator<T> config{};
  config.set_theta(SMALL_ANGLE);
  auto p{config.create()};

  EXPECT_TRUE(IsAccurate(p, ONE_PERIOD, SMALL_ANGLE));
}

TEST(PendulumTest, AccurateThroughMultiplePeriodsWithCircularErrorAdjustmentSmallAngle) {
  using T = math::Multivector<FloatT, 3, 0, 0, math::InnerProduct::LEFT_CONTRACTION>;
  PendulumConfigurator<T> config{};
  config.set_theta(SMALL_ANGLE);
  auto p{config.create()};

  EXPECT_TRUE(IsAccurate(p, MULTIPLE_PERIODS, SMALL_ANGLE));
}

TEST(PendulumTest, AccurateThroughManyPeriodsWithCircularErrorAdjustmentSmallAngle) {
  using T = math::Multivector<FloatT, 3, 0, 0, math::InnerProduct::LEFT_CONTRACTION>;
  PendulumConfigurator<T> config{};
  config.set_theta(SMALL_ANGLE);
  auto p{config.create()};

  EXPECT_TRUE(IsAccurate(p, MANY_PERIODS, SMALL_ANGLE));
}

TEST(PendulumTest, AccurateThroughManyMorePeriodsWithCircularErrorAdjustmentSmallAngle) {
  using T = math::Multivector<FloatT, 3, 0, 0, math::InnerProduct::LEFT_CONTRACTION>;
  PendulumConfigurator<T> config{};
  config.set_theta(SMALL_ANGLE);
  auto p{config.create()};

  EXPECT_TRUE(IsAccurate(p, MANY_MORE_PERIODS, SMALL_ANGLE));
}

TEST(PendulumTest, AccurateThroughSinglePeriodWithCircularErrorAdjustmentModerateAngle) {
  using T = math::Multivector<FloatT, 3, 0, 0, math::InnerProduct::LEFT_CONTRACTION>;
  PendulumConfigurator<T> config{};
  config.set_theta(MODERATE_ANGLE);
  auto p{config.create()};

  EXPECT_TRUE(IsAccurate(p, ONE_PERIOD, MODERATE_ANGLE));
}

TEST(PendulumTest, AccurateThroughMultiplePeriodsWithCircularErrorAdjustmentModerateAngle) {
  using T = math::Multivector<FloatT, 3, 0, 0, math::InnerProduct::LEFT_CONTRACTION>;
  PendulumConfigurator<T> config{};
  config.set_theta(MODERATE_ANGLE);
  auto p{config.create()};

  EXPECT_TRUE(IsAccurate(p, MULTIPLE_PERIODS, MODERATE_ANGLE));
}

TEST(PendulumTest, AccurateThroughManyPeriodsWithCircularErrorAdjustmentModerateAngle) {
  using T = math::Multivector<FloatT, 3, 0, 0, math::InnerProduct::LEFT_CONTRACTION>;
  PendulumConfigurator<T> config{};
  config.set_theta(MODERATE_ANGLE);
  auto p{config.create()};

  EXPECT_TRUE(IsAccurate(p, MANY_PERIODS, MODERATE_ANGLE));
}

TEST(PendulumTest, AccurateThroughManyMorePeriodsWithCircularErrorAdjustmentModerateAngle) {
  using T = math::Multivector<FloatT, 3, 0, 0, math::InnerProduct::LEFT_CONTRACTION>;
  PendulumConfigurator<T> config{};
  config.set_theta(MODERATE_ANGLE);
  auto p{config.create()};

  EXPECT_TRUE(IsAccurate(p, MANY_MORE_PERIODS, MODERATE_ANGLE));
}

TEST(PendulumTest, DISABLED_AccurateThroughSinglePeriodWithCircularErrorAdjustmentLargeAngle) {
  using T = math::Multivector<FloatT, 3, 0, 0, math::InnerProduct::LEFT_CONTRACTION>;
  PendulumConfigurator<T> config{};
  config.set_theta(LARGE_ANGLE);
  auto p{config.create()};

  EXPECT_TRUE(IsAccurate(p, ONE_PERIOD, LARGE_ANGLE));
}

}  // namespace ndyn::simulation
