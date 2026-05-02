#pragma once

#include <cmath>
#include <compare>
#include <ostream>
#include <string>

namespace ndyn::math {

/**
 * Provides a templated complex number implementation suitable for linear algebra computations.
 *
 * This class encapsulates real and imag components, providing overloaded operators for addition,
 * subtraction, multiplication, and division. It explicitly defines internal math functions via
 * non-member overloads to allow argument-dependent lookup (ADL) or direct calls for norm and
 * sign-related calculations.
 *
 * Note that this class primarily exists for testing other types. It often acts as the Scalar type
 * when testing other numerical types to validate that they do not hard-code float, double, or any
 * concepts that only apply to built-in types.
 */
template <typename Scalar>
class Complex final {
 public:
  constexpr Complex() = default;
  constexpr explicit Complex(Scalar r) : real(r) {}
  constexpr Complex(Scalar r, Scalar i) : real(r), imag(i) {}

  constexpr Complex(const Complex& rhs) = default;
  constexpr Complex(Complex&& rhs) = default;

  constexpr Complex& operator=(const Complex& rhs) = default;
  constexpr Complex& operator=(Complex&& rhs) = default;

  [[nodiscard]] constexpr auto operator-() const noexcept { return Complex(-real, -imag); }

  [[nodiscard]] constexpr auto operator+(const Complex& rhs) const noexcept {
    return Complex(real + rhs.real, imag + rhs.imag);
  }

  [[nodiscard]] constexpr auto operator-(const Complex& rhs) const noexcept {
    return Complex(real - rhs.real, imag - rhs.imag);
  }

  [[nodiscard]] constexpr auto operator*(const Scalar& scalar) const noexcept {
    return Complex{scalar * real, scalar * imag};
  }

  [[nodiscard]] constexpr auto operator*(const Complex& rhs) const noexcept {
    return Complex(real * rhs.real - imag * rhs.imag, real * rhs.imag + imag * rhs.real);
  }

  [[nodiscard]] constexpr auto operator/(const Scalar& scalar) const noexcept {
    return Complex(real / scalar, imag / scalar);
  }

  [[nodiscard]] constexpr auto operator/(const Complex& rhs) const noexcept {
    return (*this) * Complex{rhs.real, -rhs.imag} / (rhs.real * rhs.real + rhs.imag * rhs.imag);
  }

  [[nodiscard]] constexpr auto conj() const noexcept { return Complex{real, -imag}; }

  [[nodiscard]] constexpr std::weak_ordering operator<=>(const Complex& rhs) const noexcept {
    using std::hypot;
    const auto l_value{hypot(real, imag)};
    const auto r_value{hypot(rhs.real, rhs.imag)};
    if (l_value < r_value) {
      return std::weak_ordering::less;
    } else if (l_value > r_value) {
      return std::weak_ordering::greater;
    } else {
      return std::weak_ordering::equivalent;
    }
  }

  [[nodiscard]] constexpr bool operator==(const Complex& rhs) const noexcept {
    return real == rhs.real and imag == rhs.imag;
  }

  [[nodiscard]] constexpr auto operator+=(const Complex& rhs) noexcept {
    real += rhs.real;
    imag += rhs.imag;
  }

  [[nodiscard]] constexpr auto operator-=(const Complex& rhs) noexcept {
    real -= rhs.real;
    imag -= rhs.imag;
  }

  Scalar real{};
  Scalar imag{};
};

template <typename Scalar>
std::string to_string(const Complex<Scalar>& c) {
  using std::to_string;
  std::string result{};
  result.append("[").append(to_string(c.real)).append(", ").append(to_string(c.imag)).append("]");
  return result;
}

template <typename Scalar>
std::ostream& operator<<(std::ostream& os, const Complex<Scalar>& c) {
  os << to_string(c);
  return os;
}

/**
 * Computes the absolute value or magnitude of a Complex number.
 *
 * The function calculates the square root of the sum of the squares of the real and imag
 * components. It serves as the complex modulus, providing a scalar measure of the number's
 * distance from the origin in the complex plane.
 *
 * This is essential for SVD algorithms to determine the magnitude of singular values and to
 * evaluate convergence criteria against EPSILON.
 */
template <typename Scalar>
[[nodiscard]] constexpr auto abs(const Complex<Scalar>& c) noexcept {
  using std::hypot;
  return hypot(c.real, c.imag);
}

/**
 * Extracts the square root of a complex number.
 *
 * It utilizes the polar form derivation to find a complex result whose square equals the input.
 * The logic handles the principal branch to ensure consistent results during matrix
 * transformations or normalization steps.
 *
 * Precision in this operation is vital for maintaining the numerical integrity of the
 * transformation matrices U and v during iterative orthogonalization.
 */
template <typename Scalar>
[[nodiscard]] constexpr auto sqrt(const Complex<Scalar>& c) noexcept {
  using std::sqrt;
  const auto m{abs(c)};
  const auto r{sqrt((m + c.real) / Scalar{2})};
  const auto i = [&c, &m]() {
    const auto value{sqrt((m - c.real) / Scalar{2})};
    if (c.imag > Scalar{0}) {
      return value;
    } else {
      return -value;
    }
  }();
  return Complex<Scalar>(r, i);
}

template <typename Scalar>
[[nodiscard]] constexpr auto square(const Complex<Scalar>& c) noexcept {
  return c * c.conj();
}

template <typename Scalar>
[[nodiscard]] constexpr auto conjugate(const Complex<Scalar>& c) noexcept {
  return c.conj();
}

/**
 * Applies the sign of one scalar to the magnitude of a complex number.
 *
 * This overload extracts the magnitude of the complex value c using the abs function and
 * multiplies it by the sign of the Scalar s. It mimics the behavior of std::copysign for
 * real-to-complex magnitude adjustments.
 *
 * This is frequently used in stable Householder reflections or Jacobi rotations to avoid
 * catastrophic cancellation by choosing the reflection direction that is furthest from the
 * current vector.
 */
template <typename Scalar>
[[nodiscard]] constexpr auto copysign(const Complex<Scalar>& c, Scalar s) noexcept {
  using std::copysign;
  return Complex<Scalar>{copysign(c.real, s), copysign(c.imag, s)};
}

template <typename Scalar>
[[nodiscard]] constexpr auto copysign(const Complex<Scalar>& c, const Complex<Scalar> s) noexcept {
  return copysign(c, s.real * s.real - s.imag * s.imag);
}

}  // namespace ndyn::math
