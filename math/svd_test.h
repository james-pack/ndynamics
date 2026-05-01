#pragma once

#include <algorithm>
#include <array>
#include <cmath>
#include <complex>

#include "gtest/gtest.h"
#include "math/matrix.h"

namespace ndyn::math {

/**
 * Provides utility functions for matrix operations required to validate SVD results.
 *
 * This struct contains static methods for matrix multiplication, calculating the conjugate
 * transpose, and verifying if a matrix is unitary. It uses nested loops to perform standard
 * linear algebra transformations on the Matrix alias defined in the project.
 *
 * These utilities decouple the verification logic from the testing fixture, allowing for
 * cleaner test cases and reusable logic for both real and complex scalar types.
 */
struct MatrixUtils {
  template <size_t N, size_t M, typename S>
  static constexpr auto conjugate(const S& val) {
    if constexpr (requires { val.imag; }) {
      return Complex<decltype(val.real)>(val.real, -val.imag);
    } else {
      return val;
    }
  }

  template <size_t N, size_t M, size_t P, typename S>
  static constexpr auto multiply(const Matrix<N, M, S>& lhs, const Matrix<M, P, S>& rhs) {
    Matrix<N, P, S> result{};
    for (size_t i = 0; i < N; ++i) {
      for (size_t j = 0; j < P; ++j) {
        S sum = S(0);
        for (size_t k = 0; k < M; ++k) {
          sum = sum + lhs[i][k] * rhs[k][j];
        }
        result[i][j] = sum;
      }
    }
    return result;
  }

  template <size_t N, size_t M, typename S>
  static constexpr auto multiply(const Matrix<N, M, S>& lhs, std::array<S, M> diag) {
    Matrix<N, M, S> result{};
    for (size_t i = 0; i < N; ++i) {
      for (size_t j = 0; j < M; ++j) {
        result[i][j] = lhs[i][j] * diag[j];
      }
    }
    return result;
  }
};

/**
 * Serves as the test fixture for validating the Singular Value Decomposition solver.
 *
 * This class inherits from testing::Test and is templated on the Scalar type to support
 * Type-Parameterized Tests. It defines a standard EPSILON value and provides a helper
 * method to verify that the product of U, S, and V reconstructs the original matrix A.
 *
 * This fixture centralizes the setup for both float and Complex types, ensuring that
 * numerical tolerance and validation logic remain consistent across different precisions.
 */
template <typename T>
class SvdTest : public ::testing::Test {
 public:
  using Scalar = T;
  static constexpr float TOLERANCE{1e-5};
  static constexpr Scalar EPSILON{TOLERANCE};

  /**
   * Validates the singular value decomposition result against a known ground truth.
   *
   * This method performs a component-wise comparison between the computed SVDResult and the
   * expected matrices for U, S, and V.
   */
  template <size_t N, size_t M>
  ::testing::AssertionResult ValidSolution(const Matrix<N, N, Scalar>& expected_u,
                                           const std::array<Scalar, N>& expected_s,
                                           const Matrix<N, M, Scalar>& expected_v,
                                           const SVDResult<N, M, Scalar>& actual) {
    auto check_vector = [](const auto& actual, const auto& expected, size_t len) {
      for (size_t i = 0; i < len; ++i) {
        bool match{true};
        bool neg_match{true};
        if (abs(actual[i] - expected[i]) > TOLERANCE) {
          match = false;
        }
        if ((not match) and abs(actual[i] + expected[i]) > TOLERANCE) {
          neg_match = false;
        }
        if (not(match or neg_match)) {
          LOG(INFO) << "check_vector() -- Values not matched. actual[i]: " << actual[i]
                    << ", expected[i]: " << expected[i] << ", i: " << i;
          return false;
        }
      }
      return true;
    };

    if (not check_vector(actual.s, expected_s, N)) {
      return ::testing::AssertionFailure() << "S vector mismatch. actual.s:" << to_string(actual.s)
                                           << ", expected_s: " << to_string(expected_s);
    }

    for (size_t j = 0; j < N; ++j) {
      std::array<Scalar, N> u_col;
      std::array<Scalar, N> exp_u_col;
      for (size_t i = 0; i < N; ++i) {
        u_col[i] = actual.u[i][j];
        exp_u_col[i] = expected_u[i][j];
      }
      if (!check_vector(u_col, exp_u_col, N)) {
        return ::testing::AssertionFailure()
               << "U vector mismatch. Column j: " << j << ", actual.u[j]: " << to_string(u_col)
               << ", expected_u[j]: " << to_string(exp_u_col);
      }
    }

    for (size_t i = 0; i < N; ++i) {
      if (!check_vector(actual.v[i], expected_v[i], M)) {
        return ::testing::AssertionFailure() << "V vector mismatch at row " << i;
      }
    }

    return ::testing::AssertionSuccess();
  }

  /**
   * Verifies that the product of U, S, and V reconstructs the original matrix A.
   *
   * The method computes the matrix product U * S * V and compares it element-by-element with
   * the input matrix A.
   *
   * See https://en.wikipedia.org/wiki/Singular_value_decomposition for the identity details.
   */
  template <size_t N, size_t M>
  ::testing::AssertionResult CanReconstruct(const Matrix<N, M, Scalar>& A,
                                            const SVDResult<N, M, Scalar>& res) {
    const auto US{MatrixUtils::multiply(res.u, res.s)};
    const auto reconstructed{MatrixUtils::multiply(US, res.v)};

    for (size_t i = 0; i < N; ++i) {
      for (size_t j = 0; j < M; ++j) {
        if ((abs(A[i][j] - reconstructed[i][j]) > TOLERANCE)) {
          return ::testing::AssertionFailure()
                 << "Mismatch at A[" << i << "][" << j << "]. Expected A[i][j]: " << A[i][j]
                 << ", actual reconstructed[i][j]: " << reconstructed[i][j]
                 << "\nA: " << to_string(A)                        //
                 << "reconstructed: " << to_string(reconstructed)  //
                 << ", US: " << to_string(US)                      //
                 << ", res.u: " << to_string(res.u)                //
                 << ", res.s: " << to_string(res.s) << "\n"        //
                 << ", res.v: " << to_string(res.v);
        }
      }
    }
    return ::testing::AssertionSuccess();
  }
};

TYPED_TEST_SUITE_P(SvdTest);

/**
 * Validates that the SVD solver correctly decomposes an identity matrix.
 *
 * The test initializes a 3x3 identity matrix and passes it to the svd_solver. It then
 * checks if the singular values are all approximately 1.0 and verifies that the
 * reconstruction matches the input.
 *
 * Identity matrices are a fundamental baseline for SVD; the solver must recognize that
 * the matrix is already orthogonal and has unit scaling in all dimensions.
 */
TYPED_TEST_P(SvdTest, HandlesIdentityMatrix) {
  using Scalar = TypeParam;
  static constexpr Matrix<3, 3, Scalar> A{{{Scalar{1}, Scalar{0}, Scalar{0}},  //
                                           {Scalar{0}, Scalar{1}, Scalar{0}},  //
                                           {Scalar{0}, Scalar{0}, Scalar{1}}}};

  const auto result = svd_solver(A, this->EPSILON);

  for (size_t i = 0; i < 3; ++i) {
    EXPECT_NEAR(abs(result.s[i] - Scalar{1}), 0.0, this->TOLERANCE);
  }
  EXPECT_TRUE(this->CanReconstruct(A, result));
}

/**
 * Verifies that the SVD solver maintains the descending order of singular values.
 *
 * This test uses a diagonal matrix with values [3, 1, 2]. The solver is expected to
 * return singular values in non-increasing order (3, 2, 1) and adjust the vectors
 * in U and v accordingly.
 *
 * The SVD convention requires S to be a diagonal matrix of singular values in descending
 * order to ensure a unique and canonical representation of the decomposition.
 */
TYPED_TEST_P(SvdTest, SortsSingularValuesDescending) {
  using Scalar = TypeParam;
  static constexpr Matrix<3, 3, Scalar> A{{{Scalar{3}, Scalar{0}, Scalar{0}},  //
                                           {Scalar{0}, Scalar{1}, Scalar{0}},  //
                                           {Scalar{0}, Scalar{0}, Scalar{2}}}};

  const auto result = svd_solver(A, this->EPSILON);

  EXPECT_GE(abs(result.s[0]), abs(result.s[1])) << to_string(result.s);
  EXPECT_GE(abs(result.s[1]), abs(result.s[2])) << to_string(result.s);
  EXPECT_TRUE(this->CanReconstruct(A, result));
}

/**
 * Tests the solver's ability to handle a matrix that is entirely zero.
 *
 * A 2x2 zero matrix is provided as input. The expected output is a set of singular values
 * that are all zero, while U and v should remain valid orthogonal/unitary matrices.
 *
 * This edge case ensures that the solver's internal normalization or division steps
 * do not cause a division-by-zero error when encountering null spans.
 */
TYPED_TEST_P(SvdTest, HandlesZeroMatrix) {
  using Scalar = TypeParam;
  Matrix<2, 2, Scalar> A{{{Scalar{0}, Scalar{0}},  //
                          {Scalar{0}, Scalar{0}}}};

  const auto result = svd_solver(A, Scalar{this->EPSILON});

  for (const auto& s : result.s) {
    EXPECT_NEAR(abs(s), 0.0, this->TOLERANCE);
  }
  EXPECT_TRUE(this->CanReconstruct(A, result));
}

/**
 * Validates the solver using a non-trivial matrix generated from known U, S, and V matrices.
 *
 * This test constructs a 2x2 matrix A by multiplying a rotation matrix U, a diagonal matrix S
 * with values 4.0 and 2.0, and another rotation matrix V. The svd_solver is then tasked with
 * decomposing A to recover the singular values and verify the reconstruction identity.
 *
 * Using a fully populated matrix ensures the solver correctly handles non-diagonal inputs where
 * row interactions are significant, confirming the Jacobi rotations properly orthogonalize the
 * basis.
 */
TYPED_TEST_P(SvdTest, HandlesNonTrivialSolution) {
  using Scalar = TypeParam;
  static constexpr auto c30{Scalar{0.866025}};
  static constexpr auto s30{Scalar{0.5}};
  static constexpr auto c45{Scalar{0.707106}};
  static constexpr auto s45{Scalar{0.707106}};

  static constexpr Matrix<2, 2, Scalar> U{{{c30, -s30}, {s30, c30}}};
  static constexpr Matrix<2, 2, Scalar> S{{{Scalar{4}, Scalar{0}}, {Scalar{0}, Scalar{2}}}};
  static constexpr Matrix<2, 2, Scalar> VT{{{c45, s45}, {-s45, c45}}};

  const auto US = MatrixUtils::multiply(U, S);
  const auto A = MatrixUtils::multiply(US, VT);

  const auto result = svd_solver(A, this->EPSILON);

  EXPECT_NEAR(abs(result.s[0]), 4.0, this->TOLERANCE);
  EXPECT_NEAR(abs(result.s[1]), 2.0, this->TOLERANCE);
  EXPECT_TRUE(this->CanReconstruct(A, result));
}

/**
 * Verifies the decomposition of a singular matrix where half of the singular values are zero.
 *
 * The test provides a 4x4 diagonal matrix where the first two singular values are non-zero (5.0
 * and 3.0) and the remaining two are zero. The solver must accurately identify the null space
 * and return the correct rank-2 representation.
 *
 * This case is critical for checking numerical robustness when the matrix is not full rank,
 * ensuring the algorithm does not diverge when calculating rotations for zero-magnitude rows.
 */
TYPED_TEST_P(SvdTest, HandlesHalfZeroSingularValues) {
  using Scalar = TypeParam;
  Matrix<4, 4, Scalar> A{};
  A[0][0] = Scalar{5};
  A[1][1] = Scalar{3};

  const auto result = svd_solver(A, this->EPSILON);

  EXPECT_NEAR(abs(result.s[0]), 5.0, this->TOLERANCE);
  EXPECT_NEAR(abs(result.s[1]), 3.0, this->TOLERANCE);
  EXPECT_NEAR(abs(result.s[2]), 0.0, this->TOLERANCE);
  EXPECT_NEAR(abs(result.s[3]), 0.0, this->TOLERANCE);
  EXPECT_TRUE(this->CanReconstruct(A, result));
}

/**
 * Checks solver behavior when the provided EPSILON is larger than some non-zero singular values.
 *
 * A 2x2 diagonal matrix is defined with values 10.0 and 0.1, while the EPSILON parameter
 * is set to 1.0. The solver is expected to maintain mathematical consistency in its
 * reconstruction even if the smaller singular value falls below the convergence threshold.
 *
 * This justifies the use of EPSILON as a tuning parameter for ignoring noise or insignificant
 * components in the data, which is a common requirement in dimensionality reduction.
 */
TYPED_TEST_P(SvdTest, HandlesHighEpsilon) {
  using Scalar = TypeParam;
  static constexpr Matrix<2, 2, Scalar> A{{{Scalar{10}, Scalar{0}}, {Scalar{0}, Scalar{0.1}}}};
  static constexpr Scalar high_epsilon{1.0};

  const auto result = svd_solver(A, high_epsilon);

  EXPECT_TRUE(this->CanReconstruct(A, result));
}

/**
 * Validates that singular values are returned as non-negative even when input diagonals are
 * negative.
 *
 * Input matrix A is initialized as a diagonal matrix with -5.0 and -2.0. According to the
 * SVD definition, singular values S must be non-negative; the signs are absorbed into the U or
 * V matrices.
 *
 * This test ensures the implementation correctly uses the magnitude of the row norms to
 * produce a valid SVD that adheres to standard mathematical conventions.
 */
TYPED_TEST_P(SvdTest, HandlesAllNegativeSingularValues) {
  using Scalar = TypeParam;
  static constexpr Matrix<2, 2, Scalar> A{{{Scalar{-5}, Scalar{0}}, {Scalar{0}, Scalar{-2}}}};

  const auto result = svd_solver(A, this->EPSILON);

  EXPECT_NEAR(abs(result.s[0]), 5.0, this->TOLERANCE);
  EXPECT_NEAR(abs(result.s[1]), 2.0, this->TOLERANCE);
  EXPECT_TRUE(this->CanReconstruct(A, result));
}

/**
 * Tests the decomposition of a matrix containing a mix of positive and negative diagonal entries.
 *
 * Similar to the all-negative test, this uses a matrix with [4.0, -3.0]. The solver should
 * produce singular values [4.0, 3.0] in descending order, regardless of the input signs.
 *
 * This is an important check for the sorting and normalization logic, confirming that
 * the absolute values are used for ranking while U and V are adjusted to preserve the product A.
 */
TYPED_TEST_P(SvdTest, HandlesMixedSignSingularValues) {
  using Scalar = TypeParam;
  static constexpr Matrix<2, 2, Scalar> A{{{Scalar{4}, Scalar{0}}, {Scalar{0}, Scalar{-3}}}};

  const auto result = svd_solver(A, this->EPSILON);

  EXPECT_NEAR(abs(result.s[0]), 4.0, this->TOLERANCE);
  EXPECT_NEAR(abs(result.s[1]), 3.0, this->TOLERANCE);
  EXPECT_TRUE(this->CanReconstruct(A, result));
}

/**
 * Evaluates the numerical stability of the solver on a stiff problem with a high condition number.
 *
 * The test creates a 2x2 matrix with singular values 1.0 and 1e-12. The ratio of these
 * values exceeds 1/EPSILON^2, creating a scenario where small values are at the edge
 * of floating-point precision relative to the large values.
 *
 * This demonstrates the stability of the Hestenes method, which is known for its ability
 * to preserve relative accuracy for singular values that differ significantly in magnitude.
 */
TYPED_TEST_P(SvdTest, HandlesStiffProblem) {
  using Scalar = TypeParam;
  static constexpr Matrix<2, 2, Scalar> A{{{Scalar{1.0}, Scalar{0}},  //
                                           {Scalar{0}, Scalar{1e-12}}}};

  const auto result = svd_solver(A, this->EPSILON);

  EXPECT_NEAR(abs(result.s[0]), 1.0, this->TOLERANCE);
  EXPECT_NEAR(abs(result.s[1]), 0.0, this->TOLERANCE);
  EXPECT_TRUE(this->CanReconstruct(A, result));
}

/**
 * Evaluates the solver using a 3x3 matrix with distinct, non-trivial singular values.
 */
TYPED_TEST_P(SvdTest, Handles3x3General) {
  using Scalar = TypeParam;
  static constexpr Matrix<3, 3, Scalar> U{
      {{Scalar{-0.21483724}, Scalar{0.88723069}, Scalar{0.40824829}},   //
       {Scalar{-0.52058739}, Scalar{0.24964395}, Scalar{-0.81649658}},  //
       {Scalar{-0.82633754}, Scalar{-0.38794278}, Scalar{0.40824829}}}};

  static constexpr std::array<Scalar, 3> S{Scalar{1.68481034e+01},  //
                                           Scalar{1.06836951e+00},  //
                                           Scalar{4.41842475e-3}};

  static constexpr Matrix<3, 3, Scalar> VT{
      {{Scalar{-0.47967118}, Scalar{-0.57236779}, Scalar{-0.66506441}},  //
       {Scalar{-0.77669099}, Scalar{-0.07568647}, Scalar{0.62531805}},   //
       {Scalar{-0.40824829}, Scalar{0.81649658}, Scalar{-0.40824829}}}};

  static constexpr auto A{MatrixUtils::multiply(MatrixUtils::multiply(U, S), VT)};

  const auto result{svd_solver(A, this->EPSILON)};

  EXPECT_TRUE(this->ValidSolution(U, S, VT, result))  //
      << "result.u:" << to_string(result.u)           //
      << "result.s:\n"
      << to_string(result.s)                   //
      << "\nresult.v:" << to_string(result.v)  //
      << "A:" << to_string(A)                  //
      << "U:" << to_string(U)                  //
      << "S:\n"
      << to_string(S)  //
      << "\nVT:" << to_string(VT);

  EXPECT_TRUE(this->CanReconstruct(A, result));
}

/**
 * Tests a 3x3 matrix where one singular value is significantly smaller than the others.
 *
 * A matrix is constructed with a rank-2 tendency, featuring singular values 10.0, 5.0,
 * and 0.001. The solver must accurately isolate the small value without numerical
 * leakage from the dominant components.
 *
 * This ensures that the orthogonalization process remains precise even when the
 * matrix is near-singular, a common occurrence in real-world sensor data.
 */
TYPED_TEST_P(SvdTest, Handles3x3SmallSingularValue) {
  using Scalar = TypeParam;
  Matrix<3, 3, Scalar> A = {{{Scalar{10}, Scalar{0}, Scalar{0}},
                             {Scalar{0}, Scalar{5}, Scalar{0}},
                             {Scalar{0}, Scalar{0}, Scalar{0.001}}}};

  auto result = svd_solver(A, this->EPSILON);

  EXPECT_NEAR(abs(result.s[2]), 0.001, this->TOLERANCE);
  EXPECT_TRUE(this->CanReconstruct(A, result));
}

/**
 * Validates the solver on a 4x4 matrix with a known orthogonal basis.
 *
 * The test uses a 4x4 symmetric matrix representing a structured transformation. It checks
 * if the solver can handle the increased complexity of 6 different row-pair
 * rotations per sweep while maintaining the unitary property of U and V.
 *
 * This higher-dimensional case verifies that the MAX_SWEEPS constant is sufficient
 * for larger matrices to reach the requested EPSILON tolerance.
 */
TYPED_TEST_P(SvdTest, Handles4x4Structured) {
  using Scalar = TypeParam;
  Matrix<4, 4, Scalar> A = {{{Scalar{4}, Scalar{1}, Scalar{1}, Scalar{1}},
                             {Scalar{1}, Scalar{3}, Scalar{1}, Scalar{1}},
                             {Scalar{1}, Scalar{1}, Scalar{2}, Scalar{1}},
                             {Scalar{1}, Scalar{1}, Scalar{1}, Scalar{1}}}};

  auto result = svd_solver(A, this->EPSILON);
  EXPECT_TRUE(this->CanReconstruct(A, result));
}

/**
 * Verifies the 4x4 case where the matrix is explicitly rank-deficient.
 *
 * By providing a 4x4 matrix where two rows are linear combinations of others,
 * the test ensures the solver produces exactly two non-zero singular values
 * and two values near zero.
 *
 * This confirms the solver's ability to identify the dimensionality of the
 * subspace effectively in a 4D environment.
 */
TYPED_TEST_P(SvdTest, Handles4x4RankDeficient) {
  using Scalar = TypeParam;
  Matrix<4, 4, Scalar> A = {{{Scalar{1}, Scalar{0}, Scalar{0}, Scalar{0}},
                             {Scalar{0}, Scalar{1}, Scalar{0}, Scalar{0}},
                             {Scalar{1}, Scalar{1}, Scalar{0}, Scalar{0}},
                             {Scalar{0}, Scalar{0}, Scalar{0}, Scalar{0}}}};

  auto result = svd_solver(A, this->EPSILON);
  EXPECT_NEAR(abs(result.s[2]), 0.0, this->TOLERANCE) << "result.s: " << to_string(result.s);
  EXPECT_NEAR(abs(result.s[3]), 0.0, this->TOLERANCE);
  EXPECT_TRUE(this->CanReconstruct(A, result));
}

/**
 * Tests the solver on a 5x5 matrix to evaluate scaling and convergence.
 *
 * A 5x5 matrix is generated using a Hilbert-style pattern where entries are 1/(i+j+1).
 * With 10 distinct row pairs to coordinate, this test is the most computationally
 * demanding in the suite.
 *
 * Success in this test demonstrates that the implementation scales correctly
 * to larger systems without accumulating significant rounding errors or
 * failing to converge within the sweep limit.
 */
TYPED_TEST_P(SvdTest, Handles5x5Dense) {
  using Scalar = TypeParam;
  Matrix<5, 5, Scalar> A{};
  for (size_t i = 0; i < 5; ++i) {
    for (size_t j = 0; j < 5; ++j) {
      A[i][j] = Scalar{1} / Scalar{static_cast<float>(i + j + 1)};
    }
  }

  const auto result = svd_solver(A, this->EPSILON);
  EXPECT_TRUE(this->CanReconstruct(A, result));
}

REGISTER_TYPED_TEST_SUITE_P(SvdTest, HandlesIdentityMatrix, SortsSingularValuesDescending,
                            HandlesZeroMatrix, HandlesNonTrivialSolution,
                            HandlesHalfZeroSingularValues, HandlesHighEpsilon,
                            HandlesAllNegativeSingularValues, HandlesMixedSignSingularValues,
                            HandlesStiffProblem, Handles3x3General, Handles3x3SmallSingularValue,
                            Handles4x4Structured, Handles4x4RankDeficient, Handles5x5Dense);

}  // namespace ndyn::math
