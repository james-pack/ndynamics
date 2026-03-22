#include <memory>
#include <string_view>

#include "calculator/interpreter.h"
#include "calculator/interpreter_test_utils.h"
#include "calculator/parser.h"
#include "gtest/gtest.h"
#include "math/algebra.h"
#include "math/canonical_basis_representation.h"
#include "math/multivector_test_utils.h"

namespace ndyn::ui {

using namespace ndyn::math;

TEST(BasicInterpreterTest, CanInterpretSimpleScalarExpressions) {
  using AlgebraType = Scalar<>;
  EXPECT_TRUE(MatchesValue<AlgebraType>("2", 2));
  // EXPECT_TRUE(MatchesValue<AlgebraType>("1 * 2", 2));
  // EXPECT_TRUE(MatchesValue<AlgebraType>("2 * 2", 4));
  // EXPECT_TRUE(MatchesValue<AlgebraType>("1 + 2", 3));
  // EXPECT_TRUE(MatchesValue<AlgebraType>("2 + 2", 4));
  // EXPECT_TRUE(MatchesValue<AlgebraType>("2 - 2", 0));
  // EXPECT_TRUE(MatchesValue<AlgebraType>("4 - 2", 2, true));

  // EXPECT_TRUE(MatchesValue<AlgebraType>("(4 + 1) * (2 + 2)", 20, true));
}

// Note: this test catches a particular bug in the parsing where a '+' or a '-' without spaces gets
// bound as a unary operator when it was intended to be the binary operator. Multiplication is then
// assumed by the parser to be the binary operator between these operands.
TEST(BasicInterpreterTest, CanInterpretSimpleScalarExpressionsWithoutSeparators) {
  using AlgebraType = Scalar<>;
  EXPECT_TRUE(MatchesValue<AlgebraType>("1*2", 2));
  EXPECT_TRUE(MatchesValue<AlgebraType>("2*2", 4));
  EXPECT_TRUE(MatchesValue<AlgebraType>("1+2", 3));
  EXPECT_TRUE(MatchesValue<AlgebraType>("2+2", 4));
  EXPECT_TRUE(MatchesValue<AlgebraType>("2-2", 0));
  EXPECT_TRUE(MatchesValue<AlgebraType>("4-2", 2));
}

TEST(BasicInterpreterTest, CanInterpretComplexBasisVector) {
  using AlgebraType = Complex<>;
  // Note that here we are using the generic representation for the complex numbers. This means that
  // we have to use "e1" rather than "i" to mean the first basis vector.
  EXPECT_TRUE(MatchesValue<AlgebraType>("e1", AlgebraType::VectorType::e<0>()));
}

TEST(BasicInterpreterTest, CanInterpretExpressionsInComplexAlgebra) {
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

TEST(BasicInterpreterTest, CanInterpretVga2dBasisVectors) {
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

TEST(BasicInterpreterTest, CanInterpretVgaBasisVectors) {
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

class InterpreterTest : public ::testing::Test {
 public:
  using AlgebraType = Vga<>;
  using Representation = math::GenericBasisRepresentation<AlgebraType>;

  Parser parser{};
  Interpreter<AlgebraType, Representation> interpreter{};
  std::shared_ptr<LineAst> ast{};

  bool parse(std::string_view line, bool enable_trace = true) {
    if (enable_trace) {
      parser.enable_trace();
    }
    return parser.parse(line, ast);
  }

  bool interpret(std::string_view line) {
    if (parser.parse(line, ast)) {
      interpreter.interpret(*ast);
      return interpreter.success;
    } else {
      return false;
    }
  }
};

TEST_F(InterpreterTest, AccentedIdentifier) { EXPECT_TRUE(parse("sphère")); }
TEST_F(InterpreterTest, UnicodeIdentifier) { EXPECT_TRUE(parse("ε")); }
TEST_F(InterpreterTest, SubscriptedIdentifier) { EXPECT_TRUE(parse("v₀")); }
TEST_F(InterpreterTest, SubscriptedUnicodeIdentifier) { EXPECT_TRUE(parse("λ₁")); }
TEST_F(InterpreterTest, SuperscriptedIdentifier) { EXPECT_TRUE(parse("β⁻")); }
TEST_F(InterpreterTest, OtherUnicodeIdentifier) { EXPECT_TRUE(parse("gμν")); }

TEST_F(InterpreterTest, AccentedIdentifierInAssignment) { EXPECT_TRUE(parse("sphère = e1")); }
TEST_F(InterpreterTest, UnicodeIdentifierInAssignment) { EXPECT_TRUE(parse("ε = e1")); }
TEST_F(InterpreterTest, SubscriptedIdentifierInAssignment) { EXPECT_TRUE(parse("v₀ = e1")); }
TEST_F(InterpreterTest, SubscriptedUnicodeIdentifierInAssignment) { EXPECT_TRUE(parse("λ₁ = e1")); }
TEST_F(InterpreterTest, SuperscriptedIdentifierInAssignment) { EXPECT_TRUE(parse("β⁻ = e1*e2")); }
TEST_F(InterpreterTest, MiscUnicodeStatement) {
  EXPECT_TRUE(parse("r₊ = (G*M + (G²*M² - a²*c²)^(1/2)) / c²"));
}

}  // namespace ndyn::ui
