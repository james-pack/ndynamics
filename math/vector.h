#pragma once

#include <initializer_list>
#include <utility>

#include "math/coordinates.h"

namespace ndyn::math {

template <Coordinates COORD, typename ScalarT, size_t DIM, typename UnitsT>
class Vector final {
 public:
  using ScalarType = ScalarT;
  using UnitsType = UnitsT;

  static constexpr Coordinates COORDINATES{COORD};
  static constexpr size_t DIMENSIONS{DIM};

  constexpr Vector() = default;

  constexpr Vector(const Vector& rhs) = default;
  constexpr Vector(Vector&& rhs) = default;

  constexpr Vector(std::initializer_list<T> values);

  constexpr Vector& operator=(const Vector& rhs) = default;
  constexpr Vector& operator=(Vector&& rhs) = default;

  constexpr bool operator==(const Vector& rhs) const = default;

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

  constexpr Vector inner(const Vector& rhs) const;

  constexpr std::pair<Vector, Vector> decompose(const Vector& axis) const;

  constexpr ScalarType square_magnitude() const;
  constexpr ScalarType abs() const;

  // Operator overloads.
  constexpr Vector operator*(const ScalarType& scalar) const;
  constexpr Vector operator/(const ScalarType& scalar) const;

  constexpr Vector operator+(const Vector& rhs) const;
  constexpr Vector operator-(const Vector& rhs) const;

  template <size_t N>
  static constexpr Vector e();
};

}  // namespace ndyn::math
