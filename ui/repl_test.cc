#include "gtest/gtest.h"
#include "ui/grammar.h"

namespace ndyn::ui {

TEST(ReplTest, CanGenerateParserForGeometricAlgebra) {
  auto parser{create_parser()};

  EXPECT_TRUE(static_cast<bool>(parser));
}

TEST(ReplTest, CanParseExpectedInputs) {
  auto parser{create_parser()};

  ASSERT_TRUE(static_cast<bool>(parser));

  const char* inputs[] = {
      R"()",                      // Empty string
  };

  for (const auto& input : inputs) {
    EXPECT_TRUE(parser.parse(input)) << "Could not parse:\n\t" << input << "\n";
  }
}

}  // namespace ndyn::ui
