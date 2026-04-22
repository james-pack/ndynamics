#pragma once

#include <cstddef>

#include "gtest/gtest.h"
#include "math/abs.h"
#include "math/geometry_model.h"

namespace ndyn::math {

/**
 *  Typed test suite for geometry models.
 *
 * Usage:
 *   1. Include this header in a concrete test file.
 *   2. Define a list of geometry types to test:
 *
 *        using MyGeometries = ::testing::Types<MyCga3D, MyPga3D, MyCga2D>;
 *
 *   3. Instantiate the suite with your types:
 *
 *        INSTANTIATE_TYPED_TEST_SUITE_P(MyPrefix, GeometryConceptsTest, MyGeometries);
 *
 * Each test uses if constexpr guards so that a geometry type that does not
 * satisfy a given primitive concept simply skips that test body at compile
 * time. No runtime branching occurs.
 */
template <typename G>
class GeometryConceptsTest : public ::testing::Test {
 public:
  static bool near(typename G::Scalar a, typename G::Scalar b) {
    return abs(a - b) < G::Algebra::EPSILON;
  }

  // Representative coordinates for a point or first endpoint.
  static constexpr typename G::Scalar x{1};
  static constexpr typename G::Scalar y{2};
  static constexpr typename G::Scalar z{3};
  static constexpr typename G::Scalar w{4};

  // Representative direction or second endpoint coordinates.
  static constexpr typename G::Scalar dx{5};
  static constexpr typename G::Scalar dy{6};
  static constexpr typename G::Scalar dz{7};
  static constexpr typename G::Scalar dw{8};

  // Radius for circles, spheres, hyperspheres.
  static constexpr typename G::Scalar radius{9};

  // Signed distance from origin for planes and hyperplanes.
  static constexpr typename G::Scalar offset{10};
};

TYPED_TEST_SUITE_P(GeometryConceptsTest);

TYPED_TEST_P(GeometryConceptsTest, PointRoundtrip1D) {
  if constexpr (HasPoint<TypeParam>) {
    const auto mv = TypeParam::make_point(TestFixture::x);
    EXPECT_TRUE(TypeParam::is_point(mv));

    typename TypeParam::Scalar out_x{};
    TypeParam::extract_point(mv, out_x);
    EXPECT_TRUE(TestFixture::near(out_x, TestFixture::x));
  }
}

TYPED_TEST_P(GeometryConceptsTest, PointRoundtrip2D) {
  if constexpr (HasPoint<TypeParam> && TypeParam::NUM_PHYSICAL_DIMENSIONS >= 2) {
    const auto mv = TypeParam::make_point(TestFixture::x, TestFixture::y);
    EXPECT_TRUE(TypeParam::is_point(mv));

    typename TypeParam::Scalar out_x{}, out_y{};
    TypeParam::extract_point(mv, out_x, out_y);
    EXPECT_TRUE(TestFixture::near(out_x, TestFixture::x));
    EXPECT_TRUE(TestFixture::near(out_y, TestFixture::y));
  }
}

TYPED_TEST_P(GeometryConceptsTest, PointRoundtrip3D) {
  if constexpr (HasPoint<TypeParam> && TypeParam::NUM_PHYSICAL_DIMENSIONS >= 3) {
    const auto mv = TypeParam::make_point(TestFixture::x, TestFixture::y, TestFixture::z);
    EXPECT_TRUE(TypeParam::is_point(mv));

    typename TypeParam::Scalar out_x{}, out_y{}, out_z{};
    TypeParam::extract_point(mv, out_x, out_y, out_z);
    EXPECT_TRUE(TestFixture::near(out_x, TestFixture::x));
    EXPECT_TRUE(TestFixture::near(out_y, TestFixture::y));
    EXPECT_TRUE(TestFixture::near(out_z, TestFixture::z));
  }
}

TYPED_TEST_P(GeometryConceptsTest, PointRoundtrip4D) {
  if constexpr (HasPoint<TypeParam> && TypeParam::NUM_PHYSICAL_DIMENSIONS >= 4) {
    const auto mv =
        TypeParam::make_point(TestFixture::x, TestFixture::y, TestFixture::z, TestFixture::w);
    EXPECT_TRUE(TypeParam::is_point(mv));

    typename TypeParam::Scalar out_x{}, out_y{}, out_z{}, out_w{};
    TypeParam::extract_point(mv, out_x, out_y, out_z, out_w);
    EXPECT_TRUE(TestFixture::near(out_x, TestFixture::x));
    EXPECT_TRUE(TestFixture::near(out_y, TestFixture::y));
    EXPECT_TRUE(TestFixture::near(out_z, TestFixture::z));
    EXPECT_TRUE(TestFixture::near(out_w, TestFixture::w));
  }
}

TYPED_TEST_P(GeometryConceptsTest, PointPairRoundtrip2D) {
  if constexpr (HasPointPair<TypeParam> && TypeParam::NUM_PHYSICAL_DIMENSIONS >= 2) {
    const auto mv = TypeParam::make_point_pair(TestFixture::x, TestFixture::y, TestFixture::dx,
                                               TestFixture::dy);
    EXPECT_TRUE(TypeParam::is_point_pair(mv));

    typename TypeParam::Scalar out_x1{}, out_y1{}, out_x2{}, out_y2{};
    TypeParam::extract_point_pair(mv, out_x1, out_y1, out_x2, out_y2);
    EXPECT_TRUE(TestFixture::near(out_x1, TestFixture::x));
    EXPECT_TRUE(TestFixture::near(out_y1, TestFixture::y));
    EXPECT_TRUE(TestFixture::near(out_x2, TestFixture::dx));
    EXPECT_TRUE(TestFixture::near(out_y2, TestFixture::dy));
  }
}

TYPED_TEST_P(GeometryConceptsTest, PointPairRoundtrip3D) {
  if constexpr (HasPointPair<TypeParam> && TypeParam::NUM_PHYSICAL_DIMENSIONS >= 3) {
    const auto mv = TypeParam::make_point_pair(TestFixture::x, TestFixture::y, TestFixture::z,
                                               TestFixture::dx, TestFixture::dy, TestFixture::dz);
    EXPECT_TRUE(TypeParam::is_point_pair(mv));

    typename TypeParam::Scalar out_x1{}, out_y1{}, out_z1{};
    typename TypeParam::Scalar out_x2{}, out_y2{}, out_z2{};
    TypeParam::extract_point_pair(mv, out_x1, out_y1, out_z1, out_x2, out_y2, out_z2);
    EXPECT_TRUE(TestFixture::near(out_x1, TestFixture::x));
    EXPECT_TRUE(TestFixture::near(out_y1, TestFixture::y));
    EXPECT_TRUE(TestFixture::near(out_z1, TestFixture::z));
    EXPECT_TRUE(TestFixture::near(out_x2, TestFixture::dx));
    EXPECT_TRUE(TestFixture::near(out_y2, TestFixture::dy));
    EXPECT_TRUE(TestFixture::near(out_z2, TestFixture::dz));
  }
}

TYPED_TEST_P(GeometryConceptsTest, PointPairRoundtrip4D) {
  if constexpr (HasPointPair<TypeParam> && TypeParam::NUM_PHYSICAL_DIMENSIONS >= 4) {
    const auto mv = TypeParam::make_point_pair(TestFixture::x, TestFixture::y, TestFixture::z,
                                               TestFixture::w, TestFixture::dx, TestFixture::dy,
                                               TestFixture::dz, TestFixture::dw);
    EXPECT_TRUE(TypeParam::is_point_pair(mv));

    typename TypeParam::Scalar out_x1{}, out_y1{}, out_z1{}, out_w1{};
    typename TypeParam::Scalar out_x2{}, out_y2{}, out_z2{}, out_w2{};
    TypeParam::extract_point_pair(mv, out_x1, out_y1, out_z1, out_w1, out_x2, out_y2, out_z2,
                                  out_w2);
    EXPECT_TRUE(TestFixture::near(out_x1, TestFixture::x));
    EXPECT_TRUE(TestFixture::near(out_y1, TestFixture::y));
    EXPECT_TRUE(TestFixture::near(out_z1, TestFixture::z));
    EXPECT_TRUE(TestFixture::near(out_w1, TestFixture::w));
    EXPECT_TRUE(TestFixture::near(out_x2, TestFixture::dx));
    EXPECT_TRUE(TestFixture::near(out_y2, TestFixture::dy));
    EXPECT_TRUE(TestFixture::near(out_z2, TestFixture::dz));
    EXPECT_TRUE(TestFixture::near(out_w2, TestFixture::dw));
  }
}

TYPED_TEST_P(GeometryConceptsTest, LineRoundtrip2D) {
  if constexpr (HasLine<TypeParam> && TypeParam::NUM_PHYSICAL_DIMENSIONS >= 2) {
    const auto mv =
        TypeParam::make_line(TestFixture::x, TestFixture::y, TestFixture::dx, TestFixture::dy);
    EXPECT_TRUE(TypeParam::is_line(mv));

    typename TypeParam::Scalar out_px{}, out_py{}, out_dx{}, out_dy{};
    TypeParam::extract_line(mv, out_px, out_py, out_dx, out_dy);
    EXPECT_TRUE(TestFixture::near(out_px, TestFixture::x));
    EXPECT_TRUE(TestFixture::near(out_py, TestFixture::y));
    EXPECT_TRUE(TestFixture::near(out_dx, TestFixture::dx));
    EXPECT_TRUE(TestFixture::near(out_dy, TestFixture::dy));
  }
}

TYPED_TEST_P(GeometryConceptsTest, LineRoundtrip3D) {
  if constexpr (HasLine<TypeParam> && TypeParam::NUM_PHYSICAL_DIMENSIONS >= 3) {
    const auto mv = TypeParam::make_line(TestFixture::x, TestFixture::y, TestFixture::z,
                                         TestFixture::dx, TestFixture::dy, TestFixture::dz);
    EXPECT_TRUE(TypeParam::is_line(mv));

    typename TypeParam::Scalar out_px{}, out_py{}, out_pz{};
    typename TypeParam::Scalar out_dx{}, out_dy{}, out_dz{};
    TypeParam::extract_line(mv, out_px, out_py, out_pz, out_dx, out_dy, out_dz);
    EXPECT_TRUE(TestFixture::near(out_px, TestFixture::x));
    EXPECT_TRUE(TestFixture::near(out_py, TestFixture::y));
    EXPECT_TRUE(TestFixture::near(out_pz, TestFixture::z));
    EXPECT_TRUE(TestFixture::near(out_dx, TestFixture::dx));
    EXPECT_TRUE(TestFixture::near(out_dy, TestFixture::dy));
    EXPECT_TRUE(TestFixture::near(out_dz, TestFixture::dz));
  }
}

TYPED_TEST_P(GeometryConceptsTest, LineRoundtrip4D) {
  if constexpr (HasLine<TypeParam> && TypeParam::NUM_PHYSICAL_DIMENSIONS >= 4) {
    const auto mv =
        TypeParam::make_line(TestFixture::x, TestFixture::y, TestFixture::z, TestFixture::w,
                             TestFixture::dx, TestFixture::dy, TestFixture::dz, TestFixture::dw);
    EXPECT_TRUE(TypeParam::is_line(mv));

    typename TypeParam::Scalar out_px{}, out_py{}, out_pz{}, out_pw{};
    typename TypeParam::Scalar out_dx{}, out_dy{}, out_dz{}, out_dw{};
    TypeParam::extract_line(mv, out_px, out_py, out_pz, out_pw, out_dx, out_dy, out_dz, out_dw);
    EXPECT_TRUE(TestFixture::near(out_px, TestFixture::x));
    EXPECT_TRUE(TestFixture::near(out_py, TestFixture::y));
    EXPECT_TRUE(TestFixture::near(out_pz, TestFixture::z));
    EXPECT_TRUE(TestFixture::near(out_pw, TestFixture::w));
    EXPECT_TRUE(TestFixture::near(out_dx, TestFixture::dx));
    EXPECT_TRUE(TestFixture::near(out_dy, TestFixture::dy));
    EXPECT_TRUE(TestFixture::near(out_dz, TestFixture::dz));
    EXPECT_TRUE(TestFixture::near(out_dw, TestFixture::dw));
  }
}

TYPED_TEST_P(GeometryConceptsTest, PlaneRoundtrip2D) {
  if constexpr (HasPlane<TypeParam> && TypeParam::NUM_PHYSICAL_DIMENSIONS >= 2) {
    const auto mv = TypeParam::make_plane(TestFixture::x, TestFixture::y, TestFixture::offset);
    EXPECT_TRUE(TypeParam::is_plane(mv));

    typename TypeParam::Scalar out_nx{}, out_ny{}, out_offset{};
    TypeParam::extract_plane(mv, out_nx, out_ny, out_offset);
    EXPECT_TRUE(TestFixture::near(out_nx, TestFixture::x));
    EXPECT_TRUE(TestFixture::near(out_ny, TestFixture::y));
    EXPECT_TRUE(TestFixture::near(out_offset, TestFixture::offset));
  }
}

TYPED_TEST_P(GeometryConceptsTest, PlaneRoundtrip3D) {
  if constexpr (HasPlane<TypeParam> && TypeParam::NUM_PHYSICAL_DIMENSIONS >= 3) {
    const auto mv =
        TypeParam::make_plane(TestFixture::x, TestFixture::y, TestFixture::z, TestFixture::offset);
    EXPECT_TRUE(TypeParam::is_plane(mv));

    typename TypeParam::Scalar out_nx{}, out_ny{}, out_nz{}, out_offset{};
    TypeParam::extract_plane(mv, out_nx, out_ny, out_nz, out_offset);
    EXPECT_TRUE(TestFixture::near(out_nx, TestFixture::x));
    EXPECT_TRUE(TestFixture::near(out_ny, TestFixture::y));
    EXPECT_TRUE(TestFixture::near(out_nz, TestFixture::z));
    EXPECT_TRUE(TestFixture::near(out_offset, TestFixture::offset));
  }
}

TYPED_TEST_P(GeometryConceptsTest, PlaneRoundtrip4D) {
  if constexpr (HasPlane<TypeParam> && TypeParam::NUM_PHYSICAL_DIMENSIONS >= 4) {
    const auto mv = TypeParam::make_plane(TestFixture::x, TestFixture::y, TestFixture::z,
                                          TestFixture::w, TestFixture::offset);
    EXPECT_TRUE(TypeParam::is_plane(mv));

    typename TypeParam::Scalar out_nx{}, out_ny{}, out_nz{}, out_nw{}, out_offset{};
    TypeParam::extract_plane(mv, out_nx, out_ny, out_nz, out_nw, out_offset);
    EXPECT_TRUE(TestFixture::near(out_nx, TestFixture::x));
    EXPECT_TRUE(TestFixture::near(out_ny, TestFixture::y));
    EXPECT_TRUE(TestFixture::near(out_nz, TestFixture::z));
    EXPECT_TRUE(TestFixture::near(out_nw, TestFixture::w));
    EXPECT_TRUE(TestFixture::near(out_offset, TestFixture::offset));
  }
}

TYPED_TEST_P(GeometryConceptsTest, CircleRoundtrip2D) {
  if constexpr (HasCircle<TypeParam> && TypeParam::NUM_PHYSICAL_DIMENSIONS >= 2) {
    const auto mv = TypeParam::make_circle(TestFixture::x, TestFixture::y, TestFixture::radius);
    EXPECT_TRUE(TypeParam::is_circle(mv));

    typename TypeParam::Scalar out_cx{}, out_cy{}, out_radius{};
    TypeParam::extract_circle(mv, out_cx, out_cy, out_radius);
    EXPECT_TRUE(TestFixture::near(out_cx, TestFixture::x));
    EXPECT_TRUE(TestFixture::near(out_cy, TestFixture::y));
    EXPECT_TRUE(TestFixture::near(out_radius, TestFixture::radius));
  }
}

TYPED_TEST_P(GeometryConceptsTest, CircleRoundtrip3D) {
  if constexpr (HasCircle<TypeParam> && TypeParam::NUM_PHYSICAL_DIMENSIONS >= 3) {
    const auto mv =
        TypeParam::make_circle(TestFixture::x, TestFixture::y, TestFixture::z, TestFixture::radius);
    EXPECT_TRUE(TypeParam::is_circle(mv));

    typename TypeParam::Scalar out_cx{}, out_cy{}, out_cz{}, out_radius{};
    TypeParam::extract_circle(mv, out_cx, out_cy, out_cz, out_radius);
    EXPECT_TRUE(TestFixture::near(out_cx, TestFixture::x));
    EXPECT_TRUE(TestFixture::near(out_cy, TestFixture::y));
    EXPECT_TRUE(TestFixture::near(out_cz, TestFixture::z));
    EXPECT_TRUE(TestFixture::near(out_radius, TestFixture::radius));
  }
}

TYPED_TEST_P(GeometryConceptsTest, CircleRoundtrip4D) {
  if constexpr (HasCircle<TypeParam> && TypeParam::NUM_PHYSICAL_DIMENSIONS >= 4) {
    const auto mv = TypeParam::make_circle(TestFixture::x, TestFixture::y, TestFixture::z,
                                           TestFixture::w, TestFixture::radius);
    EXPECT_TRUE(TypeParam::is_circle(mv));

    typename TypeParam::Scalar out_cx{}, out_cy{}, out_cz{}, out_cw{}, out_radius{};
    TypeParam::extract_circle(mv, out_cx, out_cy, out_cz, out_cw, out_radius);
    EXPECT_TRUE(TestFixture::near(out_cx, TestFixture::x));
    EXPECT_TRUE(TestFixture::near(out_cy, TestFixture::y));
    EXPECT_TRUE(TestFixture::near(out_cz, TestFixture::z));
    EXPECT_TRUE(TestFixture::near(out_cw, TestFixture::w));
    EXPECT_TRUE(TestFixture::near(out_radius, TestFixture::radius));
  }
}

TYPED_TEST_P(GeometryConceptsTest, SphereRoundtrip2D) {
  if constexpr (HasSphere<TypeParam> && TypeParam::NUM_PHYSICAL_DIMENSIONS >= 2) {
    const auto mv = TypeParam::make_sphere(TestFixture::x, TestFixture::y, TestFixture::radius);
    EXPECT_TRUE(TypeParam::is_sphere(mv));

    typename TypeParam::Scalar out_cx{}, out_cy{}, out_radius{};
    TypeParam::extract_sphere(mv, out_cx, out_cy, out_radius);
    EXPECT_TRUE(TestFixture::near(out_cx, TestFixture::x));
    EXPECT_TRUE(TestFixture::near(out_cy, TestFixture::y));
    EXPECT_TRUE(TestFixture::near(out_radius, TestFixture::radius));
  }
}

TYPED_TEST_P(GeometryConceptsTest, SphereRoundtrip3D) {
  if constexpr (HasSphere<TypeParam> && TypeParam::NUM_PHYSICAL_DIMENSIONS >= 3) {
    const auto mv =
        TypeParam::make_sphere(TestFixture::x, TestFixture::y, TestFixture::z, TestFixture::radius);
    EXPECT_TRUE(TypeParam::is_sphere(mv));

    typename TypeParam::Scalar out_cx{}, out_cy{}, out_cz{}, out_radius{};
    TypeParam::extract_sphere(mv, out_cx, out_cy, out_cz, out_radius);
    EXPECT_TRUE(TestFixture::near(out_cx, TestFixture::x));
    EXPECT_TRUE(TestFixture::near(out_cy, TestFixture::y));
    EXPECT_TRUE(TestFixture::near(out_cz, TestFixture::z));
    EXPECT_TRUE(TestFixture::near(out_radius, TestFixture::radius));
  }
}

TYPED_TEST_P(GeometryConceptsTest, SphereRoundtrip4D) {
  if constexpr (HasSphere<TypeParam> && TypeParam::NUM_PHYSICAL_DIMENSIONS >= 4) {
    const auto mv = TypeParam::make_sphere(TestFixture::x, TestFixture::y, TestFixture::z,
                                           TestFixture::w, TestFixture::radius);
    EXPECT_TRUE(TypeParam::is_sphere(mv));

    typename TypeParam::Scalar out_cx{}, out_cy{}, out_cz{}, out_cw{}, out_radius{};
    TypeParam::extract_sphere(mv, out_cx, out_cy, out_cz, out_cw, out_radius);
    EXPECT_TRUE(TestFixture::near(out_cx, TestFixture::x));
    EXPECT_TRUE(TestFixture::near(out_cy, TestFixture::y));
    EXPECT_TRUE(TestFixture::near(out_cz, TestFixture::z));
    EXPECT_TRUE(TestFixture::near(out_cw, TestFixture::w));
    EXPECT_TRUE(TestFixture::near(out_radius, TestFixture::radius));
  }
}

TYPED_TEST_P(GeometryConceptsTest, HyperplaneRoundtrip3D) {
  if constexpr (HasHyperplane<TypeParam> && TypeParam::NUM_PHYSICAL_DIMENSIONS >= 3) {
    const auto mv = TypeParam::make_hyperplane(TestFixture::x, TestFixture::y, TestFixture::z,
                                               TestFixture::offset);
    EXPECT_TRUE(TypeParam::is_hyperplane(mv));

    typename TypeParam::Scalar out_nx{}, out_ny{}, out_nz{}, out_offset{};
    TypeParam::extract_hyperplane(mv, out_nx, out_ny, out_nz, out_offset);
    EXPECT_TRUE(TestFixture::near(out_nx, TestFixture::x));
    EXPECT_TRUE(TestFixture::near(out_ny, TestFixture::y));
    EXPECT_TRUE(TestFixture::near(out_nz, TestFixture::z));
    EXPECT_TRUE(TestFixture::near(out_offset, TestFixture::offset));
  }
}

TYPED_TEST_P(GeometryConceptsTest, HyperplaneRoundtrip4D) {
  if constexpr (HasHyperplane<TypeParam> && TypeParam::NUM_PHYSICAL_DIMENSIONS >= 4) {
    const auto mv = TypeParam::make_hyperplane(TestFixture::x, TestFixture::y, TestFixture::z,
                                               TestFixture::w, TestFixture::offset);
    EXPECT_TRUE(TypeParam::is_hyperplane(mv));

    typename TypeParam::Scalar out_nx{}, out_ny{}, out_nz{}, out_nw{}, out_offset{};
    TypeParam::extract_hyperplane(mv, out_nx, out_ny, out_nz, out_nw, out_offset);
    EXPECT_TRUE(TestFixture::near(out_nx, TestFixture::x));
    EXPECT_TRUE(TestFixture::near(out_ny, TestFixture::y));
    EXPECT_TRUE(TestFixture::near(out_nz, TestFixture::z));
    EXPECT_TRUE(TestFixture::near(out_nw, TestFixture::w));
    EXPECT_TRUE(TestFixture::near(out_offset, TestFixture::offset));
  }
}

TYPED_TEST_P(GeometryConceptsTest, HypersphereRoundtrip) {
  if constexpr (HasHypersphere<TypeParam>) {
    const auto mv = TypeParam::make_hypersphere(TestFixture::x, TestFixture::y, TestFixture::z,
                                                TestFixture::w, TestFixture::radius);
    EXPECT_TRUE(TypeParam::is_hypersphere(mv));

    typename TypeParam::Scalar out_cx{}, out_cy{}, out_cz{}, out_cw{}, out_radius{};
    TypeParam::extract_hypersphere(mv, out_cx, out_cy, out_cz, out_cw, out_radius);
    EXPECT_TRUE(TestFixture::near(out_cx, TestFixture::x));
    EXPECT_TRUE(TestFixture::near(out_cy, TestFixture::y));
    EXPECT_TRUE(TestFixture::near(out_cz, TestFixture::z));
    EXPECT_TRUE(TestFixture::near(out_cw, TestFixture::w));
    EXPECT_TRUE(TestFixture::near(out_radius, TestFixture::radius));
  }
}

REGISTER_TYPED_TEST_SUITE_P(GeometryConceptsTest,
                            // Point tests.
                            PointRoundtrip1D, PointRoundtrip2D, PointRoundtrip3D, PointRoundtrip4D,
                            // Point pair tests.
                            PointPairRoundtrip2D, PointPairRoundtrip3D, PointPairRoundtrip4D,
                            // Line tests.
                            LineRoundtrip2D, LineRoundtrip3D, LineRoundtrip4D,
                            // Plane tests.
                            PlaneRoundtrip2D, PlaneRoundtrip3D, PlaneRoundtrip4D,
                            // Circle tests.
                            CircleRoundtrip2D, CircleRoundtrip3D, CircleRoundtrip4D,
                            // Sphere tests.
                            SphereRoundtrip2D, SphereRoundtrip3D, SphereRoundtrip4D,
                            // Hyperplane tests.
                            HyperplaneRoundtrip3D, HyperplaneRoundtrip4D,
                            // Hypersphere tests.
                            HypersphereRoundtrip);

}  // namespace ndyn::math
