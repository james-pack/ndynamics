#include "simulation/pendulum.h"

#include "base/pi.h"
#include "glog/logging.h"
#include "gtest/gtest.h"
#include "math/algebra.h"
#include "math/multivector.h"
#include "math/multivector_test_utils.h"

namespace ndyn::simulation {

using FloatT = float;
using AlgebraType = math::Algebra<FloatT, 3, 0, 0, math::InnerProduct::LEFT_CONTRACTION>;
using VectorType = typename AlgebraType::VectorType;

static constexpr size_t HORIZONTAL_ELEMENT_INDEX{0};
static constexpr size_t VERTICAL_ELEMENT_INDEX{2};

static constexpr size_t ONE_PERIOD{1};
static constexpr size_t MULTIPLE_PERIODS{5};
static constexpr size_t MANY_PERIODS{10};
static constexpr size_t MANY_MORE_PERIODS{50};

static constexpr FloatT SMALL_ANGLE{0.01};
static constexpr FloatT MODERATE_ANGLE{pi / 4};
static constexpr FloatT LARGE_ANGLE{pi / 2};

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

template <typename T>
::testing::AssertionResult is_positive(T value) {
  if (value > 0) {
    return ::testing::AssertionSuccess();
  } else {
    return ::testing::AssertionFailure() << "value: " << value;
  }
}

template <typename T>
::testing::AssertionResult is_negative(T value) {
  if (value < 0) {
    return ::testing::AssertionSuccess();
  } else {
    return ::testing::AssertionFailure() << "value: " << value;
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

  ScalarType STEP_SIZE{0.01};
  for (size_t i = 0; i < num_periods; ++i) {
    pendulum.goto_time(i * 4 * quarter_period);

    pendulum.evolve(quarter_period / 2, STEP_SIZE);
    result = is_negative(pendulum.velocity().template element<HORIZONTAL_ELEMENT_INDEX>())
             << " Expected x() of velocity to be negative. velocity: " << pendulum.velocity()
             << ", pendulum.velocity().template element<HORIZONTAL_ELEMENT_INDEX>(): "
             << pendulum.velocity().template element<HORIZONTAL_ELEMENT_INDEX>();
    if (!result) {
      return result << ", pendulum.current_time(): " << pendulum.current_time()
                    << ", pendulum.period(): " << pendulum.period();
    }
    result = is_negative(pendulum.velocity().template element<VERTICAL_ELEMENT_INDEX>())
             << " Expected y() of velocity to be negative. velocity: " << pendulum.velocity();
    if (!result) {
      return result << ", pendulum.current_time(): " << pendulum.current_time()
                    << ", pendulum.period(): " << pendulum.period();
    }

    pendulum.evolve(quarter_period / 2, STEP_SIZE);
    result = is_near(ZERO_ANGLE, pendulum.theta(), EPSILON);
    if (!result) {
      return result << ", pendulum.current_time(): " << pendulum.current_time()
                    << ", pendulum.period(): " << pendulum.period();
    }

    pendulum.evolve(quarter_period / 2, STEP_SIZE);
    result = is_negative(pendulum.velocity().template element<HORIZONTAL_ELEMENT_INDEX>())
             << " Expected x() of velocity to be negative. velocity: " << pendulum.velocity();
    if (!result) {
      return result << ", pendulum.current_time(): " << pendulum.current_time()
                    << ", pendulum.period(): " << pendulum.period();
    }
    result = is_positive(pendulum.velocity().template element<VERTICAL_ELEMENT_INDEX>())
             << " Expected y() of velocity to be positive. velocity: " << pendulum.velocity();
    if (!result) {
      return result << ", pendulum.current_time(): " << pendulum.current_time()
                    << ". Expected positive velocity in y-direction. velocity: "
                    << pendulum.velocity() << ", pendulum.period(): " << pendulum.period();
    }

    pendulum.evolve(quarter_period / 2, STEP_SIZE);
    result = is_near(-angle, pendulum.theta(), EPSILON);
    if (!result) {
      return result << ", pendulum.current_time(): " << pendulum.current_time()
                    << ", pendulum.period(): " << pendulum.period();
    }

    pendulum.evolve(quarter_period / 2, STEP_SIZE);
    result = is_positive(pendulum.velocity().template element<HORIZONTAL_ELEMENT_INDEX>())
             << " Expected x() of velocity to be positive. velocity: " << pendulum.velocity();
    if (!result) {
      return result << ", pendulum.current_time(): " << pendulum.current_time()
                    << ", pendulum.period(): " << pendulum.period();
    }
    result = is_negative(pendulum.velocity().template element<VERTICAL_ELEMENT_INDEX>())
             << " Expected y() of velocity to be negative. velocity: " << pendulum.velocity();
    if (!result) {
      return result << ", pendulum.current_time(): " << pendulum.current_time()
                    << ", pendulum.period(): " << pendulum.period();
    }

    pendulum.evolve(quarter_period / 2, STEP_SIZE);
    result = is_near(ZERO_ANGLE, pendulum.theta(), EPSILON);
    if (!result) {
      return result << ", pendulum.current_time(): " << pendulum.current_time()
                    << ", pendulum.period(): " << pendulum.period();
    }

    pendulum.evolve(quarter_period / 2, STEP_SIZE);
    result = is_positive(pendulum.velocity().template element<HORIZONTAL_ELEMENT_INDEX>())
             << " Expected x() of velocity to be positive. velocity: " << pendulum.velocity();
    if (!result) {
      return result << ", pendulum.current_time(): " << pendulum.current_time()
                    << ", pendulum.period(): " << pendulum.period();
    }
    result = is_positive(pendulum.velocity().template element<VERTICAL_ELEMENT_INDEX>())
             << " Expected y() of velocity to be positive. velocity: " << pendulum.velocity();
    if (!result) {
      return result << ", pendulum.current_time(): " << pendulum.current_time()
                    << ", pendulum.period(): " << pendulum.period();
    }

    pendulum.evolve(quarter_period / 2, STEP_SIZE);
    result = is_near(angle, pendulum.theta(), EPSILON);
    if (!result) {
      return result << ", pendulum.current_time(): " << pendulum.current_time()
                    << ", pendulum.period(): " << pendulum.period();
    }
  }
  return result;
}

template <typename PendulumT, typename ScalarType>
::testing::AssertionResult IsAccurateAtHalfPeriod(PendulumT pendulum, size_t num_periods,
                                                  ScalarType angle) {
  ::testing::AssertionResult result{::testing::AssertionSuccess()};

  // Calculate the expected period including a correction term for the circular error.
  const ScalarType quarter_period = compute_period(pendulum.length(), pendulum.g(), angle) / 4;

  // Compare the expected and computed values to be within a percentage of the given angle.
  const ScalarType EPSILON{static_cast<ScalarType>(0.05) * angle};

  ScalarType STEP_SIZE{0.01};
  for (size_t i = 0; i < num_periods; ++i) {
    pendulum.goto_time(i * 4 * quarter_period);

    pendulum.evolve(quarter_period * 2, STEP_SIZE);
    result = is_near(-angle, pendulum.theta(), EPSILON);
    if (!result) {
      return result << ", pendulum.current_time(): " << pendulum.current_time()
                    << ", pendulum.period(): " << pendulum.period();
    }

    pendulum.evolve(quarter_period * 2, STEP_SIZE);
    result = is_near(angle, pendulum.theta(), EPSILON);
    if (!result) {
      return result << ", pendulum.current_time(): " << pendulum.current_time()
                    << ", pendulum.period(): " << pendulum.period();
    }
  }
  return result;
}

TEST(PendulumTest, LengthSameAfterCreation) {
  PendulumConfigurator<AlgebraType> config{};
  for (const auto length : {1., 2., 0.5}) {
    config.set_length(length);
    auto p{config.create()};
    EXPECT_NEAR(length, p.length(), 0.0001);
  }
}

TEST(PendulumTest, CorrectPositionAfterCreationThetaZero) {
  PendulumConfigurator<AlgebraType> config{};
  config.set_theta(0);
  auto p{config.create()};
  EXPECT_TRUE(
      math::AreNear(-VectorType::template e<VERTICAL_ELEMENT_INDEX>(), p.position(), 0.0001));
}

TEST(PendulumTest, CorrectPositionAfterCreationThetaPiOverTwo) {
  PendulumConfigurator<AlgebraType> config{};
  config.set_theta(pi / 2);
  auto p{config.create()};
  EXPECT_TRUE(
      math::AreNear(VectorType::template e<HORIZONTAL_ELEMENT_INDEX>(), p.position(), 0.0001));
}

TEST(PendulumTest, CorrectPositionAfterCreationThetaNegativePiOverTwo) {
  PendulumConfigurator<AlgebraType> config{};
  config.set_theta(-pi / 2);
  auto p{config.create()};
  EXPECT_TRUE(
      math::AreNear(-VectorType::template e<HORIZONTAL_ELEMENT_INDEX>(), p.position(), 0.0001));
}

TEST(PendulumTest, CorrectPositionAfterCreationThetaPi) {
  PendulumConfigurator<AlgebraType> config{};
  config.set_theta(pi);
  auto p{config.create()};
  EXPECT_TRUE(
      math::AreNear(VectorType::template e<VERTICAL_ELEMENT_INDEX>(), p.position(), 0.0001));
}

TEST(PendulumTest, CorrectPositionAfterCreationThetaNegativePi) {
  PendulumConfigurator<AlgebraType> config{};
  config.set_theta(-pi);
  auto p{config.create()};
  EXPECT_TRUE(
      math::AreNear(VectorType::template e<VERTICAL_ELEMENT_INDEX>(), p.position(), 0.0001));
}

TEST(PendulumTest, ThetaSameAfterCreation) {
  PendulumConfigurator<AlgebraType> config{};
  for (const auto angle : {0., pi / 2, pi - 0.01, -pi / 2, -(pi - 0.01), 3 * pi / 4, -3 * pi / 4}) {
    config.set_theta(angle);
    auto p{config.create()};
    EXPECT_NEAR(angle, p.theta(), 0.0001);
  }
}

TEST(PendulumTest, CorrectHeightAfterCreationThetaZero) {
  PendulumConfigurator<AlgebraType> config{};
  config.set_theta(0);
  auto p{config.create()};
  EXPECT_NEAR(0, p.height(), 0.01);
}

TEST(PendulumTest, CorrectHeightAfterCreationThetaPiOverTwo) {
  PendulumConfigurator<AlgebraType> config{};
  config.set_theta(pi / 2);
  auto p{config.create()};
  EXPECT_NEAR(p.length(), p.height(), 0.01);
}

TEST(PendulumTest, CorrectHeightAfterCreationThetaNegativePiOverTwo) {
  PendulumConfigurator<AlgebraType> config{};
  config.set_theta(-pi / 2);
  auto p{config.create()};
  EXPECT_NEAR(p.length(), p.height(), 0.01);
}

TEST(PendulumTest, CorrectHeightAfterCreationThetaPi) {
  PendulumConfigurator<AlgebraType> config{};
  config.set_theta(pi);
  auto p{config.create()};
  EXPECT_NEAR(2 * p.length(), p.height(), 0.01);
}

TEST(PendulumTest, CorrectHeightAfterCreationThetaNegativePi) {
  PendulumConfigurator<AlgebraType> config{};
  config.set_theta(-pi);
  auto p{config.create()};
  EXPECT_NEAR(2 * p.length(), p.height(), 0.01);
}

TEST(PendulumTest, StateAlwaysZeroIfNoInitialEnergy) {
  PendulumConfigurator<AlgebraType> config{};
  config.set_theta(0);

  auto p{config.create()};
  ASSERT_EQ(0, p.theta());

  EXPECT_EQ(0, p.compute_potential_energy());
  EXPECT_EQ(0, p.compute_kinetic_energy());

  p.evolve(1);
  EXPECT_EQ(0, p.theta());

  p.evolve(1);
  EXPECT_EQ(0, p.theta());
}

TEST(PendulumTest, CorrectHeightAfterCreationThetaPiFourths) {
  using std::sqrt;
  PendulumConfigurator<AlgebraType> config{};
  config.set_theta(pi / 4);
  auto p{config.create()};
  EXPECT_NEAR(p.length() - p.length() / sqrt(2), p.height(), 0.01);
}

TEST(PendulumTest, CorrectHeightAfterCreationThetaNegativePiFourths) {
  using std::sqrt;
  PendulumConfigurator<AlgebraType> config{};
  config.set_theta(-pi / 4);
  auto p{config.create()};
  EXPECT_NEAR(p.length() - p.length() / sqrt(2), p.height(), 0.01);
}

TEST(PendulumTest, ApproximatesCanonicalSmallAngleSolution) {
  PendulumConfigurator<AlgebraType> config{};
  config.set_theta(SMALL_ANGLE);
  auto p{config.create()};

  EXPECT_TRUE(IsAccurate(p, ONE_PERIOD, SMALL_ANGLE));
}

TEST(PendulumTest, AccurateThroughMultiplePeriodsWithCircularErrorAdjustmentSmallAngle) {
  PendulumConfigurator<AlgebraType> config{};
  config.set_theta(SMALL_ANGLE);
  auto p{config.create()};

  EXPECT_TRUE(IsAccurate(p, MULTIPLE_PERIODS, SMALL_ANGLE));
}

TEST(PendulumTest, AccurateThroughManyPeriodsWithCircularErrorAdjustmentSmallAngle) {
  PendulumConfigurator<AlgebraType> config{};
  config.set_theta(SMALL_ANGLE);
  auto p{config.create()};

  EXPECT_TRUE(IsAccurate(p, MANY_PERIODS, SMALL_ANGLE));
}

TEST(PendulumTest, AccurateThroughManyMorePeriodsWithCircularErrorAdjustmentSmallAngle) {
  PendulumConfigurator<AlgebraType> config{};
  config.set_theta(SMALL_ANGLE);
  auto p{config.create()};

  EXPECT_TRUE(IsAccurate(p, MANY_MORE_PERIODS, SMALL_ANGLE));
}

TEST(PendulumTest, AccurateThroughSinglePeriodWithCircularErrorAdjustmentModerateAngle) {
  PendulumConfigurator<AlgebraType> config{};
  config.set_theta(MODERATE_ANGLE);
  auto p{config.create()};

  EXPECT_TRUE(IsAccurate(p, ONE_PERIOD, MODERATE_ANGLE));
}

TEST(PendulumTest, AccurateThroughMultiplePeriodsWithCircularErrorAdjustmentModerateAngle) {
  PendulumConfigurator<AlgebraType> config{};
  config.set_theta(MODERATE_ANGLE);
  auto p{config.create()};

  EXPECT_TRUE(IsAccurateAtHalfPeriod(p, MULTIPLE_PERIODS, MODERATE_ANGLE));
}

TEST(PendulumTest, AccurateThroughManyPeriodsWithCircularErrorAdjustmentModerateAngle) {
  PendulumConfigurator<AlgebraType> config{};
  config.set_theta(MODERATE_ANGLE);
  auto p{config.create()};

  EXPECT_TRUE(IsAccurateAtHalfPeriod(p, MANY_PERIODS, MODERATE_ANGLE));
}

TEST(PendulumTest,
     DISABLED_AccurateThroughManyMorePeriodsWithCircularErrorAdjustmentModerateAngle) {
  PendulumConfigurator<AlgebraType> config{};
  config.set_theta(MODERATE_ANGLE);
  auto p{config.create()};

  EXPECT_TRUE(IsAccurateAtHalfPeriod(p, MANY_MORE_PERIODS, MODERATE_ANGLE));
}

TEST(PendulumTest, AccurateThroughSinglePeriodWithCircularErrorAdjustmentLargeAngle) {
  PendulumConfigurator<AlgebraType> config{};
  config.set_theta(LARGE_ANGLE);
  auto p{config.create()};

  EXPECT_TRUE(IsAccurate(p, ONE_PERIOD, LARGE_ANGLE));
}

TEST(PendulumTest, AccurateThroughMultiplePeriodsWithCircularErrorAdjustmentLargeAngle) {
  PendulumConfigurator<AlgebraType> config{};
  config.set_theta(LARGE_ANGLE);
  auto p{config.create()};

  EXPECT_TRUE(IsAccurateAtHalfPeriod(p, MULTIPLE_PERIODS, LARGE_ANGLE));
}

TEST(PendulumTest, DISABLED_AccurateThroughManyPeriodsWithCircularErrorAdjustmentLargeAngle) {
  PendulumConfigurator<AlgebraType> config{};
  config.set_theta(LARGE_ANGLE);
  auto p{config.create()};

  EXPECT_TRUE(IsAccurateAtHalfPeriod(p, MANY_PERIODS, LARGE_ANGLE));
}

TEST(PendulumTest, DISABLED_AccurateThroughManyMorePeriodsWithCircularErrorAdjustmentLargeAngle) {
  PendulumConfigurator<AlgebraType> config{};
  config.set_theta(LARGE_ANGLE);
  auto p{config.create()};

  EXPECT_TRUE(IsAccurateAtHalfPeriod(p, MANY_MORE_PERIODS, LARGE_ANGLE));
}

}  // namespace ndyn::simulation
