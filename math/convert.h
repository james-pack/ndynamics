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

template <typename ScalarT, InnerProduct INNER_PRODUCT_STYLE, size_t POSITIVE_BASES,
          size_t NEGATIVE_BASES, size_t ZERO_BASES>
class Convert<Multivector<ScalarT, POSITIVE_BASES, NEGATIVE_BASES, ZERO_BASES, INNER_PRODUCT_STYLE>,
              CartesianMeters, PolarMeters>
    final {
 public:
  using VectorT =
      Multivector<ScalarT, POSITIVE_BASES, NEGATIVE_BASES, ZERO_BASES, INNER_PRODUCT_STYLE>;

  VectorT operator()(const VectorT& incoming) const {
    using std::acos;
    using std::copysign;
    using std::hypot;
    VectorT result{incoming};
    const ScalarT length{hypot(incoming.x(), incoming.y())};
    const ScalarT theta{copysign(acos(incoming.x() / length), incoming.y())};
    result.set_r(length);
    result.set_theta(theta);
    return result;
  }
};

template <typename ScalarT, InnerProduct INNER_PRODUCT_STYLE, size_t POSITIVE_BASES,
          size_t NEGATIVE_BASES, size_t ZERO_BASES>
class Convert<Multivector<ScalarT, POSITIVE_BASES, NEGATIVE_BASES, ZERO_BASES, INNER_PRODUCT_STYLE>,
              PolarMeters, CartesianMeters>
    final {
 public:
  using VectorT =
      Multivector<ScalarT, POSITIVE_BASES, NEGATIVE_BASES, ZERO_BASES, INNER_PRODUCT_STYLE>;

  VectorT operator()(const VectorT& incoming) const {
    using std::cos;
    using std::sin;

    VectorT result{incoming};
    result.set_x(incoming.r() * cos(incoming.theta()));
    result.set_y(incoming.r() * sin(incoming.theta()));
    return result;
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

template <typename ScalarT, InnerProduct INNER_PRODUCT_STYLE>
class Convert<Multivector<ScalarT, 3, 0, 0, INNER_PRODUCT_STYLE>, SphericalMeters, CartesianMeters>
    final {
 public:
  using VectorT = Multivector<ScalarT, 3, 0, 0, INNER_PRODUCT_STYLE>;

  VectorT operator()(const VectorT& incoming) const {
    using std::cos;
    using std::sin;

    const ScalarT x{incoming.r() * cos(incoming.phi()) * sin(incoming.theta())};
    const ScalarT y{incoming.r() * sin(incoming.phi()) * sin(incoming.theta())};
    const ScalarT z{incoming.r() * cos(incoming.theta())};

    return x * VectorT::template e<0>() + y * VectorT::template e<1>() +
           z * VectorT::template e<2>();
  }
};

}  // namespace ndyn::math
