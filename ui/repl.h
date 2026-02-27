#pragma once

#include <iostream>
#include <string>

#include "gflags/gflags.h"
#include "ui/ast_printer.h"
#include "ui/interpreter.h"
#include "ui/parser.h"

DECLARE_bool(show_ast);

namespace ndyn::ui {

template <typename AlgebraT, typename RepresentationT>
class Repl final {
 public:
  using AlgebraType = AlgebraT;
  using RepresentationType = RepresentationT;

 private:
  std::string line_{};
  std::shared_ptr<LineAst> ast_{};
  Parser parser_{};
  Interpreter<AlgebraType, RepresentationType> interpreter_{};

 public:
  void prompt() const { std::cout << "% " << std::flush; }

  void read() { std::getline(std::cin, line_); }

  void eval() {
    bool success = parser_.parse(line_, ast_);
    if (success) {
      if (FLAGS_show_ast) {
        print_ast(*ast_);
      }
      interpreter_.interpret(*ast_);
    }
  }

  void print() const {
    if (!interpreter_.success) {
      std::cout << "[FAIL] ";
    }
    if (interpreter_.was_value) {
      std::cout << to_string(interpreter_.current_value);
    }
    if (!interpreter_.message.empty()) {
      std::cout << interpreter_.message;
    }
    std::cout << "\n";
  }

  bool run_once() {
    prompt();
    read();
    eval();
    print();
    return true;
  }

  void loop() {
    while (run_once()) {
    }
  }
};

}  // namespace ndyn::ui
