#pragma once

#include <cmath>

#include "math/multivector.h"
#include "math/multivector_utils.h"
#include "math/unit_set.h"

namespace ndyn::math {

template <typename VectorT, typename IncomingUnitsT, typename ResultUnitsT>
class Convert final {
 public:
  VectorT operator()(const VectorT& incoming) const;
};

template <typename ScalarT, InnerProduct INNER_PRODUCT_STYLE>
class Convert<Multivector<ScalarT, 2, 0, 0, INNER_PRODUCT_STYLE>, CartesianMeters, PolarMeters>
    final {
 public:
  using VectorT = Multivector<ScalarT, 2, 0, 0, INNER_PRODUCT_STYLE>;

  VectorT operator()(const VectorT& incoming) const {
    using std::abs;
    using std::acos;
    using std::copysign;
    const ScalarT length{abs(incoming)};
    const ScalarT theta{copysign(acos(incoming.x() / length), incoming.y())};
    return length * VectorT::template e<0>() + theta * VectorT::template e<1>();
  }
};

template <typename ScalarT, InnerProduct INNER_PRODUCT_STYLE>
class Convert<Multivector<ScalarT, 3, 0, 0, INNER_PRODUCT_STYLE>, CartesianMeters, PolarMeters>
    final {
 public:
  using VectorT = Multivector<ScalarT, 3, 0, 0, INNER_PRODUCT_STYLE>;

  VectorT operator()(const VectorT& incoming) const {
    using std::abs;
    using std::acos;
    using std::copysign;
    using std::hypot;
    const ScalarT length{hypot(incoming.x(), incoming.y())};
    const ScalarT theta{copysign(acos(incoming.x() / length), incoming.y())};
    return length * VectorT::template e<0>() + theta * VectorT::template e<1>() +
           incoming.z() * VectorT::template e<2>();
  }
};

template <typename ScalarT, InnerProduct INNER_PRODUCT_STYLE>
class Convert<Multivector<ScalarT, 2, 0, 0, INNER_PRODUCT_STYLE>, CartesianMeters, SphericalMeters>
    final {
 public:
  using VectorT = Multivector<ScalarT, 2, 0, 0, INNER_PRODUCT_STYLE>;

  VectorT operator()(const VectorT& incoming) const {
    using std::abs;
    using std::acos;
    using std::copysign;
    const ScalarT length{abs(incoming)};
    const ScalarT theta{copysign(acos(incoming.x() / length), incoming.y())};
    return length * VectorT::template e<0>() + theta * VectorT::template e<1>();
  }
};

template <typename ScalarT, InnerProduct INNER_PRODUCT_STYLE>
class Convert<Multivector<ScalarT, 3, 0, 0, INNER_PRODUCT_STYLE>, CartesianMeters, SphericalMeters>
    final {
 public:
  using VectorT = Multivector<ScalarT, 3, 0, 0, INNER_PRODUCT_STYLE>;

  VectorT operator()(const VectorT& incoming) const {
    using std::abs;
    using std::acos;
    using std::atan2;
    const ScalarT length{abs(incoming)};
    const ScalarT phi{atan2(incoming.y(), incoming.x())};
    const ScalarT theta{acos(incoming.z() / length)};

    return length * VectorT::template e<0>() + theta * VectorT::template e<1>() +
           phi * VectorT::template e<2>();
  }
};

}  // namespace ndyn::math
