#include "ui/interpreter.h"

#include <memory>

#include "gtest/gtest.h"
#include "math/algebra.h"
#include "math/multivector_test_utils.h"
#include "ui/grammar.h"

namespace ndyn::ui {

using namespace ndyn::math;

template <typename AlgebraT>
::testing::AssertionResult MatchesValue(const std::string_view line,
                                        const typename AlgebraT::VectorType& expected) {
  Grammar parser{};
  std::shared_ptr<LineAst> ast{};
  if (!parser.parse(line, ast)) {
    return ::testing::AssertionFailure() << "Could not parse line";
  }

  Interpreter<AlgebraT> interpreter{};
  interpreter.interpret(*ast);

  if (!interpreter.success) {
    return ::testing::AssertionFailure() << "Error during interpretation -- success was false.";
  }
  if (!interpreter.was_value) {
    return ::testing::AssertionFailure() << "Error during interpretation -- was_value was false.";
  }
  if (!interpreter.message.empty()) {
    return ::testing::AssertionFailure()
           << "Error during interpretation -- message not empty. message: '" << interpreter.message
           << "'";
  }

  return AreNear(interpreter.current_value, expected);
}

template <typename AlgebraT>
::testing::AssertionResult MatchesValue(const std::string_view line,
                                        const typename AlgebraT::ScalarType& expected) {
  return MatchesValue<AlgebraT>(line, typename AlgebraT::VectorType{expected});
}

TEST(InterpreterTest, CanInterpretSimpleScalarExpressions) {
  using AlgebraType = Scalar<>;
  EXPECT_TRUE(MatchesValue<AlgebraType>("1", 1));
  EXPECT_TRUE(MatchesValue<AlgebraType>("1 * 2", 2));
  EXPECT_TRUE(MatchesValue<AlgebraType>("2 * 2", 4));
  EXPECT_TRUE(MatchesValue<AlgebraType>("1 + 2", 3));
  EXPECT_TRUE(MatchesValue<AlgebraType>("2 + 2", 4));
  EXPECT_TRUE(MatchesValue<AlgebraType>("2 - 2", 0));
  EXPECT_TRUE(MatchesValue<AlgebraType>("4 - 2", 2));

  EXPECT_TRUE(MatchesValue<AlgebraType>("(4 + 1) * (2 + 2)", 20));
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
  EXPECT_TRUE(MatchesValue<AlgebraType>("i", AlgebraType::VectorType::e<0>()));
}

TEST(InterpreterTest, CanInterpretExpressionsInComplexAlgebra) {
  using AlgebraType = Complex<>;

  constexpr auto i{AlgebraType::VectorType::e<0>()};

  EXPECT_TRUE(MatchesValue<AlgebraType>("1", 1));
  EXPECT_TRUE(MatchesValue<AlgebraType>("1 * 2", 2));
  EXPECT_TRUE(MatchesValue<AlgebraType>("2 * 2", 4));
  EXPECT_TRUE(MatchesValue<AlgebraType>("1 + 2", 3));
  EXPECT_TRUE(MatchesValue<AlgebraType>("2 + 2", 4));
  EXPECT_TRUE(MatchesValue<AlgebraType>("2 - 2", 0));
  EXPECT_TRUE(MatchesValue<AlgebraType>("4 - 2", 2));

  EXPECT_TRUE(MatchesValue<AlgebraType>("i", i));
  EXPECT_TRUE(MatchesValue<AlgebraType>("1 * i", i));
  EXPECT_TRUE(MatchesValue<AlgebraType>("i * 1", i));
  EXPECT_TRUE(MatchesValue<AlgebraType>("i * 2", 2 * i));
  EXPECT_TRUE(MatchesValue<AlgebraType>("i * i", i * i));
  EXPECT_TRUE(MatchesValue<AlgebraType>("i * i", -1));

  EXPECT_TRUE(MatchesValue<AlgebraType>("(i + 1) * (1 + i)", 2 * i));
}

TEST(InterpreterTest, CanInterpretVga2dBasisVectors) {
  using AlgebraType = Vga2d<>;
  constexpr auto e1{AlgebraType::VectorType::e<0>()};
  constexpr auto e2{AlgebraType::VectorType::e<1>()};
  EXPECT_TRUE(MatchesValue<AlgebraType>("e1", e1));
  EXPECT_TRUE(MatchesValue<AlgebraType>("e2", e2));
  EXPECT_TRUE(MatchesValue<AlgebraType>("e12", e1 * e2));
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
