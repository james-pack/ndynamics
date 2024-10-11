#pragma once

#include <cmath>
#include <vector>

#include "gtest/gtest.h"
#include "math/coordinates.h"
#include "math/unit_set.h"
#include "math/vector.h"
#include "units.h"

namespace ndyn::math {

template <Coordinates COORD, size_t DIM, typename ScalarT = float>
class VectorTest : public ::testing::Test {
 public:
  static constexpr Coordinates COORDINATES{COORD};
  static constexpr size_t DIMENSIONS{DIM};

  using ScalarType = ScalarT;
  using VectorType = Vector<COORDINATES, ScalarType, DIMENSIONS, UnitSet<units::length::meter_t>>;

  struct AdditiveSet {
    VectorType operand1;
    VectorType operand2;
    VectorType sum;
  };
  std::vector<AdditiveSet> additive_sets{};

  struct MultiplicativeSet {
    ScalarType scalar;
    VectorType operand;
    VectorType product;
  };
  std::vector<MultiplicativeSet> multiplicative_sets{};

  struct Magnitude {
    VectorType v;
    ScalarType square_magnitude;
  };
  std::vector<Magnitude> magnitudes{};

  struct InnerProduct {
    VectorType v1;
    VectorType v2;
    ScalarType product;
  };
  std::vector<InnerProduct> inner_products{};

  struct BasisDecomposition {
    VectorType v;
    VectorType axis;
    VectorType parallel;
    VectorType orthogonal;
  };
  std::vector<BasisDecomposition> basis_decompositions{};

  virtual ~VectorTest() = default;

  void CanListInitialize() {
    {
      VectorType foo{};
      EXPECT_GE(foo.size(), 0);
    }
    if constexpr (DIMENSIONS >= 1) {
      VectorType foo{1};
      EXPECT_GE(foo.size(), 1);
    }
    if constexpr (DIMENSIONS >= 2) {
      VectorType foo{1, 2};
      EXPECT_GE(foo.size(), 2);
    }
    if constexpr (DIMENSIONS >= 3) {
      VectorType foo{1, 2, 3};
      EXPECT_GE(foo.size(), 3);
    }
    if constexpr (DIMENSIONS >= 4) {
      VectorType foo{1, 2, 3, 4};
      EXPECT_GE(foo.size(), 4);
    }
  }

  void CanCompareVectors() {
    for (const auto& set : additive_sets) {
      if (set.operand1 != VectorType{}) {
        EXPECT_NE(set.operand2, set.sum);
      } else {
        EXPECT_EQ(set.operand2, set.sum);
      }
      if (set.operand2 != VectorType{}) {
        EXPECT_NE(set.operand1, set.sum);
      } else {
        EXPECT_EQ(set.operand1, set.sum);
      }
    }

    for (const auto& set : multiplicative_sets) {
      if (set.scalar != 1) {
        // If the product is the zero vector, operand could be anything, or alternatively, the
        // scalar could be anything. Don't even bother with the logic for those scenarios.
        if (set.product != VectorType{}) {
          EXPECT_NE(set.operand, set.product);
        }
      } else {
        EXPECT_EQ(set.operand, set.product);
      }
    }
  }

  void CanAccessWithRuntimeIndex() {
    // The goal of this test is to prove that the accessors work properly; the addition test is only
    // the mechanism.
    for (const auto& set : additive_sets) {
      bool equal{true};
      for (size_t i = 0; i < VectorType::size(); ++i) {
        if (set.operand2.element(i) != set.sum.element(i)) {
          equal = false;
          break;
        }
      }
      // operand2 and sum should be equal iff operand1 is the zero vector.
      EXPECT_EQ(set.operand1 == VectorType{}, equal);
    }
  }

  void CanAccessWithCompileTimeIndex() {
    // The goal of this test is to prove that the accessors work properly; the addition test is only
    // the mechanism.
    for (const auto& set : additive_sets) {
      bool equal{true};
      if constexpr (DIMENSIONS >= 1) {
        if (set.operand2.template element<0>() != set.sum.template element<0>()) {
          equal = false;
        }
      }
      if constexpr (DIMENSIONS >= 2) {
        if (set.operand2.template element<1>() != set.sum.template element<1>()) {
          equal = false;
        }
      }
      if constexpr (DIMENSIONS >= 3) {
        if (set.operand2.template element<2>() != set.sum.template element<2>()) {
          equal = false;
        }
      }
      if constexpr (DIMENSIONS >= 4) {
        if (set.operand2.template element<3>() != set.sum.template element<3>()) {
          equal = false;
        }
      }

      // operand2 and sum should be equal iff operand1 is the zero vector.
      EXPECT_EQ(set.operand1 == VectorType{}, equal);
    }
  }

  void CanMultiplyByScalar() {
    for (const auto& set : multiplicative_sets) {
      // Multiplication by a scalar should be commutative, so verify both combinations.
      {
        // Scalar times vector.
        const VectorType result{set.scalar * set.operand};
        EXPECT_EQ(set.product, result);
      }
      {
        // Vector times scalar.
        const VectorType result{set.operand * set.scalar};
        EXPECT_EQ(set.product, result);
      }
      // And verify the inverse relationship. Again, both combinations.
      if (set.scalar != 0) {
        // Scalar times vector.
        const VectorType result{(1 / set.scalar) * set.product};
        EXPECT_EQ(set.operand, result);
      }
      if (set.scalar != 0) {
        // Vector times scalar.
        const VectorType result{set.product * (1 / set.scalar)};
        EXPECT_EQ(set.operand, result);
      }
    }
  }

  void CanDivideByScalar() {
    for (const auto& set : multiplicative_sets) {
      if (set.scalar != 0) {
        const VectorType result{set.product / set.scalar};
        EXPECT_EQ(set.operand, result);
      }
      // And verify the inverse relationship.
      if (set.scalar != 0) {
        const VectorType result{set.operand / (1 / set.scalar)};
        EXPECT_EQ(set.product, result);
      }
    }
  }

  void CanAddVectors() {
    for (const auto& set : additive_sets) {
      // Vector addition should be commutative, so verify both combinations.
      {
        const VectorType result{set.operand1 + set.operand2};
        EXPECT_EQ(set.sum, result);
      }
      {
        const VectorType result{set.operand2 + set.operand1};
        EXPECT_EQ(set.sum, result);
      }
    }
  }

  void CanSubtractVectors() {
    for (const auto& set : additive_sets) {
      {
        const VectorType result{set.sum - set.operand2};
        EXPECT_EQ(set.operand1, result);
      }
      {
        const VectorType result{set.sum - set.operand1};
        EXPECT_EQ(set.operand2, result);
      }
    }
  }

  void CanComputeMagnitude() {
    using std::sqrt;
    for (const auto& set : magnitudes) {
      {
        const ScalarType result{set.v.square_magnitude()};
        EXPECT_EQ(set.square_magnitude, result);
      }
      {
        const ScalarType result{set.v.abs()};
        EXPECT_EQ(sqrt(set.square_magnitude), result);
      }
    }
  }

  void CanComputeInnerProductOnSelf() {
    using std::sqrt;
    for (const auto& set : magnitudes) {
      {
        const ScalarType result{set.v.inner(set.v)};
        EXPECT_EQ(set.square_magnitude, result);
      }
    }
  }

  void CanComputeInnerProduct() {
    using std::sqrt;
    for (const auto& set : inner_products) {
      // The inner product should be commutative, so we test both combinations.
      {
        const ScalarType result{set.v1.inner(set.v2)};
        EXPECT_EQ(set.product, result);
      }
      {
        const ScalarType result{set.v2.inner(set.v1)};
        EXPECT_EQ(set.product, result);
      }
    }
  }

  void CanComputeInnerProductOnOrthogonalVectors() {
    using std::sqrt;
    for (const auto& set : basis_decompositions) {
      {
        const ScalarType result{set.parallel.inner(set.orthogonal)};
        EXPECT_EQ(0, result);
      }
      {
        const ScalarType result{set.axis.inner(set.orthogonal)};
        EXPECT_EQ(0, result);
      }
    }
  }

  void CanComputeParallelDecomposition() {
    using std::sqrt;
    for (const auto& set : basis_decompositions) {
      const VectorType result{set.v.parallel(set.axis)};
      EXPECT_EQ(set.parallel, result);
    }
  }

  void CanComputeOrthogonalDecomposition() {
    using std::sqrt;
    for (const auto& set : basis_decompositions) {
      const VectorType result{set.v.orthogonal(set.axis)};
      EXPECT_EQ(set.orthogonal, result);
    }
  }

  void RunAllTests() {
    CanListInitialize();
    CanCompareVectors();

    CanAccessWithRuntimeIndex();
    CanAccessWithCompileTimeIndex();

    CanMultiplyByScalar();
    CanDivideByScalar();

    CanAddVectors();
    CanSubtractVectors();

    CanComputeMagnitude();
    CanComputeInnerProductOnSelf();
    CanComputeInnerProduct();
    CanComputeInnerProductOnOrthogonalVectors();

    CanComputeParallelDecomposition();
    CanComputeOrthogonalDecomposition();
  }
};

}  // namespace ndyn::math
