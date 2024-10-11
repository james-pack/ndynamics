#pragma once

#include <array>
#include <cmath>
#include <initializer_list>

#include "base/pi.h"
#include "math/coordinates.h"
#include "math/vector.h"

namespace ndyn::math {

template <typename ScalarT, size_t DIM, typename UnitsT>
class Vector<Coordinates::SPHERICAL, ScalarT, DIM, UnitsT> final {
 public:
  using ScalarType = ScalarT;
  using UnitsType = UnitsT;

  static constexpr Coordinates COORDINATES{Coordinates::SPHERICAL};
  static constexpr size_t DIMENSIONS{DIM};

  static_assert(DIMENSIONS <= 3,
                "Vectors in spherical coordinates with more than 3 dimensions are not "
                "well-defined. Or at least, they are not handled by the current implementation.");

  static constexpr size_t size() { return DIMENSIONS; }

  static_assert(UnitsType::size() <= DIMENSIONS,
                "Too many units specified for the size of the vector.");

 private:
  std::array<ScalarT, DIMENSIONS> elements_{};

  constexpr void normalize() {
    using std::fmod;
    using std::remainder;

    if constexpr (DIMENSIONS == 2) {
      // Ensure that the radius is positive.
      if (elements_[0] < 0) {
        elements_[0] *= -1;
        elements_[1] += pi;
      }

      // Ensure that theta is on (-pi, pi]
      elements_[1] = remainder(elements_[1], 2 * pi);
      if (elements_[1] <= -pi) {
        elements_[1] += 2 * pi;
      }
      if (elements_[1] > pi) {
        elements_[1] -= 2 * pi;
      }
    }

    if constexpr (DIMENSIONS == 3) {
      // Ensure that the radius is positive.
      if (elements_[0] < 0) {
        elements_[0] *= -1;
        elements_[1] += pi;
      }

      // Ensure that theta is on [0, pi].
      elements_[1] = fmod(elements_[1], 2 * pi);
      if (elements_[1] < 0) {
        elements_[1] *= -1;
        elements_[2] += pi;
      }
      if (elements_[1] > pi) {
        elements_[1] = 2 * pi - elements_[1];
        elements_[2] += pi;
      }

      // Ensure that phi is on (-pi, pi]
      elements_[2] = remainder(elements_[2], 2 * pi);
      if (elements_[2] <= -pi) {
        elements_[2] += 2 * pi;
      }
      if (elements_[2] > pi) {
        elements_[2] -= 2 * pi;
      }
    }
  }

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
    normalize();
  }

  constexpr Vector& operator=(const Vector& rhs) = default;
  constexpr Vector& operator=(Vector&& rhs) = default;

  constexpr bool operator==(const Vector& rhs) const {
    if (elements_[0] == 0 || rhs.elements_[0] == 0) {
      return elements_[0] == rhs.elements_[0];
    }

    if (elements_[1] == 0 || rhs.elements_[1] == 0) {
      return elements_[1] == rhs.elements_[1];
    }

    if (elements_[1] == pi || rhs.elements_[1] == pi) {
      return elements_[1] == rhs.elements_[1];
    }

    for (size_t i = 1; i < DIMENSIONS; ++i) {
      if (elements_[i] != rhs.elements_[i]) {
        return false;
      }
    }
    return true;
  }

  constexpr bool operator!=(const Vector& rhs) const {
    if (elements_[0] == 0 || rhs.elements_[0] == 0) {
      return elements_[0] != rhs.elements_[0];
    }

    if (elements_[1] == 0 || rhs.elements_[1] == 0) {
      return elements_[1] != rhs.elements_[1];
    }

    if (elements_[1] == pi || rhs.elements_[1] == pi) {
      return elements_[1] != rhs.elements_[1];
    }

    for (size_t i = 1; i < DIMENSIONS; ++i) {
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
  constexpr void set_element(const ScalarType& value) {
    static_assert(INDEX < DIMENSIONS, "Index out of range");
    elements_[INDEX] = value;
    normalize();
  }

  constexpr const ScalarType& element(size_t index) const { return elements_.at(index); }
  constexpr void set_element(size_t index, const ScalarType& value) {
    elements_.at(index) = value;
    normalize();
  }

  constexpr const ScalarType& r() const { return element<0>(); }
  constexpr void set_r(const ScalarType& value) {
    set_element<0>(value);
    normalize();
  }

  constexpr const ScalarType& theta() const { return element<1>(); }
  constexpr void set_theta(const ScalarType& value) {
    set_element<1>(value);
    normalize();
  }

  constexpr const ScalarType& phi() const { return element<2>(); }
  constexpr void set_phi(const ScalarType& value) {
    set_element<2>(value);
    normalize();
  }

  constexpr Vector multiply(const ScalarType& scalar) const {
    Vector result{*this};
    result.set_r(result.r() * scalar);
    return result;
  }

  constexpr Vector divide(const ScalarType& scalar) const {
    Vector result{*this};
    result.set_r(result.r() / scalar);
    return result;
  }

  constexpr Vector add(const Vector& rhs) const {
    using std::atan2;
    using std::cos;
    using std::hypot;
    using std::sin;

    if constexpr (DIMENSIONS == 1) {
      return Vector{element<0>() + rhs.element<0>()};
    }

    if constexpr (DIMENSIONS == 2) {
      Vector result{};
      ScalarType x = element<0>() * cos(element<1>()) + rhs.element<0>() * cos(rhs.element<1>());
      ScalarType y = element<0>() * sin(element<1>()) + rhs.element<0>() * sin(rhs.element<1>());
      result.set_r(hypot(x, y));
      result.set_theta(atan2(y, x));
      return result;
    }

    if constexpr (DIMENSIONS == 3) {
      Vector result{};
      ScalarType x = element<0>() * cos(element<2>()) * sin(element<1>()) +
                     rhs.element<0>() * cos(rhs.element<2>()) * sin(rhs.element<1>());

      ScalarType y = element<0>() * sin(element<2>()) * sin(element<1>()) +
                     rhs.element<0>() * sin(rhs.element<2>()) * sin(rhs.element<1>());

      ScalarType z = element<0>() * cos(element<1>()) + rhs.element<0>() * cos(rhs.element<1>());

      result.set_r(hypot(x, y, z));
      ScalarType xy_hypot = hypot(x, y);
      result.set_theta(atan2(xy_hypot, z));
      result.set_phi(atan2(y, x));
      return result;
    }
  }

  constexpr Vector subtract(const Vector& rhs) const { return add(rhs.multiply(-1)); }

  constexpr ScalarType inner(const Vector& rhs) const {}

  constexpr Vector parallel(const Vector& axis) const {
    return inner(axis) / axis.square_magnitude() * axis;
  }

  constexpr Vector orthogonal(const Vector& axis) const { return subtract(parallel(axis)); }

  constexpr ScalarType square_magnitude() const { return r() * r(); }

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
