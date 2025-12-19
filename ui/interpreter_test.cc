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

TEST(InterpreterTest, CanAssignToIdentifier) {
  static constexpr auto a{1.f};
  Interpreter<Vga<>> interpreter{};
  EvalResult<Vga<>> result{interpreter.eval(R"(a = 1)")};
  LOG(INFO) << "result: " << result;
  EXPECT_EQ(a, result.as_scalar());
}

TEST(InterpreterTest, CanEvalIdentifiers) {
  static constexpr auto a{1.f};
  Interpreter<Vga<>> interpreter{};
  EvalResult<Vga<>> result1{interpreter.eval(R"(a = 1)")};
  ASSERT_EQ(a, result1.as_scalar());
  EvalResult<Vga<>> result2{interpreter.eval(R"(a)")};
  EXPECT_EQ(a, result2.as_scalar());
}

TEST(InterpreterTest, StoresResultOfLastEvaluation) {
  static constexpr auto a{1.f};
  Interpreter<Vga<>> interpreter{};

  const char* expressions[] = {
      R"(1)",      //
      R"(_)",      //
      R"(a = 1)",  //
      R"(_)",      //
      R"(4-3)",    //
      R"(_)",      //
  };

  for (const auto& expression : expressions) {
    EvalResult<Vga<>> result{interpreter.eval(expression)};
    EXPECT_EQ(a, result.as_scalar());
  }
}

TEST(InterpreterTest, CanAddScalars) {
  static constexpr auto a{1.f};
  static constexpr auto b{2.f};
  Interpreter<Vga<>> interpreter{};

  const char* expressions[] = {
      R"(1 + 2)",   //
      R"(1+2)",     //
      R"(1 +2)",    //
      R"(1+ 2)",    //
      R"( 1 + 2)",  //
      R"( 1+2)",    //
      R"( 1 +2)",   //
      R"( 1+ 2)",   //
      R"(1 + 2 )",  //
      R"(1+2 )",    //
      R"(1 +2 )",   //
      R"(1+ 2 )",   //
  };
  for (const auto& expression : expressions) {
    EvalResult<Vga<>> result{interpreter.eval(expression)};
    EXPECT_EQ(a + b, result.as_scalar());
  }
}

TEST(InterpreterTest, CanSubtractScalars) {
  static constexpr auto a{4.f};
  static constexpr auto b{3.f};
  Interpreter<Vga<>> interpreter{};

  const char* expressions[] = {
      R"(4 - 3)",   //
      R"(4-3)",     //
      R"(4 -3)",    //
      R"(4- 3)",    //
      R"( 4 - 3)",  //
      R"( 4-3)",    //
      R"( 4 -3)",   //
      R"( 4- 3)",   //
      R"(4 - 3 )",  //
      R"(4-3 )",    //
      R"(4 -3 )",   //
      R"(4- 3 )",   //
  };
  for (const auto& expression : expressions) {
    EvalResult<Vga<>> result{interpreter.eval(expression)};
    EXPECT_EQ(a - b, result.as_scalar());
  }
}

TEST(InterpreterTest, CanMultiplyScalars) {
  static constexpr auto a{4.f};
  static constexpr auto b{3.f};
  Interpreter<Vga<>> interpreter{};

  const char* expressions[] = {
      R"(4 * 3)",   //
      R"(4*3)",     //
      R"(4 *3)",    //
      R"(4* 3)",    //
      R"( 4 * 3)",  //
      R"( 4*3)",    //
      R"( 4 *3)",   //
      R"( 4* 3)",   //
      R"(4 * 3 )",  //
      R"(4*3 )",    //
      R"(4 *3 )",   //
      R"(4* 3 )",   //
  };
  for (const auto& expression : expressions) {
    EvalResult<Vga<>> result{interpreter.eval(expression)};
    EXPECT_EQ(a * b, result.as_scalar());
  }
}

TEST(InterpreterTest, CanDivideScalars) {
  static constexpr auto a{4.f};
  static constexpr auto b{3.f};
  Interpreter<Vga<>> interpreter{};

  const char* expressions[] = {
      R"(4 / 3)",   //
      R"(4/3)",     //
      R"(4 /3)",    //
      R"(4/ 3)",    //
      R"( 4 / 3)",  //
      R"( 4/3)",    //
      R"( 4 /3)",   //
      R"( 4/ 3)",   //
      R"(4 / 3 )",  //
      R"(4/3 )",    //
      R"(4 /3 )",   //
      R"(4/ 3 )",   //
  };
  for (const auto& expression : expressions) {
    EvalResult<Vga<>> result{interpreter.eval(expression)};
    EXPECT_EQ(a / b, result.as_scalar());
  }
}

}  // namespace ndyn::ui
