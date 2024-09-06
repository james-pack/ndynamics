#include "math/pendulum.h"

#include "base/pi.h"
#include "glog/logging.h"
#include "gtest/gtest.h"
#include "math/multivector.h"
#include "math/multivector_test_utils.h"

namespace ndyn::math {

using FloatT = float;

static constexpr size_t ONE_PERIOD{1};
static constexpr size_t MULTIPLE_PERIODS{2};
static constexpr size_t MANY_PERIODS{4};
static constexpr size_t MANY_MORE_PERIODS{10};

static constexpr FloatT SMALL_ANGLE{0.01};
static constexpr FloatT MODERATE_ANGLE{pi / 8};
static constexpr FloatT LARGE_ANGLE{pi - 0.1};

// For more details on circular error, see
// https://en.wikipedia.org/wiki/Pendulum#Period_of_oscillation
template <typename T>
T calculate_corrected_quarter_period(T angle) {
  return pi / 2 * (1 + pow(angle, 2) / 16 + 11 * pow(angle, 4) / 3072);
}

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
  const ScalarType quarter_period = calculate_corrected_quarter_period(angle);

  // Compare the expected and computed values to be within a percentage of the given angle.
  const ScalarType EPSILON{static_cast<ScalarType>(0.05) * angle};

  static constexpr ScalarType ZERO_ANGLE{0};

  // Use a smaller step size as the number of periods is greater. This helps offset the accumulated
  // error.
  ScalarType STEP_SIZE{static_cast<ScalarType>(0.01) / num_periods};
  for (size_t i = 0; i < num_periods; ++i) {
    pendulum.goto_time(4 * i * quarter_period);

    pendulum.evolve(quarter_period, STEP_SIZE);
    result = is_near(ZERO_ANGLE, pendulum.theta(), EPSILON);
    if (!result) {
      return result << ", pendulum.current_time(): " << pendulum.current_time();
    }

    pendulum.evolve(quarter_period, STEP_SIZE);
    result = is_near(-angle, pendulum.theta(), EPSILON);
    if (!result) {
      return result << ", pendulum.current_time(): " << pendulum.current_time();
    }

    pendulum.evolve(quarter_period, STEP_SIZE);
    result = is_near(ZERO_ANGLE, pendulum.theta(), EPSILON);
    if (!result) {
      return result << ", pendulum.current_time(): " << pendulum.current_time();
    }

    pendulum.evolve(quarter_period, STEP_SIZE);
    result = is_near(angle, pendulum.theta(), EPSILON);
    if (!result) {
      return result << ", pendulum.current_time(): " << pendulum.current_time();
    }
  }
  return result;
}

TEST(ClassicPendulumTest, StateAlwaysZeroIfNoInitialEnergy) {
  ClassicPendulumConfigurator<FloatT> config{};
  config.set_theta(0);
  auto p{config.create()};
  ASSERT_EQ(0, p.theta());
  p.evolve(1, 0.1);
  EXPECT_EQ(0, p.theta());
  p.evolve(1, 0.1);
  EXPECT_EQ(0, p.theta());
}

TEST(ClassicPendulumTest, ApproximatesCanonicalSmallAngleSolution) {
  ClassicPendulumConfigurator<FloatT> config{};
  config.set_theta(SMALL_ANGLE);
  auto p{config.create()};

  EXPECT_TRUE(IsAccurate(p, ONE_PERIOD, SMALL_ANGLE));
}

TEST(ClassicPendulumTest, AccurateThroughMultiplePeriodsWithCircularErrorAdjustmentSmallAngle) {
  using std::pow;
  ClassicPendulumConfigurator<FloatT> config{};
  config.set_theta(SMALL_ANGLE);
  auto p{config.create()};

  EXPECT_TRUE(IsAccurate(p, MULTIPLE_PERIODS, SMALL_ANGLE));
}

TEST(ClassicPendulumTest, AccurateThroughManyPeriodsWithCircularErrorAdjustmentSmallAngle) {
  using std::pow;
  ClassicPendulumConfigurator<FloatT> config{};
  config.set_theta(SMALL_ANGLE);
  auto p{config.create()};

  EXPECT_TRUE(IsAccurate(p, MANY_PERIODS, SMALL_ANGLE));
}

TEST(ClassicPendulumTest, AccurateThroughManyMorePeriodsWithCircularErrorAdjustmentSmallAngle) {
  using std::pow;
  ClassicPendulumConfigurator<FloatT> config{};
  config.set_theta(SMALL_ANGLE);
  auto p{config.create()};

  EXPECT_TRUE(IsAccurate(p, MANY_MORE_PERIODS, SMALL_ANGLE));
}

TEST(ClassicPendulumTest, AccurateThroughSinglePeriodWithCircularErrorAdjustmentModerateAngle) {
  using std::pow;
  ClassicPendulumConfigurator<FloatT> config{};
  config.set_theta(MODERATE_ANGLE);
  auto p{config.create()};

  EXPECT_TRUE(IsAccurate(p, ONE_PERIOD, MODERATE_ANGLE));
}

TEST(ClassicPendulumTest, AccurateThroughMultiplePeriodsWithCircularErrorAdjustmentModerateAngle) {
  using std::pow;
  ClassicPendulumConfigurator<FloatT> config{};
  config.set_theta(MODERATE_ANGLE);
  auto p{config.create()};

  EXPECT_TRUE(IsAccurate(p, MULTIPLE_PERIODS, MODERATE_ANGLE));
}

TEST(ClassicPendulumTest,
     DISABLED_AccurateThroughSinglePeriodWithCircularErrorAdjustmentLargeAngle) {
  using std::pow;
  ClassicPendulumConfigurator<FloatT> config{};
  config.set_theta(LARGE_ANGLE);
  auto p{config.create()};

  EXPECT_TRUE(IsAccurate(p, ONE_PERIOD, LARGE_ANGLE));
}

TEST(GA2DPendulumTest, LengthSameAfterCreation) {
  using T = math::Multivector<FloatT, 2, 0, 0, math::InnerProduct::LEFT_CONTRACTION>;
  GAPendulumConfigurator<T> config{};
  for (const auto length : {1., 2., 0.5}) {
    config.set_length(length);
    auto p{config.create()};
    EXPECT_NEAR(length, p.length(), 0.0001);
  }
}

TEST(GA2DPendulumTest, CorrectPositionAfterCreationThetaZero) {
  using T = math::Multivector<FloatT, 2, 0, 0, math::InnerProduct::LEFT_CONTRACTION>;
  GAPendulumConfigurator<T> config{};
  config.set_theta(0);
  auto p{config.create()};
  EXPECT_TRUE(math::AreNear(-T::template e<1>(), p.position(), 0.0001));
}

TEST(GA2DPendulumTest, CorrectPositionAfterCreationThetaPiOverTwo) {
  using T = math::Multivector<FloatT, 2, 0, 0, math::InnerProduct::LEFT_CONTRACTION>;
  GAPendulumConfigurator<T> config{};
  config.set_theta(pi / 2);
  auto p{config.create()};
  EXPECT_TRUE(math::AreNear(T::template e<0>(), p.position(), 0.0001));
}

TEST(GA2DPendulumTest, CorrectPositionAfterCreationThetaNegativePiOverTwo) {
  using T = math::Multivector<FloatT, 2, 0, 0, math::InnerProduct::LEFT_CONTRACTION>;
  GAPendulumConfigurator<T> config{};
  config.set_theta(-pi / 2);
  auto p{config.create()};
  EXPECT_TRUE(math::AreNear(-T::template e<0>(), p.position(), 0.0001));
}

TEST(GA2DPendulumTest, CorrectPositionAfterCreationThetaPi) {
  using T = math::Multivector<FloatT, 2, 0, 0, math::InnerProduct::LEFT_CONTRACTION>;
  GAPendulumConfigurator<T> config{};
  config.set_theta(pi);
  auto p{config.create()};
  EXPECT_TRUE(math::AreNear(T::template e<1>(), p.position(), 0.0001));
}

TEST(GA2DPendulumTest, CorrectPositionAfterCreationThetaNegativePi) {
  using T = math::Multivector<FloatT, 2, 0, 0, math::InnerProduct::LEFT_CONTRACTION>;
  GAPendulumConfigurator<T> config{};
  config.set_theta(-pi);
  auto p{config.create()};
  EXPECT_TRUE(math::AreNear(T::template e<1>(), p.position(), 0.0001));
}

TEST(GA2DPendulumTest, ThetaSameAfterCreation) {
  using T = math::Multivector<FloatT, 2, 0, 0, math::InnerProduct::LEFT_CONTRACTION>;
  GAPendulumConfigurator<T> config{};
  for (const auto angle : {0., pi / 2, pi - 0.01, -pi / 2, -(pi - 0.01), 3 * pi / 4, -3 * pi / 4}) {
    config.set_theta(angle);
    auto p{config.create()};
    EXPECT_NEAR(angle, p.theta(), 0.0001);
  }
}

TEST(GA2DPendulumTest, ApproximatesCanonicalSmallAngleSolution) {
  using T = math::Multivector<FloatT, 2, 0, 0, math::InnerProduct::LEFT_CONTRACTION>;
  GAPendulumConfigurator<T> config{};
  config.set_theta(SMALL_ANGLE);
  auto p{config.create()};

  EXPECT_TRUE(IsAccurate(p, ONE_PERIOD, SMALL_ANGLE));
}

TEST(GA2DPendulumTest, AccurateThroughMultiplePeriodsWithCircularErrorAdjustmentSmallAngle) {
  using std::pow;
  using T = math::Multivector<FloatT, 2, 0, 0, math::InnerProduct::LEFT_CONTRACTION>;
  GAPendulumConfigurator<T> config{};
  config.set_theta(SMALL_ANGLE);
  auto p{config.create()};

  EXPECT_TRUE(IsAccurate(p, MULTIPLE_PERIODS, SMALL_ANGLE));
}

TEST(GA2DPendulumTest, AccurateThroughManyPeriodsWithCircularErrorAdjustmentSmallAngle) {
  using std::pow;
  using T = math::Multivector<FloatT, 2, 0, 0, math::InnerProduct::LEFT_CONTRACTION>;
  GAPendulumConfigurator<T> config{};
  config.set_theta(SMALL_ANGLE);
  auto p{config.create()};

  EXPECT_TRUE(IsAccurate(p, MANY_PERIODS, SMALL_ANGLE));
}

TEST(GA2DPendulumTest,
     DISABLED_AccurateThroughManyMorePeriodsWithCircularErrorAdjustmentSmallAngle) {
  using std::pow;
  using T = math::Multivector<FloatT, 2, 0, 0, math::InnerProduct::LEFT_CONTRACTION>;
  GAPendulumConfigurator<T> config{};
  config.set_theta(SMALL_ANGLE);
  auto p{config.create()};

  EXPECT_TRUE(IsAccurate(p, MANY_MORE_PERIODS, SMALL_ANGLE));
}

TEST(GA2DPendulumTest,
     DISABLED_AccurateThroughSinglePeriodWithCircularErrorAdjustmentModerateAngle) {
  using std::pow;
  using T = math::Multivector<FloatT, 2, 0, 0, math::InnerProduct::LEFT_CONTRACTION>;
  GAPendulumConfigurator<T> config{};
  config.set_theta(MODERATE_ANGLE);
  auto p{config.create()};

  EXPECT_TRUE(IsAccurate(p, ONE_PERIOD, MODERATE_ANGLE));
}

TEST(GA2DPendulumTest,
     DISABLED_AccurateThroughMultiplePeriodsWithCircularErrorAdjustmentModerateAngle) {
  using std::pow;
  using T = math::Multivector<FloatT, 2, 0, 0, math::InnerProduct::LEFT_CONTRACTION>;
  GAPendulumConfigurator<T> config{};
  config.set_theta(MODERATE_ANGLE);
  auto p{config.create()};

  EXPECT_TRUE(IsAccurate(p, MULTIPLE_PERIODS, MODERATE_ANGLE));
}

TEST(GA2DPendulumTest, DISABLED_AccurateThroughSinglePeriodWithCircularErrorAdjustmentLargeAngle) {
  using std::pow;
  using T = math::Multivector<FloatT, 2, 0, 0, math::InnerProduct::LEFT_CONTRACTION>;
  GAPendulumConfigurator<T> config{};
  config.set_theta(LARGE_ANGLE);
  auto p{config.create()};

  EXPECT_TRUE(IsAccurate(p, ONE_PERIOD, LARGE_ANGLE));
}

TEST(GAPendulumTest, LengthSameAfterCreation) {
  using T = math::Multivector<FloatT, 3, 0, 0, math::InnerProduct::LEFT_CONTRACTION>;
  GAPendulumConfigurator<T> config{};
  for (const auto length : {1., 2., 0.5}) {
    config.set_length(length);
    auto p{config.create()};
    EXPECT_NEAR(length, p.length(), 0.0001);
  }
}

TEST(GAPendulumTest, CorrectPositionAfterCreationThetaZero) {
  using T = math::Multivector<FloatT, 3, 0, 0, math::InnerProduct::LEFT_CONTRACTION>;
  GAPendulumConfigurator<T> config{};
  config.set_theta(0);
  auto p{config.create()};
  EXPECT_TRUE(math::AreNear(-T::template e<1>(), p.position(), 0.0001));
}

TEST(GAPendulumTest, CorrectPositionAfterCreationThetaPiOverTwo) {
  using T = math::Multivector<FloatT, 3, 0, 0, math::InnerProduct::LEFT_CONTRACTION>;
  GAPendulumConfigurator<T> config{};
  config.set_theta(pi / 2);
  auto p{config.create()};
  EXPECT_TRUE(math::AreNear(T::template e<0>(), p.position(), 0.0001));
}

TEST(GAPendulumTest, CorrectPositionAfterCreationThetaNegativePiOverTwo) {
  using T = math::Multivector<FloatT, 3, 0, 0, math::InnerProduct::LEFT_CONTRACTION>;
  GAPendulumConfigurator<T> config{};
  config.set_theta(-pi / 2);
  auto p{config.create()};
  EXPECT_TRUE(math::AreNear(-T::template e<0>(), p.position(), 0.0001));
}

TEST(GAPendulumTest, CorrectPositionAfterCreationThetaPi) {
  using T = math::Multivector<FloatT, 3, 0, 0, math::InnerProduct::LEFT_CONTRACTION>;
  GAPendulumConfigurator<T> config{};
  config.set_theta(pi);
  auto p{config.create()};
  EXPECT_TRUE(math::AreNear(T::template e<1>(), p.position(), 0.0001));
}

TEST(GAPendulumTest, CorrectPositionAfterCreationThetaNegativePi) {
  using T = math::Multivector<FloatT, 3, 0, 0, math::InnerProduct::LEFT_CONTRACTION>;
  GAPendulumConfigurator<T> config{};
  config.set_theta(-pi);
  auto p{config.create()};
  EXPECT_TRUE(math::AreNear(T::template e<1>(), p.position(), 0.0001));
}

TEST(GAPendulumTest, ThetaSameAfterCreation) {
  using T = math::Multivector<FloatT, 3, 0, 0, math::InnerProduct::LEFT_CONTRACTION>;
  GAPendulumConfigurator<T> config{};
  for (const auto angle : {0., pi / 2, pi - 0.01, -pi / 2, -(pi - 0.01), 3 * pi / 4, -3 * pi / 4}) {
    config.set_theta(angle);
    auto p{config.create()};
    EXPECT_NEAR(angle, p.theta(), 0.0001);
  }
}

TEST(GAPendulumTest, StateAlwaysZeroIfNoInitialEnergy) {
  using T = math::Multivector<FloatT, 3, 0, 0, math::InnerProduct::LEFT_CONTRACTION>;
  GAPendulumConfigurator<T> config{};
  config.set_theta(0);
  auto p{config.create()};
  ASSERT_EQ(0, p.theta());
  p.evolve(1);
  EXPECT_EQ(0, p.theta());
  p.evolve(1);
  EXPECT_EQ(0, p.theta());
}

TEST(GAPendulumTest, ApproximatesCanonicalSmallAngleSolution) {
  using T = math::Multivector<FloatT, 3, 0, 0, math::InnerProduct::LEFT_CONTRACTION>;
  GAPendulumConfigurator<T> config{};
  config.set_theta(SMALL_ANGLE);
  auto p{config.create()};

  EXPECT_TRUE(IsAccurate(p, ONE_PERIOD, SMALL_ANGLE));
}

TEST(GAPendulumTest, AccurateThroughMultiplePeriodsWithCircularErrorAdjustmentSmallAngle) {
  using T = math::Multivector<FloatT, 3, 0, 0, math::InnerProduct::LEFT_CONTRACTION>;
  GAPendulumConfigurator<T> config{};
  config.set_theta(SMALL_ANGLE);
  auto p{config.create()};

  EXPECT_TRUE(IsAccurate(p, MULTIPLE_PERIODS, SMALL_ANGLE));
}

TEST(GAPendulumTest, AccurateThroughManyPeriodsWithCircularErrorAdjustmentSmallAngle) {
  using T = math::Multivector<FloatT, 3, 0, 0, math::InnerProduct::LEFT_CONTRACTION>;
  GAPendulumConfigurator<T> config{};
  config.set_theta(SMALL_ANGLE);
  auto p{config.create()};

  EXPECT_TRUE(IsAccurate(p, MANY_PERIODS, SMALL_ANGLE));
}

TEST(GAPendulumTest, DISABLED_AccurateThroughManyMorePeriodsWithCircularErrorAdjustmentSmallAngle) {
  using T = math::Multivector<FloatT, 3, 0, 0, math::InnerProduct::LEFT_CONTRACTION>;
  GAPendulumConfigurator<T> config{};
  config.set_theta(SMALL_ANGLE);
  auto p{config.create()};

  EXPECT_TRUE(IsAccurate(p, MANY_MORE_PERIODS, SMALL_ANGLE));
}

TEST(GAPendulumTest, DISABLED_AccurateThroughSinglePeriodWithCircularErrorAdjustmentModerateAngle) {
  using T = math::Multivector<FloatT, 3, 0, 0, math::InnerProduct::LEFT_CONTRACTION>;
  GAPendulumConfigurator<T> config{};
  config.set_theta(MODERATE_ANGLE);
  auto p{config.create()};

  EXPECT_TRUE(IsAccurate(p, ONE_PERIOD, MODERATE_ANGLE));
}

TEST(GAPendulumTest,
     DISABLED_AccurateThroughMultiplePeriodsWithCircularErrorAdjustmentModerateAngle) {
  using T = math::Multivector<FloatT, 3, 0, 0, math::InnerProduct::LEFT_CONTRACTION>;
  GAPendulumConfigurator<T> config{};
  config.set_theta(MODERATE_ANGLE);
  auto p{config.create()};

  EXPECT_TRUE(IsAccurate(p, MULTIPLE_PERIODS, MODERATE_ANGLE));
}

TEST(GAPendulumTest, DISABLED_AccurateThroughSinglePeriodWithCircularErrorAdjustmentLargeAngle) {
  using T = math::Multivector<FloatT, 3, 0, 0, math::InnerProduct::LEFT_CONTRACTION>;
  GAPendulumConfigurator<T> config{};
  config.set_theta(LARGE_ANGLE);
  auto p{config.create()};

  EXPECT_TRUE(IsAccurate(p, ONE_PERIOD, LARGE_ANGLE));
}

}  // namespace ndyn::math
