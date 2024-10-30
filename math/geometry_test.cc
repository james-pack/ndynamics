
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

TEST(PrimitiveTest, CanIdentifyLine) {
  const PrimitiveType line1{PrimitiveType::line(1, 0, 0)};
  const PrimitiveType line2{PrimitiveType::line(0, 1, 0)};
  const PrimitiveType line3{PrimitiveType::line(0, 0, 1)};
  const PrimitiveType line4{PrimitiveType::line(1, 0, 1)};
  const PrimitiveType line5{PrimitiveType::line(1, 1, 0)};
  const PrimitiveType line6{PrimitiveType::line(1, 1, 1)};

  EXPECT_TRUE(line1.is_line()) << line1;
  EXPECT_TRUE(line2.is_line()) << line2;
  EXPECT_TRUE(line3.is_line()) << line3;
  EXPECT_TRUE(line4.is_line()) << line4;
  EXPECT_TRUE(line5.is_line()) << line5;
  EXPECT_TRUE(line6.is_line()) << line6;
}

TEST(PrimitiveTest, CanIdentifyPlane) {
  const PrimitiveType plane{PrimitiveType::plane(1, 2, 3, 5)};

  EXPECT_TRUE(plane.is_plane()) << "plane: " << plane;
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

TEST(PrimitiveTest, LineIsMeetOfTwoPlanes) {
  const PrimitiveType plane1{PrimitiveType::plane(1, 2, 1, 0)};
  const PrimitiveType plane2{PrimitiveType::plane(3, 7, 1, 0)};

  ASSERT_TRUE(plane1.is_plane()) << "plane1: " << plane1;
  ASSERT_TRUE(plane2.is_plane()) << "plane2: " << plane2;

  ASSERT_FALSE(PrimitiveType::is_parallel(plane1, plane2))
      << "plane1: " << plane1 << ", plane2: " << plane2;

  const PrimitiveType line1{PrimitiveType::meet(plane1, plane2)};
  EXPECT_TRUE(line1.is_line()) << "line1: " << line1;
}

TEST(PrimitiveTest, PointIsMeetOfLineAndPlane) {
  const PrimitiveType line1{PrimitiveType::line(-1, 1, -1)};
  const PrimitiveType plane2{PrimitiveType::plane(0.1, -0.2, -6, 1)};

  ASSERT_TRUE(line1.is_line()) << line1;
  ASSERT_TRUE(plane2.is_plane()) << plane2;

  ASSERT_FALSE(PrimitiveType::is_parallel(line1, plane2))
      << "line1: " << line1 << ", plane2: " << plane2;

  const PrimitiveType meet{PrimitiveType::meet(line1, plane2)};
  LOG(INFO) << "meet: " << meet;
  EXPECT_TRUE(meet.is_point()) << "line1: " << line1 << ", plane2: " << plane2;

  EXPECT_NE(PrimitiveType::zero(), meet);
}

}  // namespace ndyn::math
