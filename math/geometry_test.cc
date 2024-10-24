
#include "math/geometry.h"

#include "gtest/gtest.h"
#include "math/multivector_test_utils.h"

namespace ndyn::math {
using ScalarType = float;
using PrimitiveType = Primitive<ScalarType>;

TEST(PrimitiveTest, CanIdentifyPoint) {
  const PrimitiveType p1{PrimitiveType::point(1, 0, 0)};
  const PrimitiveType p2{PrimitiveType::point(0, 1, 0)};
  const PrimitiveType p3{PrimitiveType::point(0, 0, 1)};
  const PrimitiveType p4{PrimitiveType::point(0, 1, 1)};
  const PrimitiveType p5{PrimitiveType::point(1, 0, 1)};
  const PrimitiveType p6{PrimitiveType::point(1, 1, 1)};

  EXPECT_TRUE(p1.is_point()) << p1;
  EXPECT_TRUE(p2.is_point()) << p2;
  EXPECT_TRUE(p3.is_point()) << p3;
  EXPECT_TRUE(p4.is_point()) << p4;
  EXPECT_TRUE(p5.is_point()) << p5;
  EXPECT_TRUE(p6.is_point()) << p6;
}

TEST(PrimitiveTest, LineIsJoinOfTwoPoints) {
  const PrimitiveType p1{PrimitiveType::point(1, 0, 0)};
  const PrimitiveType p2{PrimitiveType::point(0, 1, 0)};
  const PrimitiveType l1{PrimitiveType::join(p1, p2)};

  EXPECT_TRUE(l1.is_line()) << l1;
}

TEST(PrimitiveTest, PlaneIsJoinOfLineAndPoint) {
  const PrimitiveType p1{PrimitiveType::point(1, 0, 0)};
  const PrimitiveType p2{PrimitiveType::point(0, 1, 0)};
  const PrimitiveType l1{PrimitiveType::join(p1, p2)};
  const PrimitiveType p3{PrimitiveType::point(0, 0, 1)};
  const PrimitiveType plane{PrimitiveType::join(l1, p3)};

  EXPECT_TRUE(plane.is_plane()) << "plane: " << plane << ", l1: " << l1 << ", p3: " << p3;
}

}  // namespace ndyn::math
