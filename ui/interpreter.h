#pragma once

#include <stdexcept>
#include <string>
#include <unordered_map>

#include "base/except.h"
#include "math/basis_representation.h"
#include "ui/parser.h"

namespace ndyn::ui {

template <typename AlgebraType>
class Interpreter final : public Visitor {
  using VectorType = typename AlgebraType::VectorType;
  using ScalarType = typename AlgebraType::ScalarType;
  using BasisRepresentation = math::BasisRepresentation<AlgebraType>;

  // Global symbol table
  std::unordered_map<std::string, VectorType> symbols{};

 public:
  Interpreter() {
    for (const auto& base : BasisRepresentation::bases()) {
      symbols.insert({base.name, base.basis});
    }
  }

  // Result of the last evaluated expression.
  VectorType current_value{};
  std::string message{};
  bool success{true};
  bool was_value{true};

  void visit(LineAst& node) override {
    if (node.statement) {
      node.statement->visit(*this);
    }
    DLOG(INFO) << "Line";
  }

  void visit(StatementExpressionAst& node) override {
    node.expression->visit(*this);
    DLOG(INFO) << "StatementExpression";
    symbols["_"] = current_value;
  }

  void visit(AssignmentAst& node) override {
    node.name->visit(*this);
    node.value->visit(*this);
    symbols[node.name->name] = current_value;
  }

  void visit(ScalarAst& node) override { current_value = VectorType{node.value}; }

  void visit(IdentifierAst& node) override {
    DLOG(INFO) << "Identifier -- node->name: " << node.name;
  }

  void visit(RvalueAst& node) override {
    node.identifier->visit(*this);

    DLOG(INFO) << "Rvalue -- node.identifier->name: " << node.identifier->name;
    auto it = symbols.find(node.identifier->name);
    if (it != symbols.end()) {
      current_value = it->second;
    } else {
      success = false;
      was_value = false;
      message = "Unknown identifier: " + node.identifier->name;
    }
  }

  void visit(UnaryAst& node) override {
    node.operand->visit(*this);

    DLOG(INFO) << "Unary";
    switch (node.op) {
      case UnaryOp::Plus:
        // No-op. Nothing gets changed.
        break;
      case UnaryOp::Minus:
        current_value = -1 * current_value;
        break;
    }
  }

  void visit(BinaryAst& node) override {
    node.lhs->visit(*this);
    VectorType lhs = current_value;
    node.rhs->visit(*this);
    VectorType rhs = current_value;

    DLOG(INFO) << "Binary";

    switch (node.op->op) {
      case BinaryOp::Add:
        current_value = lhs + rhs;
        break;
      case BinaryOp::Sub:
        current_value = lhs - rhs;
        break;
      case BinaryOp::Mult:
        current_value = lhs * rhs;
        break;
      // case BinaryOp::Div:
      //   current_value = lhs / rhs;
      //   break;
      case BinaryOp::Outer:
        current_value = lhs.outer(rhs);
        break;
      case BinaryOp::Inner:
        current_value = lhs.inner(rhs);
        break;
      default:
        except<std::logic_error>("Unsupported binary operation: " + to_string(node.op->op));
    }
  }

  void visit(DictCommandAst& node) override {
    for (const auto& [name, value] : symbols) {
      message.append("  ");
      message.append(name);
      if (node.long_form) {
        message.append("\t=\t").append(to_string(value));
      }
      message.append("\n");
    }
    was_value = false;
  }

  void visit(ExitCommandAst&) override { std::exit(0); }

  void visit(HelpCommandAst&) override {
    message.append("Commands:\n")
        .append("  help        Show this help\n")
        .append("  exit        Exit the REPL\n")
        .append("  dict [-l]   Show defined symbols\n");
    was_value = false;
  }

  void interpret(Ast& node) {
    // Mark as failure where failure occurs.
    success = true;
    // Assume result will be a value.
    was_value = true;
    // Clear out the message so that previous error messages don't get reported in this run.
    message.clear();
    node.visit(*this);
  }
};

}  // namespace ndyn::ui
