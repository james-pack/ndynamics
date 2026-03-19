#include <memory>

#include "gtest/gtest.h"
#include "math/algebra.h"
#include "math/canonical_basis_representation.h"
#include "math/multivector_test_utils.h"
#include "ui/interpreter.h"
#include "ui/interpreter_test_utils.h"
#include "ui/parser.h"

namespace ndyn::ui {

using namespace ndyn::math;

TEST(InterpreterTest, CanInterpretSimpleScalarExpressions) {
  using AlgebraType = Scalar<>;
  EXPECT_TRUE(MatchesValue<AlgebraType>("1", 1));
  EXPECT_TRUE(MatchesValue<AlgebraType>("1 * 2", 2));
  EXPECT_TRUE(MatchesValue<AlgebraType>("2 * 2", 4));
  EXPECT_TRUE(MatchesValue<AlgebraType>("1 + 2", 3));
  EXPECT_TRUE(MatchesValue<AlgebraType>("2 + 2", 4));
  EXPECT_TRUE(MatchesValue<AlgebraType>("2 - 2", 0));
  EXPECT_TRUE(MatchesValue<AlgebraType>("4 - 2", 2, true));

  EXPECT_TRUE(MatchesValue<AlgebraType>("(4 + 1) * (2 + 2)", 20, true));
}

// Note: this test catches a particular bug in the parsing where a '+' or a '-' without spaces gets
// bound as a unary operator when it was intended to be the binary operator. Multiplication is then
// assumed by the parser to be the binary operator between these operands.
TEST(InterpreterTest, CanInterpretSimpleScalarExpressionsWithoutSeparators) {
  using AlgebraType = Scalar<>;
  EXPECT_TRUE(MatchesValue<AlgebraType>("1*2", 2));
  EXPECT_TRUE(MatchesValue<AlgebraType>("2*2", 4));
  EXPECT_TRUE(MatchesValue<AlgebraType>("1+2", 3));
  EXPECT_TRUE(MatchesValue<AlgebraType>("2+2", 4));
  EXPECT_TRUE(MatchesValue<AlgebraType>("2-2", 0));
  EXPECT_TRUE(MatchesValue<AlgebraType>("4-2", 2));
}

TEST(InterpreterTest, CanInterpretComplexBasisVector) {
  using AlgebraType = Complex<>;
  // Note that here we are using the generic representation for the complex numbers. This means that
  // we have to use "e1" rather than "i" to mean the first basis vector.
  EXPECT_TRUE(MatchesValue<AlgebraType>("e1", AlgebraType::VectorType::e<0>()));
}

TEST(InterpreterTest, CanInterpretExpressionsInComplexAlgebra) {
  using AlgebraType = Complex<>;
  // Use the canonical representation for the complex numbers so that we can work with "i" instead
  // of "e1".
  using Representation = CanonicalBasisRepresentation<AlgebraType>;

  constexpr auto i{AlgebraType::VectorType::e<0>()};

  // Hack to workaround EXPECT_TRUE()'s template bugs.
  auto validate = [](auto a, auto b) {
    auto result = MatchesValue<AlgebraType, Representation>(a, b);
    EXPECT_TRUE(result);
  };

  validate("1", 1);
  validate("1 * 2", 2);
  validate("2 * 2", 4);
  validate("1 + 2", 3);
  validate("2 + 2", 4);
  validate("2 - 2", 0);
  validate("4 - 2", 2);

  validate("i", i);
  validate("1 * i", i);
  validate("i * 1", i);
  validate("i * 2", 2 * i);
  validate("i * i", i * i);
  validate("i * i", -1);

  // Testing the evaluation of select expressions. Includes the fully evaluated form to help with
  // debugging if there is a problem.
  validate("(i + 1) * (1 + i)", (i + 1) * (1 + i));
  validate("(i + 1) * (1 + i)", 2 * i);

  validate("(i + 1) * (1 - i)", (i + 1) * (1 - i));
  validate("(i + 1) * (1 - i)", 2);

  validate("(i - 1) * (1 + i)", (i - 1) * (1 + i));
  validate("(i - 1) * (1 + i)", -2);

  validate("(i - 1) * (1 - i)", (i - 1) * (1 - i));
  validate("(i - 1) * (1 - i)", 2 * i);
}

TEST(InterpreterTest, CanInterpretVga2dBasisVectors) {
  using AlgebraType = Vga2d<>;
  constexpr auto e1{AlgebraType::VectorType::e<0>()};
  constexpr auto e2{AlgebraType::VectorType::e<1>()};
  EXPECT_TRUE(MatchesValue<AlgebraType>("e1", e1));
  EXPECT_TRUE(MatchesValue<AlgebraType>("e2", e2));
  EXPECT_TRUE(MatchesValue<AlgebraType>("e12", e1 * e2));

  // Testing the evaluation of select expressions. Includes the fully evaluated form to help with
  // debugging if there is a problem.
  EXPECT_TRUE(MatchesValue<AlgebraType>("(e1 + 1) * (e1 + 1)", (e1 + 1) * (e1 + 1)));
  EXPECT_TRUE(MatchesValue<AlgebraType>("(e1 + 1) * (e1 + 1)", 2 * e1 + 2));

  EXPECT_TRUE(MatchesValue<AlgebraType>("(e2 - 1) * (e2 + 1)", (e2 - 1) * (e2 + 1)));
  EXPECT_TRUE(MatchesValue<AlgebraType>("(e2 - 1) * (e2 + 1)", 0));

  EXPECT_TRUE(MatchesValue<AlgebraType>("(e2 - 1) * (e1 + 1)", (e2 - 1) * (e1 + 1)));
  EXPECT_TRUE(MatchesValue<AlgebraType>("(e2 - 1) * (e1 + 1)", -e1 * e2 - e1 + e2 - 1));

  EXPECT_TRUE(MatchesValue<AlgebraType>("(e12 - 1) * (e12 + 1)", (e1 * e2 - 1) * (e1 * e2 + 1)));
  EXPECT_TRUE(MatchesValue<AlgebraType>("(e12 - 1) * (e12 + 1)", -2));
}

TEST(InterpreterTest, CanInterpretVgaBasisVectors) {
  using AlgebraType = Vga<>;
  constexpr auto e1{AlgebraType::VectorType::e<0>()};
  constexpr auto e2{AlgebraType::VectorType::e<1>()};
  constexpr auto e3{AlgebraType::VectorType::e<2>()};
  EXPECT_TRUE(MatchesValue<AlgebraType>("e1", e1));
  EXPECT_TRUE(MatchesValue<AlgebraType>("e2", e2));
  EXPECT_TRUE(MatchesValue<AlgebraType>("e3", e3));
  EXPECT_TRUE(MatchesValue<AlgebraType>("e12", e1 * e2));
  EXPECT_TRUE(MatchesValue<AlgebraType>("e13", e1 * e3));
  EXPECT_TRUE(MatchesValue<AlgebraType>("e23", e2 * e3));
  EXPECT_TRUE(MatchesValue<AlgebraType>("e123", e1 * e2 * e3));
}

}  // namespace ndyn::ui
