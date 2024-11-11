
#include "math/geometry.h"

#include "gtest/gtest.h"
#include "math/algebra.h"
#include "math/multivector_test_utils.h"

namespace ndyn::math {
using ScalarType = float;
using AlgebraType = Algebra<ScalarType, 3, 0, 1>;
using GeometryType = Geometry3D<AlgebraType>;

TEST(PrimitiveTest, CanIdentifyPoint) {
  const typename AlgebraType::VectorType p1{GeometryType::point(1, 0, 0)};
  const typename AlgebraType::VectorType p2{GeometryType::point(0, 1, 0)};
  const typename AlgebraType::VectorType p3{GeometryType::point(0, 0, 1)};
  const typename AlgebraType::VectorType p4{GeometryType::point(0, 1, 1)};
  const typename AlgebraType::VectorType p5{GeometryType::point(1, 0, 1)};
  const typename AlgebraType::VectorType p6{GeometryType::point(1, 1, 1)};

  EXPECT_TRUE(GeometryType::is_point(p1)) << p1;
  EXPECT_TRUE(GeometryType::is_point(p2)) << p2;
  EXPECT_TRUE(GeometryType::is_point(p3)) << p3;
  EXPECT_TRUE(GeometryType::is_point(p4)) << p4;
  EXPECT_TRUE(GeometryType::is_point(p5)) << p5;
  EXPECT_TRUE(GeometryType::is_point(p6)) << p6;
}

TEST(PrimitiveTest, PrincipalAxesAreMeetLines) {
  EXPECT_TRUE(GeometryType::is_meet_line(GeometryType::x_axis)) << GeometryType::x_axis;
  EXPECT_TRUE(GeometryType::is_meet_line(GeometryType::y_axis)) << GeometryType::y_axis;
  EXPECT_TRUE(GeometryType::is_meet_line(GeometryType::z_axis)) << GeometryType::z_axis;
}

TEST(PrimitiveTest, CanIdentifyMeetLine) {
  const typename AlgebraType::VectorType line1{GeometryType::meet_line(1, 0, 0)};
  const typename AlgebraType::VectorType line2{GeometryType::meet_line(0, 1, 0)};
  const typename AlgebraType::VectorType line3{GeometryType::meet_line(0, 0, 1)};
  const typename AlgebraType::VectorType line4{GeometryType::meet_line(1, 0, 1)};
  const typename AlgebraType::VectorType line5{GeometryType::meet_line(1, 1, 0)};
  const typename AlgebraType::VectorType line6{GeometryType::meet_line(1, 1, 1)};

  EXPECT_TRUE(GeometryType::is_meet_line(line1)) << line1;
  EXPECT_TRUE(GeometryType::is_meet_line(line2)) << line2;
  EXPECT_TRUE(GeometryType::is_meet_line(line3)) << line3;
  EXPECT_TRUE(GeometryType::is_meet_line(line4)) << line4;
  EXPECT_TRUE(GeometryType::is_meet_line(line5)) << line5;
  EXPECT_TRUE(GeometryType::is_meet_line(line6)) << line6;
}

TEST(PrimitiveTest, CanIdentifyPlane) {
  const typename AlgebraType::VectorType plane{GeometryType::plane(1, 2, 3, 5)};

  EXPECT_TRUE(GeometryType::is_plane(plane)) << "plane: " << plane;
}

TEST(PrimitiveTest, JoinLineIsJoinOfTwoPoints) {
  LOG(INFO) << "origin: " << GeometryType::origin;
  const typename AlgebraType::VectorType p1{GeometryType::point(1, 0, 0)};
  const typename AlgebraType::VectorType p2{GeometryType::point(0, 1, 0)};
  const typename AlgebraType::VectorType l1{GeometryType::join(p1, p2)};

  EXPECT_TRUE(GeometryType::is_join_line(l1)) << l1 << " (p1: " << p1 << ", p2: " << p2 << ")";
}

TEST(PrimitiveTest, PlaneIsJoinOfLineAndPoint) {
  const typename AlgebraType::VectorType p1{GeometryType::point(1, 0, 0)};
  const typename AlgebraType::VectorType p2{GeometryType::point(0, 1, 0)};
  const typename AlgebraType::VectorType l1{GeometryType::join(p1, p2)};
  const typename AlgebraType::VectorType p3{GeometryType::point(0, 0, 1)};
  const typename AlgebraType::VectorType plane{GeometryType::join(l1, p3)};

  EXPECT_TRUE(GeometryType::is_plane(plane))
      << "plane: " << plane << ", l1: " << l1 << ", p3: " << p3;
}

TEST(PrimitiveTest, MeetLineIsMeetOfTwoPlanes) {
  const typename AlgebraType::VectorType plane1{GeometryType::plane(1, 2, 1, 0)};
  const typename AlgebraType::VectorType plane2{GeometryType::plane(3, 7, 1, 0)};

  ASSERT_TRUE(GeometryType::is_plane(plane1)) << "plane1: " << plane1;
  ASSERT_TRUE(GeometryType::is_plane(plane2)) << "plane2: " << plane2;

  ASSERT_FALSE(GeometryType::is_parallel(plane1, plane2))
      << "plane1: " << plane1 << ", plane2: " << plane2;

  const typename AlgebraType::VectorType line1{GeometryType::meet(plane1, plane2)};
  EXPECT_TRUE(GeometryType::is_meet_line(line1)) << "line1: " << line1;
}

TEST(PrimitiveTest, PointIsMeetOfLineAndPlane) {
  const typename AlgebraType::VectorType line1{GeometryType::meet_line(-1, 1, -1)};
  const typename AlgebraType::VectorType plane2{GeometryType::plane(0.1, -0.2, -6, 1)};

  ASSERT_TRUE(GeometryType::is_meet_line(line1)) << line1;
  ASSERT_TRUE(GeometryType::is_plane(plane2)) << plane2;

  ASSERT_FALSE(GeometryType::is_parallel(line1, plane2))
      << "line1: " << line1 << ", plane2: " << plane2;

  const typename AlgebraType::VectorType meet{GeometryType::meet(line1, plane2)};
  LOG(INFO) << "meet: " << meet;
  EXPECT_TRUE(GeometryType::is_point(meet)) << "line1: " << line1 << ", plane2: " << plane2;

  EXPECT_NE(GeometryType::zero, meet);
}

}  // namespace ndyn::math
