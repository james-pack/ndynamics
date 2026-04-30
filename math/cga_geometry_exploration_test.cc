#include "glog/logging.h"
#include "gtest/gtest.h"
#include "math/canonical_basis_representation.h"
#include "math/cga_geometry.h"

namespace ndyn::math {

TEST(CgaExploration, CircleWedgeEInfGivesPlane) {
  auto run = []<typename G>() {
    LOG(INFO) << "G::NUM_PHYSICAL_DIMENSIONS: " << G::NUM_PHYSICAL_DIMENSIONS;
    typename G::Multivector circle;
    if constexpr (G::NUM_PHYSICAL_DIMENSIONS == 2) {
      // In two dimensions, a circle can be distinguished by its center and radius.
      circle = G::make_circle(0, 0, 1);
    } else if constexpr (G::NUM_PHYSICAL_DIMENSIONS == 3) {
      // In three or more dimensions, determining a specific circle requires a center and
      // identification of the plane where the circle resides, as well as the radius.
      circle = G::make_circle(0, 0, 0, 0, 0, 1, 1);
    }
    LOG(INFO) << "circle: " << circle;
    ASSERT_TRUE(G::is_circle(circle))
        << "circle: " << circle << ", G::NUM_PHYSICAL_DIMENSIONS: " << G::NUM_PHYSICAL_DIMENSIONS;

    const auto pi{circle ^ G::e_inf()};
    LOG(INFO) << "pi: " << pi;

    EXPECT_TRUE(pi.template is_grade<4>())
        << "is_grade<4>: " << pi.template is_grade<4>() << ", pi: " << pi;
  };

  run.template operator()<Cga2dGeometry<>>();
  run.template operator()<Cga3dGeometry<>>();
}

TEST(CgaExploration, ProjectedCircleGives) {
  using G = Cga3dGeometry<>;
  LOG(INFO) << "G::NUM_PHYSICAL_DIMENSIONS: " << G::NUM_PHYSICAL_DIMENSIONS;
  const auto circle{G::make_circle(0, 0, 0, 0, 0, 1, 1)};
  LOG(INFO) << "circle: " << circle;
  ASSERT_TRUE(G::is_circle(circle))
      << "circle: " << circle << ", G::NUM_PHYSICAL_DIMENSIONS: " << G::NUM_PHYSICAL_DIMENSIONS;

  const auto plane{G::make_plane(0, 0, 1, 0, 0, 0)};

  const auto result{plane << circle};

  EXPECT_TRUE(G::is_circle(result))
      << "result: " << result << ", circle: " << circle << ", plane: " << plane;
}

}  // namespace ndyn::math
