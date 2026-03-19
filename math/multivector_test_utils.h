#pragma once

#include <cmath>
#include <string>

#include "gtest/gtest.h"
#include "math/basis_representation.h"
#include "math/generic_basis_representation.h"
#include "math/multivector.h"

namespace ndyn::math {

template <typename Algebra,  //
          BasisRepresentation<Algebra> Representation = math::GenericBasisRepresentation<Algebra> >
::testing::AssertionResult AreNear(const typename Algebra::VectorType& lhs,  //
                                   const typename Algebra::VectorType& rhs,  //
                                   typename Algebra::ScalarType epsilon = 0.0001) {
  using std::abs;
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

}  // namespace ndyn::math
