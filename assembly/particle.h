#pragma once

#include <array>
#include <cmath>
#include <vector>

#include "gfx/mesh.h"
#include "math/algebra.h"
#include "math/state.h"

namespace ndyn::assembly {

template <typename GeometryT>
struct Particle final {
  using Geometry = GeometryT;
  using Algebra = Geometry::Algebra;
  using Multivector = Geometry::Multivector;
  using Scalar = Geometry::Scalar;
  using StateVector = math::State<Algebra, 2>;

  MeshId mesh{};

  void update(const Multivector& field, Scalar d_tau) {
  }
};

}  // namespace ndyn::assembly
