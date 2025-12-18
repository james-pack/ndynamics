#include "ui/interpreter.h"

#include "gtest/gtest.h"
#include "math/algebra.h"
#include "math/multivector.h"
#include "peglib.h"

namespace ndyn::ui {

using namespace ndyn::math;

TEST(InterpreterTest, CanParseExpectedInputs) {
  Interpreter<Vga<>> interpreter{};
  static constexpr auto a{VgaMultivector<>{1.f}};  // 1

  EXPECT_EQ(a, interpreter.eval(R"(1)"));
}

}  // namespace ndyn::ui
