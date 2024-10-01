#include "math/convert.h"

#include "base/pi.h"
#include "glog/logging.h"
#include "gtest/gtest.h"
#include "math/multivector.h"
#include "math/multivector_test_utils.h"

namespace ndyn::math {

TEST(ConvertCartesianPolarTest, TwoDimensionAllPositiveBases) {
  using std::sqrt;

  using ScalarType = float;
  using VectorType = Multivector<ScalarType, 2, 0, 0>;
  using IncomingUnits = math::CartesianMeters;
  using ResultUnits = math::PolarMeters;

  Convert<VectorType, IncomingUnits, ResultUnits> convert{};

  {
    const VectorType incoming{VectorType::template e<0>() + VectorType::template e<1>()};
    const VectorType expected{static_cast<ScalarType>(sqrt(2)) * VectorType::template e<0>() +
                              pi_v<ScalarType> / 4 * VectorType::template e<1>()};
    const VectorType result{convert(incoming)};
    EXPECT_TRUE(AreNear(expected, result, 0.0001)) << "First quadrant";
  }

  {
    const VectorType incoming{-VectorType::template e<0>() + VectorType::template e<1>()};
    const VectorType expected{static_cast<ScalarType>(sqrt(2)) * VectorType::template e<0>() +
                              3 * pi_v<ScalarType> / 4 * VectorType::template e<1>()};
    const VectorType result{convert(incoming)};
    EXPECT_TRUE(AreNear(expected, result, 0.0001)) << "Second quadrant";
  }

  {
    const VectorType incoming{-VectorType::template e<0>() - VectorType::template e<1>()};
    const VectorType expected{static_cast<ScalarType>(sqrt(2)) * VectorType::template e<0>() +
                              -3 * pi_v<ScalarType> / 4 * VectorType::template e<1>()};
    const VectorType result{convert(incoming)};
    EXPECT_TRUE(AreNear(expected, result, 0.0001)) << "Third quadrant";
  }

  {
    const VectorType incoming{VectorType::template e<0>() - VectorType::template e<1>()};
    const VectorType expected{static_cast<ScalarType>(sqrt(2)) * VectorType::template e<0>() -
                              pi_v<ScalarType> / 4 * VectorType::template e<1>()};
    const VectorType result{convert(incoming)};
    EXPECT_TRUE(AreNear(expected, result, 0.0001)) << "Fourth quadrant";
  }
}

TEST(ConvertCartesianPolarTest, ThreeDimensionAllPositiveBases) {
  using std::sqrt;

  using ScalarType = float;
  using VectorType = Multivector<ScalarType, 3, 0, 0>;
  using IncomingUnits = math::CartesianMeters;
  using ResultUnits = math::PolarMeters;

  Convert<VectorType, IncomingUnits, ResultUnits> convert{};

  {
    const VectorType incoming{VectorType::template e<0>() + VectorType::template e<1>() +
                              VectorType::template e<2>()};
    const VectorType expected{static_cast<ScalarType>(sqrt(2)) * VectorType::template e<0>() +
                              pi_v<ScalarType> / 4 * VectorType::template e<1>() +
                              VectorType::template e<2>()};
    const VectorType result{convert(incoming)};
    EXPECT_TRUE(AreNear(expected, result, 0.0001)) << "First quadrant";
  }

  {
    const VectorType incoming{-VectorType::template e<0>() + VectorType::template e<1>() +
                              VectorType::template e<2>()};
    const VectorType expected{static_cast<ScalarType>(sqrt(2)) * VectorType::template e<0>() +
                              3 * pi_v<ScalarType> / 4 * VectorType::template e<1>() +
                              VectorType::template e<2>()};
    const VectorType result{convert(incoming)};
    EXPECT_TRUE(AreNear(expected, result, 0.0001)) << "Second quadrant";
  }

  {
    const VectorType incoming{-VectorType::template e<0>() - VectorType::template e<1>() +
                              VectorType::template e<2>()};
    const VectorType expected{static_cast<ScalarType>(sqrt(2)) * VectorType::template e<0>() +
                              -3 * pi_v<ScalarType> / 4 * VectorType::template e<1>() +
                              VectorType::template e<2>()};
    const VectorType result{convert(incoming)};
    EXPECT_TRUE(AreNear(expected, result, 0.0001)) << "Third quadrant";
  }

  {
    const VectorType incoming{VectorType::template e<0>() - VectorType::template e<1>() +
                              VectorType::template e<2>()};
    const VectorType expected{static_cast<ScalarType>(sqrt(2)) * VectorType::template e<0>() -
                              pi_v<ScalarType> / 4 * VectorType::template e<1>() +
                              VectorType::template e<2>()};
    const VectorType result{convert(incoming)};
    EXPECT_TRUE(AreNear(expected, result, 0.0001)) << "Fourth quadrant";
  }
}

TEST(ConvertCartesianSphericalTest, TwoDimensionAllPositiveBases) {
  using std::sqrt;

  using ScalarType = float;
  using VectorType = Multivector<ScalarType, 2, 0, 0>;
  using IncomingUnits = math::CartesianMeters;
  using ResultUnits = math::SphericalMeters;

  Convert<VectorType, IncomingUnits, ResultUnits> convert{};

  {
    const VectorType incoming{VectorType::template e<0>() + VectorType::template e<1>()};
    const VectorType expected{static_cast<ScalarType>(sqrt(2)) * VectorType::template e<0>() +
                              pi_v<ScalarType> / 4 * VectorType::template e<1>()};
    const VectorType result{convert(incoming)};
    EXPECT_TRUE(AreNear(expected, result, 0.0001)) << "First quadrant";
  }

  {
    const VectorType incoming{-VectorType::template e<0>() + VectorType::template e<1>()};
    const VectorType expected{static_cast<ScalarType>(sqrt(2)) * VectorType::template e<0>() +
                              3 * pi_v<ScalarType> / 4 * VectorType::template e<1>()};
    const VectorType result{convert(incoming)};
    EXPECT_TRUE(AreNear(expected, result, 0.0001)) << "Second quadrant";
  }

  {
    const VectorType incoming{-VectorType::template e<0>() - VectorType::template e<1>()};
    const VectorType expected{static_cast<ScalarType>(sqrt(2)) * VectorType::template e<0>() +
                              -3 * pi_v<ScalarType> / 4 * VectorType::template e<1>()};
    const VectorType result{convert(incoming)};
    EXPECT_TRUE(AreNear(expected, result, 0.0001)) << "Third quadrant";
  }

  {
    const VectorType incoming{VectorType::template e<0>() - VectorType::template e<1>()};
    const VectorType expected{static_cast<ScalarType>(sqrt(2)) * VectorType::template e<0>() -
                              pi_v<ScalarType> / 4 * VectorType::template e<1>()};
    const VectorType result{convert(incoming)};
    EXPECT_TRUE(AreNear(expected, result, 0.0001)) << "Fourth quadrant";
  }
}

TEST(ConvertCartesianSphericalTest, ThreeDimensionAllPositiveBases) {
  using std::sqrt;

  using ScalarType = float;
  using VectorType = Multivector<ScalarType, 3, 0, 0>;
  using IncomingUnits = math::CartesianMeters;
  using ResultUnits = math::SphericalMeters;

  static constexpr ScalarType SQRT_2{sqrt(2)};

  Convert<VectorType, IncomingUnits, ResultUnits> convert{};

  {
    const VectorType incoming{VectorType::template e<0>() + VectorType::template e<1>() +
                              SQRT_2 * VectorType::template e<2>()};
    const VectorType expected{static_cast<ScalarType>(2) * VectorType::template e<0>() +
                              pi_v<ScalarType> / 4 * VectorType::template e<1>() +
                              pi_v<ScalarType> / 4 * VectorType::template e<2>()};
    const VectorType result{convert(incoming)};
    EXPECT_TRUE(AreNear(expected, result, 0.0001)) << "First quadrant";
  }

  {
    const VectorType incoming{-VectorType::template e<0>() + VectorType::template e<1>() +
                              SQRT_2 * VectorType::template e<2>()};
    const VectorType expected{static_cast<ScalarType>(2) * VectorType::template e<0>() +
                              pi_v<ScalarType> / 4 * VectorType::template e<1>() +
                              3 * pi_v<ScalarType> / 4 * VectorType::template e<2>()};
    const VectorType result{convert(incoming)};
    EXPECT_TRUE(AreNear(expected, result, 0.0001)) << "Second quadrant";
  }

  {
    const VectorType incoming{-VectorType::template e<0>() - VectorType::template e<1>() +
                              SQRT_2 * VectorType::template e<2>()};
    const VectorType expected{static_cast<ScalarType>(2) * VectorType::template e<0>() +
                              pi_v<ScalarType> / 4 * VectorType::template e<1>() -
                              3 * pi_v<ScalarType> / 4 * VectorType::template e<2>()};
    const VectorType result{convert(incoming)};
    EXPECT_TRUE(AreNear(expected, result, 0.0001)) << "Third quadrant";
  }

  {
    const VectorType incoming{VectorType::template e<0>() - VectorType::template e<1>() +
                              SQRT_2 * VectorType::template e<2>()};
    const VectorType expected{static_cast<ScalarType>(2) * VectorType::template e<0>() +
                              pi_v<ScalarType> / 4 * VectorType::template e<1>() -
                              pi_v<ScalarType> / 4 * VectorType::template e<2>()};
    const VectorType result{convert(incoming)};
    EXPECT_TRUE(AreNear(expected, result, 0.0001)) << "Fourth quadrant";
  }

  {
    const VectorType incoming{VectorType::template e<0>() + VectorType::template e<1>() -
                              SQRT_2 * VectorType::template e<2>()};
    const VectorType expected{static_cast<ScalarType>(2) * VectorType::template e<0>() +
                              3 * pi_v<ScalarType> / 4 * VectorType::template e<1>() +
                              pi_v<ScalarType> / 4 * VectorType::template e<2>()};
    const VectorType result{convert(incoming)};
    EXPECT_TRUE(AreNear(expected, result, 0.0001)) << "Fifth quadrant";
  }

  {
    const VectorType incoming{-VectorType::template e<0>() + VectorType::template e<1>() -
                              SQRT_2 * VectorType::template e<2>()};
    const VectorType expected{static_cast<ScalarType>(2) * VectorType::template e<0>() +
                              3 * pi_v<ScalarType> / 4 * VectorType::template e<1>() +
                              3 * pi_v<ScalarType> / 4 * VectorType::template e<2>()};
    const VectorType result{convert(incoming)};
    EXPECT_TRUE(AreNear(expected, result, 0.0001)) << "Sixth quadrant";
  }

  {
    const VectorType incoming{-VectorType::template e<0>() - VectorType::template e<1>() -
                              SQRT_2 * VectorType::template e<2>()};
    const VectorType expected{static_cast<ScalarType>(2) * VectorType::template e<0>() +
                              3 * pi_v<ScalarType> / 4 * VectorType::template e<1>() -
                              3 * pi_v<ScalarType> / 4 * VectorType::template e<2>()};
    const VectorType result{convert(incoming)};
    EXPECT_TRUE(AreNear(expected, result, 0.0001)) << "Seventh quadrant";
  }

  {
    const VectorType incoming{VectorType::template e<0>() - VectorType::template e<1>() -
                              SQRT_2 * VectorType::template e<2>()};
    const VectorType expected{static_cast<ScalarType>(2) * VectorType::template e<0>() +
                              3 * pi_v<ScalarType> / 4 * VectorType::template e<1>() -
                              pi_v<ScalarType> / 4 * VectorType::template e<2>()};
    const VectorType result{convert(incoming)};
    EXPECT_TRUE(AreNear(expected, result, 0.0001)) << "Eigth quadrant";
  }
}

}  // namespace ndyn::math
