#include "math/complex.h"

#include <cmath>
#include <compare>

#include "gtest/gtest.h"

namespace ndyn::math {

template <typename T>
class ComplexTest : public ::testing::Test {
 protected:
  static constexpr T EPSILON = std::is_same_v<T, float> ? 1e-5f : 1e-12;
};

TYPED_TEST_SUITE_P(ComplexTest);

/**
 * Verifies that the default constructor correctly zero-initializes the real and imag components.
 *
 * The implementation relies on brace-initialization of the Scalar members to ensure that no
 * residual memory values remain, providing a predictable zero-state for linear algebra kernels.
 *
 * This test ensures that the Scalar type supports default construction and that the Complex
 * wrapper does not introduce uninitialized state.
 */
TYPED_TEST_P(ComplexTest, DefaultConstruction) {
  using Scalar = TypeParam;
  static constexpr ndyn::math::Complex<Scalar> c{};
  static_assert(c.real == Scalar{0});
  static_assert(c.imag == Scalar{0});
  EXPECT_EQ(c.real, Scalar{0});
  EXPECT_EQ(c.imag, Scalar{0});
}

TYPED_TEST_P(ComplexTest, ValueConstruction) {
  using Scalar = TypeParam;
  static constexpr ndyn::math::Complex<Scalar> c{Scalar{1.5}, Scalar{-2.5}};
  EXPECT_EQ(c.real, Scalar{1.5});
  EXPECT_EQ(c.imag, Scalar{-2.5});
}

TYPED_TEST_P(ComplexTest, Addition) {
  using Scalar = TypeParam;
  static constexpr ndyn::math::Complex<Scalar> a{Scalar{1}, Scalar{2}};
  static constexpr ndyn::math::Complex<Scalar> b{Scalar{3}, Scalar{4}};
  static constexpr auto result{a + b};
  static_assert(result == Complex{Scalar{4}, Scalar{6}});
  EXPECT_EQ(result.real, Scalar{4});
  EXPECT_EQ(result.imag, Scalar{6});
}

TYPED_TEST_P(ComplexTest, Subtraction) {
  using Scalar = TypeParam;
  static constexpr ndyn::math::Complex<Scalar> a{Scalar{1}, Scalar{2}};
  static constexpr ndyn::math::Complex<Scalar> b{Scalar{3}, Scalar{4}};
  static constexpr auto result{a - b};
  EXPECT_EQ(result.real, Scalar{-2});
  EXPECT_EQ(result.imag, Scalar{-2});
}

TYPED_TEST_P(ComplexTest, Multiplication) {
  using Scalar = TypeParam;
  static constexpr ndyn::math::Complex<Scalar> a{Scalar{1}, Scalar{2}};
  static constexpr ndyn::math::Complex<Scalar> b{Scalar{3}, Scalar{4}};
  static constexpr auto result{a * b};
  EXPECT_EQ(result.real, Scalar{-5});
  EXPECT_EQ(result.imag, Scalar{10});
}

TYPED_TEST_P(ComplexTest, ScalarDivision) {
  using Scalar = TypeParam;
  static constexpr ndyn::math::Complex<Scalar> a{Scalar{4}, Scalar{8}};
  static constexpr Scalar b{2};
  static constexpr auto result{a / b};
  EXPECT_EQ(result.real, Scalar{2});
  EXPECT_EQ(result.imag, Scalar{4});
}

TYPED_TEST_P(ComplexTest, ComplexDivision) {
  using Scalar = TypeParam;
  static constexpr ndyn::math::Complex<Scalar> a{Scalar{1}, Scalar{1}};
  static constexpr ndyn::math::Complex<Scalar> b{Scalar{1}, Scalar{1}};
  static constexpr auto result{a / b};
  EXPECT_NEAR(Scalar{1}, result.real, this->EPSILON);
  EXPECT_NEAR(Scalar{0}, result.imag, this->EPSILON);
}

/**
 * Verifies that the absolute value function correctly calculates the L2 norm of the complex number.
 */
TYPED_TEST_P(ComplexTest, AbsoluteValue) {
  using Scalar = TypeParam;
  static constexpr ndyn::math::Complex<Scalar> c{Scalar{3}, Scalar{4}};
  EXPECT_NEAR(Scalar{5}, abs(c), this->EPSILON);
}

/**
 * Verifies that square root calculations correctly handle the principal branch.
 *
 * The function uses the identity involving the magnitude and real part to calculate the root,
 * ensuring that the resulting imaginary part maintains the sign of the inputs imaginary part.
 */
TYPED_TEST_P(ComplexTest, SquareRootPrincipalBranch) {
  using Scalar = TypeParam;
  static constexpr ndyn::math::Complex<Scalar> c{Scalar{0}, Scalar{2}};
  static constexpr auto result{sqrt(c)};
  EXPECT_NEAR(Scalar{1}, result.real, this->EPSILON);
  EXPECT_NEAR(Scalar{1}, result.imag, this->EPSILON);
}

/**
 * Tests the copysign overload for steering complex numbers using a real scalar sign.
 *
 * The function applies the sign bit of Scalar s to the real and imaginary components of the
 * complex input c, effectively shifting the result across the quadrants of the complex plane.
 */
TYPED_TEST_P(ComplexTest, CopySignScalar) {
  using Scalar = TypeParam;
  static constexpr ndyn::math::Complex<Scalar> c{Scalar{1}, Scalar{1}};
  static constexpr auto result_neg{copysign(c, Scalar{-1.0})};
  static constexpr auto result_pos{copysign(c, Scalar{1.0})};
  EXPECT_EQ(result_neg.real, Scalar{-1});
  EXPECT_EQ(result_neg.imag, Scalar{-1});
  EXPECT_EQ(result_pos.real, Scalar{1});
  EXPECT_EQ(result_pos.imag, Scalar{1});
}

/**
 * Tests the copysign overload for steering complex numbers using another complex number's sign.
 *
 * This logic relies on the sign of the squared difference of the steering number's components,
 * providing a consistent way to align complex numbers based on their dominant component.
 */
TYPED_TEST_P(ComplexTest, CopySignComplex) {
  using Scalar = TypeParam;
  static constexpr ndyn::math::Complex<Scalar> c{Scalar{1}, Scalar{1}};
  // s.real^2 - s.imag^2 is negative
  static constexpr ndyn::math::Complex<Scalar> neg{Scalar{0}, Scalar{2}};
  // s.real^2 - s.imag^2 is zero
  static constexpr ndyn::math::Complex<Scalar> zero{Scalar{2}, Scalar{2}};
  // s.real^2 - s.imag^2 is positive
  static constexpr ndyn::math::Complex<Scalar> pos{Scalar{2}, Scalar{1}};
  static constexpr auto result_neg{copysign(c, neg)};
  EXPECT_EQ(result_neg.real, -c.real);
  EXPECT_EQ(result_neg.imag, -c.imag);
  static constexpr auto result_zero{copysign(c, zero)};
  EXPECT_EQ(result_zero.real, c.real);
  EXPECT_EQ(result_zero.imag, c.imag);
  static constexpr auto result_pos{copysign(c, pos)};
  EXPECT_EQ(result_pos.real, c.real);
  EXPECT_EQ(result_pos.imag, c.imag);
}

template <typename S, typename Ordering>
::testing::AssertionResult validate_order(S r1, S i1, S r2, S i2, Ordering expected) {
  const Complex<S> a{r1, i1};
  const Complex<S> b{r2, i2};
  const auto actual = (a <=> b);

  using ActualOrdering = decltype(a <=> b);
  if constexpr (!std::is_same_v<ActualOrdering, Ordering>) {
    return ::testing::AssertionFailure()
           << "Ordering category mismatch. "
           << "expected ordering type: " << typeid(Ordering).name()
           << ", actual type of the result of the spaceship operator: "
           << typeid(ActualOrdering).name();
  }

  if (actual != expected) {
    return ::testing::AssertionFailure() << "Spaceship operator mismatch." << "  actual: "
                                         << (actual == Ordering::less      ? "less"
                                             : actual == Ordering::greater ? "greater"
                                                                           : "equivalent")
                                         << ", expected: "
                                         << (expected == Ordering::less      ? "less"
                                             : expected == Ordering::greater ? "greater"
                                                                             : "equivalent");
  }

  // Verify all derivative boolean operators
  const bool is_less = (expected == Ordering::less);
  const bool is_equal = (expected == Ordering::equivalent);
  const bool is_greater = (expected == Ordering::greater);

  if ((a < b) != is_less) return ::testing::AssertionFailure() << "operator < failed";
  if ((a <= b) != (is_less || is_equal))
    return ::testing::AssertionFailure() << "operator <= failed";
  // Note that for a type like Complex where ordering is based on magnitude or a similar measure, ==
  // and != should not be used.
  // if ((a == b) != is_equal) return ::testing::AssertionFailure() << "operator == failed";
  // if ((a != b) != !is_equal) return ::testing::AssertionFailure() << "operator != failed";
  if ((a >= b) != (is_greater || is_equal))
    return ::testing::AssertionFailure() << "operator >= failed";
  if ((a > b) != is_greater) return ::testing::AssertionFailure() << "operator > failed";

  return ::testing::AssertionSuccess();
}

/**
 * Evaluates the ordering operators to ensure strict weak ordering based on magnitude.
 *
 * Comparison is defined by the absolute value (magnitude) of the complex numbers.
 */
TYPED_TEST_P(ComplexTest, SpaceshipOrdering) {
  using Scalar = TypeParam;
  static constexpr ndyn::math::Complex<Scalar> small{Scalar{1}, Scalar{0}};
  static constexpr ndyn::math::Complex<Scalar> large{Scalar{0}, Scalar{2}};
  EXPECT_TRUE((small <=> large) == std::weak_ordering::less);
}

TYPED_TEST_P(ComplexTest, ValidateOrdering) {
  using Scalar = TypeParam;
  auto HasOrder = [](Scalar r1, Scalar i1, Scalar r2, Scalar i2, std::weak_ordering order) {
    return validate_order(r1, i1, r2, i2, order);
  };
  EXPECT_TRUE(HasOrder(1, 0, 0, 2, std::weak_ordering::less));
  EXPECT_TRUE(HasOrder(1, 1, 0, 2, std::weak_ordering::less));
  EXPECT_TRUE(HasOrder(2, 0, 0, 2, std::weak_ordering::equivalent));
  EXPECT_TRUE(HasOrder(2, 0, 2, 0, std::weak_ordering::equivalent));
  EXPECT_TRUE(HasOrder(-1, 0, 0, 2, std::weak_ordering::less));
  EXPECT_TRUE(HasOrder(-2, 0, 0, 2, std::weak_ordering::equivalent));
  EXPECT_TRUE(HasOrder(-2, 0, 0, -2, std::weak_ordering::equivalent));
  EXPECT_TRUE(HasOrder(2, 0, 0, -2, std::weak_ordering::equivalent));
  EXPECT_TRUE(HasOrder(-1, 1, 0, -2, std::weak_ordering::less));
}

REGISTER_TYPED_TEST_SUITE_P(ComplexTest, DefaultConstruction, ValueConstruction, Addition,
                            Subtraction, Multiplication, ScalarDivision, ComplexDivision,
                            AbsoluteValue, SquareRootPrincipalBranch, CopySignScalar,
                            CopySignComplex, SpaceshipOrdering, ValidateOrdering);

using TestingTypes = ::testing::Types<float, double>;
INSTANTIATE_TYPED_TEST_SUITE_P(NdynMath, ComplexTest, TestingTypes);

}  // namespace ndyn::math
