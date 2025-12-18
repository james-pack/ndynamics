#include "ui/grammar.h"

#include "gtest/gtest.h"
#include "peglib.h"

namespace ndyn::ui {

TEST(GrammarTest, CanGenerateParserForGeometricAlgebra) {
  auto parser{create_parser()};

  EXPECT_TRUE(static_cast<bool>(parser));
}

TEST(GrammarTest, CanParseExpectedInputs) {
  const auto parser{create_parser()};

  const char* inputs[] = {

      // Whitespace and empty string checks.
      R"()",
      R"( )",
      "\t",
      " \t ",

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

      // Multiple operators
      R"(-b + b)",
      R"((-b) + b)",
      R"(+(-b) * b)",
      R"(a + -b + b)",
      R"(a - (-b) + b)",
      R"(a - +(-b) * b)",
  };

  for (const auto& input : inputs) {
    EXPECT_TRUE(parser.parse(input)) << "Could not parse:\n\t" << input << "\n";
  }
}

TEST(GrammarTest, RefusesInvalidInputs) {
  const auto parser{create_parser()};

  const char* inputs[] = {
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

TEST(GrammarTest, CanAttachActions) {
  auto parser{create_parser()};
  ASSERT_TRUE(static_cast<bool>(parser));

  float value{};
  parser["Scalar"] = [&value](const peg::SemanticValues& sv) {
    value = sv.token_to_number<float>();
    return value;
  };

  parser.parse("123.456");
  EXPECT_FLOAT_EQ(123.456, value);
}

}  // namespace ndyn::ui
