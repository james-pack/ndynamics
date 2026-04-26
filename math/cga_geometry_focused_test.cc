#include "glog/logging.h"
#include "gtest/gtest.h"
#include "math/canonical_basis_representation.h"
#include "math/cga_geometry.h"

namespace ndyn::math {

TEST(Cga2dGeometryTest, CanFindSecondPointInPointPair) {
  using G = Cga2dGeometry<>;
  using Scalar = G::Scalar;
  using Multivector = G::Multivector;

  // Four points with prime coordinates in Euclidean space.
  const Multivector p1{G::make_point(Scalar{2}, Scalar{3})};
  const Multivector p2{G::make_point(Scalar{5}, Scalar{7})};

  DLOG(INFO) << "p1: " << p1;
  DLOG(INFO) << "p2: " << p2;

  const Multivector point_pair1{G::join(p1, p2)};

  const Multivector pp11_meet{G::meet(p1, point_pair1)};
  const Multivector pp12_meet{G::meet(p2, point_pair1)};
  EXPECT_TRUE(pp11_meet.near_zero()) << pp11_meet << " (expected zero multivector)";
  EXPECT_TRUE(pp12_meet.near_zero()) << pp12_meet << " (expected zero multivector)";

  Multivector pp11_left_contraction{p1.left_contraction(point_pair1)};
  Multivector pp12_left_contraction{p2.left_contraction(point_pair1)};
  DLOG(INFO) << "pp11_left_contraction: " << pp11_left_contraction << " (before normalization)";
  DLOG(INFO) << "pp12_left_contraction: " << pp12_left_contraction << " (before normalization)";

  EXPECT_TRUE(G::is_point(pp11_left_contraction)) << pp11_left_contraction;
  EXPECT_TRUE(G::is_point(pp12_left_contraction)) << pp12_left_contraction;

  // Normalize.
  pp11_left_contraction = pp11_left_contraction / G::weight(pp11_left_contraction);
  pp12_left_contraction = pp12_left_contraction / G::weight(pp12_left_contraction);

  EXPECT_TRUE(pp11_left_contraction.near_equal(p1));
  EXPECT_TRUE(pp12_left_contraction.near_equal(p2));

  const Multivector mid1{G::e_inf().left_contraction(point_pair1)};
  DLOG(INFO) << "mid1: " << mid1;

  Multivector p1_projected{mid1 * p2 * ~mid1};
  Multivector p2_projected{mid1 * p1 * ~mid1};
  DLOG(INFO) << "p1_projected: " << p1_projected << " (before normalization)";
  DLOG(INFO) << "p2_projected: " << p2_projected << " (before normalization)";

  DLOG(INFO) << "G::weight(p1_projected): " << G::weight(p1_projected);
  DLOG(INFO) << "G::weight(p2_projected): " << G::weight(p2_projected);

  p1_projected = p1_projected / G::weight(p1_projected);
  p2_projected = p2_projected / G::weight(p2_projected);

  DLOG(INFO) << "p1_projected: " << p1_projected << " (after normalization)";
  DLOG(INFO) << "p2_projected: " << p2_projected << " (after normalization)";

  EXPECT_TRUE(p1_projected.near_equal(p1));
  EXPECT_TRUE(p2_projected.near_equal(p2));

  EXPECT_TRUE(G::is_normalized_point(p1_projected));
  EXPECT_TRUE(G::is_normalized_point(p2_projected));
}

TEST(Cga2dGeometryTest, LinePhysicalPointExtraction) {
  using G = Cga2dGeometry<>;
  using Scalar = G::Scalar;
  using Multivector = G::Multivector;

  const Multivector p1{G::make_point(Scalar{2}, Scalar{3})};
  const Multivector p2{G::make_point(Scalar{5}, Scalar{7})};
  const Multivector line1{G::make_line(p1, p2)};
  ASSERT_TRUE(G::is_line(line1));

  const Multivector cir_p1{G::make_point(Scalar{0}, Scalar{0})};
  const Multivector cir_p3{G::make_point(Scalar{10}, Scalar{10})};
  const Multivector cir_p2{G::make_point(Scalar{10}, Scalar{-10})};
  const Multivector circle{G::make_circle(cir_p1, cir_p2, cir_p3)};
  // ASSERT_TRUE(G::is_circle(circle));

  // Find a point pair that is the meet of the line and the circle.
  auto point_pair1{G::meet(line1, circle)};

  ASSERT_TRUE(G::is_point_pair(point_pair1));

  // Extract the physical intersection points.
  Multivector proj1{};
  Multivector proj2{};
  G::extract_point_pair(point_pair1, proj1, proj2);

  EXPECT_TRUE(G::is_point(proj1)) << "proj1: " << proj1;
  EXPECT_TRUE(G::is_point(proj2)) << "proj2: " << proj2;

  // Verify these points lie on the line.
  EXPECT_TRUE(G::join(proj1, line1).near_zero()) << "line1: " << line1 << ", proj1: " << proj1;
  EXPECT_TRUE(G::join(proj2, line1).near_zero()) << "line1: " << line1 << ", proj2: " << proj2;
}

}  // namespace ndyn::math
