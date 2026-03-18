#pragma once

#include <memory>

#include "gtest/gtest.h"
#include "math/basis_representation.h"
#include "math/multivector_test_utils.h"
#include "ui/ast_printer.h"
#include "ui/interpreter.h"
#include "ui/parser.h"

namespace ndyn::ui {

template <typename AlgebraT, typename RepresentationT = math::BasisRepresentation<AlgebraT>>
::testing::AssertionResult MatchesValue(const std::string_view line,
                                        const typename AlgebraT::VectorType& expected,
                                        bool do_ast_print = false) {
  Parser parser{};
  std::shared_ptr<LineAst> ast{};
  if (!parser.parse(line, ast)) {
    return ::testing::AssertionFailure() << "Could not parse line";
  }

  if (do_ast_print) {
    print_ast(*ast);
  }

  Interpreter<AlgebraT, RepresentationT> interpreter{};
  interpreter.interpret(*ast);

  if (!interpreter.success) {
    return ::testing::AssertionFailure() << "Error during interpretation -- success was false.";
  }
  if (!interpreter.was_value) {
    return ::testing::AssertionFailure() << "Error during interpretation -- was_value was false.";
  }
  if (!interpreter.message.empty()) {
    return ::testing::AssertionFailure()
           << "Error during interpretation -- message not empty. message: '" << interpreter.message
           << "'";
  }

  return math::AreNear(interpreter.current_value, expected);
}

template <typename AlgebraT, typename RepresentationT = math::BasisRepresentation<AlgebraT>>
::testing::AssertionResult MatchesValue(const std::string_view line,
                                        const typename AlgebraT::ScalarType& expected,
                                        bool do_ast_print = false) {
  return MatchesValue<AlgebraT, RepresentationT>(line, typename AlgebraT::VectorType{expected},
                                                 do_ast_print);
}

}  // namespace ndyn::ui
