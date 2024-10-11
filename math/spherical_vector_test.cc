#include "math/spherical_vector.h"

#include <cmath>
#include <vector>

#include "base/pi.h"
#include "gtest/gtest.h"
#include "math/coordinates.h"
#include "math/unit_set.h"
#include "math/vector_tests.h"
#include "units.h"

namespace ndyn::math {

template <size_t DIMENSIONS, typename ScalarType, typename Units,
          template <Coordinates, typename, size_t, typename> typename VectorType = Vector>
::testing::AssertionResult AreNear(
    const VectorType<Coordinates::SPHERICAL, ScalarType, DIMENSIONS, Units>& lhs,
    const VectorType<Coordinates::SPHERICAL, ScalarType, DIMENSIONS, Units>& rhs,
    typename VectorType<Coordinates::SPHERICAL, ScalarType, DIMENSIONS, Units>::ScalarType
        epsilon) {
  using std::abs;
  using std::remainder;
  using std::to_string;

  epsilon = abs(epsilon);

  if (abs(lhs.template element<0>() - rhs.template element<0>()) > epsilon) {
    return ::testing::AssertionFailure()
           << "Note: using spherical coordinate form of AreNear() -- "
           << "lhs: " << lhs << ", rhs: " << rhs << ", difference: " << (lhs - rhs)
           << " (epsilon: " << to_string(epsilon) << ")"
           << ", element index: " << 0;
  }

  // We only compare angle components if the radius is not zero. When the radius is zero, any angle
  // is equal to any other.
  if (abs(lhs.template element<0>()) < epsilon) {
    return ::testing::AssertionSuccess();
  }

  if (lhs.size() >= 3) {
    // For 3+ dimensional vectors, when theta is 0 or pi, the rest of the angles do not matter.
    if (abs(lhs.template element<1>()) < epsilon || abs(rhs.template element<1>()) < epsilon) {
      if (abs(lhs.template element<1>() - rhs.template element<1>()) > epsilon) {
        return ::testing::AssertionFailure()
               << "Note: using spherical coordinate form of AreNear() -- "
               << "lhs: " << lhs << ", rhs: " << rhs << ", difference: " << (lhs - rhs)
               << " (epsilon: " << to_string(epsilon) << ")"
               << ", element index: " << 1;
      } else {
        return ::testing::AssertionSuccess();
      }
    }

    // For 3+ dimensional vectors, when theta is 0 or pi, the rest of the angles do not matter.
    if (abs(lhs.template element<1>() - pi) < epsilon ||
        abs(rhs.template element<1>() - pi) < epsilon) {
      if (abs(lhs.template element<1>() - rhs.template element<1>()) > epsilon) {
        return ::testing::AssertionFailure()
               << "Note: using spherical coordinate form of AreNear() -- "
               << "lhs: " << lhs << ", rhs: " << rhs << ", difference: " << (lhs - rhs)
               << " (epsilon: " << to_string(epsilon) << ")"
               << ", element index: " << 1;
      } else {
        return ::testing::AssertionSuccess();
      }
    }
  }

  for (size_t i = 1; i < lhs.size(); ++i) {
    if (abs(remainder(lhs.element(i) - rhs.element(i), 2 * pi)) > epsilon) {
      return ::testing::AssertionFailure()
             << "Note: using spherical coordinate form of AreNear() -- "
             << "lhs: " << lhs << ", rhs: " << rhs << ", difference: " << (lhs - rhs)
             << " (epsilon: " << to_string(epsilon) << ")"
             << ", element index: " << i;
    }
  }

  return ::testing::AssertionSuccess();
}

template <size_t DIMENSIONS, typename ScalarType, typename Units,
          template <Coordinates, typename, size_t, typename> typename VectorType = Vector>
::testing::AssertionResult AreNear(
    const VectorType<Coordinates::SPHERICAL, ScalarType, DIMENSIONS, Units>& lhs,
    const VectorType<Coordinates::SPHERICAL, ScalarType, DIMENSIONS, Units>& rhs) {
  ScalarType lhs_mag = lhs.square_magnitude();
  ScalarType rhs_mag = rhs.square_magnitude();

  // Compute an epsilon that is based on the larger of the two magnitudes, but also has a minimum
  // value. The minimum value is especially useful if the magnitudes are zero.
  ScalarType epsilon = std::max(std::max(lhs_mag, rhs_mag) * 0.00001, 0.0001);

  return AreNear(lhs, rhs, epsilon);
}

class Spherical2DVectorTest : public VectorTest<Coordinates::SPHERICAL, 2> {
 public:
  void SetUp() override {
    using std::sqrt;

    additive_sets.emplace_back(AdditiveSet{{}, {}, {}});
    additive_sets.emplace_back(AdditiveSet{{1, pi / 2}, {1, pi}, {sqrt(2), 3 * pi / 4}});
    additive_sets.emplace_back(AdditiveSet{{1, pi / 2}, {0, 0}, {1, pi / 2}});
    additive_sets.emplace_back(AdditiveSet{{1, pi / 2}, {1, -pi}, {sqrt(2), 3 * pi / 4}});

    additive_sets.emplace_back(AdditiveSet{{1, pi / 2}, {1, -pi / 2}, {0, 0}});
    additive_sets.emplace_back(AdditiveSet{{1, pi / 4}, {1, -3 * pi / 4}, {}});

    additive_sets.emplace_back(AdditiveSet{{1, pi / 4}, {1, 3 * pi / 4}, {sqrt(2), pi / 2}});
    additive_sets.emplace_back(AdditiveSet{{1, pi / 4}, {1, -pi / 4}, {sqrt(2), 0}});
    additive_sets.emplace_back(AdditiveSet{{1, -pi / 4}, {1, -3 * pi / 4}, {sqrt(2), -pi / 2}});
    additive_sets.emplace_back(AdditiveSet{{1, 3 * pi / 4}, {1, -3 * pi / 4}, {sqrt(2), pi}});
  }
};

class Spherical3DVectorTest : public VectorTest<Coordinates::SPHERICAL, 3> {
 public:
  void SetUp() override {
    using std::sqrt;

    // Zero radii comparisons.
    equality_sets.emplace_back(EqualitySet{{}, {}});
    for (const ScalarType theta : {0., pi / 2, pi, 3 * pi / 2, 2 * pi}) {
      for (const ScalarType phi : {0., pi / 2, pi, 3 * pi / 2, 2 * pi}) {
        equality_sets.emplace_back(EqualitySet{{0, theta, phi}, {}});
        equality_sets.emplace_back(EqualitySet{{0, -theta, phi}, {}});
        equality_sets.emplace_back(EqualitySet{{0, theta, -phi}, {}});
        equality_sets.emplace_back(EqualitySet{{0, -theta, -phi}, {}});
      }
    }

    // Theta is on [0, pi]. Outside of that range, we consider theta traversing an angle on the
    // plane defined by the z-axis and the x-y hypotenuse and normalize the values accordingly.
    equality_sets.emplace_back(EqualitySet{{1, pi}, {1, -pi}});
    equality_sets.emplace_back(EqualitySet{{-1, pi / 2}, {1, pi / 2, pi}});
    equality_sets.emplace_back(EqualitySet{{1, 0, 0}, {1, 2 * pi, 0}});
    equality_sets.emplace_back(EqualitySet{{1, pi / 4, 0}, {1, 7 * pi / 4, pi}});

    equality_sets.emplace_back(EqualitySet{{1, -pi / 2}, {1, pi / 2, pi}});

    additive_sets.emplace_back(AdditiveSet{{}, {}, {}});
    additive_sets.emplace_back(
        AdditiveSet{{1, pi / 2, pi / 2}, {1, pi / 2, pi}, {sqrt(2), pi / 2, 3 * pi / 4}});

    additive_sets.emplace_back(
        AdditiveSet{{1, pi / 2, pi / 2}, {0, pi / 2, 0}, {1, pi / 2, pi / 2}});

    additive_sets.emplace_back(
        AdditiveSet{{1, pi / 2, pi / 2}, {1, pi / 2, -pi}, {sqrt(2), pi / 2, 3 * pi / 4}});

    additive_sets.emplace_back(AdditiveSet{{1, pi / 2}, {1, pi / 2, pi}, {}});

    additive_sets.emplace_back(AdditiveSet{{1, pi / 2}, {1, -pi / 2}, {}});

    additive_sets.emplace_back(AdditiveSet{{1, pi}, {1, -pi}, {2, pi}});

    additive_sets.emplace_back(AdditiveSet{{1, pi / 4}, {1, -3 * pi / 4}, {}});

    additive_sets.emplace_back(AdditiveSet{{1, pi / 4}, {1, 3 * pi / 4}, {sqrt(2), pi / 2}});
    additive_sets.emplace_back(AdditiveSet{{1, pi / 4}, {1, -pi / 4}, {sqrt(2), 0}});
    additive_sets.emplace_back(AdditiveSet{{1, -pi / 4}, {1, -3 * pi / 4}, {sqrt(2), -pi / 2}});
    additive_sets.emplace_back(AdditiveSet{{1, 3 * pi / 4}, {1, -3 * pi / 4}, {sqrt(2), pi}});
  }
};

TEST_F(Spherical2DVectorTest, RunAllTests) { RunAllTests(); }

TEST_F(Spherical3DVectorTest, RunAllTests) { RunAllTests(); }

}  // namespace ndyn::math
