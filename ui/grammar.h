#pragma once

#include <memory>
#include <string>

#include "peglib.h"

namespace ndyn::ui {

struct Visitor;

struct Ast {
  virtual ~Ast() = default;
  virtual void visit(Visitor& v) = 0;
};

enum class UnaryOp { Plus, Minus };
std::string to_string(UnaryOp);

enum class BinaryOp { Add, Sub, Mult, Div, Outer, Inner };
std::string to_string(BinaryOp);

struct ExpressionAst : Ast {};

struct ScalarAst final : ExpressionAst {
  float value;

  explicit ScalarAst(float value) : value(value) {}

  void visit(Visitor& v) override;
};

struct IdentifierAst final : ExpressionAst {
  std::string name;

  explicit IdentifierAst(std::string_view name) : name(name) {}
  explicit IdentifierAst(std::string name) : name(std::move(name)) {}

  void visit(Visitor& v) override;
};

struct UnaryAst final : ExpressionAst {
  UnaryOp op;
  std::shared_ptr<ExpressionAst> operand;

  UnaryAst(UnaryOp op, std::shared_ptr<ExpressionAst> operand)
      : op(op), operand(std::move(operand)) {}

  void visit(Visitor& v) override;
};

struct BinaryOpAst final : ExpressionAst {
  BinaryOp op;

  BinaryOpAst(BinaryOp op) : op(op) {}

  void visit(Visitor& v) override;
};

struct BinaryAst final : ExpressionAst {
  std::shared_ptr<BinaryOpAst> op;
  std::shared_ptr<ExpressionAst> lhs;
  std::shared_ptr<ExpressionAst> rhs;

  BinaryAst(std::shared_ptr<BinaryOpAst> op, std::shared_ptr<ExpressionAst> lhs,
            std::shared_ptr<ExpressionAst> rhs)
      : op(op), lhs(std::move(lhs)), rhs(std::move(rhs)) {}

  void visit(Visitor& v) override;
};

struct StatementAst : Ast {};

struct CommandAst : StatementAst {};

struct DictCommandAst final : CommandAst {
  bool long_form;

  explicit DictCommandAst(bool long_form) : long_form(long_form) {}

  void visit(Visitor& v) override;
};

struct ExitCommandAst final : CommandAst {
  void visit(Visitor& v) override;
};

struct HelpCommandAst final : CommandAst {
  void visit(Visitor& v) override;
};

struct AssignmentAst final : StatementAst {
  std::string name;
  std::shared_ptr<ExpressionAst> value;

  AssignmentAst(std::string name, std::shared_ptr<ExpressionAst> value)
      : name(std::move(name)), value(std::move(value)) {}

  void visit(Visitor& v) override;
};

struct StatementExpressionAst final : StatementAst {
  std::shared_ptr<ExpressionAst> expression;

  explicit StatementExpressionAst(std::shared_ptr<ExpressionAst> expression)
      : expression(std::move(expression)) {}

  void visit(Visitor& v) override;
};

struct LineAst final : Ast {
  std::shared_ptr<StatementAst> statement;  // null = empty line

  explicit LineAst(std::shared_ptr<StatementAst> statement) : statement(std::move(statement)) {}

  void visit(Visitor& v) override;
};

struct Visitor {
  virtual ~Visitor() = default;

  virtual void visit(LineAst&) = 0;

  virtual void visit(StatementExpressionAst&) = 0;
  virtual void visit(AssignmentAst&) = 0;

  virtual void visit(ScalarAst&) = 0;
  virtual void visit(IdentifierAst&) = 0;
  virtual void visit(UnaryAst&) = 0;
  virtual void visit(BinaryAst&) = 0;

  virtual void visit(DictCommandAst&) = 0;
  virtual void visit(ExitCommandAst&) = 0;
  virtual void visit(HelpCommandAst&) = 0;

  virtual void visit(ExpressionAst&);
  virtual void visit(CommandAst&);
  virtual void visit(StatementAst&);
  virtual void visit(BinaryOpAst&);
};

peg::parser create_parser(std::shared_ptr<LineAst>& result);

class Grammar final {
 private:
  std::shared_ptr<LineAst> result_{};
  peg::parser parser_;

 public:
  Grammar() : parser_(create_parser(result_)) {}

  bool parse(const std::string& line, std::shared_ptr<LineAst>& result) {
    bool success = parser_.parse(line);
    result = result_;
    result_.reset();
    return success;
  }
};

}  // namespace ndyn::ui
