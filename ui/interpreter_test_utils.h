#pragma once

#include <memory>

#include "glog/logging.h"
#include "gtest/gtest.h"
#include "math/generic_basis_representation.h"
#include "math/multivector_test_utils.h"
#include "ui/ast_printer.h"
#include "ui/interpreter.h"
#include "ui/parser.h"

namespace ndyn::ui {

template <typename Algebra, typename Representation = math::GenericBasisRepresentation<Algebra>>
::testing::AssertionResult MatchesValue(const std::string_view line,
                                        const typename Algebra::VectorType& expected,
                                        bool do_ast_print = false) {
  Parser parser{};
  Interpreter<Algebra, Representation> interpreter{};
  DLOG(INFO) << "interpreter symbol table on startup: " << interpreter.dump_symbol_table();

  std::shared_ptr<LineAst> ast{};
  if (!parser.parse(line, ast)) {
    return ::testing::AssertionFailure() << "Could not parse line";
  }

  if (do_ast_print) {
    print_ast(*ast);
  }

  interpreter.interpret(*ast);

  if (!interpreter.success) {
    return ::testing::AssertionFailure()
           << "Error during interpretation -- success was false. interpreter.message: "
           << interpreter.message;
  }
  if (!interpreter.was_value) {
    return ::testing::AssertionFailure() << "Error during interpretation -- was_value was false.";
  }
  if (!interpreter.message.empty()) {
    return ::testing::AssertionFailure()
           << "Error during interpretation -- message not empty. message: '" << interpreter.message
           << "'";
  }

  return math::AreNear<Algebra>(interpreter.current_value, expected)
         << "interpreter.current_value: " << Representation::to_string(interpreter.current_value)
         << ", expected: " << Representation::to_string(expected);
}

template <typename AlgebraT, typename RepresentationT = math::GenericBasisRepresentation<AlgebraT>>
::testing::AssertionResult MatchesValue(const std::string_view line,
                                        const typename AlgebraT::ScalarType& expected,
                                        bool do_ast_print = false) {
  return MatchesValue<AlgebraT, RepresentationT>(line, typename AlgebraT::VectorType{expected},
                                                 do_ast_print);
}

}  // namespace ndyn::ui
