#pragma once

#include <stdexcept>
#include <string>
#include <unordered_map>

#include "base/except.h"
#include "ui/grammar.h"
#include "ui/representations.h"

namespace ndyn::ui {

template <typename AlgebraType>
struct Interpreter : Visitor {
  using VectorType = typename AlgebraType::VectorType;
  using ScalarType = typename AlgebraType::ScalarType;

  // Global symbol table
  std::unordered_map<std::string, VectorType> symbols{};

  // Result of the last evaluated expression.
  VectorType current_value{};
  std::string message{};
  bool success{true};
  bool was_value{true};

  // Hooks for value semantics.
  VectorType add(const VectorType& a, const VectorType& b) { return a + b; }

  VectorType sub(const VectorType& a, const VectorType& b) { return a - b; }

  VectorType mul(const VectorType& a, const VectorType& b) { return a * b; }

  // VectorType div(const VectorType& a, const VectorType& b) { return a / b; }

  VectorType outer(const VectorType& a, const VectorType& b) { return a.outer(b); }

  VectorType inner(const VectorType& a, const VectorType& b) { return a.inner(b); }

  VectorType neg(const VectorType& v) { return -v; }

  void visit(LineAst& node) override {
    if (node.statement) {
      node.statement->visit(*this);
    }
  }

  void visit(StatementExpressionAst& node) override {
    node.expression->visit(*this);
    symbols["_"] = current_value;
  }

  void visit(AssignmentAst& node) override {
    node.value->visit(*this);
    symbols[node.name] = current_value;
  }

  void visit(ScalarAst& node) override { current_value = VectorType{node.value}; }

  void visit(IdentifierAst& node) override {
    auto it = symbols.find(node.name);
    if (it != symbols.end()) {
      current_value = it->second;
    } else {
      success = false;
      was_value = false;
      message = "Unknown identifier: " + node.name;
    }
  }

  void visit(UnaryAst& node) override {
    node.operand->visit(*this);

    switch (node.op) {
      case UnaryOp::Plus:
        // No-op. Nothing gets changed.
        break;
      case UnaryOp::Minus:
        current_value = neg(current_value);
        break;
    }
  }

  void visit(BinaryAst& node) override {
    node.lhs->visit(*this);
    VectorType lhs = current_value;
    node.rhs->visit(*this);
    VectorType rhs = current_value;

    switch (node.op->op) {
      case BinaryOp::Add:
        current_value = add(lhs, rhs);
        break;
      case BinaryOp::Sub:
        current_value = sub(lhs, rhs);
        break;
      case BinaryOp::Mult:
        current_value = mul(lhs, rhs);
        break;
      // case BinaryOp::Div:
      //   current_value = div(lhs, rhs);
      //   break;
      case BinaryOp::Outer:
        current_value = outer(lhs, rhs);
        break;
      case BinaryOp::Inner:
        current_value = inner(lhs, rhs);
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
