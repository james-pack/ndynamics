#include "simulation/pendulum.h"

#include "base/pi.h"
#include "glog/logging.h"
#include "gtest/gtest.h"
#include "math/multivector.h"
#include "math/multivector_test_utils.h"

namespace ndyn::simulation {

static constexpr size_t ONE_PERIOD{1};
static constexpr size_t MULTIPLE_PERIODS{3};
static constexpr size_t MANY_PERIODS{7};
static constexpr size_t EXTENSIVE_PERIODS{25};

static constexpr float SMALL_ANGLE{0.01};
static constexpr float MODERATE_ANGLE{pi / 8};
static constexpr float LARGE_ANGLE{pi - 0.1};

// For more details on circular error, see
// https://en.wikipedia.org/wiki/Pendulum#Period_of_oscillation
template <typename T>
T calculate_corrected_quarter_period(T angle) {
  return pi / 2 * (1 + pow(SMALL_ANGLE, 2) / 16 + 11 * pow(SMALL_ANGLE, 4) / 3072);
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
::testing::AssertionResult IsAccurate(PendulumT pendulum, size_t num_periods, ScalarType angle,
                                      ScalarType quarter_period = pi / 2) {
  ::testing::AssertionResult result{::testing::AssertionSuccess()};
  ScalarType EPSILON{static_cast<ScalarType>(0.05) * angle};

  static constexpr auto STEP_SIZE{0.001};
  static constexpr ScalarType ZERO_ANGLE{0};

  for (size_t i = 0; i < num_periods; ++i) {
    pendulum.goto_time(4 * i * quarter_period);

    pendulum.evolve(quarter_period, STEP_SIZE);
    result = is_near(ZERO_ANGLE, pendulum.theta(), (i + 1) * EPSILON);
    if (!result) {
      return result << ", pendulum.current_time(): " << pendulum.current_time();
    }

    pendulum.evolve(quarter_period, STEP_SIZE);
    result = is_near(-angle, pendulum.theta(), (i + 1) * EPSILON);
    if (!result) {
      return result << ", pendulum.current_time(): " << pendulum.current_time();
    }

    pendulum.evolve(quarter_period, STEP_SIZE);
    result = is_near(ZERO_ANGLE, pendulum.theta(), (i + 1) * EPSILON);
    if (!result) {
      return result << ", pendulum.current_time(): " << pendulum.current_time();
    }

    pendulum.evolve(quarter_period, STEP_SIZE);
    result = is_near(angle, pendulum.theta(), (i + 1) * EPSILON);
    if (!result) {
      return result << ", pendulum.current_time(): " << pendulum.current_time();
    }
  }
  return result;
}

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
  config.set_theta(SMALL_ANGLE);
  auto p{config.create()};

  EXPECT_TRUE(IsAccurate(p, ONE_PERIOD, SMALL_ANGLE));
}

TEST(ClassicPendulumTest, AccurateThroughMultiplePeriodsWithCircularErrorAdjustmentSmallAngle) {
  using std::pow;
  ClassicPendulumConfigurator config{};
  config.set_theta(SMALL_ANGLE);
  auto p{config.create()};

  EXPECT_TRUE(IsAccurate(p, MULTIPLE_PERIODS, SMALL_ANGLE,
                         calculate_corrected_quarter_period(SMALL_ANGLE)));
}

TEST(ClassicPendulumTest, AccurateThroughManyPeriodsWithCircularErrorAdjustmentSmallAngle) {
  using std::pow;
  ClassicPendulumConfigurator config{};
  config.set_theta(SMALL_ANGLE);
  auto p{config.create()};

  EXPECT_TRUE(
      IsAccurate(p, MANY_PERIODS, SMALL_ANGLE, calculate_corrected_quarter_period(SMALL_ANGLE)));
}

TEST(ClassicPendulumTest, AccurateThroughExtensivePeriodsWithCircularErrorAdjustmentSmallAngle) {
  using std::pow;
  ClassicPendulumConfigurator config{};
  config.set_theta(SMALL_ANGLE);
  auto p{config.create()};

  EXPECT_TRUE(IsAccurate(p, EXTENSIVE_PERIODS, SMALL_ANGLE,
                         calculate_corrected_quarter_period(SMALL_ANGLE)));
}

TEST(ClassicPendulumTest, AccurateThroughSinglePeriodWithCircularErrorAdjustmentModerateAngle) {
  using std::pow;
  ClassicPendulumConfigurator config{};
  config.set_theta(MODERATE_ANGLE);
  auto p{config.create()};

  EXPECT_TRUE(IsAccurate(p, ONE_PERIOD, MODERATE_ANGLE,
                         calculate_corrected_quarter_period(MODERATE_ANGLE)));
}

TEST(ClassicPendulumTest,
     DISABLED_AccurateThroughMultiplePeriodsWithCircularErrorAdjustmentModerateAngle) {
  using std::pow;
  ClassicPendulumConfigurator config{};
  config.set_theta(MODERATE_ANGLE);
  auto p{config.create()};

  EXPECT_TRUE(IsAccurate(p, MULTIPLE_PERIODS, MODERATE_ANGLE,
                         calculate_corrected_quarter_period(MODERATE_ANGLE)));
}

TEST(ClassicPendulumTest,
     DISABLED_AccurateThroughSinglePeriodWithCircularErrorAdjustmentLargeAngle) {
  using std::pow;
  ClassicPendulumConfigurator config{};
  config.set_theta(LARGE_ANGLE);
  auto p{config.create()};

  EXPECT_TRUE(
      IsAccurate(p, ONE_PERIOD, LARGE_ANGLE, calculate_corrected_quarter_period(LARGE_ANGLE)));
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

TEST(GA2DPendulumTest, ApproximatesCanonicalSmallAngleSolution) {
  using T = math::Multivector<float, 2, 0, 0, math::InnerProduct::LEFT_CONTRACTION>;
  GAPendulumConfigurator<T> config{};
  config.set_theta(SMALL_ANGLE);
  auto p{config.create()};

  EXPECT_TRUE(IsAccurate(p, ONE_PERIOD, SMALL_ANGLE));
}

TEST(GA2DPendulumTest, AccurateThroughMultiplePeriodsWithCircularErrorAdjustmentSmallAngle) {
  using std::pow;
  using T = math::Multivector<float, 2, 0, 0, math::InnerProduct::LEFT_CONTRACTION>;
  GAPendulumConfigurator<T> config{};
  config.set_theta(SMALL_ANGLE);
  auto p{config.create()};

  EXPECT_TRUE(IsAccurate(p, MULTIPLE_PERIODS, SMALL_ANGLE,
                         calculate_corrected_quarter_period(SMALL_ANGLE)));
}

TEST(GA2DPendulumTest, AccurateThroughManyPeriodsWithCircularErrorAdjustmentSmallAngle) {
  using std::pow;
  using T = math::Multivector<float, 2, 0, 0, math::InnerProduct::LEFT_CONTRACTION>;
  GAPendulumConfigurator<T> config{};
  config.set_theta(SMALL_ANGLE);
  auto p{config.create()};

  EXPECT_TRUE(
      IsAccurate(p, MANY_PERIODS, SMALL_ANGLE, calculate_corrected_quarter_period(SMALL_ANGLE)));
}

TEST(GA2DPendulumTest,
     DISABLED_AccurateThroughSinglePeriodWithCircularErrorAdjustmentModerateAngle) {
  using std::pow;
  using T = math::Multivector<float, 2, 0, 0, math::InnerProduct::LEFT_CONTRACTION>;
  GAPendulumConfigurator<T> config{};
  config.set_theta(MODERATE_ANGLE);
  auto p{config.create()};

  EXPECT_TRUE(IsAccurate(p, ONE_PERIOD, MODERATE_ANGLE,
                         calculate_corrected_quarter_period(MODERATE_ANGLE)));
}

TEST(GA2DPendulumTest,
     DISABLED_AccurateThroughMultiplePeriodsWithCircularErrorAdjustmentModerateAngle) {
  using std::pow;
  using T = math::Multivector<float, 2, 0, 0, math::InnerProduct::LEFT_CONTRACTION>;
  GAPendulumConfigurator<T> config{};
  config.set_theta(MODERATE_ANGLE);
  auto p{config.create()};

  EXPECT_TRUE(IsAccurate(p, MULTIPLE_PERIODS, MODERATE_ANGLE,
                         calculate_corrected_quarter_period(MODERATE_ANGLE)));
}

TEST(GA2DPendulumTest, DISABLED_AccurateThroughSinglePeriodWithCircularErrorAdjustmentLargeAngle) {
  using std::pow;
  using T = math::Multivector<float, 2, 0, 0, math::InnerProduct::LEFT_CONTRACTION>;
  GAPendulumConfigurator<T> config{};
  config.set_theta(LARGE_ANGLE);
  auto p{config.create()};

  EXPECT_TRUE(
      IsAccurate(p, ONE_PERIOD, LARGE_ANGLE, calculate_corrected_quarter_period(LARGE_ANGLE)));
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

TEST(GAPendulumTest, StateAlwaysZeroIfNoInitialEnergy) {
  using T = math::Multivector<float, 3, 0, 0, math::InnerProduct::LEFT_CONTRACTION>;
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
  using T = math::Multivector<float, 3, 0, 0, math::InnerProduct::LEFT_CONTRACTION>;
  GAPendulumConfigurator<T> config{};
  config.set_theta(SMALL_ANGLE);
  auto p{config.create()};

  EXPECT_TRUE(IsAccurate(p, ONE_PERIOD, SMALL_ANGLE));
}

TEST(GAPendulumTest, AccurateThroughMultiplePeriodsWithCircularErrorAdjustmentSmallAngle) {
  using std::pow;
  using T = math::Multivector<float, 3, 0, 0, math::InnerProduct::LEFT_CONTRACTION>;
  GAPendulumConfigurator<T> config{};
  config.set_theta(SMALL_ANGLE);
  auto p{config.create()};

  EXPECT_TRUE(IsAccurate(p, MULTIPLE_PERIODS, SMALL_ANGLE,
                         calculate_corrected_quarter_period(SMALL_ANGLE)));
}

TEST(GAPendulumTest, AccurateThroughManyPeriodsWithCircularErrorAdjustmentSmallAngle) {
  using std::pow;
  using T = math::Multivector<float, 3, 0, 0, math::InnerProduct::LEFT_CONTRACTION>;
  GAPendulumConfigurator<T> config{};
  config.set_theta(SMALL_ANGLE);
  auto p{config.create()};

  EXPECT_TRUE(
      IsAccurate(p, MANY_PERIODS, SMALL_ANGLE, calculate_corrected_quarter_period(SMALL_ANGLE)));
}

TEST(GAPendulumTest, DISABLED_AccurateThroughSinglePeriodWithCircularErrorAdjustmentModerateAngle) {
  using std::pow;
  using T = math::Multivector<float, 3, 0, 0, math::InnerProduct::LEFT_CONTRACTION>;
  GAPendulumConfigurator<T> config{};
  config.set_theta(MODERATE_ANGLE);
  auto p{config.create()};

  EXPECT_TRUE(IsAccurate(p, ONE_PERIOD, MODERATE_ANGLE,
                         calculate_corrected_quarter_period(MODERATE_ANGLE)));
}

TEST(GAPendulumTest,
     DISABLED_AccurateThroughMultiplePeriodsWithCircularErrorAdjustmentModerateAngle) {
  using std::pow;
  using T = math::Multivector<float, 3, 0, 0, math::InnerProduct::LEFT_CONTRACTION>;
  GAPendulumConfigurator<T> config{};
  config.set_theta(MODERATE_ANGLE);
  auto p{config.create()};

  EXPECT_TRUE(IsAccurate(p, MULTIPLE_PERIODS, MODERATE_ANGLE,
                         calculate_corrected_quarter_period(MODERATE_ANGLE)));
}

TEST(GAPendulumTest, DISABLED_AccurateThroughSinglePeriodWithCircularErrorAdjustmentLargeAngle) {
  using std::pow;
  using T = math::Multivector<float, 3, 0, 0, math::InnerProduct::LEFT_CONTRACTION>;
  GAPendulumConfigurator<T> config{};
  config.set_theta(LARGE_ANGLE);
  auto p{config.create()};

  EXPECT_TRUE(
      IsAccurate(p, ONE_PERIOD, LARGE_ANGLE, calculate_corrected_quarter_period(LARGE_ANGLE)));
}

}  // namespace ndyn::simulation
