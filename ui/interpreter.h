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

template <typename AlgebraT>
class EvalResult final {
 public:
  using MultivectorT = typename AlgebraT::VectorType;
  using ScalarT = typename AlgebraT::ScalarType;

  EvalResult() = default;
  EvalResult(std::string&& v) : value(std::move(v)) {}
  EvalResult(ScalarT&& v) : value(std::move(v)) {}
  EvalResult(MultivectorT&& v) : value(std::move(v)) {}
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
  bool is_command() const { return value.type() == typeid(Command); }

  std::string as_identifier() const { return std::any_cast<std::string>(value); }
  ScalarT as_scalar() const { return std::any_cast<ScalarT>(value); }
  MultivectorT as_vector() const { return std::any_cast<MultivectorT>(value); }
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
      LOG(INFO) << "[Line] -- sv.size(): " << sv.size();
      const EvalResultT& value{std::any_cast<EvalResultT>(sv[1])};
      LOG(INFO) << "[Line] -- value: " << value;
      if (value.is_scalar() or value.is_vector()) {
        dictionary_.insert({"_", value});
      }
      return value;
    };

    parser_["Statement"] = [this](const peg::SemanticValues& sv) -> EvalResultT {
      LOG(INFO) << "[Statement] -- sv.size(): " << sv.size();
      const EvalResultT& value{std::any_cast<EvalResultT>(sv[0])};
      LOG(INFO) << "[Statement] -- value: " << value;
      return value;
    };

    parser_["Assignment"] = [this](const peg::SemanticValues& sv) -> EvalResultT {
      LOG(INFO) << "[Assignment] -- sv.size(): " << sv.size();
      const EvalResultT& identifier{std::any_cast<EvalResultT>(sv[0])};
      const EvalResultT& value{std::any_cast<EvalResultT>(sv[3])};
      LOG(INFO) << "[Assignment] -- identifier: " << identifier << ", value: " << value;
      dictionary_.insert({identifier.as_identifier(), value});
      return value;
    };

    parser_["Identifier"] = [this](const peg::SemanticValues& sv) -> EvalResultT {
      EvalResultT identifier{sv.token_to_string()};
      LOG(INFO) << "[Identifier] -- identifier: " << identifier;
      return identifier;
    };

    parser_["Expression"] = [this](const peg::SemanticValues& sv) -> EvalResultT {
      const EvalResultT& value{std::any_cast<EvalResultT>(sv[0])};
      LOG(INFO) << "[Expression] -- value: " << value;
      return value;
    };

    parser_["Additive"] = [this](const peg::SemanticValues& sv) -> EvalResultT {
      const EvalResultT& value{std::any_cast<EvalResultT>(sv[0])};
      LOG(INFO) << "[Additive] -- value: " << value;
      return value;
    };

    parser_["Multiplicative"] = [this](const peg::SemanticValues& sv) -> EvalResultT {
      const EvalResultT& value{std::any_cast<EvalResultT>(sv[0])};
      LOG(INFO) << "[Multiplicative] -- value: " << value;
      return value;
    };

    parser_["Unary"] = [this](const peg::SemanticValues& sv) -> EvalResultT {
      const EvalResultT& value{std::any_cast<EvalResultT>(sv[0])};
      LOG(INFO) << "[Unary] -- value: " << value;
      return value;
    };

    parser_["Primary"] = [this](const peg::SemanticValues& sv) -> EvalResultT {
      const EvalResultT& value{std::any_cast<EvalResultT>(sv[0])};
      LOG(INFO) << "[Primary] -- value: " << value;
      return value;
    };

    parser_["Scalar"] = [this](const peg::SemanticValues& sv) {
      ScalarT scalar{sv.token_to_number<ScalarT>()};
      EvalResultT value{std::move(scalar)};
      LOG(INFO) << "[Scalar] -- value: " << value;
      return value;
    };

    parser_["RValue"] = [this](const peg::SemanticValues& sv) -> EvalResultT {
      const std::string symbol{sv.token_to_string()};
      // TODO(james): Add error logic for missing symbol.
      const EvalResultT& value{dictionary_.at(symbol)};
      LOG(INFO) << "[RValue] -- value: " << value;
      return value;
    };

    parser_["Command"] = [this](const peg::SemanticValues& sv) -> EvalResultT {
      const EvalResultT& value{std::any_cast<EvalResultT>(sv[0])};
      LOG(INFO) << "[Command] -- value: " << value;
      return value;
    };

    parser_["DictCommand"] = [this](const peg::SemanticValues& sv) -> EvalResultT {
      EvalResultT value{Command::DICT};
      LOG(INFO) << "[DictCommand] -- value: " << value;
      return value;
    };

    parser_["ExitCommand"] = [this](const peg::SemanticValues& sv) -> EvalResultT {
      EvalResultT value{Command::EXIT};
      LOG(INFO) << "[ExitCommand] -- value: " << value;
      return value;
    };

    parser_["HelpCommand"] = [this](const peg::SemanticValues& sv) -> EvalResultT {
      EvalResultT value{Command::HELP};
      LOG(INFO) << "[HelpCommand] -- value: " << value;
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
