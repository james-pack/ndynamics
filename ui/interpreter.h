#pragma once

#include <string>
#include <string_view>
#include <unordered_map>

#include "math/multivector.h"
#include "peglib.h"
#include "ui/grammar.h"

namespace ndyn::ui {

template <typename AlgebraT>
class Interpreter final {
 public:
  using MultivectorT = typename AlgebraT::VectorType;
  using ScalarT = typename AlgebraT::ScalarType;

 private:
  peg::parser parser_{create_parser()};

  void attach_parser_actions() {
    parser_["Line"] = [this](const peg::SemanticValues& sv) {
      LOG(INFO) << "[Line] -- sv.size(): " << sv.size();
      for (size_t i = 0; i < sv.size(); ++i) {
        LOG(INFO) << "[Line] -- sv[" << i << "]: " << sv.token_to_string(i);
      }
      const MultivectorT& value{std::any_cast<MultivectorT>(sv[1])};
      LOG(INFO) << "[Line] -- value: " << value;
      return value;
    };

    parser_["Statement"] = [this](const peg::SemanticValues& sv) {
      LOG(INFO) << "[Statement] -- sv.size(): " << sv.size();
      const MultivectorT& value{std::any_cast<MultivectorT>(sv[0])};
      LOG(INFO) << "[Statement] -- value: " << value;
      return value;
    };

    parser_["Assignment"] = [this](const peg::SemanticValues& sv) {
      const std::string symbol{sv.token_to_string(0)};
      const MultivectorT& value{std::any_cast<MultivectorT>(sv[1])};
      dictionary_.insert({symbol, value});
      return value;
    };

    parser_["Expression"] = [this](const peg::SemanticValues& sv) {
      const MultivectorT& value{std::any_cast<MultivectorT>(sv[0])};
      LOG(INFO) << "[Expression] -- value: " << value;
      return value;
    };

    parser_["Additive"] = [this](const peg::SemanticValues& sv) {
      const MultivectorT& value{std::any_cast<MultivectorT>(sv[0])};
      LOG(INFO) << "[Additive] -- value: " << value;
      return value;
    };

    parser_["Multiplicative"] = [this](const peg::SemanticValues& sv) {
      const MultivectorT& value{std::any_cast<MultivectorT>(sv[0])};
      LOG(INFO) << "[Multiplicative] -- value: " << value;
      return value;
    };

    parser_["Unary"] = [this](const peg::SemanticValues& sv) {
      const MultivectorT& value{std::any_cast<MultivectorT>(sv[0])};
      LOG(INFO) << "[Unary] -- value: " << value;
      return value;
    };

    parser_["Primary"] = [this](const peg::SemanticValues& sv) {
      const MultivectorT& value{std::any_cast<MultivectorT>(sv[0])};
      LOG(INFO) << "[Primary] -- value: " << value;
      return value;
    };

    parser_["Scalar"] = [this](const peg::SemanticValues& sv) {
      const ScalarT scalar{sv.token_to_number<ScalarT>()};
      MultivectorT value{scalar};
      LOG(INFO) << "[Scalar] -- value: " << value;
      return value;
    };

    parser_["RValue"] = [this](const peg::SemanticValues& sv) {
      const std::string symbol{sv.token_to_string()};
      // TODO(james): Add error logic for missing symbol.
      const auto& value{dictionary_.at(symbol)};
      return value;
    };
  }

  std::unordered_map<std::string, MultivectorT> dictionary_{};

 public:
  Interpreter() { attach_parser_actions(); }

  MultivectorT eval(std::string_view phrase) {
    // TODO(james): Find a way to create an error result.
    MultivectorT result{};
    parser_.parse(phrase, result);
    return result;
  }
};

}  // namespace ndyn::ui
