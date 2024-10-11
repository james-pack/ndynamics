#pragma once

#include <array>
#include <cmath>
#include <initializer_list>

#include "math/coordinates.h"
#include "math/vector.h"

namespace ndyn::math {

template <typename ScalarT, size_t DIM, typename UnitsT>
class Vector<Coordinates::CARTESIAN, ScalarT, DIM, UnitsT> final {
 public:
  using ScalarType = ScalarT;
  using UnitsType = UnitsT;

  static constexpr Coordinates COORDINATES{Coordinates::CARTESIAN};
  static constexpr size_t DIMENSIONS{DIM};

  static constexpr size_t size() { return DIMENSIONS; }

  static_assert(UnitsType::size() <= DIMENSIONS,
                "Too many units specified for the size of the vector.");

 private:
  std::array<ScalarT, DIMENSIONS> elements_{};

 public:
  constexpr Vector() = default;

  constexpr Vector(const Vector& rhs) = default;
  constexpr Vector(Vector&& rhs) = default;

  constexpr Vector(std::initializer_list<ScalarType> values) {
    size_t i = 0;
    for (const auto& value : values) {
      elements_.at(i) = value;
      ++i;
    }
  }

  constexpr Vector& operator=(const Vector& rhs) = default;
  constexpr Vector& operator=(Vector&& rhs) = default;

  constexpr bool operator==(const Vector& rhs) const {
    for (size_t i = 0; i < DIMENSIONS; ++i) {
      if (elements_[i] != rhs.elements_[i]) {
        return false;
      }
    }
    return true;
  }

  constexpr bool operator!=(const Vector& rhs) const {
    for (size_t i = 0; i < DIMENSIONS; ++i) {
      if (elements_[i] != rhs.elements_[i]) {
        return true;
      }
    }
    return false;
  }

  template <size_t INDEX>
  constexpr const ScalarType& element() const {
    static_assert(INDEX < DIMENSIONS, "Index out of range");
    return elements_[INDEX];
  }
  template <size_t INDEX>
  constexpr ScalarType& element() {
    static_assert(INDEX < DIMENSIONS, "Index out of range");
    return elements_[INDEX];
  }
  template <size_t INDEX>
  constexpr void set_element(const ScalarType& value) {
    static_assert(INDEX < DIMENSIONS, "Index out of range");
    elements_[INDEX] = value;
  }

  constexpr const ScalarType& element(size_t index) const { return elements_.at(index); }
  constexpr ScalarType& element(size_t index) { return elements_.at(index); }
  constexpr void set_element(size_t index, const ScalarType& value) { elements_.at(index) = value; }

  constexpr const ScalarType& x() const { return element<0>(); }
  constexpr ScalarType& x() { return element<0>(); }
  constexpr void set_x(const ScalarType& value) { set_element<0>(value); }

  constexpr const ScalarType& y() const { return element<1>(); }
  constexpr ScalarType& y() { return element<1>(); }
  constexpr void set_y(const ScalarType& value) { set_element<1>(value); }

  constexpr const ScalarType& z() const { return element<2>(); }
  constexpr ScalarType& z() { return element<2>(); }
  constexpr void set_z(const ScalarType& value) { set_element<2>(value); }

  constexpr Vector multiply(const ScalarType& scalar) const {
    Vector result{*this};
    for (size_t i = 0; i < DIMENSIONS; ++i) {
      result.elements_[i] *= scalar;
    }
    return result;
  }
  constexpr Vector divide(const ScalarType& scalar) const {
    Vector result{*this};
    for (size_t i = 0; i < DIMENSIONS; ++i) {
      result.elements_[i] /= scalar;
    }
    return result;
  }

  constexpr Vector add(const Vector& rhs) const {
    Vector result{*this};
    for (size_t i = 0; i < DIMENSIONS; ++i) {
      result.elements_[i] += rhs.elements_[i];
    }
    return result;
  }

  constexpr Vector subtract(const Vector& rhs) const {
    Vector result{*this};
    for (size_t i = 0; i < DIMENSIONS; ++i) {
      result.elements_[i] -= rhs.elements_[i];
    }
    return result;
  }

  constexpr ScalarType inner(const Vector& rhs) const {
    ScalarType result{};
    for (size_t i = 0; i < DIMENSIONS; ++i) {
      result += elements_[i] * rhs.elements_[i];
    }
    return result;
  }

  constexpr Vector parallel(const Vector& axis) const {
    return inner(axis) / axis.square_magnitude() * axis;
  }

  constexpr Vector orthogonal(const Vector& axis) const { return subtract(parallel(axis)); }

  constexpr ScalarType square_magnitude() const {
    ScalarType result{};
    for (size_t i = 0; i < DIMENSIONS; ++i) {
      result += elements_[i] * elements_[i];
    }
    return result;
  }

  constexpr ScalarType abs() const {
    using std::sqrt;
    return sqrt(square_magnitude());
  }

  // Operator overloads.
  constexpr Vector operator*(const ScalarType& scalar) const { return multiply(scalar); }
  constexpr Vector operator/(const ScalarType& scalar) const { return divide(scalar); }

  constexpr Vector operator+(const Vector& rhs) const { return add(rhs); }
  constexpr Vector operator-(const Vector& rhs) const { return subtract(rhs); }

  template <size_t N>
  static constexpr Vector e() {
    static_assert(N < DIMENSIONS, "No such basis vector. Index N out of range");
    Vector result{};
    result.elements_[N] = 1;
    return result;
  }
};

}  // namespace ndyn::math
