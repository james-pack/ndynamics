#include <algorithm>
#include <array>
#include <initializer_list>

#include "glog/logging.h"
#include "gtest/gtest.h"
#include "math/testing/bivector.h"

namespace ndyn::math {

template <typename BivectorNetType>
BivectorNetType create(std::initializer_list<float> values) {
  BivectorNetType result{};
  size_t i{};
  for (float value : values) {
    result[i] = value;
    ++i;
  }
  return result;
}

template <typename BivectorNetType>
BivectorNetType create_with_counting() {
  BivectorNetType result{};
  for (size_t i = 0; i < BivectorNetType::NUM_BASES; ++i) {
    result[i] = i;
  }
  return result;
}

template <typename BivectorNetType>
size_t index_from_bit_basis_index(size_t bit_basis) {
  for (size_t i = 0; i < BivectorNetType::NUM_BASES; ++i) {
    if (BivectorNetType::bit_basis_indices[i] == bit_basis) {
      return i;
    }
  }
  return std::numeric_limits<size_t>::max();
}

TEST(BivectorNetTest, OuterProductR201R301R401Consistency) {
  static constexpr size_t P2{2};
  static constexpr size_t P3{3};
  static constexpr size_t P4{4};
  static constexpr size_t N{0};
  static constexpr size_t Z{1};

  using BivectorNet201Type = typename BivectorNetTypes<P2, N, Z>::type;
  using BivectorNet301Type = typename BivectorNetTypes<P3, N, Z>::type;
  using BivectorNet401Type = typename BivectorNetTypes<P4, N, Z>::type;

  BivectorNet201Type bvec_201_1{create<BivectorNet201Type>({0, 0, 2, 0, 0, -3})};
  BivectorNet201Type bvec_201_2{5 * bvec_201_1};
  BivectorNet201Type result_201{bvec_201_1 ^ bvec_201_2};

  BivectorNet301Type bvec_301_1{create<BivectorNet301Type>({0, 0, 2, 0, 0, 0, 3})};
  BivectorNet301Type bvec_301_2{5 * bvec_301_1};
  BivectorNet301Type result_301{bvec_301_1 ^ bvec_301_2};

  BivectorNet401Type bvec_401_1{create<BivectorNet401Type>({0, 0, 2, 0, 0, 0, 0, 3})};
  BivectorNet401Type bvec_401_2{5 * bvec_401_1};
  BivectorNet401Type result_401{bvec_401_1 ^ bvec_401_2};

  for (size_t i = 0; i < BivectorNet201Type::NUM_BASES; ++i) {
    float sign{1};
    if (BivectorNet201Type::reversed_bases[index_from_bit_basis_index<BivectorNet201Type>(i)]) {
      sign *= -1;
    }
    if (BivectorNet401Type::reversed_bases[index_from_bit_basis_index<BivectorNet401Type>(i)]) {
      sign *= -1;
    }
    EXPECT_EQ(sign * result_201[index_from_bit_basis_index<BivectorNet201Type>(i)],
              result_401[index_from_bit_basis_index<BivectorNet401Type>(i)])
        << "i: " << i << ", 201: " << result_201 << ", 401: " << result_401;
  }

  for (size_t i = 0; i < BivectorNet301Type::NUM_BASES; ++i) {
    float sign{1};
    if (BivectorNet301Type::reversed_bases[index_from_bit_basis_index<BivectorNet301Type>(i)]) {
      sign *= -1;
    }
    if (BivectorNet401Type::reversed_bases[index_from_bit_basis_index<BivectorNet401Type>(i)]) {
      sign *= -1;
    }
    EXPECT_EQ(sign * result_301[index_from_bit_basis_index<BivectorNet301Type>(i)],
              result_401[index_from_bit_basis_index<BivectorNet401Type>(i)])
        << "i: " << i << ", 301: " << result_301 << ", 401: " << result_401;
  }
}

TEST(BivectorNetDirectTest, R000RegressiveProductConsistentWithOuter) {
  using BivectorNetType = R000;
  BivectorNetType bvec1{create_with_counting<BivectorNetType>()};
  BivectorNetType bvec2{32 * bvec1};
  BivectorNetType regressive{bvec1 & bvec2};
  BivectorNetType via_outer{!((!bvec1) ^ (!bvec2))};
  EXPECT_EQ(via_outer, regressive);
}

TEST(BivectorNetDirectTest, R001RegressiveProductConsistentWithOuter) {
  using BivectorNetType = R001;
  BivectorNetType bvec1{create_with_counting<BivectorNetType>()};
  BivectorNetType bvec2{32 * bvec1};
  BivectorNetType regressive{bvec1 & bvec2};
  BivectorNetType via_outer{!((!bvec1) ^ (!bvec2))};
  EXPECT_EQ(via_outer, regressive);
}

TEST(BivectorNetDirectTest, R010RegressiveProductConsistentWithOuter) {
  using BivectorNetType = R010;
  BivectorNetType bvec1{create_with_counting<BivectorNetType>()};
  BivectorNetType bvec2{32 * bvec1};
  BivectorNetType regressive{bvec1 & bvec2};
  BivectorNetType via_outer{!((!bvec1) ^ (!bvec2))};
  EXPECT_EQ(via_outer, regressive);
}

TEST(BivectorNetDirectTest, R011RegressiveProductConsistentWithOuter) {
  using BivectorNetType = R011;
  BivectorNetType bvec1{create_with_counting<BivectorNetType>()};
  BivectorNetType bvec2{32 * bvec1};
  BivectorNetType regressive{bvec1 & bvec2};
  BivectorNetType via_outer{!((!bvec1) ^ (!bvec2))};
  EXPECT_EQ(via_outer, regressive);
}

TEST(BivectorNetDirectTest, R100RegressiveProductConsistentWithOuter) {
  using BivectorNetType = R100;
  BivectorNetType bvec1{create_with_counting<BivectorNetType>()};
  BivectorNetType bvec2{32 * bvec1};
  BivectorNetType regressive{bvec1 & bvec2};
  BivectorNetType via_outer{!((!bvec1) ^ (!bvec2))};
  EXPECT_EQ(via_outer, regressive);
}

TEST(BivectorNetDirectTest, R110RegressiveProductConsistentWithOuter) {
  using BivectorNetType = R110;
  BivectorNetType bvec1{create_with_counting<BivectorNetType>()};
  BivectorNetType bvec2{32 * bvec1};
  BivectorNetType regressive{bvec1 & bvec2};
  BivectorNetType via_outer{!((!bvec1) ^ (!bvec2))};
  EXPECT_EQ(via_outer, regressive);
}

TEST(BivectorNetDirectTest, R101RegressiveProductConsistentWithOuter) {
  using BivectorNetType = R101;
  BivectorNetType bvec1{create_with_counting<BivectorNetType>()};
  BivectorNetType bvec2{32 * bvec1};
  BivectorNetType regressive{bvec1 & bvec2};
  BivectorNetType via_outer{!((!bvec1) ^ (!bvec2))};
  EXPECT_EQ(via_outer, regressive);
}

TEST(BivectorNetDirectTest, R111RegressiveProductConsistentWithOuter) {
  using BivectorNetType = R111;
  BivectorNetType bvec1{create_with_counting<BivectorNetType>()};
  BivectorNetType bvec2{32 * bvec1};
  BivectorNetType regressive{bvec1 & bvec2};
  BivectorNetType via_outer{!((!bvec1) ^ (!bvec2))};
  EXPECT_EQ(via_outer, regressive);
}

TEST(BivectorNetDirectTest, R130RegressiveProductConsistentWithOuter) {
  using BivectorNetType = R130;
  BivectorNetType bvec1{create_with_counting<BivectorNetType>()};
  BivectorNetType bvec2{32 * bvec1};
  BivectorNetType regressive{bvec1 & bvec2};
  BivectorNetType via_outer{!((!bvec1) ^ (!bvec2))};
  EXPECT_EQ(via_outer, regressive);
}

TEST(BivectorNetDirectTest, R200RegressiveProductConsistentWithOuter) {
  using BivectorNetType = R200;
  BivectorNetType bvec1{create_with_counting<BivectorNetType>()};
  BivectorNetType bvec2{32 * bvec1};
  BivectorNetType regressive{bvec1 & bvec2};
  BivectorNetType via_outer{!((!bvec1) ^ (!bvec2))};
  EXPECT_EQ(via_outer, regressive);
}

TEST(BivectorNetDirectTest, R201RegressiveProductConsistentWithOuter) {
  using BivectorNetType = PGA2D;

  BivectorNetType bvec1{create_with_counting<BivectorNetType>()};
  BivectorNetType bvec2{32 * bvec1};
  BivectorNetType regressive{bvec1 & bvec2};
  BivectorNetType via_outer{!((!bvec1) ^ (!bvec2))};
  EXPECT_EQ(via_outer, regressive);
}

TEST(BivectorNetDirectTest, R210RegressiveProductConsistentWithOuter) {
  using BivectorNetType = R210;

  BivectorNetType bvec1{create_with_counting<BivectorNetType>()};
  BivectorNetType bvec2{32 * bvec1};
  BivectorNetType regressive{bvec1 & bvec2};
  BivectorNetType via_outer{!((!bvec1) ^ (!bvec2))};
  EXPECT_EQ(via_outer, regressive);
}

TEST(BivectorNetDirectTest, R300RegressiveProductConsistentWithOuter) {
  using BivectorNetType = R300;

  BivectorNetType bvec1{create_with_counting<BivectorNetType>()};
  BivectorNetType bvec2{32 * bvec1};
  BivectorNetType regressive{bvec1 & bvec2};
  BivectorNetType via_outer{!((!bvec1) ^ (!bvec2))};
  EXPECT_EQ(via_outer, regressive);
}

TEST(BivectorNetDirectTest, R301RegressiveProductConsistentWithOuter) {
  using BivectorNetType = PGA3D;

  BivectorNetType bvec1{create_with_counting<BivectorNetType>()};
  BivectorNetType bvec2{32 * bvec1};
  BivectorNetType regressive{bvec1 & bvec2};
  BivectorNetType via_outer{!((!bvec1) ^ (!bvec2))};
  EXPECT_EQ(via_outer, regressive);
}

TEST(BivectorNetDirectTest, R401RegressiveProductConsistentWithOuter) {
  using BivectorNetType = R401;

  BivectorNetType bvec1{create_with_counting<BivectorNetType>()};
  BivectorNetType bvec2{32 * bvec1};
  BivectorNetType regressive{bvec1 & bvec2};
  BivectorNetType via_outer{!((!bvec1) ^ (!bvec2))};
  EXPECT_EQ(via_outer, regressive);
}

TEST(BivectorNetDirectTest, R410RegressiveProductConsistentWithOuter) {
  using BivectorNetType = R410;

  BivectorNetType bvec1{create_with_counting<BivectorNetType>()};
  BivectorNetType bvec2{32 * bvec1};
  BivectorNetType regressive{bvec1 & bvec2};
  BivectorNetType via_outer{!((!bvec1) ^ (!bvec2))};
  EXPECT_EQ(via_outer, regressive);
}

TEST(BivectorNetDirectTest, PGA2DRegressiveProductConsistentWithOuter) {
  using BivectorNetType = PGA2D;

  BivectorNetType bvec1{create_with_counting<BivectorNetType>()};
  BivectorNetType bvec2{32 * bvec1};
  BivectorNetType regressive{bvec1 & bvec2};
  BivectorNetType via_outer{!((!bvec1) ^ (!bvec2))};

  LOG(INFO) << "bvec1: " << bvec1;
  LOG(INFO) << "bvec2: " << bvec2;

  LOG(INFO) << "regressive product expression: "
            << "(" << bvec1 << ") & (" << bvec2 << ")";
  LOG(INFO) << "outer product expression: "
            << "!((!(" << bvec1 << ")) ^ (!(" << bvec2 << ")))";

  EXPECT_EQ(via_outer, regressive);
}

TEST(BivectorNetDirectTest, PGA3DRegressiveProductConsistentWithDualOfOuterOfDuals) {
  using BivectorNetType = PGA3D;

  BivectorNetType bvec1{create_with_counting<BivectorNetType>()};
  BivectorNetType bvec2{32 * bvec1};
  BivectorNetType regressive{bvec1 & bvec2};
  BivectorNetType via_outer{!((!bvec1) ^ (!bvec2))};

  LOG(INFO) << "bvec1: " << bvec1;
  LOG(INFO) << "bvec2: " << bvec2;

  LOG(INFO) << "regressive product expression: "
            << "(" << bvec1 << ") & (" << bvec2 << ")";
  LOG(INFO) << "outer product expression: "
            << "!((!(" << bvec1 << ")) ^ (!(" << bvec2 << ")))";

  EXPECT_EQ(via_outer, regressive);
}

TEST(BivectorNetDirectTest, R401RegressiveProductConsistentWithDualOfOuterOfDuals) {
  using BivectorNetType = R401;

  BivectorNetType bvec1{create_with_counting<BivectorNetType>()};
  BivectorNetType bvec2{32 * bvec1};
  BivectorNetType regressive{bvec1 & bvec2};
  BivectorNetType via_outer{!((!bvec1) ^ (!bvec2))};

  LOG(INFO) << "bvec1: " << bvec1;
  LOG(INFO) << "bvec2: " << bvec2;

  LOG(INFO) << "regressive product expression: "
            << "(" << bvec1 << ") & (" << bvec2 << ")";
  LOG(INFO) << "outer product expression: "
            << "!((!(" << bvec1 << ")) ^ (!(" << bvec2 << ")))";

  EXPECT_EQ(via_outer, regressive);
}

}  // namespace ndyn::math
