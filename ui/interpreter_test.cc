#include "ui/interpreter.h"

#include "gtest/gtest.h"
#include "math/algebra.h"
#include "math/multivector.h"
#include "peglib.h"

namespace ndyn::ui {

using namespace ndyn::math;

TEST(InterpreterTest, CanEvalSimpleScalar) {
  static constexpr auto a{1.f};
  Interpreter<Vga<>> interpreter{};
  EvalResult<Vga<>> result{interpreter.eval(R"(1)")};
  LOG(INFO) << "result: " << result;
  EXPECT_EQ(a, result.as_scalar());
}

TEST(InterpreterTest, CanInterpretHelpCommand) {
  Interpreter<Vga<>> interpreter{};
  EvalResult<Vga<>> result{interpreter.eval(R"(help)")};
  LOG(INFO) << "result: " << result;
  EXPECT_EQ(Command::HELP, result.as_command());
}

TEST(InterpreterTest, CanInterpretExitCommand) {
  Interpreter<Vga<>> interpreter{};
  EvalResult<Vga<>> result{interpreter.eval(R"(exit)")};
  LOG(INFO) << "result: " << result;
  EXPECT_EQ(Command::EXIT, result.as_command());
}

TEST(InterpreterTest, CanInterpretDictCommand) {
  Interpreter<Vga<>> interpreter{};
  EvalResult<Vga<>> result{interpreter.eval(R"(dict)")};
  LOG(INFO) << "result: " << result;
  EXPECT_EQ(Command::DICT, result.as_command());
}

}  // namespace ndyn::ui
