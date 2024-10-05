#include "math/convert.h"

#include "base/pi.h"
#include "glog/logging.h"
#include "gtest/gtest.h"
#include "math/multivector.h"
#include "math/multivector_test_utils.h"
#include "math/state_test_utils.h"

namespace ndyn::math {

template <typename State1, typename State2>
::testing::AssertionResult CanRoundTripState(const State1 incoming) {
  static constexpr ConvertState<State1, State2> transform{};
  static constexpr ConvertState<State2, State1> inverse{};

  const State2 transform_1{transform(incoming)};
  const State1 transform_2{inverse(transform_1)};
  const State2 transform_3{transform(transform_2)};
  const State1 transform_4{inverse(transform_3)};

  {
    // Verifies that we can do a transform followed by an inverse and get the original state.
    ::testing::AssertionResult result{AreStatesNear(incoming, transform_2, 0.0001)};
    if (!result) {
      return result;
    }
  }

  {
    // Verifies that we can do the inverse transform followed by the forward transform and get back
    // the first state.
    ::testing::AssertionResult result{AreStatesNear(transform_1, transform_3, 0.0001)};
    if (!result) {
      return result;
    }
  }

  {
    // Possibly overkill, but checks for numerical stability in the transforms.
    ::testing::AssertionResult result{AreStatesNear(transform_2, transform_4, 0.0001)};
    if (!result) {
      return result;
    }
  }

  return ::testing::AssertionSuccess();
}

TEST(ConvertVectorCartesianSphericalTest, CanRoundTripStatesUnitsOnly) {
  using ScalarType = float;
  using VectorType = Multivector<ScalarType, 3, 0, 0>;
  using Units1 = CartesianMeters;
  using Units2 = CartesianFeet;

  {
    using State1 = State<VectorType, /* depth */ 1, Units1>;
    using State2 = State<VectorType, /* depth */ 1, Units2>;
    State1 state{static_cast<ScalarType>(2) * VectorType::e<0>() +
                 static_cast<ScalarType>(3) * VectorType::e<1>() +
                 static_cast<ScalarType>(5) * VectorType::e<2>()};

    auto result{CanRoundTripState<State1, State2>(state)};
    EXPECT_TRUE(result);
  }

  {
    using State1 = State<VectorType, /* depth */ 2, Units1>;
    using State2 = State<VectorType, /* depth */ 2, Units2>;
    State1 state{static_cast<ScalarType>(2) * VectorType::e<0>() +
                     static_cast<ScalarType>(3) * VectorType::e<1>() +
                     static_cast<ScalarType>(5) * VectorType::e<2>(),
                 static_cast<ScalarType>(1) * VectorType::e<0>() +
                     static_cast<ScalarType>(7) * VectorType::e<1>() +
                     static_cast<ScalarType>(11) * VectorType::e<2>()};

    auto result{CanRoundTripState<State1, State2>(state)};
    EXPECT_TRUE(result);
  }
}

}  // namespace ndyn::math
