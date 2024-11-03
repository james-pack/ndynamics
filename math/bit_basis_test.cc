#include "math/bit_basis.h"

#include "gtest/gtest.h"

namespace ndyn::math {

template <size_t BASES>
::testing::AssertionResult CanRoundTripIndices() {
  using std::to_string;
  for (size_t common_index = 0; common_index < (1UL << BASES); ++common_index) {
    const size_t bit_basis_index{to_bit_basis_index<BASES>(common_index)};
    const size_t round_tripped{from_bit_basis_index<BASES>(bit_basis_index)};
    if (round_tripped != common_index) {
      return ::testing::AssertionFailure()
             << "common_index: " << to_string(common_index)
             << ", bit_basis_index: " << to_string(bit_basis_index)
             << " (round tripped: " << to_string(round_tripped) << ")";
    }
  }
  return ::testing::AssertionSuccess();
}

TEST(BasisSize0Test, CanRoundTrip) { EXPECT_TRUE(CanRoundTripIndices<0>()); }
TEST(BasisSize1Test, CanRoundTrip) { EXPECT_TRUE(CanRoundTripIndices<1>()); }
TEST(BasisSize2Test, CanRoundTrip) { EXPECT_TRUE(CanRoundTripIndices<2>()); }
TEST(BasisSize3Test, CanRoundTrip) { EXPECT_TRUE(CanRoundTripIndices<3>()); }
TEST(BasisSize4Test, CanRoundTrip) { EXPECT_TRUE(CanRoundTripIndices<4>()); }
TEST(BasisSize5Test, CanRoundTrip) { EXPECT_TRUE(CanRoundTripIndices<5>()); }

}  // namespace ndyn::math
