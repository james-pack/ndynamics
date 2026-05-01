#pragma once

#include <array>
#include <cmath>
#include <limits>
#include <ostream>
#include <string>

#include "glog/logging.h"
#include "math/abs.h"

namespace ndyn::math {

template <size_t N, size_t M, typename S>
using Matrix = std::array<std::array<S, M>, N>;

template <typename S, size_t N>
std::string to_string(const std::array<S, N>& m) {
  using std::to_string;
  std::string result{};
  result.append("[");
  for (size_t i = 0; i < N; ++i) {
    if (i > 0) {
      result.append(", ");
    }
    result.append(to_string(m[i]));
  }
  result.append("]");

  return result;
}

template <typename S, size_t N>
std::ostream& operator<<(std::ostream& os, const std::array<S, N>& m) {
  os << to_string(m);
  return os;
}

template <size_t N, size_t M, typename S>
std::string to_string(const Matrix<N, M, S>& m) {
  using std::to_string;
  std::string result{};
  result.append("\n[");
  for (size_t i = 0; i < N; ++i) {
    if (i > 0) {
      result.append("\n ");
    }
    result.append("[");
    for (size_t j = 0; j < M; ++j) {
      if (j > 0) {
        result.append(", ");
      }
      result.append(to_string(m[i][j]));
    }
    result.append("]");
  }
  result.append("]\n");

  return result;
}

template <size_t N, size_t M, typename S>
std::ostream& operator<<(std::ostream& os, const Matrix<N, M, S>& m) {
  os << to_string(m);
  return os;
}

/**
 * Result of a singular value decomposition of a Matrix A following the
 * convention: A = U Σ Vᵀ.
 *
 * Variables u, s, and v correspond to the left singular vectors,
 * singular values, and right singular vectors respectively.
 *
 * See https://en.wikipedia.org/wiki/Singular_value_decomposition for more information.
 */
template <size_t NUM_ROWS, size_t NUM_COLUMNS, typename Scalar>
struct SVDResult {
  Matrix<NUM_ROWS, NUM_ROWS, Scalar> u{};
  std::array<Scalar, NUM_ROWS> s{};
  Matrix<NUM_ROWS, NUM_COLUMNS, Scalar> v{};
};

/**
 * Performs a Singular Value Decomposition (SVD) on a matrix using the one-sided Jacobi (Hestenes)
 * method.
 *
 * This function iteratively applies rotations to pairs of rows in the matrix to minimize their
 * covariance, effectively rotating the row-basis until all rows are mutually orthogonal. The
 * magnitudes of these orthogonalized rows yield the singular values S, while the accumulated
 * rotations form the left singular vectors U.
 *
 * This approach is used because it has good numerical stability for small, non-square matrices and
 * preserves relative accuracy for singular values that differ in magnitude.
 */
template <size_t NUM_ROWS, size_t NUM_COLUMNS, typename Scalar>
[[nodiscard]] static auto svd_solver(Matrix<NUM_ROWS, NUM_COLUMNS, Scalar> A,
                                     Scalar EPSILON) noexcept {
  using std::copysign;
  using std::sqrt;

  // The number of sweeps is proportional to the number of rows to be orthogonalized.
  static constexpr size_t MAX_SWEEPS{NUM_ROWS + size_t{10}};

  // rotation_accumulator accumulates the Givens rotations. It starts as an identity matrix.
  // At the end, rotation_accumulator[i][j] will represent the j-th component of the i-th singular
  // vector.
  Matrix<NUM_ROWS, NUM_ROWS, Scalar> rotation_accumulator{};
  for (size_t i = 0; i < NUM_ROWS; ++i) {
    rotation_accumulator[i][i] = Scalar{1};
  }

  // Main Jacobi sweep
  for (size_t sweep = 0; sweep < MAX_SWEEPS; ++sweep) {
    bool converged{true};

    for (size_t i = 0; i < NUM_ROWS; ++i) {
      for (size_t j = i + 1; j < NUM_ROWS; ++j) {
        // Compute the entries of the 2x2 symmetric matrix (A * A^T)
        auto a{Scalar{0}};
        auto b{Scalar{0}};
        auto c{Scalar{0}};

        for (size_t k = 0; k < NUM_COLUMNS; ++k) {
          a += A[i][k] * A[i][k];
          b += A[j][k] * A[j][k];
          c += A[i][k] * A[j][k];
        }

        // If these two rows are already orthogonal, skip the rotation.
        if (abs(c) < abs(EPSILON) or abs(c) < (abs(EPSILON) * abs(sqrt(a * b)))) {
          continue;
        }
        converged = false;

        // Compute the Jacobi rotation parameters (t, cos, sin)
        const auto tau{(b - a) / (Scalar{2} * c)};
        const auto t{copysign(Scalar{1} / (abs(tau) + abs(sqrt(Scalar{1} + tau * tau))), tau)};
        const auto cos_theta{Scalar{1} / sqrt(Scalar{1} + t * t)};
        const auto sin_theta{t * cos_theta};

        // Apply rotation to the rows of the working matrix A
        for (size_t k = 0; k < NUM_COLUMNS; ++k) {
          const auto tmp_i{A[i][k]};
          const auto tmp_j{A[j][k]};
          A[i][k] = cos_theta * tmp_i - sin_theta * tmp_j;
          A[j][k] = sin_theta * tmp_i + cos_theta * tmp_j;
        }

        // Apply the same rotation to the accumulator to track left singular vectors
        for (size_t k{0}; k < NUM_ROWS; ++k) {
          auto tmp_ui{rotation_accumulator[i][k]};
          auto tmp_uj{rotation_accumulator[j][k]};
          rotation_accumulator[i][k] = cos_theta * tmp_ui - sin_theta * tmp_uj;
          rotation_accumulator[j][k] = sin_theta * tmp_ui + cos_theta * tmp_uj;
        }
      }
    }

    if (converged) {
      break;
    }
  }

  // Extract singular values and finalize matrices
  std::array<Scalar, NUM_ROWS> s_vals{};
  Matrix<NUM_ROWS, NUM_ROWS, Scalar> u_final{};
  Matrix<NUM_ROWS, NUM_COLUMNS, Scalar> v_final{};

  for (size_t i = 0; i < NUM_ROWS; ++i) {
    auto norm_sq{Scalar{0}};
    for (size_t k = 0; k < NUM_COLUMNS; ++k) {
      norm_sq += A[i][k] * A[i][k];
    }
    s_vals[i] = sqrt(norm_sq);

    // Transpose the rotation accumulator into the output matrix.
    // The accumulator tracks vectors as rows.
    // The output matrix requires them as columns, so we transpose.
    for (size_t j = 0; j < NUM_ROWS; ++j) {
      u_final[j][i] = rotation_accumulator[i][j];
    }

    // Normalize the working matrix to get the right singular vectors (V transposed)
    if (s_vals[i] > Scalar{0}) {
      for (size_t k = 0; k < NUM_COLUMNS; ++k) {
        v_final[i][k] = A[i][k] / s_vals[i];
      }
    }
  }

  /**
   * Sorts the singular values and their corresponding singular vectors in descending order.
   * This block implements a selection sort on the s_vals array. When a larger singular value
   * is found later in the array, it swaps the values and performs a corresponding swap of
   * the columns in u_final and the rows in v_final to maintain the mathematical
   * relationship A = U S V.
   *
   * Canonical SVD representation requires singular values to be non-increasing. This
   * ensures that the primary components of the matrix are captured in the first few
   * indices, which is a requirement for numerical rank estimation and data compression.
   */
  DLOG(INFO) << ", u_final: " << to_string(u_final)        //
             << ", s_vals: " << to_string(s_vals) << "\n"  //
             << ", v_final: " << to_string(v_final);
  for (size_t i = 0; i < NUM_ROWS; ++i) {
    size_t max_idx{i};
    for (size_t j = i + 1; j < NUM_ROWS; ++j) {
      if (abs(s_vals[j]) > abs(s_vals[max_idx])) {
        max_idx = j;
      }
    }

    if (max_idx != i) {
      DLOG(INFO) << "Swapping i: " << i << " and max_idx: " << max_idx;
      // Swap singular values
      std::swap(s_vals[i], s_vals[max_idx]);

      // Swap columns in U. Swap the columns in the transposed V, which is like swapping the rows in
      // V.
      for (size_t k = 0; k < NUM_ROWS; ++k) {
        std::swap(u_final[k][i], u_final[k][max_idx]);
      }

      for (size_t k = 0; k < NUM_COLUMNS; ++k) {
        std::swap(v_final[i][k], v_final[max_idx][k]);
      }

      DLOG(INFO) << ", u_final: " << to_string(u_final)        //
                 << ", s_vals: " << to_string(s_vals) << "\n"  //
                 << ", v_final: " << to_string(v_final);
    }
  }

  return SVDResult<NUM_ROWS, NUM_COLUMNS, Scalar>{u_final, s_vals, v_final};
}

}  // namespace ndyn::math
