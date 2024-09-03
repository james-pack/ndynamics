#pragma once

#include <array>
#include <cstring>
#include <initializer_list>
#include <ostream>
#include <string>

namespace ndyn::math {

/**
 * State vector of a particle or object for use in simulations. The meaning of the various elements
 * is unspecified. For standard mechanical systems, the position is in element zero, the velocity in
 * element one, acceleration in element two, etc., as needed.
 */
template <typename T, size_t SIZE>
class StateT final {
 private:
  std::array<T, SIZE> elements_{};

 public:
  using ValueType = T;
  static constexpr size_t size() { return SIZE; }

  constexpr StateT() = default;
  constexpr StateT(const StateT& rhs) = default;
  constexpr StateT(StateT&& rhs) = default;

  constexpr StateT& operator=(const StateT& rhs) = default;
  constexpr StateT& operator=(StateT&& rhs) = default;

  constexpr StateT(std::initializer_list<T> elements) {
    size_t i = 0;
    for (T element : elements) {
      elements_.at(i) = element;
      ++i;
    }
  }

  template <size_t INDEX>
  constexpr const T& element() const {
    static_assert(INDEX < SIZE, "Attempt to reference out of bounds index");
    return elements_[INDEX];
  }

  constexpr const T& element(size_t index) const { return elements_.at(index); }

  template <size_t INDEX>
  constexpr void set_element(const T& element) {
    static_assert(INDEX < SIZE, "Attempt to reference out of bounds index");
    elements_[INDEX] = element;
  }

  constexpr void set_element(size_t index, const T& element) { elements_.at(index) = element; }
};

template <typename T, size_t SIZE>
std::string to_string(const StateT<T, SIZE>& state) {
  using std::to_string;
  std::string result{"{"};
  bool need_comma{false};
  for (size_t i = 0; i < StateT<T, SIZE>::size(); ++i) {
    if (need_comma) {
      result.append(", ");
    }
    result.append(to_string(state.element(i)));
    need_comma = true;
  }
  result.append("}");
  return result;
}

template <typename T, size_t SIZE>
std::ostream& operator<<(std::ostream& os, const StateT<T, SIZE>& state) {
  using std::to_string;
  os << to_string(state);
  return os;
}

}  // namespace ndyn::math
