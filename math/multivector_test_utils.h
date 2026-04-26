#pragma once

#include <cmath>
#include <string>

#include "gtest/gtest.h"
#include "math/abs.h"
#include "math/basis_representation.h"
#include "math/generic_basis_representation.h"
#include "math/multivector.h"

namespace ndyn::math {

template <typename Algebra,  //
          BasisRepresentation<Algebra> Representation = math::GenericBasisRepresentation<Algebra> >
::testing::AssertionResult AreNear(const typename Algebra::VectorType& lhs,  //
                                   const typename Algebra::VectorType& rhs,  //
                                   typename Algebra::ScalarType epsilon = Algebra::EPSILON) {
  using std::to_string;
  const auto difference{lhs - rhs};
  epsilon = abs(epsilon);
  for (size_t i = 0; i < Algebra::VectorType::NUM_BASIS_BLADES; ++i) {
    if (abs(difference.coefficient(i)) > epsilon) {
      return ::testing::AssertionFailure()
             << "lhs: " << Representation::to_string(lhs)
             << ", rhs: " << Representation::to_string(rhs)
             << ", difference: " << Representation::to_string(difference)
             << " (epsilon: " << to_string(epsilon) << ")";
    }
  }
  return ::testing::AssertionSuccess();
}

/**
 * Testing function to detect if two multivectors are the same to within a scale factor.
 *
 * This function is useful in conformal geometries where two multivectors that differ only by a
 * scale factor are equivalent for most purposes.
 */
template <typename Algebra,  //
          BasisRepresentation<Algebra> Representation = math::GenericBasisRepresentation<Algebra> >
::testing::AssertionResult AreScaled(const typename Algebra::VectorType& lhs,  //
                                     const typename Algebra::VectorType& rhs,  //
                                     typename Algebra::ScalarType epsilon = Algebra::EPSILON) {
  using std::to_string;
  using Scalar = Algebra::ScalarType;

  epsilon = abs(epsilon);
  Scalar scale{1};
  for (size_t i = 0; i < Algebra::VectorType::NUM_BASIS_BLADES; ++i) {
    const Scalar difference{abs(lhs.coefficient(i) - scale * rhs.coefficient(i))};
    if (difference > epsilon and abs(rhs.coefficient(i)) > epsilon and scale == Scalar{1}) {
      // Set a scale factor if we haven't done so before. The initial scale factor is 1. Any other
      // scale factor means that it has been set before.
      scale = lhs.coefficient(i) / rhs.coefficient(i);
    } else if (difference > epsilon) {
      // Once the scale factor is no longer unity, or if no scale factor could exist
      // (rhs.coefficient(i) is zero), any difference is a failure.
      return ::testing::AssertionFailure()
             << "lhs: " << Representation::to_string(lhs)
             << ", rhs: " << Representation::to_string(rhs) << ", scale: " << scale
             << " (epsilon: " << to_string(epsilon) << ")";
    }
  }
  return ::testing::AssertionSuccess();
}

}  // namespace ndyn::math
