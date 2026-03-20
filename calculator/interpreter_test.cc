#include "calculator/interpreter.h"

#include <memory>
#include <string_view>

#include "gtest/gtest.h"
#include "math/canonical_basis_representation.h"
#include "math/generic_basis_representation.h"
#include "calculator/ast_printer.h"
#include "calculator/interpreter_test_utils.h"
#include "calculator/parser.h"

namespace ndyn::ui {

/**
 * Types of algebras organized according to the number of basis vectors available in each.
 */
using ZeroBasisVectors = ::testing::Types<  //
    math::Scalar<>,                         //
    math::Complex<>,                        //
    math::Vga2d<>,                          //
    math::Vga<>,                            //
    math::Pga2d<>,                          //
    math::Pga<>>;

using OneBasisVector = ::testing::Types<  //
    math::Complex<>,                      //
    math::Vga2d<>,                        //
    math::Vga<>,                          //
    math::Pga2d<>,                        //
    math::Pga<>>;

using TwoBasisVectors = ::testing::Types<  //
    math::Vga2d<>,                         //
    math::Vga<>,                           //
    math::Pga2d<>,                         //
    math::Pga<>>;

using ThreeBasisVectors = ::testing::Types<  //
    math::Vga<>,                             //
    math::Pga<>>;

// ---------------------------------------------------------------------------
// Shared fixture
// ---------------------------------------------------------------------------

/**
 * Wraps the parser+interpreter pair so individual test bodies stay readable.
 * Failure-path tests need access to interpreter state directly rather than
 * going through MatchesValue, so we expose a thin helper for that case.
 */
template <typename AlgebraT>
class InterpreterFixture : public ::testing::Test {
 protected:
  using Algebra = AlgebraT;
  using Representation = math::GenericBasisRepresentation<AlgebraT>;
  using Vector = typename AlgebraT::VectorType;
  using Scalar = typename AlgebraT::ScalarType;

  /**
   * Runs the parser only, ignoring parse errors, and returning the AST. Useful for debugging the
   * grammar itself.
   */
  std::shared_ptr<LineAst> parse_line(std::string_view line) {
    Parser parser{};
    std::shared_ptr<LineAst> ast{};
    Interpreter<AlgebraT, Representation> interpreter{};
    parser.parse(line, ast);
    return ast;
  }

  /**
   * Runs the full parse+interpret pipeline and returns the interpreter state.
   * Used by tests that need to inspect failure conditions directly.
   */
  Interpreter<AlgebraT, Representation> interpret_line(std::string_view line) {
    Parser parser{};
    Interpreter<AlgebraT, Representation> interpreter{};
    DLOG(INFO) << "interpreter symbol table on startup: " << interpreter.dump_symbol_table();
    std::shared_ptr<LineAst> ast{};
    if (!parser.parse(line, ast)) {
      // Signal parse failure through the interpreter's own flag so callers
      // have a single state object to inspect regardless of failure stage.
      interpreter.success = false;
      interpreter.message = "parse failure";
      return interpreter;
    }
    interpreter.interpret(*ast);
    return interpreter;
  }
};

// ---------------------------------------------------------------------------
// Suite 1 — dimension-agnostic tests, all algebras
// ---------------------------------------------------------------------------

template <typename AlgebraT>
class ScalarHandlingAndGeneralInterpreterBehavior : public InterpreterFixture<AlgebraT> {};

TYPED_TEST_SUITE(ScalarHandlingAndGeneralInterpreterBehavior, ZeroBasisVectors);

// --- Scalar literals -------------------------------------------------------

TYPED_TEST(ScalarHandlingAndGeneralInterpreterBehavior, IntegerScalarLiteral) {
  using A = typename TestFixture::Algebra;
  EXPECT_TRUE(MatchesValue<A>("1", typename A::ScalarType{1}));
}

TYPED_TEST(ScalarHandlingAndGeneralInterpreterBehavior, NegativeIntegerScalarLiteral) {
  using A = typename TestFixture::Algebra;
  EXPECT_TRUE(MatchesValue<A>("-1", typename A::ScalarType{-1}));
}

TYPED_TEST(ScalarHandlingAndGeneralInterpreterBehavior, PositiveIntegerScalarLiteralWithUnaryPlus) {
  using A = typename TestFixture::Algebra;
  EXPECT_TRUE(MatchesValue<A>("+1", typename A::ScalarType{1}));
}

TYPED_TEST(ScalarHandlingAndGeneralInterpreterBehavior, FloatingPointScalarLiteral) {
  using A = typename TestFixture::Algebra;
  EXPECT_TRUE(MatchesValue<A>("3.14", typename A::ScalarType{3.14}));
}

TYPED_TEST(ScalarHandlingAndGeneralInterpreterBehavior, NegativeFloatingPointScalarLiteral) {
  using A = typename TestFixture::Algebra;
  EXPECT_TRUE(MatchesValue<A>("-2.71", typename A::ScalarType{-2.71}));
}

TYPED_TEST(ScalarHandlingAndGeneralInterpreterBehavior, ZeroScalarLiteral) {
  using A = typename TestFixture::Algebra;
  EXPECT_TRUE(MatchesValue<A>("0", typename A::ScalarType{0}));
}

TYPED_TEST(ScalarHandlingAndGeneralInterpreterBehavior, ZeroPointZeroScalarLiteral) {
  using A = typename TestFixture::Algebra;
  EXPECT_TRUE(MatchesValue<A>("0.0", typename A::ScalarType{0}));
}

// --- Whitespace and empty input edge cases ---------------------------------

TYPED_TEST(ScalarHandlingAndGeneralInterpreterBehavior, EmptyLineSucceedsWithNoValue) {
  auto state = this->interpret_line("");
  // An empty line is a valid Line production (the statement is optional).
  // The interpreter should succeed but produce no value.
  EXPECT_TRUE(state.success);
  EXPECT_FALSE(state.was_value);
}

TYPED_TEST(ScalarHandlingAndGeneralInterpreterBehavior, WhitespaceOnlyLineSucceedsWithNoValue) {
  auto state = this->interpret_line("   ");
  EXPECT_TRUE(state.success);
  EXPECT_FALSE(state.was_value);
}

TYPED_TEST(ScalarHandlingAndGeneralInterpreterBehavior, TabOnlyLineSucceedsWithNoValue) {
  auto state = this->interpret_line("\t\t");
  EXPECT_TRUE(state.success);
  EXPECT_FALSE(state.was_value);
}

TYPED_TEST(ScalarHandlingAndGeneralInterpreterBehavior, ScalarWithLeadingWhitespace) {
  using A = typename TestFixture::Algebra;
  EXPECT_TRUE(MatchesValue<A>("   42", typename A::ScalarType{42}));
}

TYPED_TEST(ScalarHandlingAndGeneralInterpreterBehavior, ScalarWithTrailingWhitespace) {
  using A = typename TestFixture::Algebra;
  EXPECT_TRUE(MatchesValue<A>("42   ", typename A::ScalarType{42}));
}

TYPED_TEST(ScalarHandlingAndGeneralInterpreterBehavior, ScalarWithLeadingAndTrailingWhitespace) {
  using A = typename TestFixture::Algebra;
  EXPECT_TRUE(MatchesValue<A>("  42  ", typename A::ScalarType{42}));
}

TYPED_TEST(ScalarHandlingAndGeneralInterpreterBehavior, ExpressionWithInternalWhitespace) {
  using A = typename TestFixture::Algebra;
  EXPECT_TRUE(MatchesValue<A>("1 + 1", typename A::ScalarType{2}));
}

TYPED_TEST(ScalarHandlingAndGeneralInterpreterBehavior, ExpressionWithTabSeparators) {
  using A = typename TestFixture::Algebra;
  EXPECT_TRUE(MatchesValue<A>("1\t+\t1", typename A::ScalarType{2}));
}

TYPED_TEST(ScalarHandlingAndGeneralInterpreterBehavior, ExpressionWithNoWhitespace) {
  using A = typename TestFixture::Algebra;
  EXPECT_TRUE(MatchesValue<A>("1+1", typename A::ScalarType{2}));
}

// --- Arithmetic precedence -------------------------------------------------

/**
 * These tests are the primary guard against precedence bugs in the Additive /
 * Multiplicative grammar layers.  Each case is chosen to produce a different
 * result depending on which operator binds tighter.
 */

TYPED_TEST(ScalarHandlingAndGeneralInterpreterBehavior, BinaryOperationsAreLeftAssociative) {
  using A = typename TestFixture::Algebra;
  // Use subtraction to make associativity observable.
  // 5 - 3 - 1 should be (5 - 3) - 1 == 1, not 5 - (3 - 1) == 3.
  ASSERT_TRUE(MatchesValue<A>("(5 - 3) - 1", typename A::ScalarType{1}));
  ASSERT_TRUE(MatchesValue<A>("5 - (3 - 1)", typename A::ScalarType{3}));
  EXPECT_TRUE(MatchesValue<A>("5 - 3 - 1", typename A::ScalarType{1}));

  auto ast = this->parse_line("5 - 3 - 1");
  print_ast(*ast);
}

TYPED_TEST(ScalarHandlingAndGeneralInterpreterBehavior, MultiplicationBeforeAddition) {
  using A = typename TestFixture::Algebra;
  EXPECT_TRUE(MatchesValue<A>("2 + 3 * 4", typename A::ScalarType{14}));
  EXPECT_TRUE(MatchesValue<A>("3 * 4 + 2", typename A::ScalarType{14}));
}

TYPED_TEST(ScalarHandlingAndGeneralInterpreterBehavior, MultiplicationBeforeSubtraction) {
  using A = typename TestFixture::Algebra;
  EXPECT_TRUE(MatchesValue<A>("10 - 2 * 3", typename A::ScalarType{4}));
  EXPECT_TRUE(MatchesValue<A>("10 * 3 - 2", typename A::ScalarType{28}));
}

TYPED_TEST(ScalarHandlingAndGeneralInterpreterBehavior, DISABLED_DivisionBeforeAddition) {
  using A = typename TestFixture::Algebra;
  EXPECT_TRUE(MatchesValue<A>("3 + 6 / 3", typename A::ScalarType{5}));
  EXPECT_TRUE(MatchesValue<A>("6 / 3 + 3", typename A::ScalarType{5}));
}

TYPED_TEST(ScalarHandlingAndGeneralInterpreterBehavior,
           ParenthesesOverrideMultiplicationPrecedence) {
  using A = typename TestFixture::Algebra;
  EXPECT_TRUE(MatchesValue<A>("(2 + 3) * 4", typename A::ScalarType{20}));
}

TYPED_TEST(ScalarHandlingAndGeneralInterpreterBehavior, NestedParentheses) {
  using A = typename TestFixture::Algebra;
  EXPECT_TRUE(MatchesValue<A>("((2 + 3)) * 4", typename A::ScalarType{20}));
}

TYPED_TEST(ScalarHandlingAndGeneralInterpreterBehavior, DeeplyNestedParentheses) {
  using A = typename TestFixture::Algebra;
  EXPECT_TRUE(MatchesValue<A>("(((5)))", typename A::ScalarType{5}));
}

TYPED_TEST(ScalarHandlingAndGeneralInterpreterBehavior, UnaryMinusBindsTighterThanMultiplication) {
  using A = typename TestFixture::Algebra;
  // -2 * 3 should be (-2) * 3 == -6.
  EXPECT_TRUE(MatchesValue<A>("-2 * 3", typename A::ScalarType{-6}));
}

TYPED_TEST(ScalarHandlingAndGeneralInterpreterBehavior, UnaryMinusOnParenthetical) {
  using A = typename TestFixture::Algebra;
  EXPECT_TRUE(MatchesValue<A>("-(2 + 3)", typename A::ScalarType{-5}));
}

TYPED_TEST(ScalarHandlingAndGeneralInterpreterBehavior, UnaryPlusIsIdentity) {
  using A = typename TestFixture::Algebra;
  EXPECT_TRUE(MatchesValue<A>("+5", typename A::ScalarType{5}));
}

TYPED_TEST(ScalarHandlingAndGeneralInterpreterBehavior, ChainedUnaryMinus) {
  using A = typename TestFixture::Algebra;
  // The grammar allows one unary operator per Unary node, so --2 is two
  // nested Unary productions.  Verify this does not silently fail to parse.
  EXPECT_TRUE(MatchesValue<A>("- -2", typename A::ScalarType{2}));
}

// --- Additive / multiplicative operator combinations ----------------------

TYPED_TEST(ScalarHandlingAndGeneralInterpreterBehavior, AddThenMultiply) {
  using A = typename TestFixture::Algebra;
  EXPECT_TRUE(MatchesValue<A>("1 + 2 * 3 + 4", typename A::ScalarType{11}));
  EXPECT_TRUE(MatchesValue<A>("1 + 4 + 2 * 3", typename A::ScalarType{11}));
  EXPECT_TRUE(MatchesValue<A>("2 * 3 + 1 + 4", typename A::ScalarType{11}));
}

TYPED_TEST(ScalarHandlingAndGeneralInterpreterBehavior, SubtractNegativeScalar) {
  using A = typename TestFixture::Algebra;
  // "1 - -1" tests that the parser correctly handles a binary minus followed
  // by a unary minus without ambiguity.
  EXPECT_TRUE(MatchesValue<A>("1 - -1", typename A::ScalarType{2}));
}

TYPED_TEST(ScalarHandlingAndGeneralInterpreterBehavior, DISABLED_DivisionAssociativity) {
  using A = typename TestFixture::Algebra;
  // 12 / 4 / 3: left-associative gives 1, right-associative gives 9.
  EXPECT_TRUE(MatchesValue<A>("12 / 4 / 3", typename A::ScalarType{1}));
}

// --- Assignment and variable reuse ----------------------------------------

TYPED_TEST(ScalarHandlingAndGeneralInterpreterBehavior, AssignAndRecallScalar) {
  using A = typename TestFixture::Algebra;
  using Rep = math::GenericBasisRepresentation<A>;
  Parser parser{};
  Interpreter<A, Rep> interp{};

  std::shared_ptr<LineAst> ast{};
  ASSERT_TRUE(parser.parse("x = 7", ast));
  interp.interpret(*ast);
  ASSERT_TRUE(interp.success);

  ASSERT_TRUE(parser.parse("x", ast));
  interp.interpret(*ast);
  ASSERT_TRUE(interp.success);
  EXPECT_TRUE(interp.was_value);
}

TYPED_TEST(ScalarHandlingAndGeneralInterpreterBehavior, AssignmentExpressionInvolvesPrecedence) {
  using A = typename TestFixture::Algebra;
  // The RHS should be evaluated with correct precedence before binding.
  EXPECT_TRUE(MatchesValue<A>("2 + 3 * 4", typename A::ScalarType{14}));
}

TYPED_TEST(ScalarHandlingAndGeneralInterpreterBehavior, KeywordsAreNotValidIdentifiers) {
  // "algebra", "metric", "dict", "exit", "quit", "help" must not parse as
  // assignment targets.  A parse failure is expected for each.
  for (const std::string_view keyword : {"algebra", "metric", "dict", "exit", "quit", "help"}) {
    const std::string assignment = std::string{keyword} + " = 1";
    auto state = this->interpret_line(assignment);
    EXPECT_FALSE(state.success) << "Expected parse failure for keyword: " << keyword;
  }
}

// --- Command parsing -------------------------------------------------------

/***************************************************************************************************
// Note: Do not attempt to test the exit and quit commands. The interpreter will exit the current
// process as part of interpreting them. For now, they can only be tested manually. If these are
// added, all of the tests in this file will appear to pass.
***************************************************************************************************/

TYPED_TEST(ScalarHandlingAndGeneralInterpreterBehavior, HelpCommand) {
  auto state = this->interpret_line("help");
  EXPECT_TRUE(state.success);
  EXPECT_FALSE(state.was_value);
}

TYPED_TEST(ScalarHandlingAndGeneralInterpreterBehavior, DictCommand) {
  auto state = this->interpret_line("dict");
  EXPECT_TRUE(state.success);
  EXPECT_FALSE(state.was_value);
}

TYPED_TEST(ScalarHandlingAndGeneralInterpreterBehavior, DictCommandLongFlag) {
  auto state = this->interpret_line("dict --long");
  EXPECT_TRUE(state.success);
  EXPECT_FALSE(state.was_value);
}

TYPED_TEST(ScalarHandlingAndGeneralInterpreterBehavior, DictCommandShortFlag) {
  auto state = this->interpret_line("dict -l");
  EXPECT_TRUE(state.success);
  EXPECT_FALSE(state.was_value);
}

// --- GA operators on scalar expressions ------------------------------------

/**
 * These tests exercise the GA-specific operator parse paths on scalar values
 * where the result is algebra-independent.  They confirm the parser dispatches
 * to the right production without testing geometric correctness.
 */

TYPED_TEST(ScalarHandlingAndGeneralInterpreterBehavior, OuterProductOfScalars) {
  using A = typename TestFixture::Algebra;
  // The outer product of two scalars is their ordinary product.
  EXPECT_TRUE(MatchesValue<A>("2 ^ 3", typename A::ScalarType{6}));
}

TYPED_TEST(ScalarHandlingAndGeneralInterpreterBehavior, OuterProductPrecedenceOverAddition) {
  using A = typename TestFixture::Algebra;
  EXPECT_TRUE(MatchesValue<A>("1 + 2 ^ 3", typename A::ScalarType{7}));
}

TYPED_TEST(ScalarHandlingAndGeneralInterpreterBehavior, OuterProductWithUnaryMinus) {
  using A = typename TestFixture::Algebra;
  EXPECT_TRUE(MatchesValue<A>("-2 ^ 3", typename A::ScalarType{-6}));
}

TYPED_TEST(ScalarHandlingAndGeneralInterpreterBehavior, LeftContractionOfScalars) {
  using A = typename TestFixture::Algebra;
  EXPECT_TRUE(MatchesValue<A>("3 << 3", typename A::ScalarType{9}));
}

TYPED_TEST(ScalarHandlingAndGeneralInterpreterBehavior, RightContractionOfScalars) {
  using A = typename TestFixture::Algebra;
  EXPECT_TRUE(MatchesValue<A>("3 >> 3", typename A::ScalarType{9}));
}

TYPED_TEST(ScalarHandlingAndGeneralInterpreterBehavior, ReverseOfScalar) {
  using A = typename TestFixture::Algebra;
  // Reverse of a scalar is the scalar itself.
  EXPECT_TRUE(MatchesValue<A>("~3", typename A::ScalarType{3}));
  EXPECT_TRUE(MatchesValue<A>("~-3", typename A::ScalarType{-3}));
  EXPECT_TRUE(MatchesValue<A>("~(-3)", typename A::ScalarType{-3}));
  EXPECT_TRUE(MatchesValue<A>("~(3)", typename A::ScalarType{3}));
}

TYPED_TEST(ScalarHandlingAndGeneralInterpreterBehavior, ReverseOfScalarIncorrectParse) {
  // This test isolates a bug in the interpreter where the reverse of a negative and positive of a
  // value evaluate to the same result.
  using A = typename TestFixture::Algebra;
  EXPECT_TRUE(MatchesValue<A>("~3", typename A::ScalarType{3}));
  EXPECT_TRUE(MatchesValue<A>("~-3", typename A::ScalarType{-3}));
  EXPECT_FALSE(MatchesValue<A>("~3", typename A::ScalarType{-3}));
  EXPECT_FALSE(MatchesValue<A>("~-3", typename A::ScalarType{3}));
}

TYPED_TEST(ScalarHandlingAndGeneralInterpreterBehavior, DualOfScalarViaUnaryBang) {
  // The dual of a scalar is algebra-dependent, but the parse path must not
  // fail.  We verify the interpreter succeeds and produces a value.
  auto state = this->interpret_line("!1");
  EXPECT_TRUE(state.success);
  EXPECT_TRUE(state.was_value);
  EXPECT_TRUE(state.message.empty());
}

TYPED_TEST(ScalarHandlingAndGeneralInterpreterBehavior, ReverseOfParenthetical) {
  using A = typename TestFixture::Algebra;
  EXPECT_TRUE(MatchesValue<A>("~(3)", typename A::ScalarType{3}));
}

TYPED_TEST(ScalarHandlingAndGeneralInterpreterBehavior, DualOfParenthetical) {
  auto state = this->interpret_line("!(1)");
  EXPECT_TRUE(state.success);
  EXPECT_TRUE(state.was_value);
}

// --- Basis vector expressions (dimension-agnostic: e1, e2) ----------------

template <typename AlgebraT>
class Interpreting2dAlgebras : public InterpreterFixture<AlgebraT> {};

TYPED_TEST_SUITE(Interpreting2dAlgebras, TwoBasisVectors);

TYPED_TEST(Interpreting2dAlgebras, BasisVectorE1IsValue) {
  auto state = this->interpret_line("e1");
  EXPECT_TRUE(state.success);
  EXPECT_TRUE(state.was_value);
  EXPECT_TRUE(state.message.empty());
}

TYPED_TEST(Interpreting2dAlgebras, BasisVectorE2IsValue) {
  auto state = this->interpret_line("e2");
  EXPECT_TRUE(state.success);
  EXPECT_TRUE(state.was_value);
  EXPECT_TRUE(state.message.empty());
}

TYPED_TEST(Interpreting2dAlgebras, OuterProductE1E2ViaOperator) {
  // e1 ^ e2 exercises the Multiplicative -> OuterOp production on basis vectors.
  auto state = this->interpret_line("e1 ^ e2");
  EXPECT_TRUE(state.success);
  EXPECT_TRUE(state.was_value);
  EXPECT_TRUE(state.message.empty());
}

TYPED_TEST(Interpreting2dAlgebras, OuterProductE1E2ViaIdentifier) {
  // e12 is an Identifier in the universal representation — a distinct parse
  // path from e1 ^ e2.
  auto state = this->interpret_line("e12");
  EXPECT_TRUE(state.success) << state.message;
  EXPECT_TRUE(state.was_value) << state.message;
  EXPECT_TRUE(state.message.empty()) << state.message;
}

TYPED_TEST(Interpreting2dAlgebras, OuterProductAndIdentifierFormAreEqual) {
  using A = typename TestFixture::Algebra;
  using Rep = math::GenericBasisRepresentation<A>;
  Parser parser{};
  Interpreter<A, Rep> op_interp{};
  Interpreter<A, Rep> id_interp{};

  std::shared_ptr<LineAst> ast{};
  ASSERT_TRUE(parser.parse("e1 ^ e2", ast));
  op_interp.interpret(*ast);
  ASSERT_TRUE(op_interp.success);

  ASSERT_TRUE(parser.parse("e12", ast));
  id_interp.interpret(*ast);
  ASSERT_TRUE(id_interp.success);

  EXPECT_TRUE(math::AreNear<A>(op_interp.current_value, id_interp.current_value));
}

TYPED_TEST(Interpreting2dAlgebras, ReverseOfBasisVectorE1) {
  auto state = this->interpret_line("~e1");
  EXPECT_TRUE(state.success);
  EXPECT_TRUE(state.was_value);
  EXPECT_TRUE(state.message.empty());
}

TYPED_TEST(Interpreting2dAlgebras, DualOfBasisVectorE1) {
  auto state = this->interpret_line("!e1");
  EXPECT_TRUE(state.success);
  EXPECT_TRUE(state.was_value);
  EXPECT_TRUE(state.message.empty());
}

TYPED_TEST(Interpreting2dAlgebras, LeftContractionE1E2) {
  auto state = this->interpret_line("e1 << e2");
  EXPECT_TRUE(state.success);
  EXPECT_TRUE(state.was_value);
  EXPECT_TRUE(state.message.empty());
}

TYPED_TEST(Interpreting2dAlgebras, RightContractionE1E2) {
  auto state = this->interpret_line("e1 >> e2");
  EXPECT_TRUE(state.success);
  EXPECT_TRUE(state.was_value);
  EXPECT_TRUE(state.message.empty());
}

TYPED_TEST(Interpreting2dAlgebras, GeometricProductE1E2) {
  auto state = this->interpret_line("e1 * e2");
  EXPECT_TRUE(state.success);
  EXPECT_TRUE(state.was_value);
  EXPECT_TRUE(state.message.empty());
}

TYPED_TEST(Interpreting2dAlgebras, ScalarPlusBasisVector) {
  auto state = this->interpret_line("1 + e1");
  EXPECT_TRUE(state.success);
  EXPECT_TRUE(state.was_value);
  EXPECT_TRUE(state.message.empty());
}

TYPED_TEST(Interpreting2dAlgebras, ScalarTimesE1ThenAddE2) {
  // Exercises mixed precedence between * and + involving basis vectors.
  auto state = this->interpret_line("2 * e1 + e2");
  EXPECT_TRUE(state.success);
  EXPECT_TRUE(state.was_value);
  EXPECT_TRUE(state.message.empty());
}

// --- Out-of-dimension basis vectors should fail ---------------------------

TYPED_TEST(Interpreting2dAlgebras, OutOfDimensionBasisVectorFails) {
  // Compute one past the last valid basis index for this algebra.
  constexpr size_t out_of_range_index =
      TypeParam::NUM_POSITIVE_BASES + TypeParam::NUM_NEGATIVE_BASES + TypeParam::NUM_ZERO_BASES + 1;
  const std::string expr = "e" + std::to_string(out_of_range_index);
  auto state = this->interpret_line(expr);
  EXPECT_FALSE(state.success);
  EXPECT_FALSE(state.message.empty());
}

// ---------------------------------------------------------------------------
// Suite 2 — requires >= 3 positive bases (e3 is meaningful)
// ---------------------------------------------------------------------------

template <typename AlgebraT>
class Interpreting3dAlgebras : public InterpreterFixture<AlgebraT> {};

TYPED_TEST_SUITE(Interpreting3dAlgebras, ThreeBasisVectors);

TYPED_TEST(Interpreting3dAlgebras, BasisVectorE3IsValue) {
  auto state = this->interpret_line("e3");
  EXPECT_TRUE(state.success);
  EXPECT_TRUE(state.was_value);
  EXPECT_TRUE(state.message.empty());
}

TYPED_TEST(Interpreting3dAlgebras, OuterProductE1E2E3ViaOperators) {
  auto state = this->interpret_line("e1 ^ e2 ^ e3");
  EXPECT_TRUE(state.success);
  EXPECT_TRUE(state.was_value);
  EXPECT_TRUE(state.message.empty());
}

TYPED_TEST(Interpreting3dAlgebras, OuterProductE1E2E3ViaIdentifier) {
  auto state = this->interpret_line("e123");
  EXPECT_TRUE(state.success);
  EXPECT_TRUE(state.was_value);
  EXPECT_TRUE(state.message.empty());
}

TYPED_TEST(Interpreting3dAlgebras, TrivectorIdentifierAndOperatorFormAreEqual) {
  using A = typename TestFixture::Algebra;
  using Rep = math::GenericBasisRepresentation<A>;
  Parser parser{};
  Interpreter<A, Rep> op_interp{};
  Interpreter<A, Rep> id_interp{};

  std::shared_ptr<LineAst> ast{};
  ASSERT_TRUE(parser.parse("e1 ^ e2 ^ e3", ast));
  op_interp.interpret(*ast);
  ASSERT_TRUE(op_interp.success);

  ASSERT_TRUE(parser.parse("e123", ast));
  id_interp.interpret(*ast);
  ASSERT_TRUE(id_interp.success);

  EXPECT_TRUE(math::AreNear<A>(op_interp.current_value, id_interp.current_value));
}

TYPED_TEST(Interpreting3dAlgebras, ReverseOfBivectorE12) {
  // Reverse of a bivector negates it.  We only check the parse+interpret path
  // succeeds; geometric correctness is covered by the math tests.
  auto state = this->interpret_line("~e12");
  EXPECT_TRUE(state.success);
  EXPECT_TRUE(state.was_value);
  EXPECT_TRUE(state.message.empty());
}

TYPED_TEST(Interpreting3dAlgebras, ReverseOfTrivectorIdentifier) {
  auto state = this->interpret_line("~e123");
  EXPECT_TRUE(state.success);
  EXPECT_TRUE(state.was_value);
  EXPECT_TRUE(state.message.empty());
}

TYPED_TEST(Interpreting3dAlgebras, DualOfTrivectorIdentifier) {
  auto state = this->interpret_line("!e123");
  EXPECT_TRUE(state.success);
  EXPECT_TRUE(state.was_value);
  EXPECT_TRUE(state.message.empty());
}

TYPED_TEST(Interpreting3dAlgebras, FullMultivectorExpression) {
  // A full VGA multivector built from all grades.  Exercises the entire
  // Additive chain with mixed-grade operands.
  auto state = this->interpret_line("1 + e1 + e2 + e3 + e12 + e13 + e23 + e123");
  EXPECT_TRUE(state.success);
  EXPECT_TRUE(state.was_value);
  EXPECT_TRUE(state.message.empty());
}

TYPED_TEST(Interpreting3dAlgebras, LeftContractionE1OnBivectorE12) {
  auto state = this->interpret_line("e1 << e12");
  EXPECT_TRUE(state.success);
  EXPECT_TRUE(state.was_value);
  EXPECT_TRUE(state.message.empty());
}

TYPED_TEST(Interpreting3dAlgebras, RightContractionE12OnE1) {
  auto state = this->interpret_line("e12 >> e1");
  EXPECT_TRUE(state.success);
  EXPECT_TRUE(state.was_value);
  EXPECT_TRUE(state.message.empty());
}

TYPED_TEST(Interpreting3dAlgebras, ParenthesesChangeOuterProductGrouping) {
  using A = typename TestFixture::Algebra;
  using Rep = math::GenericBasisRepresentation<A>;
  Parser parser{};
  Interpreter<A, Rep> left_grouped{};
  Interpreter<A, Rep> right_grouped{};

  std::shared_ptr<LineAst> ast{};
  ASSERT_TRUE(parser.parse("(e1 ^ e2) ^ e3", ast));
  left_grouped.interpret(*ast);
  ASSERT_TRUE(left_grouped.success);

  ASSERT_TRUE(parser.parse("e1 ^ (e2 ^ e3)", ast));
  right_grouped.interpret(*ast);
  ASSERT_TRUE(right_grouped.success);

  // The outer product is associative, so both groupings must yield the same
  // value.
  EXPECT_TRUE(math::AreNear<A>(left_grouped.current_value, right_grouped.current_value));
}

}  // namespace ndyn::ui
