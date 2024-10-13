#pragma once

#include <initializer_list>

#include "math/coordinates.h"

namespace ndyn::math {

template <Coordinates COORD, typename ScalarT, size_t DIM, typename UnitsT>
class Vector final {
 public:
  using ScalarType = ScalarT;
  using Units = UnitsT;

  static constexpr Coordinates COORDINATES{COORD};
  static constexpr size_t DIMENSIONS{DIM};

  static constexpr size_t size() { return DIMENSIONS; }

  static_assert(Units::size() <= DIMENSIONS,
                "Too many units specified for the size of the vector.");

  constexpr Vector() = default;

  constexpr Vector(const Vector& rhs) = default;
  constexpr Vector(Vector&& rhs) = default;

  constexpr Vector(std::initializer_list<ScalarType> values);

  constexpr Vector& operator=(const Vector& rhs) = default;
  constexpr Vector& operator=(Vector&& rhs) = default;

  constexpr bool operator==(const Vector& rhs) const;
  constexpr bool operator!=(const Vector& rhs) const;

  template <size_t INDEX>
  constexpr const ScalarType& element() const;
  template <size_t INDEX>
  constexpr ScalarType& element();
  template <size_t INDEX>
  constexpr void set_element(const ScalarType& value);

  constexpr const ScalarType& element(size_t index) const;
  constexpr ScalarType& element(size_t index);
  constexpr void set_element(size_t index, const ScalarType& value);

  constexpr Vector multiply(const ScalarType& scalar) const;
  constexpr Vector divide(const ScalarType& scalar) const;

  constexpr Vector add(const Vector& rhs) const;
  constexpr Vector subtract(const Vector& rhs) const;

  constexpr ScalarType inner(const Vector& rhs) const;

  constexpr Vector parallel(const Vector& axis) const;
  constexpr Vector orthogonal(const Vector& axis) const;

  constexpr ScalarType square_magnitude() const;
  constexpr ScalarType abs() const;

  // Operator overloads.
  constexpr Vector operator*(const ScalarType& scalar) const;
  constexpr Vector operator/(const ScalarType& scalar) const;

  constexpr Vector operator+(const Vector& rhs) const;
  constexpr Vector operator-(const Vector& rhs) const;

  // Unary minus.
  constexpr Vector operator-() const;

  template <size_t N>
  static constexpr Vector e();
};

template <Coordinates COORDINATES, typename ScalarType, size_t DIMENSIONS, typename UnitsType>
constexpr Vector<COORDINATES, ScalarType, DIMENSIONS, UnitsType> operator*(
    const ScalarType& scalar,
    const Vector<COORDINATES, ScalarType, DIMENSIONS, UnitsType>& vector) {
  return vector * scalar;
}

// String and printing operations.

template <Coordinates COORDINATES, typename ScalarType, size_t DIMENSIONS, typename UnitsType>
std::string to_string(const Vector<COORDINATES, ScalarType, DIMENSIONS, UnitsType>& v) {
  using std::to_string;
  std::string result{};
  result.append("[");
  bool need_comma{false};
  for (size_t i = 0; i < v.size(); ++i) {
    if (need_comma) {
      result.append(", ");
    }
    need_comma = true;
    result.append(to_string(v.element(i)));
  }
  result.append("]");
  return result;
}

template <Coordinates COORDINATES, typename ScalarType, size_t DIMENSIONS, typename UnitsType>
std::ostream& operator<<(std::ostream& os,
                         const Vector<COORDINATES, ScalarType, DIMENSIONS, UnitsType>& v) {
  os << to_string(v);
  return os;
}

}  // namespace ndyn::math
