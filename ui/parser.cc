#include "ui/parser.h"

#include <memory>
#include <string>

#include "base/except.h"
#include "glog/logging.h"
#include "peglib.h"

namespace ndyn::ui {

const char grammar_definition[] = R"GRAMMAR(
Line <- _ (Statement _)?

Statement <- Assignment
           / Expression
           / Command

Assignment <- Identifier _ "=" _ Expression

Identifier <- !Keyword [a-zA-Z_][a-zA-Z0-9_]*

Keyword <- Command / "algebra" / "metric"

Expression <- Additive

Additive <- Multiplicative ( _ ( AddOp / SubOp ) _ Additive )?
AddOp <- "+"
SubOp <- "-"

Multiplicative <- Unary ( _ ( MultOp / DivOp / OuterOp / InnerOp ) _ Multiplicative )?
MultOp <- "*"
DivOp <- "/"
OuterOp <- "^"
InnerOp <- "|"

Unary <- [+-]? Primary

Primary <- Scalar
         / RValue
         / Parenthetical

Parenthetical <- "(" _ Expression _ ")"

RValue <- Identifier

Scalar <- [+-]? [0-9]+ ("." [0-9]+)?

Command <- DictCommand / ExitCommand / HelpCommand
DictCommand <- "dict" ( _ ("-l" / "--long") )?
ExitCommand <- "exit" / "quit"
HelpCommand <- "help"

_ <- [ \t]*
)GRAMMAR";

std::string to_string(UnaryOp op) {
  switch (op) {
    case UnaryOp::Plus:
      return "+";
    case UnaryOp::Minus:
      return "-";
  }
  return "<unknown-unary-op>";
}

std::string to_string(BinaryOp op) {
  switch (op) {
    case BinaryOp::Add:
      return "+";
    case BinaryOp::Sub:
      return "-";
    case BinaryOp::Mult:
      return "*";
    case BinaryOp::Div:
      return "/";
    case BinaryOp::Outer:
      return "^";
    case BinaryOp::Inner:
      return "|";
  }
  return "<unknown-binary-op>";
}

void attach_actions(peg::parser& p, std::shared_ptr<LineAst>& result) {
  p["Scalar"] = [](const peg::SemanticValues& sv) -> std::shared_ptr<ExpressionAst> {
    DLOG(INFO) << "[Scalar] -- sv.size(): " << sv.size();
    DLOG(INFO) << "[Scalar] -- sv.token_to_number<float>(): " << sv.token_to_number<float>();
    return std::make_shared<ScalarAst>(sv.token_to_number<float>());
  };

  p["Identifier"] = [](const peg::SemanticValues& sv) -> std::shared_ptr<IdentifierAst> {
    DLOG(INFO) << "[Identifier] -- sv.size(): " << sv.size();
    DLOG(INFO) << "[Identifier] -- sv.token(): " << sv.token();
    return std::make_shared<IdentifierAst>(sv.token());
  };

  p["RValue"] = [](const peg::SemanticValues& sv) -> std::shared_ptr<ExpressionAst> {
    DLOG(INFO) << "[RValue] -- sv.size(): " << sv.size();
    auto identifier = std::any_cast<std::shared_ptr<IdentifierAst>>(sv[0]);
    return std::make_shared<RvalueAst>(identifier);
  };

  p["Parenthetical"] = [](const peg::SemanticValues& sv) -> std::shared_ptr<ExpressionAst> {
    DLOG(INFO) << "[Parenthetical] -- sv.size(): " << sv.size();
    return std::any_cast<std::shared_ptr<ExpressionAst>>(sv[1]);
  };

  p["Unary"] = [](const peg::SemanticValues& sv) -> std::shared_ptr<ExpressionAst> {
    DLOG(INFO) << "[Unary] -- sv.size(): " << sv.size();
    if (sv.size() == 1) {
      return std::any_cast<std::shared_ptr<ExpressionAst>>(sv[0]);
    }
    char op = std::any_cast<std::string>(sv[0])[0];
    auto operand = std::any_cast<std::shared_ptr<ExpressionAst>>(sv[1]);
    return std::make_shared<UnaryAst>(op == '+' ? UnaryOp::Plus : UnaryOp::Minus,
                                      std::move(operand));
  };

  p["MultOp"] = [](const peg::SemanticValues& sv) -> std::shared_ptr<BinaryOpAst> {
    DLOG(INFO) << "[MultOp] -- sv.size(): " << sv.size();
    DLOG(INFO) << "[MultOp] -- sv.token: '" << sv.token() << "'";
    return std::make_shared<BinaryOpAst>(BinaryOp::Mult);
  };

  p["DivOp"] = [](const peg::SemanticValues& sv) -> std::shared_ptr<BinaryOpAst> {
    DLOG(INFO) << "[DivOp] -- sv.size(): " << sv.size();
    DLOG(INFO) << "[DivOp] -- sv.token: '" << sv.token() << "'";
    return std::make_shared<BinaryOpAst>(BinaryOp::Div);
  };

  p["AddOp"] = [](const peg::SemanticValues& sv) -> std::shared_ptr<BinaryOpAst> {
    DLOG(INFO) << "[AddOp] -- sv.size(): " << sv.size();
    DLOG(INFO) << "[AddOp] -- sv.token: '" << sv.token() << "'";
    return std::make_shared<BinaryOpAst>(BinaryOp::Add);
  };

  p["SubOp"] = [](const peg::SemanticValues& sv) -> std::shared_ptr<BinaryOpAst> {
    DLOG(INFO) << "[SubOp] -- sv.size(): " << sv.size();
    DLOG(INFO) << "[SubOp] -- sv.token: '" << sv.token() << "'";
    return std::make_shared<BinaryOpAst>(BinaryOp::Sub);
  };

  p["InnerOp"] = [](const peg::SemanticValues& sv) -> std::shared_ptr<BinaryOpAst> {
    DLOG(INFO) << "[InnerOp] -- sv.size(): " << sv.size();
    DLOG(INFO) << "[InnerOp] -- sv.token: '" << sv.token() << "'";
    return std::make_shared<BinaryOpAst>(BinaryOp::Inner);
  };

  p["OuterOp"] = [](const peg::SemanticValues& sv) -> std::shared_ptr<BinaryOpAst> {
    DLOG(INFO) << "[OuterOp] -- sv.size(): " << sv.size();
    DLOG(INFO) << "[OuterOp] -- sv.token: '" << sv.token() << "'";
    return std::make_shared<BinaryOpAst>(BinaryOp::Outer);
  };

  p["Multiplicative"] = [](const peg::SemanticValues& sv) -> std::shared_ptr<ExpressionAst> {
    DLOG(INFO) << "[Multiplicative] -- sv.size(): " << sv.size();
    auto lhs = std::any_cast<std::shared_ptr<ExpressionAst>>(sv[0]);
    if (sv.size() == 1) {
      return lhs;
    }
    auto op = std::any_cast<std::shared_ptr<BinaryOpAst>>(sv[2]);
    auto rhs = std::any_cast<std::shared_ptr<ExpressionAst>>(sv[4]);
    return std::make_shared<BinaryAst>(op, lhs, rhs);
  };

  p["Additive"] = [](const peg::SemanticValues& sv) -> std::shared_ptr<ExpressionAst> {
    DLOG(INFO) << "[Additive] -- sv.size(): " << sv.size();
    auto lhs = std::any_cast<std::shared_ptr<ExpressionAst>>(sv[0]);
    if (sv.size() == 1) {
      return lhs;
    }
    auto op = std::any_cast<std::shared_ptr<BinaryOpAst>>(sv[2]);
    auto rhs = std::any_cast<std::shared_ptr<ExpressionAst>>(sv[4]);
    return std::make_shared<BinaryAst>(op, lhs, rhs);
  };

  p["Expression"] = [](const peg::SemanticValues& sv) -> std::shared_ptr<ExpressionAst> {
    DLOG(INFO) << "[Expression] -- sv.size(): " << sv.size();
    return std::any_cast<std::shared_ptr<ExpressionAst>>(sv[0]);
  };

  p["Assignment"] = [](const peg::SemanticValues& sv) -> std::shared_ptr<AssignmentAst> {
    DLOG(INFO) << "[Assignment] -- sv.size(): " << sv.size();
    auto name = std::any_cast<std::shared_ptr<IdentifierAst>>(sv[0]);
    auto value = std::any_cast<std::shared_ptr<ExpressionAst>>(sv[3]);
    return std::make_shared<AssignmentAst>(name, value);
  };

  p["DictCommand"] = [](const peg::SemanticValues& sv) -> std::shared_ptr<CommandAst> {
    DLOG(INFO) << "[DictCommand] -- sv.size(): " << sv.size();
    bool long_form = !sv.empty() && sv.size() > 1;
    return std::make_shared<DictCommandAst>(long_form);
  };

  p["ExitCommand"] = [](const peg::SemanticValues& sv) -> std::shared_ptr<CommandAst> {
    DLOG(INFO) << "[ExitCommand] -- sv.size(): " << sv.size();
    return std::make_shared<ExitCommandAst>();
  };

  p["HelpCommand"] = [](const peg::SemanticValues& sv) -> std::shared_ptr<CommandAst> {
    DLOG(INFO) << "[HelpCommand] -- sv.size(): " << sv.size();
    return std::make_shared<HelpCommandAst>();
  };

  p["Command"] = [](const peg::SemanticValues& sv) -> std::shared_ptr<CommandAst> {
    DLOG(INFO) << "[Command] -- sv.size(): " << sv.size();
    return std::any_cast<std::shared_ptr<CommandAst>>(sv[0]);
  };

  p["Statement"] = [](const peg::SemanticValues& sv) -> std::shared_ptr<StatementAst> {
    DLOG(INFO) << "[Statement] -- sv.size(): " << sv.size();
    if (sv.size() == 1) {
      if (sv[0].type() == typeid(std::shared_ptr<AssignmentAst>)) {
        return std::any_cast<std::shared_ptr<AssignmentAst>>(sv[0]);
      } else if (sv[0].type() == typeid(std::shared_ptr<ExpressionAst>)) {
        return std::make_shared<StatementExpressionAst>(
            std::any_cast<std::shared_ptr<ExpressionAst>>(sv[0]));
      } else if (sv[0].type() == typeid(std::shared_ptr<CommandAst>)) {
        return std::any_cast<std::shared_ptr<CommandAst>>(sv[0]);
      } else {
        except<std::logic_error>("Unknown statement type");
      }
    }
    return nullptr;
  };

  p["Line"] = [&result](const peg::SemanticValues& sv) -> std::shared_ptr<LineAst> {
    DLOG(INFO) << "[Line] -- sv.size(): " << sv.size();
    if (!sv.empty()) {
      std::shared_ptr<StatementAst> stmt = std::any_cast<std::shared_ptr<StatementAst>>(sv[1]);
      result = std::make_shared<LineAst>(stmt);
    } else {
      result.reset();
    }
    return result;
  };
}

peg::parser create_raw_parser(std::shared_ptr<LineAst>& result) {
  peg::parser parser{grammar_definition};

  attach_actions(parser, result);

  return parser;
}

void Visitor::visit(ExpressionAst&) { /* Do nothing by default but can be overridden. */ }
void Visitor::visit(CommandAst&) { /* Do nothing by default but can be overridden. */ }
void Visitor::visit(StatementAst&) { /* Do nothing by default but can be overridden. */ }
void Visitor::visit(BinaryOpAst&) { /* Do nothing by default but can be overridden. */ }

void ScalarAst::visit(Visitor& v) { v.visit(*this); }
void IdentifierAst::visit(Visitor& v) { v.visit(*this); }
void RvalueAst::visit(Visitor& v) { v.visit(*this); }
void UnaryAst::visit(Visitor& v) { v.visit(*this); }
void BinaryAst::visit(Visitor& v) { v.visit(*this); }

void BinaryOpAst::visit(Visitor& v) { v.visit(*this); }

void DictCommandAst::visit(Visitor& v) { v.visit(*this); }
void ExitCommandAst::visit(Visitor& v) { v.visit(*this); }
void HelpCommandAst::visit(Visitor& v) { v.visit(*this); }

void StatementExpressionAst::visit(Visitor& v) { v.visit(*this); }
void AssignmentAst::visit(Visitor& v) { v.visit(*this); }

void LineAst::visit(Visitor& v) { v.visit(*this); }

}  // namespace ndyn::ui
