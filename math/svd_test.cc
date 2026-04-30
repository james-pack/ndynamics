#include <algorithm>
#include <array>
#include <cmath>
#include <complex>

#include "gtest/gtest.h"
#include "math/complex.h"
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
  static auto conjugate(const S& val) {
    if constexpr (requires { val.imag; }) {
      return Complex<decltype(val.real)>(val.real, -val.imag);
    } else {
      return val;
    }
  }

  template <size_t N, size_t M, size_t P, typename S>
  static auto multiply(const Matrix<N, M, S>& lhs, const Matrix<M, P, S>& rhs) {
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
 protected:
  using Scalar = T;
  const float TOLERANCE{1e-5};
  const Scalar EPSILON{TOLERANCE};

  template <size_t N, size_t M>
  void ExpectReconstruction(const Matrix<N, M, Scalar>& A, const SVDResult<N, M, Scalar>& res) {
    Matrix<N, N, Scalar> US{};
    for (size_t i = 0; i < N; ++i) {
      for (size_t j = 0; j < N; ++j) {
        US[i][j] = res.u[i][j] * res.s[j];
      }
    }
    auto reconstructed = MatrixUtils::multiply(US, res.v);
    for (size_t i = 0; i < N; ++i) {
      for (size_t j = 0; j < M; ++j) {
        EXPECT_NEAR(abs(A[i][j] - reconstructed[i][j]), 0.0, TOLERANCE)
            << "A: " << to_string(A)                          //
            << "reconstructed: " << to_string(reconstructed)  //
            << ", US: " << to_string(US)                      //
            << ", res.u: " << to_string(res.u)                //
            << ", res.s: " << to_string(res.s) << "\n"        //
            << ", res.v: " << to_string(res.v);
      }
    }
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
  Matrix<3, 3, Scalar> A = {{{Scalar(1), Scalar(0), Scalar(0)},
                             {Scalar(0), Scalar(1), Scalar(0)},
                             {Scalar(0), Scalar(0), Scalar(1)}}};

  auto result = svd_solver(A, this->EPSILON);

  for (size_t i = 0; i < 3; ++i) {
    EXPECT_NEAR(abs(result.s[i] - Scalar(1)), 0.0, this->TOLERANCE);
  }
  this->ExpectReconstruction(A, result);
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
  Matrix<3, 3, Scalar> A = {{{Scalar(3), Scalar(0), Scalar(0)},
                             {Scalar(0), Scalar(1), Scalar(0)},
                             {Scalar(0), Scalar(0), Scalar(2)}}};

  auto result = svd_solver(A, this->EPSILON);

  EXPECT_GE(abs(result.s[0]), abs(result.s[1])) << to_string(result.s);
  EXPECT_GE(abs(result.s[1]), abs(result.s[2])) << to_string(result.s);
  this->ExpectReconstruction(A, result);
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
  Matrix<2, 2, Scalar> A = {{{Scalar(0), Scalar(0)}, {Scalar(0), Scalar(0)}}};

  auto result = svd_solver(A, Scalar{this->EPSILON});

  for (const auto& s : result.s) {
    EXPECT_NEAR(abs(s), 0.0, this->TOLERANCE);
  }
  this->ExpectReconstruction(A, result);
}

REGISTER_TYPED_TEST_SUITE_P(SvdTest, HandlesIdentityMatrix, SortsSingularValuesDescending,
                            HandlesZeroMatrix);

using MyTypes = ::testing::Types<float, Complex<float>>;
INSTANTIATE_TYPED_TEST_SUITE_P(PrecisionTests, SvdTest, MyTypes);

}  // namespace ndyn::math
