#include "gtest/gtest.h"
#include "peglib.h"

namespace ndyn::third_party::peglib {

TEST(IntegrationTest, CanGenerateSimpleParser) {
  // Quick test to verify that the library has been integrated into the build and is usable.
  peg::parser parser{R"(
    # Grammar for Calculator...
    Additive    <- Multiplicative '+' Additive / Multiplicative
    Multiplicative   <- Primary '*' Multiplicative / Primary
    Primary     <- '(' Additive ')' / Number
    Number      <- < [0-9]+ >
    %whitespace <- [ \t]*
)"};

  EXPECT_TRUE(static_cast<bool>(parser));
}

}  // namespace ndyn::third_party::peglib
