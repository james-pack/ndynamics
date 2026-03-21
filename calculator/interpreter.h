#pragma once

#include <stdexcept>
#include <string>
#include <unordered_map>

#include "base/except.h"
#include "calculator/parser.h"
#include "math/basis_representation.h"

namespace ndyn::ui {

template <typename AlgebraType, math::BasisRepresentation Representation>
class Interpreter final : public Visitor {
  using VectorType = typename AlgebraType::VectorType;
  using ScalarType = typename AlgebraType::ScalarType;

 public:
  Interpreter() {
    for (auto iter = Representation::bases_begin(); iter != Representation::bases_end(); ++iter) {
      symbols.insert({iter->name, iter->basis});
    }
  }

  // Global symbol table
  std::unordered_map<std::string, VectorType> symbols{};

  // Result of the last evaluated expression.
  VectorType current_value{};
  std::string message{};
  bool success{true};
  bool was_value{true};

  void visit(LineAst& node) override {
    if (node.statement) {
      node.statement->visit(*this);
      was_value = true;
    } else if (node.command) {
      node.command->visit(*this);
      was_value = false;
    } else {
      was_value = false;
    }
    DLOG(INFO) << "Line";
  }

  void visit(StatementAst& node) override {
    if (node.expression) {
      node.expression->visit(*this);
    } else if (node.assignment) {
      node.assignment->visit(*this);
    }
    DLOG(INFO) << "Statement";
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

  void visit(ParentheticalAst& node) override {
    node.operand->visit(*this);

    DLOG(INFO) << "Parenthetical";
  }

  void visit(UnaryAst& node) override {
    node.operand->visit(*this);

    DLOG(INFO) << "Unary";
    switch (node.op->op) {
      case UnaryOp::Plus:
        // No-op. Nothing gets changed.
        break;
      case UnaryOp::Minus:
        current_value = -1 * current_value;
        break;
      case UnaryOp::Dual:
        current_value = current_value.dual();
        break;
      case UnaryOp::Reverse:
        current_value = current_value.reverse();
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
      case BinaryOp::LeftContract:
        current_value = lhs.left_contraction(rhs);
        break;
      case BinaryOp::RightContract:
        current_value = lhs.right_contraction(rhs);
        break;
      default:
        except<std::logic_error>("Unsupported binary operation: " + to_string(node.op->op));
    }
  }

  std::string dump_symbol_table() const {
    std::string result{};
    if (!symbols.empty()) {
      for (const auto& [name, value] : symbols) {
        result.append("  ");
        result.append(name);
        result.append("\t=\t").append(Representation::to_string(value));
        result.append("\n");
      }
    } else {
      result = "    <empty symbol table>\n";
    }
    return result;
  }

  void visit(DictCommandAst& node) override { message.append(dump_symbol_table()); }

  void visit(ExitCommandAst&) override { std::exit(0); }

  void visit(HelpCommandAst&) override {
    message.append("Commands:\n")
        .append("  help|?        Show this help.\n")
        .append("  exit          Exit the calculator.\n")
        .append("  dict          Show defined symbols.\n");
  }

  void interpret(Ast& node) {
    // Mark as failure where failure occurs.
    success = true;
    // Clear out the message so that previous error messages don't get reported in this run.
    message.clear();
    node.visit(*this);
  }
};

}  // namespace ndyn::ui
