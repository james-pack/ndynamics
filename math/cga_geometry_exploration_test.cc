#include <initializer_list>
#include <iterator>

#include "glog/logging.h"
#include "gtest/gtest.h"
#include "math/conformal_geometry.h"
#include "math/geometries.h"
#include "math/representation.h"

namespace ndyn::math {

template <typename G>
auto make_test_point(std::initializer_list<typename G::Scalar> values) {
  return G::make_point(
      values.begin(),
      std::next(values.begin(), std::min(G::NUM_PHYSICAL_DIMENSIONS, values.size())));
}

TEST(CgaExploration, ThreePointsMakeCircle) {
  auto run = []<typename G>() {
    using Embed = typename G::EmbeddedSpace;
    const auto ep1{make_test_point<Embed>({0, 1, 2, 3, 4, 5})};
    const auto ep2{make_test_point<Embed>({1, 2, 3, 4, 5})};
    const auto p1{G::lift(ep1)};
    const auto p2{G::lift(ep2)};
    EXPECT_TRUE(G::is_point(p1)) << "p1: " << p1;
    EXPECT_TRUE(G::is_point(p1)) << "p2: " << p2;
    const auto line{G::join(p1, p2, G::e_inf())};
    EXPECT_FALSE(G::is_line(p1)) << "p1: " << p1;
    EXPECT_TRUE(G::is_line(line)) << "line: " << line;
  };

  run.template operator()<Cga2dGeometry<>>();
  run.template operator()<Cga3dGeometry<>>();
  run.template operator()<Cga4dGeometry<>>();
}

}  // namespace ndyn::math
