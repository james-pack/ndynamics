#include "math/bitset.h"

#include <limits>

#include "gtest/gtest.h"

namespace ndyn::math {

TEST(BitSetTest, EmptyBitSetHasZeros) { EXPECT_EQ(0UL, BitSet<64>{}.to_ulong()); }

TEST(BitSetTest, CanInitializeWithULong) { EXPECT_EQ(4UL, BitSet<64>{4UL}.to_ulong()); }

TEST(BitSetTest, CanInitializeWithMaxULong) {
  EXPECT_EQ(std::numeric_limits<unsigned long>::max(),
            BitSet<64>{std::numeric_limits<unsigned long>::max()}.to_ulong());
}

TEST(BitSetTest, CanUseWidthLessThanMax) {
  EXPECT_EQ(std::numeric_limits<unsigned short>::max(),
            BitSet<64>{std::numeric_limits<unsigned short>::max()}.to_ulong());
}

TEST(BitSetTest, CanSetBit) {
  static constexpr size_t BIT{12};
  BitSet<64> bits{};
  bits.set(BIT);
  EXPECT_EQ(1UL << BIT, bits.to_ulong());
}

TEST(BitSetTest, CanTestBit) {
  static constexpr size_t BIT{12};
  BitSet<64> bits{};
  bits.set(BIT);
  EXPECT_TRUE(bits.test(BIT));
}

TEST(BitSetTest, CanInstantiateZeroSizeBitSet) {
  BitSet<0> bits{};
  EXPECT_EQ(0, bits.to_ulong());
}

TEST(BitSetTest, CanCreateMask) {
  BitSet<4> bits{BitSet<4>::create_mask(2)};
  EXPECT_EQ(3, bits.to_ulong());
}

TEST(BitSetTest, CanCreateOffsetMask) {
  BitSet<4> bits{BitSet<4>::create_mask(2, 1)};
  EXPECT_EQ(6, bits.to_ulong());
}

}  // namespace ndyn::math
