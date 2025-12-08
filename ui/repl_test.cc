#include "gtest/gtest.h"
#include "ui/grammar.h"

namespace ndyn::ui {

TEST(ReplTest, CanGenerateParserForGeometricAlgebra) {
  auto parser{create_parser()};

  EXPECT_TRUE(static_cast<bool>(parser));
}

}  // namespace ndyn::ui
