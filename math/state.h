#pragma once

#include <array>
#include <cstdint>
#include <initializer_list>
#include <ostream>
#include <string>
#include <type_traits>

namespace ndyn::math {

namespace internal {

template <typename T>
class ScalarTypes {
 public:
  using ScalarType = typename T::ScalarType;
};

template <>
class ScalarTypes<float> {
 public:
  using ScalarType = float;
};

template <>
class ScalarTypes<double> {
 public:
  using ScalarType = double;
};

}  // namespace internal

/**
 * State vector of a particle or object for use in simulations. The meaning of the various elements
 * is unspecified. For standard mechanical systems, the position is in element zero, the velocity in
 * element one, acceleration in element two, etc., as needed.
 */
template <typename T, size_t DEPTH>
class State final {
 private:
  std::array<T, DEPTH> elements_{};

 public:
  using ValueType = T;
  using ScalarType = typename internal::ScalarTypes<ValueType>::ScalarType;

  static constexpr size_t depth() { return DEPTH; }

  constexpr State() = default;
  constexpr State(const State& rhs) = default;
  constexpr State(State&& rhs) = default;

  constexpr State& operator=(const State& rhs) = default;
  constexpr State& operator=(State&& rhs) = default;

  constexpr State(std::initializer_list<T> elements) {
    size_t i = 0;
    for (T element : elements) {
      elements_.at(i) = element;
      ++i;
    }
  }

  template <size_t INDEX>
  constexpr const T& element() const {
    static_assert(INDEX < DEPTH, "Attempt to reference out of bounds index");
    return elements_[INDEX];
  }

  constexpr const T& element(size_t index) const { return elements_.at(index); }

  template <size_t INDEX>
  constexpr void set_element(const T& element) {
    static_assert(INDEX < DEPTH, "Attempt to reference out of bounds index");
    elements_[INDEX] = element;
  }

  constexpr void set_element(size_t index, const T& element) { elements_.at(index) = element; }

  constexpr State shift() const {
    State result{};
    for (size_t i = 0; i < DEPTH - 1; ++i) {
      result.elements_[i] = elements_[i + 1];
    }
    return result;
  }
};

template <typename T, size_t DEPTH>
std::string to_string(const State<T, DEPTH>& state) {
  using std::to_string;
  std::string result{"{"};
  bool need_comma{false};
  for (size_t i = 0; i < State<T, DEPTH>::depth(); ++i) {
    if (need_comma) {
      result.append(", ");
    }
    result.append(to_string(state.element(i)));
    need_comma = true;
  }
  result.append("}");
  return result;
}

template <typename T, size_t DEPTH>
std::ostream& operator<<(std::ostream& os, const State<T, DEPTH>& state) {
  using std::to_string;
  os << to_string(state);
  return os;
}

}  // namespace ndyn::math
