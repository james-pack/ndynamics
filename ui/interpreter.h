#pragma once

#include <any>
#include <ostream>
#include <string>
#include <string_view>
#include <typeinfo>
#include <unordered_map>

#include "math/multivector.h"
#include "peglib.h"
#include "ui/grammar.h"

namespace ndyn::ui {

enum class Command {
  HELP,
  EXIT,
  DICT,
};

enum class Op {
  ADD,
  SUB,
  MULT,
  DIV,
  OUTER,
  INNER,
};

template <typename AlgebraT>
class EvalResult final {
 public:
  using MultivectorT = typename AlgebraT::VectorType;
  using ScalarT = typename AlgebraT::ScalarType;

  EvalResult() = default;
  EvalResult(std::string&& v) : value(std::move(v)) {}
  EvalResult(ScalarT&& v) : value(std::move(v)) {}
  EvalResult(MultivectorT&& v) : value(std::move(v)) {}
  EvalResult(Op v) : value(v) {}
  EvalResult(Command v) : value(v) {}

  EvalResult(const EvalResult&) = default;
  EvalResult(EvalResult&&) = default;

  EvalResult& operator=(const EvalResult&) = default;
  EvalResult& operator=(EvalResult&&) = default;

  std::any value{};
  std::string message{};
  bool success{true};

  bool is_identifier() const { return value.type() == typeid(std::string); }
  bool is_scalar() const { return value.type() == typeid(ScalarT); }
  bool is_vector() const { return value.type() == typeid(MultivectorT); }
  bool is_op() const { return value.type() == typeid(Op); }
  bool is_command() const { return value.type() == typeid(Command); }

  std::string as_identifier() const { return std::any_cast<std::string>(value); }
  ScalarT as_scalar() const { return std::any_cast<ScalarT>(value); }
  MultivectorT as_vector() const { return std::any_cast<MultivectorT>(value); }
  Op as_op() const { return std::any_cast<Op>(value); }
  Command as_command() const { return std::any_cast<Command>(value); }
};

template <typename AlgebraT>
std::string to_string(const EvalResult<AlgebraT>& result) {
  using std::to_string;
  std::string s{};
  s.append(result.value.type().name());
  s.append(", ");

  if (result.is_scalar()) {
    s.append(to_string(result.as_scalar()));
    s.append(", ");
  } else if (result.is_vector()) {
    s.append(to_string(result.as_vector()));
    s.append(", ");
  } else if (result.is_identifier()) {
    s.append(result.as_identifier());
    s.append(", ");
  }

  s.append(result.message);
  s.append(", ");
  s.append(result.success ? "success" : "fail");
  return s;
}

template <typename AlgebraT>
std::ostream& operator<<(std::ostream& os, const EvalResult<AlgebraT>& result) {
  os << to_string(result);
  return os;
}

template <typename AlgebraT>
class Interpreter final {
 public:
  using EvalResultT = EvalResult<AlgebraT>;
  using MultivectorT = typename AlgebraT::VectorType;
  using ScalarT = typename AlgebraT::ScalarType;

 private:
  peg::parser parser_{create_parser()};

  void attach_parser_actions() {
    parser_["Line"] = [this](const peg::SemanticValues& sv) -> EvalResultT {
      DLOG(INFO) << "[Line] -- sv.size(): " << sv.size();
      const EvalResultT& value{std::any_cast<EvalResultT>(sv[1])};
      DLOG(INFO) << "[Line] -- value: " << value;
      if (value.is_scalar() or value.is_vector()) {
        dictionary_.insert({"_", value});
      }
      return value;
    };

    parser_["Statement"] = [this](const peg::SemanticValues& sv) -> EvalResultT {
      DLOG(INFO) << "[Statement] -- sv.size(): " << sv.size();
      const EvalResultT& value{std::any_cast<EvalResultT>(sv[0])};
      DLOG(INFO) << "[Statement] -- value: " << value;
      return value;
    };

    parser_["Assignment"] = [this](const peg::SemanticValues& sv) -> EvalResultT {
      DLOG(INFO) << "[Assignment] -- sv.size(): " << sv.size();
      const EvalResultT& identifier{std::any_cast<EvalResultT>(sv[0])};
      const EvalResultT& value{std::any_cast<EvalResultT>(sv[3])};
      DLOG(INFO) << "[Assignment] -- identifier: " << identifier << ", value: " << value;
      dictionary_.insert({identifier.as_identifier(), value});
      return value;
    };

    parser_["Identifier"] = [this](const peg::SemanticValues& sv) -> EvalResultT {
      EvalResultT identifier{sv.token_to_string()};
      DLOG(INFO) << "[Identifier] -- identifier: " << identifier;
      return identifier;
    };

    parser_["Expression"] = [this](const peg::SemanticValues& sv) -> EvalResultT {
      const EvalResultT& value{std::any_cast<EvalResultT>(sv[0])};
      DLOG(INFO) << "[Expression] -- value: " << value;
      return value;
    };

    parser_["Additive"] = [this](const peg::SemanticValues& sv) -> EvalResultT {
      DLOG(INFO) << "[Additive] -- sv.size(): " << sv.size();
      if (sv.size() == 1) {
        const EvalResultT& value{std::any_cast<EvalResultT>(sv[0])};
        DLOG(INFO) << "[Additive] -- value: " << value;
        return value;
      } else if (sv.size() == 5) {
        const EvalResultT& op = std::any_cast<EvalResultT>(sv[2]);
        DLOG(INFO) << "[Additive] -- op: " << op;
        const EvalResultT& left{std::any_cast<EvalResultT>(sv[0])};
        DLOG(INFO) << "[Additive] -- left: " << left;
        const EvalResultT& right{std::any_cast<EvalResultT>(sv[4])};
        DLOG(INFO) << "[Additive] -- right: " << right;
        if (op.is_op()) {
          if (op.as_op() == Op::ADD) {
            EvalResultT value{left.as_scalar() + right.as_scalar()};
            DLOG(INFO) << "[Additive] -- value: " << value << ", left: " << left
                       << ", right: " << right;
            return value;
          } else if (op.as_op() == Op::SUB) {
            EvalResultT value{left.as_scalar() - right.as_scalar()};
            DLOG(INFO) << "[Additive] -- value: " << value << ", left: " << left
                       << ", right: " << right;
            return value;
          }
        }
      }
    };

    parser_["AddOp"] = [this](const peg::SemanticValues& sv) -> EvalResultT {
      EvalResultT value{Op::ADD};
      return value;
    };

    parser_["SubOp"] = [this](const peg::SemanticValues& sv) -> EvalResultT {
      EvalResultT value{Op::SUB};
      return value;
    };

    parser_["Multiplicative"] = [this](const peg::SemanticValues& sv) -> EvalResultT {
      DLOG(INFO) << "[Multiplicative] -- sv.size(): " << sv.size();
      if (sv.size() == 1) {
        const EvalResultT& value{std::any_cast<EvalResultT>(sv[0])};
        DLOG(INFO) << "[Multiplicative] -- value: " << value;
        return value;
      } else if (sv.size() == 5) {
        const EvalResultT& op = std::any_cast<EvalResultT>(sv[2]);
        DLOG(INFO) << "[Multiplicative] -- op: " << op;
        const EvalResultT& left{std::any_cast<EvalResultT>(sv[0])};
        DLOG(INFO) << "[Multiplicative] -- left: " << left;
        const EvalResultT& right{std::any_cast<EvalResultT>(sv[4])};
        DLOG(INFO) << "[Multiplicative] -- right: " << right;
        if (op.is_op()) {
          if (op.as_op() == Op::MULT) {
            EvalResultT value{left.as_scalar() * right.as_scalar()};
            DLOG(INFO) << "[Multiplicative] -- value: " << value << ", left: " << left
                       << ", right: " << right;
            return value;
          } else if (op.as_op() == Op::DIV) {
            EvalResultT value{left.as_scalar() / right.as_scalar()};
            DLOG(INFO) << "[Multiplicative] -- value: " << value << ", left: " << left
                       << ", right: " << right;
            return value;
          }
        }
      }
    };

    parser_["MultOp"] = [this](const peg::SemanticValues& sv) -> EvalResultT {
      EvalResultT value{Op::MULT};
      return value;
    };

    parser_["DivOp"] = [this](const peg::SemanticValues& sv) -> EvalResultT {
      EvalResultT value{Op::DIV};
      return value;
    };

    parser_["OuterOp"] = [this](const peg::SemanticValues& sv) -> EvalResultT {
      EvalResultT value{Op::OUTER};
      return value;
    };

    parser_["InnerOp"] = [this](const peg::SemanticValues& sv) -> EvalResultT {
      EvalResultT value{Op::INNER};
      return value;
    };

    parser_["Unary"] = [this](const peg::SemanticValues& sv) -> EvalResultT {
      const EvalResultT& value{std::any_cast<EvalResultT>(sv[0])};
      DLOG(INFO) << "[Unary] -- value: " << value;
      return value;
    };

    parser_["Primary"] = [this](const peg::SemanticValues& sv) -> EvalResultT {
      const EvalResultT& value{std::any_cast<EvalResultT>(sv[0])};
      DLOG(INFO) << "[Primary] -- value: " << value;
      return value;
    };

    parser_["Scalar"] = [this](const peg::SemanticValues& sv) {
      ScalarT scalar{sv.token_to_number<ScalarT>()};
      EvalResultT value{std::move(scalar)};
      DLOG(INFO) << "[Scalar] -- value: " << value;
      return value;
    };

    parser_["RValue"] = [this](const peg::SemanticValues& sv) -> EvalResultT {
      const std::string symbol{sv.token_to_string()};
      // TODO(james): Add error logic for missing symbol.
      const EvalResultT& value{dictionary_.at(symbol)};
      DLOG(INFO) << "[RValue] -- value: " << value;
      return value;
    };

    parser_["Command"] = [this](const peg::SemanticValues& sv) -> EvalResultT {
      const EvalResultT& value{std::any_cast<EvalResultT>(sv[0])};
      DLOG(INFO) << "[Command] -- value: " << value;
      return value;
    };

    parser_["DictCommand"] = [this](const peg::SemanticValues& sv) -> EvalResultT {
      EvalResultT value{Command::DICT};
      DLOG(INFO) << "[DictCommand] -- value: " << value;
      return value;
    };

    parser_["ExitCommand"] = [this](const peg::SemanticValues& sv) -> EvalResultT {
      EvalResultT value{Command::EXIT};
      DLOG(INFO) << "[ExitCommand] -- value: " << value;
      return value;
    };

    parser_["HelpCommand"] = [this](const peg::SemanticValues& sv) -> EvalResultT {
      EvalResultT value{Command::HELP};
      DLOG(INFO) << "[HelpCommand] -- value: " << value;
      return value;
    };
  }

  std::unordered_map<std::string, EvalResultT> dictionary_{};

 public:
  Interpreter() { attach_parser_actions(); }

  EvalResultT eval(std::string_view phrase) {
    EvalResultT result{};
    parser_.parse(phrase, result);
    return result;
  }
};

}  // namespace ndyn::ui
