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

TEST(InterpreterTest, CanInterpretSimpleScalarExpression) {
  using AlgebraType = Scalar<>;
  EXPECT_TRUE(MatchesValue<AlgebraType>("1", AlgebraType::VectorType{1}));
  EXPECT_TRUE(MatchesValue<AlgebraType>("1 * 2", AlgebraType::VectorType{2}));
  EXPECT_TRUE(MatchesValue<AlgebraType>("2 * 2", AlgebraType::VectorType{4}));
  EXPECT_TRUE(MatchesValue<AlgebraType>("1 + 2", AlgebraType::VectorType{3}));
  EXPECT_TRUE(MatchesValue<AlgebraType>("2 + 2", AlgebraType::VectorType{4}));
  EXPECT_TRUE(MatchesValue<AlgebraType>("2 - 2", AlgebraType::VectorType{0}));
  EXPECT_TRUE(MatchesValue<AlgebraType>("4 - 2", AlgebraType::VectorType{2}));
}

// Note: this test catches a particular bug in the parsing where a '+' or a '-' without spaces gets
// bound as a unary operator when it was intended to be the binary operator. Multiplication is then
// assumed by the parser to be the binary operator between these operands.
TEST(InterpreterTest, CanInterpretSimpleScalarExpressionWithoutSeparators) {
  using AlgebraType = Scalar<>;
  EXPECT_TRUE(MatchesValue<AlgebraType>("1*2", AlgebraType::VectorType{2}));
  EXPECT_TRUE(MatchesValue<AlgebraType>("2*2", AlgebraType::VectorType{4}));
  EXPECT_TRUE(MatchesValue<AlgebraType>("1+2", AlgebraType::VectorType{3}));
  EXPECT_TRUE(MatchesValue<AlgebraType>("2+2", AlgebraType::VectorType{4}));
  EXPECT_TRUE(MatchesValue<AlgebraType>("2-2", AlgebraType::VectorType{0}));
  EXPECT_TRUE(MatchesValue<AlgebraType>("4-2", AlgebraType::VectorType{2}));
}

TEST(InterpreterTest, CanInterpretComplexBasisVector) {
  using AlgebraType = Complex<>;
  EXPECT_TRUE(MatchesValue<AlgebraType>("i", AlgebraType::VectorType::e<0>()));
}

}  // namespace ndyn::ui
