#pragma once

#include <limits>

namespace ndyn::math {

template <typename MultivectorType>
size_t index_to_bit_basis(size_t index) {
  return MultivectorType::bit_basis_indices[index];
}

template <typename MultivectorType>
size_t index_from_bit_basis(size_t index) {
  for (size_t i = 0; i < MultivectorType::NUM_BASES; ++i) {
    if (MultivectorType::bit_basis_indices[i] == index) {
      return i;
    }
  }
  return std::numeric_limits<size_t>::max();
}

template <typename MultivectorType>
float basis_sign_from_bit_basis(size_t bit_basis_index) {
  if (MultivectorType::reversed_bases[index_from_bit_basis<MultivectorType>(bit_basis_index)]) {
    return -1.f;
  } else {
    return 1.f;
  }
}

template <typename MultivectorType>
float basis_sign(size_t index) {
  if (MultivectorType::reversed_bases[index]) {
    return -1.f;
  } else {
    return 1.f;
  }
}

}  // namespace ndyn::math
