#include "math/multivector.h"

#include <cmath>

#include "glog/logging.h"
#include "gtest/gtest.h"
#include "math/basis_representation.h"
#include "math/multivector_test_utils.h"

namespace ndyn::math {

template <typename MultivectorT>
class MultivectorTest : public ::testing::Test {};

using MultivectorTypes =
    ::testing::Types<Vga2dMultivector<float>, VgaMultivector<float>, Pga2dMultivector<float>,
                     PgaMultivector<float>, SpacetimeMultivector<float>>;

TYPED_TEST_SUITE(MultivectorTest, MultivectorTypes);

//--------------------------------------------------------------------------------------------------
// Construction & Representation
//--------------------------------------------------------------------------------------------------

TYPED_TEST(MultivectorTest, DefaultConstructionYieldsZeroMultivector) {
  static constexpr TypeParam zero{};
  static constexpr typename TypeParam::ScalarType ZERO{0};

  for (size_t i = 0; i < TypeParam::NUM_BASIS_BLADES; ++i) {
    EXPECT_EQ(zero.coefficient(i), ZERO);
  }
}

TYPED_TEST(MultivectorTest, CanConstructFromScalar) {
  static constexpr typename TypeParam::ScalarType SCALAR{3};
  static constexpr typename TypeParam::ScalarType ZERO{0};
  static constexpr TypeParam mv{SCALAR};

  EXPECT_EQ(mv.scalar(), SCALAR);
  for (size_t i = 1; i < TypeParam::NUM_BASIS_BLADES; ++i) {
    EXPECT_EQ(mv.coefficient(i), ZERO);
  }
}

TYPED_TEST(MultivectorTest, CanConstructFromBasisVector) {
  static constexpr typename TypeParam::ScalarType ZERO{0};
  static constexpr TypeParam e0{TypeParam::template e<0>()};

  // The coefficient at the bit-index for e0 (index 1) must be 1; all others zero.
  EXPECT_EQ(e0.coefficient(1), typename TypeParam::ScalarType{1});
  for (size_t i = 0; i < TypeParam::NUM_BASIS_BLADES; ++i) {
    if (i != 1) {
      EXPECT_EQ(e0.coefficient(i), ZERO);
    }
  }
}

TYPED_TEST(MultivectorTest, CanConstructFromBasisBivector) {
  static constexpr typename TypeParam::ScalarType ZERO{0};

  // The bivector e0^e1 lives at bit-index 3 (bits 0 and 1 set).
  static constexpr TypeParam e01{TypeParam::template e<0>().outer(TypeParam::template e<1>())};

  EXPECT_EQ(e01.coefficient(3), typename TypeParam::ScalarType{1});
  for (size_t i = 0; i < TypeParam::NUM_BASIS_BLADES; ++i) {
    if (i != 3) {
      EXPECT_EQ(e01.coefficient(i), ZERO);
    }
  }
}

TYPED_TEST(MultivectorTest, CanConstructFromFullGradedComponents) {
  // Build a multivector via initializer list and verify every coefficient is
  // stored in the expected position.
  typename TypeParam::ScalarType expected[TypeParam::NUM_BASIS_BLADES];
  for (size_t i = 0; i < TypeParam::NUM_BASIS_BLADES; ++i) {
    expected[i] = static_cast<typename TypeParam::ScalarType>(i + 1);
  }

  TypeParam mv{};
  for (size_t i = 0; i < TypeParam::NUM_BASIS_BLADES; ++i) {
    mv.set_coefficient(i, expected[i]);
  }

  for (size_t i = 0; i < TypeParam::NUM_BASIS_BLADES; ++i) {
    EXPECT_EQ(mv.coefficient(i), expected[i]);
  }
}

TYPED_TEST(MultivectorTest, CopyConstructionProducesEqualMultivector) {
  static constexpr TypeParam original{TypeParam::template e<0>()};
  static constexpr TypeParam copy{original};

  EXPECT_EQ(original, copy);
}

TYPED_TEST(MultivectorTest, MoveConstructionTransfersComponents) {
  TypeParam source{TypeParam::template e<0>()};
  const TypeParam expected{source};
  const TypeParam moved{std::move(source)};

  EXPECT_EQ(moved, expected);
}

TYPED_TEST(MultivectorTest, GradeExtractionReturnsCorrectKVector) {
  static constexpr TypeParam e0{TypeParam::template e<0>()};
  static constexpr TypeParam e1{TypeParam::template e<1>()};
  static constexpr TypeParam mixed{e0.add(e0.outer(e1))};

  // The grade-1 projection must equal e0; the grade-2 projection must equal e0^e1.
  EXPECT_EQ(mixed.grade_projection(1), e0);
  EXPECT_EQ(mixed.grade_projection(2), e0.outer(e1));

  // The grade-0 projection of a purely vector+bivector element must be zero.
  EXPECT_EQ(mixed.grade_projection(0), TypeParam{});
}

TYPED_TEST(MultivectorTest, ZeroMultivectorHasNoComponents) {
  static constexpr TypeParam zero{};
  static constexpr typename TypeParam::ScalarType ZERO{0};

  for (size_t i = 0; i < TypeParam::NUM_BASIS_BLADES; ++i) {
    EXPECT_EQ(zero.coefficient(i), ZERO);
  }
}

//--------------------------------------------------------------------------------------------------
// Equality & Comparison
//--------------------------------------------------------------------------------------------------

TYPED_TEST(MultivectorTest, EqualityHoldsForIdenticalMultivectors) {
  static constexpr TypeParam a{TypeParam::template e<0>()};
  static constexpr TypeParam b{TypeParam::template e<0>()};

  EXPECT_EQ(a, b);
}

TYPED_TEST(MultivectorTest, InequalityDetectsDifferingComponents) {
  static constexpr TypeParam a{TypeParam::template e<0>()};
  static constexpr TypeParam b{TypeParam::template e<1>()};

  EXPECT_NE(a, b);
}

TYPED_TEST(MultivectorTest, EqualityIsReflexive) {
  static constexpr TypeParam a{TypeParam::template e<0>()};

  EXPECT_EQ(a, a);
}

TYPED_TEST(MultivectorTest, EqualityIsSymmetric) {
  static constexpr TypeParam a{TypeParam::template e<0>()};
  static constexpr TypeParam b{TypeParam::template e<0>()};

  EXPECT_EQ(a, b);
  EXPECT_EQ(b, a);
}

TYPED_TEST(MultivectorTest, EqualityIsTransitive) {
  static constexpr TypeParam a{TypeParam::template e<0>()};
  static constexpr TypeParam b{TypeParam::template e<0>()};
  static constexpr TypeParam c{TypeParam::template e<0>()};

  EXPECT_EQ(a, b);
  EXPECT_EQ(b, c);
  EXPECT_EQ(a, c);
}

TYPED_TEST(MultivectorTest, NearEqualityRespectsEpsilonTolerance) {
  // Two multivectors differing by less than EPSILON in every component must
  // compare as nearly equal; one differing by more must not.
  using ScalarType = typename TypeParam::ScalarType;
  static constexpr ScalarType EPSILON{TypeParam::EPSILON};

  TypeParam a{TypeParam::template e<0>()};
  TypeParam b{TypeParam::template e<0>()};

  b.set_coefficient(1, b.coefficient(1) + EPSILON / ScalarType{2});
  EXPECT_TRUE(a.near_equal(b));

  b.set_coefficient(1, a.coefficient(1) + EPSILON * ScalarType{2});
  EXPECT_FALSE(a.near_equal(b));
}

//--------------------------------------------------------------------------------------------------
// Addition & Subtraction
//--------------------------------------------------------------------------------------------------

TYPED_TEST(MultivectorTest, AdditionOfMultivectorsIsCommutative) {
  static constexpr TypeParam a{TypeParam::template e<0>()};
  static constexpr TypeParam b{TypeParam::template e<1>()};

  EXPECT_EQ(a.add(b), b.add(a));
}

TYPED_TEST(MultivectorTest, AdditionOfMultivectorsIsAssociative) {
  static constexpr TypeParam a{TypeParam::template e<0>()};
  static constexpr TypeParam b{TypeParam::template e<1>()};
  static constexpr TypeParam c{TypeParam::pseudoscalar()};

  EXPECT_EQ(a.add(b).add(c), a.add(b.add(c)));
}

TYPED_TEST(MultivectorTest, AddingZeroMultivectorIsIdentity) {
  static constexpr TypeParam a{TypeParam::template e<0>()};
  static constexpr TypeParam zero{};

  EXPECT_EQ(a.add(zero), a);
}

TYPED_TEST(MultivectorTest, SubtractionIsAnticommutative) {
  static constexpr TypeParam a{TypeParam::template e<0>()};
  static constexpr TypeParam b{TypeParam::template e<1>()};

  EXPECT_EQ(a.subtract(b), -b.subtract(a));
}

TYPED_TEST(MultivectorTest, AddingNegationYieldsZero) {
  static constexpr TypeParam a{TypeParam::template e<0>()};
  static constexpr TypeParam zero{};

  EXPECT_EQ(a.add(-a), zero);
}

TYPED_TEST(MultivectorTest, NegationInvertsAllComponents) {
  static constexpr TypeParam a{TypeParam::template e<0>()};
  static constexpr TypeParam neg_a{-a};

  for (size_t i = 0; i < TypeParam::NUM_BASIS_BLADES; ++i) {
    EXPECT_EQ(neg_a.coefficient(i), -a.coefficient(i));
  }
}

//--------------------------------------------------------------------------------------------------
// Scalar Multiplication
//--------------------------------------------------------------------------------------------------

TYPED_TEST(MultivectorTest, ScalarMultiplicationDistributesOverAddition) {
  using ScalarType = typename TypeParam::ScalarType;
  static constexpr ScalarType s{3};
  static constexpr TypeParam a{TypeParam::template e<0>()};
  static constexpr TypeParam b{TypeParam::template e<1>()};

  EXPECT_EQ(a.add(b).multiply(s), a.multiply(s).add(b.multiply(s)));
}

TYPED_TEST(MultivectorTest, MultiplicationByOneIsIdentity) {
  static constexpr TypeParam a{TypeParam::template e<0>()};

  EXPECT_EQ(a.multiply(typename TypeParam::ScalarType{1}), a);
}

TYPED_TEST(MultivectorTest, MultiplicationByZeroYieldsZero) {
  static constexpr TypeParam a{TypeParam::template e<0>()};
  static constexpr TypeParam zero{};

  EXPECT_EQ(a.multiply(typename TypeParam::ScalarType{0}), zero);
}

TYPED_TEST(MultivectorTest, MultiplicationByNegativeOneIsNegation) {
  static constexpr TypeParam a{TypeParam::template e<0>()};

  EXPECT_EQ(a.multiply(typename TypeParam::ScalarType{-1}), -a);
}

TYPED_TEST(MultivectorTest, ScalarMultiplicationIsAssociativeWithScalars) {
  using ScalarType = typename TypeParam::ScalarType;
  static constexpr ScalarType s{3};
  static constexpr ScalarType t{5};
  static constexpr TypeParam a{TypeParam::template e<0>()};

  EXPECT_EQ(a.multiply(s).multiply(t), a.multiply(s * t));
}

//--------------------------------------------------------------------------------------------------
// Geometric Product
//--------------------------------------------------------------------------------------------------

TYPED_TEST(MultivectorTest, GeometricProductOfBasisVectorWithItselfYieldsMetricScalar) {
  // e_i * e_i must equal the metric value for that basis vector, which lands
  // entirely in the scalar component. The value of that scalar is algebra-defined
  // and extracted directly from the result rather than assumed.
  static constexpr TypeParam e0{TypeParam::template e<0>()};
  static constexpr TypeParam product{e0.multiply(e0)};

  // All non-scalar components must be zero.
  for (size_t i = 1; i < TypeParam::NUM_BASIS_BLADES; ++i) {
    EXPECT_EQ(product.coefficient(i), typename TypeParam::ScalarType{0});
  }
}

TYPED_TEST(MultivectorTest, GeometricProductIsAssociative) {
  static constexpr TypeParam a{TypeParam::template e<0>()};
  static constexpr TypeParam b{TypeParam::template e<1>()};
  static constexpr TypeParam c{TypeParam::pseudoscalar()};

  EXPECT_EQ(a.multiply(b).multiply(c), a.multiply(b.multiply(c)));
}

TYPED_TEST(MultivectorTest, GeometricProductIsDistributiveOverAddition) {
  static constexpr TypeParam a{TypeParam::template e<0>()};
  static constexpr TypeParam b{TypeParam::template e<1>()};
  static constexpr TypeParam c{TypeParam::pseudoscalar()};

  EXPECT_EQ(a.multiply(b.add(c)), a.multiply(b).add(a.multiply(c)));
}

TYPED_TEST(MultivectorTest, GeometricProductWithScalarScalesAllComponents) {
  using ScalarType = typename TypeParam::ScalarType;
  static constexpr ScalarType s{5};
  static constexpr TypeParam a{TypeParam::template e<0>()};

  static constexpr TypeParam via_scalar_multiply{a.multiply(s)};
  static constexpr TypeParam scalar_mv{TypeParam{}.add(s)};
  static constexpr TypeParam via_geometric{a.multiply(scalar_mv)};

  EXPECT_EQ(via_scalar_multiply, via_geometric);
}

TYPED_TEST(MultivectorTest, GeometricProductOfOrthogonalBasisVectorsIsPureBivector) {
  // e0 * e1 must be a pure grade-2 element; no grade-0 or grade-1 components.
  static constexpr TypeParam e0{TypeParam::template e<0>()};
  static constexpr TypeParam e1{TypeParam::template e<1>()};
  static constexpr TypeParam product{e0.multiply(e1)};

  EXPECT_EQ(product.grade_projection(0), TypeParam{});
  EXPECT_EQ(product.grade_projection(1), TypeParam{});
  EXPECT_EQ(product.grade_projection(2), product);
}

TYPED_TEST(MultivectorTest, GeometricProductOfBasisVectorsAnticommutes) {
  // e_i * e_j = -(e_j * e_i) for i != j.
  static constexpr TypeParam e0{TypeParam::template e<0>()};
  static constexpr TypeParam e1{TypeParam::template e<1>()};

  EXPECT_EQ(e0.multiply(e1), -e1.multiply(e0));
}

TYPED_TEST(MultivectorTest, GeometricProductIsNoncommutativeInGeneral) {
  static constexpr TypeParam e0{TypeParam::template e<0>()};
  static constexpr TypeParam e1{TypeParam::template e<1>()};

  EXPECT_NE(e0.multiply(e1), e1.multiply(e0));
}

TYPED_TEST(MultivectorTest, SquareOfBivectorYieldsGradeZeroAndGradeFourComponents) {
  if constexpr (TypeParam::NUM_GRADES > 3) {
    // (e0^e1)^2 must have no grade-1, grade-2, or grade-3 components; the result
    // is constrained to grades 0 and 4 (and higher even grades in larger algebras).
    static constexpr TypeParam e01{TypeParam::template e<0>().outer(TypeParam::template e<1>())};
    static constexpr TypeParam product{e01.multiply(e01)};

    EXPECT_EQ(product.template grade_projection<1>(), TypeParam{});
    EXPECT_EQ(product.template grade_projection<2>(), TypeParam{});
    EXPECT_EQ(product.template grade_projection<3>(), TypeParam{});
  } else {
    GTEST_SKIP() << "Test skipped. Only valid for algebras that use more than 3 grades.";
  }
}

//--------------------------------------------------------------------------------------------------
// Outer (Wedge) Product
//--------------------------------------------------------------------------------------------------

TYPED_TEST(MultivectorTest, OuterProductOfVectorWithItselfIsZero) {
  static constexpr TypeParam e0{TypeParam::template e<0>()};
  static constexpr TypeParam zero{};

  EXPECT_EQ(e0.outer(e0), zero);
}

TYPED_TEST(MultivectorTest, OuterProductIsAnticommutative) {
  static constexpr TypeParam e0{TypeParam::template e<0>()};
  static constexpr TypeParam e1{TypeParam::template e<1>()};

  EXPECT_EQ(e0.outer(e1), -e1.outer(e0));
}

TYPED_TEST(MultivectorTest, OuterProductIsAssociative) {
  if constexpr (TypeParam::NUM_BASIS_VECTORS > 2) {
    static constexpr TypeParam e0{TypeParam::template e<0>()};
    static constexpr TypeParam e1{TypeParam::template e<1>()};
    static constexpr TypeParam e2{TypeParam::template e<2>()};

    EXPECT_EQ(e0.outer(e1).outer(e2), e0.outer(e1.outer(e2)));
  } else {
    static constexpr TypeParam e0{TypeParam::template e<0>()};
    static constexpr TypeParam e1{TypeParam::template e<1>()};

    // Use a linear combination as the third operand so that the triple outer
    // product is nontrivial without requiring a third basis vector.
    static constexpr TypeParam a{e0};
    static constexpr TypeParam b{e1};
    static constexpr TypeParam c{e0.add(e1)};

    EXPECT_EQ(a.outer(b).outer(c), a.outer(b.outer(c)));
  }
}

TYPED_TEST(MultivectorTest, OuterProductDistributesOverAddition) {
  static constexpr TypeParam e0{TypeParam::template e<0>()};
  static constexpr TypeParam e1{TypeParam::template e<1>()};

  static constexpr TypeParam a{e0};
  static constexpr TypeParam b{e1};
  static constexpr TypeParam c{e0.add(e1)};

  EXPECT_EQ(a.outer(b.add(c)), a.outer(b).add(a.outer(c)));
}

TYPED_TEST(MultivectorTest, OuterProductGradeIsAdditive) {
  if constexpr (TypeParam::NUM_BASIS_VECTORS > 2) {
    static constexpr TypeParam e0{TypeParam::template e<0>()};
    static constexpr TypeParam e1{TypeParam::template e<1>()};
    static constexpr TypeParam e2{TypeParam::template e<2>()};

    static constexpr TypeParam trivector{e0.outer(e1).outer(e2)};

    // e0^e1^e2 must be purely grade 3.
    EXPECT_EQ(trivector.grade_projection(3), trivector);
    EXPECT_EQ(trivector.grade_projection(0), TypeParam{});
    EXPECT_EQ(trivector.grade_projection(1), TypeParam{});
    EXPECT_EQ(trivector.grade_projection(2), TypeParam{});
  } else {
    GTEST_SKIP() << "Test skipped. Only valid for algebras with 3 or more basis vectors.";
  }
}

TYPED_TEST(MultivectorTest, OuterProductOfLinearlyDependentVectorsIsZero) {
  using ScalarType = typename TypeParam::ScalarType;
  static constexpr TypeParam e0{TypeParam::template e<0>()};
  static constexpr TypeParam scaled{e0.multiply(ScalarType{3})};
  static constexpr TypeParam zero{};

  EXPECT_EQ(e0.outer(scaled), zero);
}

TYPED_TEST(MultivectorTest, OuterProductOfBasisVectorsYieldsExpectedBasisBlade) {
  // e0^e1 must land at bit-index 3 (bits 0 and 1 set) with coefficient 1.
  static constexpr TypeParam e0{TypeParam::template e<0>()};
  static constexpr TypeParam e1{TypeParam::template e<1>()};
  static constexpr TypeParam e01{e0.outer(e1)};

  EXPECT_EQ(e01.coefficient(3), typename TypeParam::ScalarType{1});
  for (size_t i = 0; i < TypeParam::NUM_BASIS_BLADES; ++i) {
    if (i != 3) {
      EXPECT_EQ(e01.coefficient(i), typename TypeParam::ScalarType{0});
    }
  }
}

TYPED_TEST(MultivectorTest, OuterProductWithScalarScalesMultivector) {
  using ScalarType = typename TypeParam::ScalarType;
  static constexpr ScalarType s{7};
  static constexpr TypeParam e0{TypeParam::template e<0>()};
  static constexpr TypeParam scalar_mv{TypeParam{}.add(s)};

  EXPECT_EQ(scalar_mv.outer(e0), e0.multiply(s));
}

//--------------------------------------------------------------------------------------------------
// Inner / Contraction Products
//--------------------------------------------------------------------------------------------------

TYPED_TEST(MultivectorTest, LeftContractionReducesGradeByExpectedAmount) {
  // (e0) _| (e0^e1) must be grade (2-1) = 1.
  static constexpr TypeParam e0{TypeParam::template e<0>()};
  static constexpr TypeParam e1{TypeParam::template e<1>()};
  static constexpr TypeParam e01{e0.outer(e1)};
  static constexpr TypeParam result{e0.left_contraction(e01)};

  EXPECT_EQ(result.grade_projection(1), result);
}

TYPED_TEST(MultivectorTest, RightContractionReducesGradeByExpectedAmount) {
  // (e0^e1) |_ (e0) must be grade (2-1) = 1.
  static constexpr TypeParam e0{TypeParam::template e<0>()};
  static constexpr TypeParam e1{TypeParam::template e<1>()};
  static constexpr TypeParam e01{e0.outer(e1)};
  static constexpr TypeParam result{e01.right_contraction(e0)};

  EXPECT_EQ(result.grade_projection(1), result);
}

TYPED_TEST(MultivectorTest, ScalarProductOfOrthogonalBasisVectorsIsZero) {
  static constexpr TypeParam e0{TypeParam::template e<0>()};
  static constexpr TypeParam e1{TypeParam::template e<1>()};
  static constexpr TypeParam zero{};

  EXPECT_EQ(e0.left_contraction(e1), zero);
}

TYPED_TEST(MultivectorTest, ScalarProductOfBasisVectorWithItselfEqualsMetricValue) {
  // e_i _| e_i must be a pure scalar equal to the metric value of that basis;
  // the value is algebra-defined and extracted from the result rather than assumed.
  static constexpr TypeParam e0{TypeParam::template e<0>()};
  static constexpr TypeParam result{e0.left_contraction(e0)};

  for (size_t i = 1; i < TypeParam::NUM_BASIS_BLADES; ++i) {
    EXPECT_EQ(result.coefficient(i), typename TypeParam::ScalarType{0});
  }
}

TYPED_TEST(MultivectorTest, RightContractionIsReverseOfLeftContractionWithReversedArguments) {
  // For any two multivectors X and Y:
  //   X |_ Y = ~(~Y _| ~X)
  // This invariant expresses the left/right contraction duality through the
  // reverse operation.
  static constexpr TypeParam zero{};
  static constexpr TypeParam one{1, 0, 0, 0};
  static constexpr TypeParam e0{TypeParam::template e<0>()};
  static constexpr TypeParam e1{TypeParam::template e<1>()};
  static constexpr TypeParam e01{e0.outer(e1)};

  static constexpr std::array<TypeParam, 4> blades{one, e0, e1, e01};

  for (const auto& x : blades) {
    for (const auto& y : blades) {
      EXPECT_EQ(x.right_contraction(y), y.reverse().left_contraction(x.reverse()).reverse())
          << "x: " << x << ", y: " << y;
    }
  }
}

TYPED_TEST(MultivectorTest, HestenesDotProductSymmetrizesContraction) {
  // For two vectors a and b: a _| b == b _| a (both yield the same scalar).
  static constexpr TypeParam e0{TypeParam::template e<0>()};
  static constexpr TypeParam e1{TypeParam::template e<1>()};

  EXPECT_EQ(e0.left_contraction(e1), e1.left_contraction(e0));
}

TYPED_TEST(MultivectorTest, LeftContractionWithScalarScalesMultivector) {
  using ScalarType = typename TypeParam::ScalarType;
  static constexpr ScalarType s{4};
  static constexpr TypeParam e0{TypeParam::template e<0>()};
  static constexpr TypeParam scalar_mv{TypeParam{}.add(s)};

  EXPECT_EQ(scalar_mv.left_contraction(e0), e0.multiply(s));
}

TYPED_TEST(MultivectorTest, ContractionGradeObeysFatDotConvention) {
  // For grades r and s, the left contraction result has grade |s - r| when r <= s,
  // and is zero when r > s.
  static constexpr TypeParam e0{TypeParam::template e<0>()};
  static constexpr TypeParam e1{TypeParam::template e<1>()};
  static constexpr TypeParam e01{e0.outer(e1)};
  static constexpr TypeParam zero{};

  // Grade 2 _| grade 1 must be zero (r > s).
  EXPECT_EQ(e01.left_contraction(e0), zero);
}

//--------------------------------------------------------------------------------------------------
// Grade Operations
//--------------------------------------------------------------------------------------------------

TYPED_TEST(MultivectorTest, GradeInvolutionNegatesOddGrades) {
  static constexpr TypeParam e0{TypeParam::template e<0>()};

  // Grade-1 element negated under grade involution.
  const TypeParam involution{e0.involute()};
  EXPECT_EQ(involution, -e0);
}

TYPED_TEST(MultivectorTest, GradeInvolutionPreservesEvenGrades) {
  static constexpr TypeParam e0{TypeParam::template e<0>()};
  static constexpr TypeParam e1{TypeParam::template e<1>()};
  static constexpr TypeParam e01{e0.outer(e1)};

  const TypeParam involution{e01.involute()};
  EXPECT_EQ(involution, e01);
}

TYPED_TEST(MultivectorTest, ReverseFlipsSignOfBivectorsAndTrivectors) {
  static constexpr TypeParam e0{TypeParam::template e<0>()};
  static constexpr TypeParam e1{TypeParam::template e<1>()};
  static constexpr TypeParam e01{e0.outer(e1)};

  EXPECT_EQ(e01.reverse(), -e01);
  EXPECT_EQ(e0.reverse(), e0);
  EXPECT_EQ(e1.reverse(), e1);
}

TYPED_TEST(MultivectorTest, ReverseOfReverseIsIdentity) {
  static constexpr TypeParam e0{TypeParam::template e<0>()};
  static constexpr TypeParam e1{TypeParam::template e<1>()};
  static constexpr TypeParam mv{e0.add(e0.outer(e1))};

  EXPECT_EQ(mv.reverse().reverse(), mv);
}

TYPED_TEST(MultivectorTest, CliffordConjugateComposesReverseAndGradeInvolution) {
  static constexpr TypeParam e0{TypeParam::template e<0>()};
  static constexpr TypeParam e1{TypeParam::template e<1>()};
  static constexpr TypeParam mv{e0.add(e0.outer(e1))};

  // The Clifford conjugate is grade-involution followed by reverse (or equivalently
  // reverse followed by grade-involution; the two commute).
  EXPECT_EQ(mv.conj(), mv.involute().reverse());
  EXPECT_EQ(mv.conj(), mv.reverse().involute());
}

TYPED_TEST(MultivectorTest, GradeProjectionOntoAbsentGradeYieldsZero) {
  static constexpr TypeParam e0{TypeParam::template e<0>()};

  // A pure grade-1 element projected onto grade 2 must be zero.
  EXPECT_EQ(e0.grade_projection(2), TypeParam{});
}

TYPED_TEST(MultivectorTest, SumOfAllGradeProjectionsReconstitutesOriginal) {
  static constexpr TypeParam e0{TypeParam::template e<0>()};
  static constexpr TypeParam e1{TypeParam::template e<1>()};
  static constexpr TypeParam mv{e0.add(e0.outer(e1))};

  TypeParam reconstructed{};
  for (size_t g = 0; g < TypeParam::NUM_GRADES; ++g) {
    reconstructed = reconstructed.add(mv.grade_projection(g));
  }
  EXPECT_EQ(reconstructed, mv);
}

//--------------------------------------------------------------------------------------------------
// Norms & Inverses
//--------------------------------------------------------------------------------------------------

TYPED_TEST(MultivectorTest, NormSquaredMatchesGeometricProductWithReverse) {
  static constexpr TypeParam e0{TypeParam::template e<0>()};
  static constexpr TypeParam e1{TypeParam::template e<1>()};
  static constexpr TypeParam mv{e0.add(e1)};

  // square_magnitude() must equal the scalar part of mv * ~mv.
  static constexpr TypeParam product{mv.multiply(mv.reverse())};

  EXPECT_EQ(mv.square_magnitude(), product.scalar());
}

TYPED_TEST(MultivectorTest, InverseOfInvertibleMultivectorSatisfiesMultiplicativeIdentity) {
  static constexpr TypeParam e0{TypeParam::template e<0>()};

  // This test is only meaningful when the basis vector is invertible (metric != 0).
  if (e0.multiply(e0).scalar() == typename TypeParam::ScalarType{0}) {
    GTEST_SKIP() << "Basis vector is null in this algebra; inverse is undefined.";
  }

  static constexpr TypeParam inv{e0.inverse()};
  static constexpr TypeParam identity{TypeParam{}.add(typename TypeParam::ScalarType{1})};

  EXPECT_EQ(e0.multiply(inv), identity);
}

TYPED_TEST(MultivectorTest, InverseOfScalarMatchesArithmeticReciprocal) {
  using ScalarType = typename TypeParam::ScalarType;
  static constexpr ScalarType s{4};
  static constexpr TypeParam scalar_mv{TypeParam{}.add(s)};
  static constexpr TypeParam inv{scalar_mv.inverse()};
  static constexpr TypeParam expected{TypeParam{}.add(ScalarType{1} / s)};

  EXPECT_EQ(inv, expected);
}

TYPED_TEST(MultivectorTest, NonInvertibleMultivectorThrowsOrIndicatesFailure) {
  // In degenerate algebras (e.g. PGA), null elements exist. In non-degenerate
  // algebras we construct a null-like element by combining a basis vector with
  // its negation, which always yields zero regardless of signature.
  static constexpr TypeParam e0{TypeParam::template e<0>()};
  static constexpr TypeParam null_mv{e0.subtract(e0)};

  // The inverse of zero must be zero (the sentinel value from inverse()).
  static constexpr TypeParam result{null_mv.inverse()};
  EXPECT_EQ(result, TypeParam{});
}

//--------------------------------------------------------------------------------------------------
// Pseudoscalars
//--------------------------------------------------------------------------------------------------

TYPED_TEST(MultivectorTest, InvertiblePseudoscalarHasAllNonzeroBases) {
  static constexpr TypeParam I{TypeParam::invertible_pseudoscalar()};
  TypeParam expected{1};

  for (size_t i = TypeParam::NUM_ZERO_BASES; i < TypeParam::NUM_BASIS_VECTORS; ++i) {
    expected = expected.outer(TypeParam::e(i));
  }

  EXPECT_EQ(expected, I);
}

TYPED_TEST(MultivectorTest, PseudoscalarHasExpectedGrade) {
  static constexpr TypeParam I{TypeParam::pseudoscalar()};
  static constexpr TypeParam ZERO{};

  EXPECT_EQ(I.grade_projection(TypeParam::NUM_BASIS_VECTORS), I);
  for (size_t g = 0; g < TypeParam::NUM_BASIS_VECTORS; ++g) {
    EXPECT_EQ(ZERO, I.grade_projection(g));
  }
}

TYPED_TEST(MultivectorTest, InvertiblePseudoscalarHasExpectedGrade) {
  static constexpr TypeParam I{TypeParam::invertible_pseudoscalar()};
  static constexpr TypeParam ZERO{};

  for (size_t g = 0; g < TypeParam::NUM_BASIS_VECTORS; ++g) {
    if (g == TypeParam::NUM_BASIS_VECTORS - TypeParam::NUM_ZERO_BASES) {
      EXPECT_EQ(I, I.grade_projection(g));
    } else {
      EXPECT_EQ(ZERO, I.grade_projection(g));
    }
  }
}

//--------------------------------------------------------------------------------------------------
// Duality
//--------------------------------------------------------------------------------------------------

TYPED_TEST(MultivectorTest, DualOfNonzeroBasesIsNotZero) {
  static constexpr TypeParam ZERO{};

  for (size_t i = TypeParam::NUM_ZERO_BASES; i < TypeParam::NUM_BASIS_VECTORS; ++i) {
    TypeParam e{TypeParam::e(i)};
    TypeParam d{e.dual()};
    EXPECT_NE(ZERO, d) << "d: " << d << ", e: " << e;
  }
}

TYPED_TEST(MultivectorTest, DualOfDualReturnsSimpleOriginalUpToSign) {
  static constexpr TypeParam ZERO{};
  static constexpr TypeParam e0{TypeParam::template e<TypeParam::NUM_ZERO_BASES>()};
  static constexpr TypeParam e1{TypeParam::template e<TypeParam::NUM_ZERO_BASES + 1>()};
  static constexpr TypeParam d0{e0.dual()};
  static constexpr TypeParam dd0{e0.dual().dual()};
  static constexpr TypeParam d1{e1.dual()};
  static constexpr TypeParam dd1{e1.dual().dual()};

  EXPECT_NE(ZERO, d0);
  EXPECT_NE(ZERO, dd0);
  EXPECT_TRUE(dd0 == e0 || dd0 == -e0) << "e0: " << e0 << ", dd0: " << dd0 << ", d0: " << d0;

  EXPECT_NE(ZERO, d1);
  EXPECT_NE(ZERO, dd1);
  EXPECT_TRUE(dd1 == e1 || dd1 == -e1) << "e1: " << e1 << ", dd1: " << dd1 << ", d1: " << d1;
}

TYPED_TEST(MultivectorTest, DualOfDualReturnsOriginalUpToSign) {
  static constexpr TypeParam e{TypeParam::template e<TypeParam::NUM_ZERO_BASES>() * 2};
  static constexpr TypeParam dd{e.dual().dual()};

  // dual(dual(X)) == X or dual(dual(X)) == -X depending on I^2.
  const bool is_original{dd == e};
  const bool is_negated{dd == -e};

  EXPECT_TRUE(is_original || is_negated) << "e: " << e << ", dd: " << dd;
}

TYPED_TEST(MultivectorTest, DualAndUndualAreInverses) {
  static constexpr TypeParam v{TypeParam::template e<TypeParam::NUM_ZERO_BASES>() * 2};
  static constexpr TypeParam d{v.dual()};
  static constexpr TypeParam u{v.undual()};
  static constexpr TypeParam I{TypeParam::invertible_pseudoscalar()};
  static constexpr TypeParam du{v.dual().undual()};
  static constexpr TypeParam ud{v.undual().dual()};

  EXPECT_EQ(du, v) << "du: " << du << ", v: " << v << ", d: " << d << ", u: " << u << ", I: " << I;
  EXPECT_EQ(ud, v);
}

TYPED_TEST(MultivectorTest, DualityRespectsPseudoscalarOfAlgebra) {
  // The dual of the pseudoscalar must be a pure scalar (grade 0), since the
  // pseudoscalar is the top-grade element and its complement is the scalar.
  static constexpr TypeParam I{TypeParam::pseudoscalar()};
  static constexpr TypeParam dual_I{I.dual()};

  EXPECT_EQ(dual_I.grade_projection(0), dual_I);
}

TYPED_TEST(MultivectorTest, ContractionWithReversePseudoscalarYieldsDual) {
  static constexpr TypeParam e0{TypeParam::template e<TypeParam::NUM_ZERO_BASES>()};
  static constexpr TypeParam I{TypeParam::invertible_pseudoscalar()};

  EXPECT_EQ(e0.left_contraction(I.reverse()), e0.dual());
}

TYPED_TEST(MultivectorTest, ContractionWithPseudoscalarYieldsUndualUpToSign) {
  static constexpr TypeParam e0{TypeParam::template e<TypeParam::NUM_ZERO_BASES>()};
  static constexpr TypeParam I{TypeParam::invertible_pseudoscalar()};

  static constexpr TypeParam value{e0.left_contraction(I)};

  static constexpr bool is_same(e0.undual() == value);
  static constexpr bool is_negated(-e0.undual() == value);

  EXPECT_TRUE(is_same || is_negated) << "value: " << value;
}

//--------------------------------------------------------------------------------------------------
// Exponentiation & Transcendentals
//--------------------------------------------------------------------------------------------------

// These tests require exp(), log(), and sqrt() on Multivector, which are not
// present in the current API. See the list of unimplemented tests below.

//--------------------------------------------------------------------------------------------------
// Rotors & Versors
//--------------------------------------------------------------------------------------------------

// These tests require exp() to construct rotors from bivectors, and a sandwich
// product helper. See the list of unimplemented tests below.

//--------------------------------------------------------------------------------------------------
// Numerical Robustness
//--------------------------------------------------------------------------------------------------

TYPED_TEST(MultivectorTest, LargeScalarCoefficientsMaintainRelativePrecision) {
  using ScalarType = typename TypeParam::ScalarType;
  static constexpr ScalarType LARGE{1e6};
  static constexpr TypeParam a{TypeParam::template e<0>().multiply(LARGE)};
  static constexpr TypeParam b{TypeParam::template e<0>().multiply(LARGE)};

  // Adding two large-coefficient multivectors and subtracting one back must
  // recover the original to within floating-point relative precision.
  const TypeParam result{a.add(b).subtract(b)};
  EXPECT_NEAR(result.coefficient(1), a.coefficient(1),
              LARGE * std::numeric_limits<ScalarType>::epsilon() * ScalarType{10});
}

TYPED_TEST(MultivectorTest, NearZeroCoefficientsAreConsistentWithEpsilon) {
  using ScalarType = typename TypeParam::ScalarType;
  static constexpr ScalarType HALF_EPSILON{TypeParam::EPSILON / ScalarType{2}};

  TypeParam mv{};
  mv.set_coefficient(1, HALF_EPSILON);

  // A coefficient below EPSILON must compare equal to zero.
  EXPECT_TRUE(AreNear(mv, TypeParam{}));
}

TYPED_TEST(MultivectorTest, RepeatedProductsDoNotAccumulateUnboundedError) {
  using ScalarType = typename TypeParam::ScalarType;

  // Repeatedly multiply by e0 and its inverse. After an even number of round
  // trips the result must be close to the original.
  TypeParam e0{TypeParam::template e<0>()};

  if (e0.multiply(e0).scalar() == ScalarType{0}) {
    GTEST_SKIP() << "Basis vector is null in this algebra; inverse is undefined.";
  }

  TypeParam result{e0};
  const TypeParam inv{e0.inverse()};
  static constexpr int ITERATIONS{100};
  for (int i = 0; i < ITERATIONS; ++i) {
    result = result.multiply(e0).multiply(inv);
  }

  EXPECT_TRUE(AreNear(result, e0));
}

TYPED_TEST(MultivectorTest, NormalizationOfNearZeroMultivectorHandledGracefully) {
  // Normalizing a zero multivector should produce NaN in the scalar part.
  static constexpr TypeParam zero{};
  // TODO(james): Mark as constexpr if using C++26 or later.
  const TypeParam result{zero.normalize()};

  EXPECT_TRUE(std::isnan(result.scalar()));
}

}  // namespace ndyn::math
