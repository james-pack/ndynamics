#include "gtest/gtest.h"
#include "ui/grammar.h"

namespace ndyn::ui {

TEST(ReplTest, CanGenerateParserForGeometricAlgebra) {
  auto parser{create_parser()};

  EXPECT_TRUE(static_cast<bool>(parser));
}

TEST(ReplTest, CanParseExpectedInputs) {
  const auto parser{create_parser()};

  const char* inputs[] = {

      // Whitespace and empty string checks.
      R"()",
      R"( )",
      "\t",
      " \t ",

      // Algebra declarations
      R"(algebra = i)",
      R"(algebra = i, j, k)",
      R"(algebra = e1, e2, e3)",

      // Algebra declarations using simple metric form.
      R"(algebra = e1:1, e2:1, e3:1)",
      R"(algebra = e1:1, e2:-1, e3:1)",
      R"(algebra = e1, e2:-1, e3:1)",
      R"(algebra = e1, e2:-1, e3:0)",

      // Algebra declarations using matrix metric form.
      R"(algebra = e1, e2, e3; metric = [ [1, 0.5, 0], [0.5, 1, 0], [0, 0, -1] ])",

      // Expressions involving various forms of numbers.
      R"(1)",
      R"(1.1)",
      R"(1.1e10)",
      R"(1.1e+10)",
      R"(1.1e-10)",
      R"(-1)",
      R"(-1.1)",
      R"(-1.1e10)",
      R"(-1.1e+10)",
      R"(-1.1e-10)",

      // Variables
      R"(b)",
      R"(apple)",
      R"(app13)",
      R"(boy)",
      R"(b0y)",

      // Binary operators
      R"(b + c)",
      R"(b - c)",
      R"(b * c)",
      R"(b ^ c)",
      R"(b | c)",
      R"(b * e1)",

      // Unary operators
      R"(-b)",
      R"((-b))",
      R"(+(-b))",
      R"(-(-b))",
      R"(+b)",
      R"((+b))",
      R"(+(+b))",
      R"(-(+b))",
  };

  for (const auto& input : inputs) {
    EXPECT_TRUE(parser.parse(input)) << "Could not parse:\n\t" << input << "\n";
  }
}

TEST(ReplTest, RefusesInvalidInputs) {
  auto parser{create_parser()};

  ASSERT_TRUE(static_cast<bool>(parser));

  const char* inputs[] = {
      // Shouldn't allow a ':' after the basis name without a metric value.
      R"(algebra = e1:, e2:-1, e3:1)",
      // Shouldn't allow decimal values in metrics when expressed in simple form.
      R"(algebra = e1:0.5, e2:-1, e3:1)",
      // Multiple unary operators without parentheses should be disallowed.
      R"(a = + -b)",
      // Disallow multiplication without an explicit operator. The alternative gets difficult to
      // parse when creating a multivector from bases.
      R"(a = b e1)",
  };

  for (const auto& input : inputs) {
    EXPECT_FALSE(parser.parse(input)) << "Allowed invalid input:\n\t" << input << "\n";
  }
}

}  // namespace ndyn::ui
