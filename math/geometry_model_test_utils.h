#pragma once

#include <cstddef>

#include "gtest/gtest.h"
#include "math/abs.h"
#include "math/canonical_basis_representation.h"
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

  // Random angle value.
  static constexpr typename G::Scalar angle{2};

  // Random value for scaling.
  static constexpr typename G::Scalar scale{11};
};

enum class Primitives {
  POINT,
  POINT_PAIR,
  DIRECTION,
  LINE,
  PLANE,
  CIRCLE,
  SPHERE,
  HYPERPLANE,
  HYPERSPHERE,
};

enum class Operators {
  ROTOR,
  TRANSLATOR,
  DILATOR,
};

template <typename G, Primitives P>
typename G::Multivector make_primitive() {
  if constexpr (P == Primitives::POINT) {
    if constexpr (G::NUM_PHYSICAL_DIMENSIONS >= 4) {
      return G::make_point(GeometryConceptsTest<G>::x, GeometryConceptsTest<G>::y,
                           GeometryConceptsTest<G>::z, GeometryConceptsTest<G>::w);
    } else if constexpr (G::NUM_PHYSICAL_DIMENSIONS == 3) {
      return G::make_point(GeometryConceptsTest<G>::x, GeometryConceptsTest<G>::y,
                           GeometryConceptsTest<G>::z);
    } else if constexpr (G::NUM_PHYSICAL_DIMENSIONS == 2) {
      return G::make_point(GeometryConceptsTest<G>::x, GeometryConceptsTest<G>::y);
    } else {
      return G::make_point(GeometryConceptsTest<G>::x);
    }
  } else if constexpr (P == Primitives::POINT_PAIR) {
    if constexpr (G::NUM_PHYSICAL_DIMENSIONS >= 4) {
      return G::make_point_pair(GeometryConceptsTest<G>::x, GeometryConceptsTest<G>::y,
                                GeometryConceptsTest<G>::z, GeometryConceptsTest<G>::w,
                                GeometryConceptsTest<G>::dx, GeometryConceptsTest<G>::dy,
                                GeometryConceptsTest<G>::dz, GeometryConceptsTest<G>::dw);
    } else if constexpr (G::NUM_PHYSICAL_DIMENSIONS == 3) {
      return G::make_point_pair(GeometryConceptsTest<G>::x, GeometryConceptsTest<G>::y,
                                GeometryConceptsTest<G>::z, GeometryConceptsTest<G>::dx,
                                GeometryConceptsTest<G>::dy, GeometryConceptsTest<G>::dz);
    } else {
      return G::make_point_pair(GeometryConceptsTest<G>::x, GeometryConceptsTest<G>::y,
                                GeometryConceptsTest<G>::dx, GeometryConceptsTest<G>::dy);
    }
  } else if constexpr (P == Primitives::LINE) {
    if constexpr (G::NUM_PHYSICAL_DIMENSIONS >= 4) {
      return G::make_line(GeometryConceptsTest<G>::x, GeometryConceptsTest<G>::y,
                          GeometryConceptsTest<G>::z, GeometryConceptsTest<G>::w,
                          GeometryConceptsTest<G>::dx, GeometryConceptsTest<G>::dy,
                          GeometryConceptsTest<G>::dz, GeometryConceptsTest<G>::dw);
    } else if constexpr (G::NUM_PHYSICAL_DIMENSIONS == 3) {
      return G::make_line(GeometryConceptsTest<G>::x, GeometryConceptsTest<G>::y,
                          GeometryConceptsTest<G>::z, GeometryConceptsTest<G>::dx,
                          GeometryConceptsTest<G>::dy, GeometryConceptsTest<G>::dz);
    } else {
      return G::make_line(GeometryConceptsTest<G>::x, GeometryConceptsTest<G>::y,
                          GeometryConceptsTest<G>::dx, GeometryConceptsTest<G>::dy);
    }
  } else if constexpr (P == Primitives::PLANE) {
    if constexpr (G::NUM_PHYSICAL_DIMENSIONS >= 4) {
      return G::make_plane(GeometryConceptsTest<G>::x, GeometryConceptsTest<G>::y,
                           GeometryConceptsTest<G>::z, GeometryConceptsTest<G>::w,
                           GeometryConceptsTest<G>::offset);
    } else if constexpr (G::NUM_PHYSICAL_DIMENSIONS == 3) {
      return G::make_plane(GeometryConceptsTest<G>::x, GeometryConceptsTest<G>::y,
                           GeometryConceptsTest<G>::z, GeometryConceptsTest<G>::offset);
    } else {
      return G::make_plane(GeometryConceptsTest<G>::x, GeometryConceptsTest<G>::y,
                           GeometryConceptsTest<G>::offset);
    }
  } else if constexpr (P == Primitives::CIRCLE) {
    if constexpr (G::NUM_PHYSICAL_DIMENSIONS >= 4) {
      return G::make_circle(GeometryConceptsTest<G>::x, GeometryConceptsTest<G>::y,
                            GeometryConceptsTest<G>::z, GeometryConceptsTest<G>::w,
                            GeometryConceptsTest<G>::radius);
    } else if constexpr (G::NUM_PHYSICAL_DIMENSIONS == 3) {
      return G::make_circle(GeometryConceptsTest<G>::x, GeometryConceptsTest<G>::y,
                            GeometryConceptsTest<G>::z, GeometryConceptsTest<G>::radius);
    } else {
      return G::make_circle(GeometryConceptsTest<G>::x, GeometryConceptsTest<G>::y,
                            GeometryConceptsTest<G>::radius);
    }
  } else if constexpr (P == Primitives::SPHERE) {
    return G::make_sphere(GeometryConceptsTest<G>::x, GeometryConceptsTest<G>::y,
                          GeometryConceptsTest<G>::z, GeometryConceptsTest<G>::radius);
  } else if constexpr (P == Primitives::HYPERPLANE) {
    if constexpr (G::NUM_PHYSICAL_DIMENSIONS >= 4) {
      return G::make_hyperplane(GeometryConceptsTest<G>::x, GeometryConceptsTest<G>::y,
                                GeometryConceptsTest<G>::z, GeometryConceptsTest<G>::w,
                                GeometryConceptsTest<G>::offset);
    } else {
      return G::make_hyperplane(GeometryConceptsTest<G>::x, GeometryConceptsTest<G>::y,
                                GeometryConceptsTest<G>::z, GeometryConceptsTest<G>::offset);
    }
  } else if constexpr (P == Primitives::HYPERSPHERE) {
    return G::make_hypersphere(GeometryConceptsTest<G>::x, GeometryConceptsTest<G>::y,
                               GeometryConceptsTest<G>::z, GeometryConceptsTest<G>::w,
                               GeometryConceptsTest<G>::radius);
  }
}

template <typename G, Operators Op>
typename G::Multivector make_operator() {
  if constexpr (Op == Operators::ROTOR) {
    if constexpr (G::NUM_PHYSICAL_DIMENSIONS == 2) {
      return G::make_rotor(GeometryConceptsTest<G>::angle);
    } else if constexpr (G::NUM_PHYSICAL_DIMENSIONS == 3) {
      return G::make_rotor(GeometryConceptsTest<G>::x, GeometryConceptsTest<G>::y,
                           GeometryConceptsTest<G>::z, GeometryConceptsTest<G>::angle);
    } else {
      // 3D and above: use bivector multivector + angle.
      // Construct a unit bivector from gamma01 as the plane of rotation.
      return G::make_rotor(G::gamma01(), GeometryConceptsTest<G>::angle);
    }
  } else if constexpr (Op == Operators::TRANSLATOR) {
    if constexpr (G::NUM_PHYSICAL_DIMENSIONS >= 4) {
      return G::make_translator(GeometryConceptsTest<G>::dx, GeometryConceptsTest<G>::dy,
                                GeometryConceptsTest<G>::dz, GeometryConceptsTest<G>::dw);
    } else if constexpr (G::NUM_PHYSICAL_DIMENSIONS == 3) {
      return G::make_translator(GeometryConceptsTest<G>::dx, GeometryConceptsTest<G>::dy,
                                GeometryConceptsTest<G>::dz);
    } else if constexpr (G::NUM_PHYSICAL_DIMENSIONS == 2) {
      return G::make_translator(GeometryConceptsTest<G>::dx, GeometryConceptsTest<G>::dy);
    } else {
      return G::make_translator(GeometryConceptsTest<G>::dx);
    }
  } else if constexpr (Op == Operators::DILATOR) {
    return G::make_dilator(GeometryConceptsTest<G>::scale);
  }
}

template <typename G, Primitives P>
bool check_is_primitive(const typename G::Multivector& mv) {
  if constexpr (P == Primitives::POINT) {
    return G::is_point(mv);
  } else if constexpr (P == Primitives::POINT_PAIR) {
    return G::is_point_pair(mv);
  } else if constexpr (P == Primitives::LINE) {
    return G::is_line(mv);
  } else if constexpr (P == Primitives::PLANE) {
    return G::is_plane(mv);
  } else if constexpr (P == Primitives::CIRCLE) {
    return G::is_circle(mv);
  } else if constexpr (P == Primitives::SPHERE) {
    return G::is_sphere(mv);
  } else if constexpr (P == Primitives::HYPERPLANE) {
    return G::is_hyperplane(mv);
  } else if constexpr (P == Primitives::HYPERSPHERE) {
    return G::is_hypersphere(mv);
  } else {
    return false;
  }
}

template <typename G, Primitives P>
constexpr bool geometry_has_primitive() {
  if constexpr (P == Primitives::POINT) {
    return HasPoint<G>;
  } else if constexpr (P == Primitives::POINT_PAIR) {
    return HasPointPair<G>;
  } else if constexpr (P == Primitives::LINE) {
    return HasLine<G>;
  } else if constexpr (P == Primitives::PLANE) {
    return HasPlane<G>;
  } else if constexpr (P == Primitives::CIRCLE) {
    return HasCircle<G>;
  } else if constexpr (P == Primitives::SPHERE) {
    return HasSphere<G>;
  } else if constexpr (P == Primitives::HYPERPLANE) {
    return HasHyperplane<G>;
  } else if constexpr (P == Primitives::HYPERSPHERE) {
    return HasHypersphere<G>;
  } else {
    return false;
  }
}

template <typename G, Operators Op>
constexpr bool geometry_has_operator() {
  if constexpr (Op == Operators::ROTOR) {
    return HasRotor<G>;
  } else if constexpr (Op == Operators::TRANSLATOR) {
    return HasTranslator<G>;
  } else if constexpr (Op == Operators::DILATOR) {
    return HasDilator<G>;
  } else {
    return false;
  }
}

/**
 * Verifies that a primitive acted on by an operator is closed under that operation. That is, a
 * translated point is still a point.
 */
template <typename G, Primitives P, Operators Op>
::testing::AssertionResult verify_closure() {
  if constexpr (geometry_has_primitive<G, P>() && geometry_has_operator<G, Op>()) {
    const auto primitive = make_primitive<G, P>();
    const auto op = make_operator<G, Op>();
    const auto result = op * primitive * op.reverse();

    if (check_is_primitive<G, P>(result)) {
      return ::testing::AssertionSuccess();
    } else {
      LOG(INFO) << "primitive: " << primitive;
      LOG(INFO) << "op: " << op;
      LOG(INFO) << "result: " << result;
      return ::testing::AssertionFailure()
             << "Closure failed: primitive " << static_cast<int>(P) << " acted on by operator "
             << static_cast<int>(Op)
             << " did not produce the same primitive type. (NUM_PHYSICAL_DIMENSIONS: "
             << G::NUM_PHYSICAL_DIMENSIONS << ")";
    }
  } else {
    return ::testing::AssertionSuccess();
  }
}

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

TYPED_TEST_P(GeometryConceptsTest, PointClosure) {
  EXPECT_TRUE((verify_closure<TypeParam, Primitives::POINT, Operators::ROTOR>()));
  EXPECT_TRUE((verify_closure<TypeParam, Primitives::POINT, Operators::TRANSLATOR>()));
  EXPECT_TRUE((verify_closure<TypeParam, Primitives::POINT, Operators::DILATOR>()));
}

TYPED_TEST_P(GeometryConceptsTest, PointPairClosure) {
  EXPECT_TRUE((verify_closure<TypeParam, Primitives::POINT_PAIR, Operators::ROTOR>()));
  EXPECT_TRUE((verify_closure<TypeParam, Primitives::POINT_PAIR, Operators::TRANSLATOR>()));
  EXPECT_TRUE((verify_closure<TypeParam, Primitives::POINT_PAIR, Operators::DILATOR>()));
}

TYPED_TEST_P(GeometryConceptsTest, LineClosure) {
  EXPECT_TRUE((verify_closure<TypeParam, Primitives::LINE, Operators::ROTOR>()));
  EXPECT_TRUE((verify_closure<TypeParam, Primitives::LINE, Operators::TRANSLATOR>()));
  EXPECT_TRUE((verify_closure<TypeParam, Primitives::LINE, Operators::DILATOR>()));
}

TYPED_TEST_P(GeometryConceptsTest, PlaneClosure) {
  EXPECT_TRUE((verify_closure<TypeParam, Primitives::PLANE, Operators::ROTOR>()));
  EXPECT_TRUE((verify_closure<TypeParam, Primitives::PLANE, Operators::TRANSLATOR>()));
  EXPECT_TRUE((verify_closure<TypeParam, Primitives::PLANE, Operators::DILATOR>()));
}

TYPED_TEST_P(GeometryConceptsTest, CircleClosure) {
  EXPECT_TRUE((verify_closure<TypeParam, Primitives::CIRCLE, Operators::ROTOR>()));
  EXPECT_TRUE((verify_closure<TypeParam, Primitives::CIRCLE, Operators::TRANSLATOR>()));
  EXPECT_TRUE((verify_closure<TypeParam, Primitives::CIRCLE, Operators::DILATOR>()));
}

TYPED_TEST_P(GeometryConceptsTest, SphereClosure) {
  EXPECT_TRUE((verify_closure<TypeParam, Primitives::SPHERE, Operators::ROTOR>()));
  EXPECT_TRUE((verify_closure<TypeParam, Primitives::SPHERE, Operators::TRANSLATOR>()));
  EXPECT_TRUE((verify_closure<TypeParam, Primitives::SPHERE, Operators::DILATOR>()));
}

TYPED_TEST_P(GeometryConceptsTest, HyperplaneClosure) {
  EXPECT_TRUE((verify_closure<TypeParam, Primitives::HYPERPLANE, Operators::ROTOR>()));
  EXPECT_TRUE((verify_closure<TypeParam, Primitives::HYPERPLANE, Operators::TRANSLATOR>()));
  EXPECT_TRUE((verify_closure<TypeParam, Primitives::HYPERPLANE, Operators::DILATOR>()));
}

TYPED_TEST_P(GeometryConceptsTest, HypersphereClosure) {
  EXPECT_TRUE((verify_closure<TypeParam, Primitives::HYPERSPHERE, Operators::ROTOR>()));
  EXPECT_TRUE((verify_closure<TypeParam, Primitives::HYPERSPHERE, Operators::TRANSLATOR>()));
  EXPECT_TRUE((verify_closure<TypeParam, Primitives::HYPERSPHERE, Operators::DILATOR>()));
}

REGISTER_TYPED_TEST_SUITE_P(  //
    GeometryConceptsTest,
    // Point tests.
    PointRoundtrip1D, PointRoundtrip2D, PointRoundtrip3D, PointRoundtrip4D, PointClosure,
    //  Point pair tests.
    PointPairRoundtrip2D, PointPairRoundtrip3D, PointPairRoundtrip4D, PointPairClosure,
    // Line tests.
    LineRoundtrip2D, LineRoundtrip3D, LineRoundtrip4D, LineClosure,
    // Plane tests.
    PlaneRoundtrip2D, PlaneRoundtrip3D, PlaneRoundtrip4D, PlaneClosure,
    // Circle tests.
    CircleRoundtrip2D, CircleRoundtrip3D, CircleRoundtrip4D, CircleClosure,
    // Sphere tests.
    SphereRoundtrip2D, SphereRoundtrip3D, SphereRoundtrip4D, SphereClosure,
    // Hyperplane tests.
    HyperplaneRoundtrip3D, HyperplaneRoundtrip4D, HyperplaneClosure,
    // Hypersphere tests.
    HypersphereRoundtrip, HypersphereClosure);

}  // namespace ndyn::math
