#pragma once

namespace ndyn::sensor {

template <typename ValueType>
class Characteristics final {
 private:
  ValueType min_;
  ValueType max_;

 public:
  constexpr Characteristics(ValueType min, ValueType max) : min_(min), max_(max) {}

  constexpr ValueType min_value() const { return ValueType{}; }
  constexpr ValueType max_value() const { return ValueType{}; }
};

}  // namespace ndyn::sensor
