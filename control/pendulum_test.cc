#include "control/pendulum.h"

#include "base/pi.h"
#include "glog/logging.h"
#include "gtest/gtest.h"
#include "math/multivector.h"
#include "math/multivector_test_utils.h"

namespace ndyn::control {

static constexpr size_t ONE_PERIOD{1};
static constexpr size_t MULTIPLE_PERIODS{5};
static constexpr size_t MANY_PERIODS{10};
static constexpr size_t EXTENSIVE_PERIODS{100};

static constexpr float SMALL_ANGLE{0.01};

// For more details on circular error, see
// https://en.wikipedia.org/wiki/Pendulum#Period_of_oscillation
static constexpr decltype(SMALL_ANGLE) CORRECTED_QUARTER_PERIOD{
    pi / 2 * (1 + pow(SMALL_ANGLE, 2) / 16 + 11 * pow(SMALL_ANGLE, 4) / 3072)};

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
  auto EPSILON{angle / 10};

  static constexpr auto STEP_SIZE{0.0001};
  static constexpr ScalarType ZERO_ANGLE{0};

  for (size_t i = 0; i < num_periods; ++i) {
    LOG(INFO) << "pendulum.current_time(): " << pendulum.current_time()
              << ", pendulum.theta(): " << pendulum.theta();

    pendulum.evolve(quarter_period, STEP_SIZE);
    result = is_near(ZERO_ANGLE, pendulum.theta(), EPSILON);
    if (!result) {
      return result;
    }

    pendulum.evolve(quarter_period, STEP_SIZE);
    result = is_near(-angle, pendulum.theta(), EPSILON);
    if (!result) {
      return result;
    }

    pendulum.evolve(quarter_period, STEP_SIZE);
    result = is_near(ZERO_ANGLE, pendulum.theta(), EPSILON);
    if (!result) {
      return result;
    }

    pendulum.evolve(quarter_period, STEP_SIZE);
    result = is_near(angle, pendulum.theta(), EPSILON);
    if (!result) {
      return result;
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

TEST(ClassicPendulumTest, DISABLED_AccurateThroughMultiplePeriodsWithCircularErrorAdjustment) {
  using std::pow;
  ClassicPendulumConfigurator config{};
  config.set_theta(SMALL_ANGLE);
  auto p{config.create()};

  EXPECT_TRUE(IsAccurate(p, MULTIPLE_PERIODS, SMALL_ANGLE, CORRECTED_QUARTER_PERIOD));
}

TEST(ClassicPendulumTest, DISABLED_AccurateThroughManyPeriodsWithCircularErrorAdjustment) {
  using std::pow;
  ClassicPendulumConfigurator config{};
  config.set_theta(SMALL_ANGLE);
  auto p{config.create()};

  EXPECT_TRUE(IsAccurate(p, MANY_PERIODS, SMALL_ANGLE, CORRECTED_QUARTER_PERIOD));
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

TEST(GA2DPendulumTest, AccurateThroughMultiplePeriodsWithCircularErrorAdjustment) {
  using std::pow;
  using T = math::Multivector<float, 2, 0, 0, math::InnerProduct::LEFT_CONTRACTION>;
  GAPendulumConfigurator<T> config{};
  config.set_theta(SMALL_ANGLE);
  auto p{config.create()};

  EXPECT_TRUE(IsAccurate(p, MULTIPLE_PERIODS, SMALL_ANGLE, CORRECTED_QUARTER_PERIOD));
}

TEST(GA2DPendulumTest, DISABLED_AccurateThroughManyPeriodsWithCircularErrorAdjustment) {
  using std::pow;
  using T = math::Multivector<float, 2, 0, 0, math::InnerProduct::LEFT_CONTRACTION>;
  GAPendulumConfigurator<T> config{};
  config.set_theta(SMALL_ANGLE);
  auto p{config.create()};

  EXPECT_TRUE(IsAccurate(p, MANY_PERIODS, SMALL_ANGLE, CORRECTED_QUARTER_PERIOD));
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

TEST(GAPendulumTest, AccurateThroughMultiplePeriodsWithCircularErrorAdjustment) {
  using std::pow;
  using T = math::Multivector<float, 3, 0, 0, math::InnerProduct::LEFT_CONTRACTION>;
  GAPendulumConfigurator<T> config{};
  config.set_theta(SMALL_ANGLE);
  auto p{config.create()};

  EXPECT_TRUE(IsAccurate(p, MULTIPLE_PERIODS, SMALL_ANGLE, CORRECTED_QUARTER_PERIOD));
}

TEST(GAPendulumTest, DISABLED_AccurateThroughManyPeriodsWithCircularErrorAdjustment) {
  using std::pow;
  using T = math::Multivector<float, 3, 0, 0, math::InnerProduct::LEFT_CONTRACTION>;
  GAPendulumConfigurator<T> config{};
  config.set_theta(SMALL_ANGLE);
  auto p{config.create()};

  EXPECT_TRUE(IsAccurate(p, MANY_PERIODS, SMALL_ANGLE, CORRECTED_QUARTER_PERIOD));
}

}  // namespace ndyn::control
