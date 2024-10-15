#pragma once

#include <functional>

#include "math/state.h"

namespace ndyn::simulation {

template <typename VectorT>
class Potential final {
 public:
  using VectorType = VectorT;
  using ScalarType = typename math::ScalarTypes<VectorType>::ScalarType;
  using FnType = std::function<VectorType(const ScalarType&, const VectorType&)>;

 private:
  ScalarType t_{};
  FnType fn_{};
  VectorType value_{};

 public:
  Potential() = default;
  explicit Potential(const FnType& fn) : fn_(fn) {}
  explicit Potential(const VectorType& value) : value_(value) {}

  void set_function(const FnType& fn) { fn_ = fn; }

  void set_value(const VectorType& value) {
    fn_ = FnType{};
    value_ = value;
  }

  constexpr VectorType at(const VectorType& position) const {
    if (fn_) {
      return fn_(t_, position);
    } else {
      return value_;
    }
  }

  constexpr ScalarType magnitude() const {
    using std::abs;
    return abs(at(VectorType{}));
  }

  constexpr ScalarType magnitude(const VectorT& position) const {
    using std::abs;
    return abs(at(position));
  }

  void update(const ScalarType& t) { t_ = t; }
};

}  // namespace ndyn::simulation
