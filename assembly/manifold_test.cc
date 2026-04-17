#include "assembly/manifold.h"

#include <cmath>

#include "glog/logging.h"
#include "gtest/gtest.h"
#include "math/cga_geometry.h"
#include "math/sta_geometry.h"

namespace ndyn::test {

using namespace ndyn::math;
using namespace ndyn::assembly;

template <typename Geometry>
class ManifoldTest : public ::testing::Test {
 protected:
  using Scalar = typename Geometry::ScalarType;
  using MV = typename Geometry::Multivector;

  static constexpr Scalar C_INF = 1e15;
  static constexpr Scalar C_STD = 1.0;

  auto constant_c(Scalar value) {
    return [value](Scalar /*time*/) { return value; };
  }

  // Helper to create a state at a specific time and spatial translation
  math::State<Geometry, 2> make_state(Scalar t, Scalar x = 0, Scalar y = 0, Scalar z = 0) {
    math::State<Geometry, 2> s;
    // element<0> is the pose motor
    s.template set_element<0>(Geometry::translator(x, y, z) * Geometry::identity_at_time(t));
    return s;
  }
};

// Define the geometry types to run the test suite against
using GeometryTypes = ::testing::Types<TcgaGeometry, CstaGeometry>;
TYPED_TEST_SUITE(ManifoldTest, GeometryTypes);

/**
 * IDENTITY INVARIANT
 * Transporting between identical events must yield the multiplicative identity (1).
 */
TYPED_TEST(ManifoldTest, IdentityTransportAtSameEvent) {
  using Geometry = TypeParam;
  Manifold<Geometry> manifold(this->constant_c(this->C_STD));
  Worldline<Geometry> wl(10, manifold);

  auto state = this->make_state(0.0);
  wl.record_state(state);

  auto M = manifold.transport(wl, state.template element<0>());

  EXPECT_NEAR(M.scalar(), 1.0, 1e-9);
  EXPECT_LT((M - Geometry::Multivector::identity()).square_magnitude(), 1e-9);
}

/**
 * GALILEAN LIMIT
 * In the high-c limit, retardation disappears. t_ret should approach t_now.
 */
TYPED_TEST(ManifoldTest, GalileanLimitRetardation) {
  using Geometry = TypeParam;
  Manifold<Geometry> manifold(this->constant_c(this->C_INF));
  Worldline<Geometry> wl(10, manifold);

  // Source is 50 units away at t=0
  wl.record_state(this->make_state(0.0, 50.0, 0.0, 0.0));

  // Target is at origin at t=1.0
  auto target_pose = Geometry::identity_at_time(1.0);

  double t_ret = manifold.solve_retardation(wl, target_pose);
  // With c near infinity, light travel is instantaneous
  EXPECT_NEAR(t_ret, 1.0, 1e-10);
}

/**
 * MOTOR UNITARITY (M * ~M = 1)
 * Parallel transport must be an isometry, preserving the normalization of the motor.
 */
TYPED_TEST(ManifoldTest, TransportPreservesMotorUnitarity) {
  using Geometry = TypeParam;
  Manifold<Geometry> manifold(this->constant_c(this->C_STD));
  Worldline<Geometry> wl(10, manifold);

  wl.record_state(this->make_state(0.0));
  auto target_pose = Geometry::translator(10.0, 5.0, -2.0) * Geometry::identity_at_time(15.0);

  auto M = manifold.transport(wl, target_pose);
  auto check = M * (~M);

  EXPECT_NEAR(check.scalar(), 1.0, 1e-9);
}

/**
 * CAUSAL MONOTONICITY
 * Slower light speed must result in a strictly earlier (deeper) retarded time.
 */
TYPED_TEST(ManifoldTest, CausalMonotonicityWithVariableLight) {
  using Geometry = TypeParam;
  auto target_pose = Geometry::identity_at_time(20.0);
  auto source_state = this->make_state(0.0, 15.0, 0.0, 0.0);

  Manifold<Geometry> fast_m(this->constant_c(10.0));
  Manifold<Geometry> slow_m(this->constant_c(1.0));

  Worldline<Geometry> wl_f(10, fast_m);
  Worldline<Geometry> wl_s(10, slow_m);
  wl_f.record_state(source_state);
  wl_s.record_state(source_state);

  double t_fast = fast_m.solve_retardation(wl_f, target_pose);
  double t_slow = slow_m.solve_retardation(wl_s, target_pose);

  EXPECT_LT(t_slow, t_fast);
}

/**
 * BIVECTOR MAGNITUDE PRESERVATION
 * Parallel transport of a field/torque bivector must preserve its norm.
 */
TYPED_TEST(ManifoldTest, BivectorMagnitudePreservation) {
  using Geometry = TypeParam;
  Manifold<Geometry> manifold(this->constant_c(this->C_STD));
  Worldline<Geometry> wl(10, manifold);

  wl.record_state(this->make_state(0.0));
  auto target_pose = Geometry::translator(3.0, 4.0, 0.0) * Geometry::identity_at_time(10.0);

  auto M = manifold.transport(wl, target_pose);

  // Construct a unit bivector (e.g., rotation plane or magnetic field component)
  auto bivector = Geometry::bivector_xy(1.0);
  double original_mag = bivector.square_magnitude();

  auto transported = M * bivector * (~M);
  double final_mag = transported.square_magnitude();

  EXPECT_NEAR(original_mag, final_mag, 1e-9);
}

/**
 * FLAT SPACE RETARDATION CONSISTENCY
 * In a static manifold, verifying t_ret = t_now - (dist / c).
 */
TYPED_TEST(ManifoldTest, EuclideanDistanceTimeConsistency) {
  using Geometry = TypeParam;
  double c = 2.0;
  Manifold<Geometry> manifold(this->constant_c(c));
  Worldline<Geometry> wl(10, manifold);

  wl.record_state(this->make_state(0.0, 0.0, 0.0, 0.0));

  // Target is 10 units away at t=10.0
  auto target_pose = Geometry::translator(10.0, 0.0, 0.0) * Geometry::identity_at_time(10.0);

  double t_ret = manifold.solve_retardation(wl, target_pose);
  // dist(10) / c(2) = 5.0 seconds lag. t_ret should be 5.0.
  EXPECT_NEAR(t_ret, 5.0, 1e-7);
}

/**
 * ROTATIONAL ISOTROPY
 * Transport results should be invariant under global rotation of the setup.
 */
TYPED_TEST(ManifoldTest, RotationalIsotropy) {
  using Geometry = TypeParam;
  Manifold<Geometry> manifold(this->constant_c(this->C_STD));
  Worldline<Geometry> wl(10, manifold);

  // Setup A: Source at (1,0,0)
  wl.record_state(this->make_state(0.0, 1.0, 0.0, 0.0));
  auto target = Geometry::identity_at_time(5.0);
  auto Ma = manifold.transport(wl, target);

  // Setup B: Source at (0,1,0) - a 90 deg Z-rotation
  Worldline<Geometry> wl_b(10, manifold);
  wl_b.record_state(this->make_state(0.0, 0.0, 1.0, 0.0));
  auto Mb = manifold.transport(wl_b, target);

  // The motors should have identical "lengths" in the Lie Algebra
  EXPECT_NEAR(Ma.square_magnitude(), Mb.square_magnitude(), 1e-9);
}

/**
 * HORIZON CLAMPING
 * Verify that the solver clamps to the oldest history point if the light-cone
 * cannot be closed (Source is too far away).
 */
TYPED_TEST(ManifoldTest, HorizonClamping) {
  using Geometry = TypeParam;
  Manifold<Geometry> manifold(this->constant_c(0.01));  // Extremely slow light
  Worldline<Geometry> wl(10, manifold);

  wl.record_state(this->make_state(0.0, 100.0, 0.0, 0.0));  // Far source
  wl.record_state(this->make_state(1.0, 100.0, 0.0, 0.0));

  auto target = Geometry::identity_at_time(1.1);

  double t_ret = manifold.solve_retardation(wl, target);
  // Light cannot travel 100 units in 1.1s at c=0.01. Should clamp to oldest time (0.0).
  EXPECT_DOUBLE_EQ(t_ret, 0.0);
}

/**
 * NON-LINEAR LIGHT SPEED
 * Test convergence when c(t) is a complex function.
 */
TYPED_TEST(ManifoldTest, ComplexLightSpeedConvergence) {
  using Geometry = TypeParam;
  auto complex_c = [](double t) { return 5.0 + std::sin(t); };
  Manifold<Geometry> manifold(complex_c);
  Worldline<MockGeometry> wl(10, manifold);

  wl.record_state(this->make_state(0.0, 1.0, 1.0, 1.0));
  auto target = Geometry::identity_at_time(10.0);

  double t_ret = manifold.solve_retardation(wl, target);
  EXPECT_LT(t_ret, 10.0);
  EXPECT_GT(t_ret, 0.0);
}

/**
 * REVERSIBILITY (Flat Case)
 * In flat space, transport A->B followed by B->A should return identity.
 */
TYPED_TEST(ManifoldTest, TransportReversibility) {
  using Geometry = TypeParam;
  Manifold<Geometry> manifold(this->constant_c(this->C_STD));
  Worldline<Geometry> wl_a(10, manifold);
  Worldline<Geometry> wl_b(10, manifold);

  auto pos_a = this->make_state(0.0, 0.0, 0.0, 0.0);
  auto pos_b = this->make_state(0.0, 5.0, 0.0, 0.0);

  wl_a.record_state(pos_a);
  wl_b.record_state(pos_b);

  auto Mab = manifold.transport(wl_a, pos_b.template element<0>());
  auto Mba = manifold.transport(wl_b, pos_a.template element<0>());

  auto combined = Mab * Mba;
  EXPECT_NEAR(combined.scalar(), 1.0, 1e-7);
}

}  // namespace ndyn::test
