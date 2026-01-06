#pragma once

#include <iostream>
#include <string>

#include "ui/interpreter.h"

namespace ndyn::ui {

template <typename AlgebraT>
class Repl final {
 public:
  using AlgebraType = AlgebraT;

 private:
  Interpreter<AlgebraType> interpreter_{};

 public:
  void prompt() const { std::cout << "% " << std::flush; }

  std::string read() const {
    std::string line;
    std::getline(std::cin, line);
    return line;
  }

  EvalResult<AlgebraType> eval(const std::string& line) { return interpreter_.eval(line); }

  void print(const EvalResult<AlgebraType>& result) const {
    std::string output{to_string(result)};
    std::cout << output << "\n";
  }

  void run_once() {
    prompt();
    std::string line{read()};
    EvalResult<AlgebraType> result{eval(line)};
    print(result);
  }

  [[noreturn]] void loop() {
    while (true) {
      run_once();
    }
  }
};

}  // namespace ndyn::ui
