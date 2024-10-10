#pragma once

#include <cmath>
#include <type_traits>

#include "math/coordinates.h"
#include "math/multivector.h"
#include "math/multivector_utils.h"
#include "math/state.h"
#include "math/unit_set.h"

namespace ndyn::math {

/**
 * Conversion that converts the coordinates of a state.
 */
template <Coordinates INCOMING_COORDINATES, Coordinates RESULT_COORDINATES,
          typename IncomingStateType, typename ResultStateType>
ResultStateType convert_coordinates(const IncomingStateType& incoming) {
  using std::acos;
  using std::atan2;
  using std::cos;
  using std::hypot;
  using std::sin;

  static constexpr size_t CONVERSION_DEPTH{
      std::min(IncomingStateType::depth(), ResultStateType::depth())};

  using VectorT = typename ResultStateType::VectorType;
  using ScalarT = typename ResultStateType::ScalarType;

  // Assert that we have an implementation for the requested incoming and result coordinates.
  static_assert(INCOMING_COORDINATES == Coordinates::CARTESIAN ||
                    INCOMING_COORDINATES == Coordinates::POLAR ||
                    INCOMING_COORDINATES == Coordinates::SPHERICAL,
                "Incoming coordinates not implemented");
  static_assert(RESULT_COORDINATES == Coordinates::CARTESIAN ||
                    RESULT_COORDINATES == Coordinates::POLAR ||
                    RESULT_COORDINATES == Coordinates::SPHERICAL,
                "Result coordinates not implemented");

  static_assert(CONVERSION_DEPTH < 4,
                "Coordinate conversions are not implemented beyond a state depth of 3");

  ResultStateType result{};

  if constexpr (INCOMING_COORDINATES == Coordinates::CARTESIAN) {
    if constexpr (RESULT_COORDINATES == Coordinates::POLAR) {
      if constexpr (CONVERSION_DEPTH >= 1) {
        const VectorT& incoming_vector{incoming.template element<0>()};
        VectorT result_vector{incoming_vector};
        const ScalarT length{hypot(incoming_vector.x(), incoming_vector.y())};
        const ScalarT theta{atan2(incoming_vector.y(), incoming_vector.x())};
        result_vector.set_r(length);
        result_vector.set_theta(theta);
        result.template set_element<0>(result_vector);

        if constexpr (CONVERSION_DEPTH >= 2) {
          // TODO(james): Handle derivative cases.

          if constexpr (CONVERSION_DEPTH >= 3) {
            // TODO(james): Handle derivative cases.
          }
        }
      }
    }

    if constexpr (RESULT_COORDINATES == Coordinates::SPHERICAL) {
      if constexpr (CONVERSION_DEPTH >= 1) {
        const VectorT& incoming_vector{incoming.template element<0>()};
        VectorT result_vector{incoming_vector};
        const ScalarT length{hypot(incoming_vector.x(), incoming_vector.y(), incoming_vector.z())};
        const ScalarT phi{atan2(incoming_vector.y(), incoming_vector.x())};
        const ScalarT theta{acos(incoming_vector.z() / length)};
        result_vector.set_r(length);
        result_vector.set_theta(theta);
        result_vector.set_phi(phi);
        result.template set_element<0>(result_vector);

        if constexpr (CONVERSION_DEPTH >= 2) {
          // TODO(james): Handle derivative cases.

          if constexpr (CONVERSION_DEPTH >= 3) {
            // TODO(james): Handle derivative cases.
          }
        }
      }
    }
  }

  if constexpr (INCOMING_COORDINATES == Coordinates::POLAR) {
    if constexpr (RESULT_COORDINATES == Coordinates::CARTESIAN) {
      if constexpr (CONVERSION_DEPTH >= 1) {
        const VectorT& incoming_0{incoming.template element<0>()};
        const ScalarT x0{incoming_0.r() * cos(incoming_0.theta())};
        const ScalarT y0{incoming_0.r() * sin(incoming_0.theta())};

        VectorT result_0{incoming_0};
        result_0.set_x(x0);
        result_0.set_y(y0);
        result.template set_element<0>(result_0);

        if constexpr (CONVERSION_DEPTH >= 2) {
          const VectorT& incoming_1{incoming.template element<1>()};

          const ScalarT x1{incoming_1.r() * cos(incoming_0.theta()) -
                           incoming_0.r() * sin(incoming_0.theta()) * incoming_1.theta()};

          const ScalarT y1{incoming_1.r() * sin(incoming_0.theta()) +
                           incoming_0.r() * cos(incoming_0.theta()) * incoming_1.theta()};

          VectorT result_1{incoming_1};
          result_1.set_x(x1);
          result_1.set_y(y1);
          result.template set_element<1>(result_1);

          if constexpr (CONVERSION_DEPTH >= 3) {
            const VectorT& incoming_2{incoming.template element<2>()};

            const ScalarT x2{incoming_2.r() * cos(incoming_0.theta()) -
                             incoming_1.r() * sin(incoming_0.theta()) * incoming_1.theta() -
                             (incoming_1.r() * sin(incoming_0.theta()) * incoming_1.theta() +
                              incoming_0.r() * cos(incoming_0.theta()) * incoming_1.theta() *
                                  incoming_1.theta() +
                              incoming_0.r() * sin(incoming_0.theta()) * incoming_2.theta())};

            const ScalarT y2{incoming_2.r() * sin(incoming_0.theta()) +
                             incoming_1.r() * cos(incoming_0.theta()) * incoming_1.theta() +
                             incoming_1.r() * cos(incoming_0.theta()) * incoming_1.theta() -
                             incoming_0.r() * sin(incoming_0.theta()) * incoming_1.theta() *
                                 incoming_1.theta() +
                             incoming_0.r() * cos(incoming_0.theta()) * incoming_2.theta()};

            VectorT result_2{incoming_2};
            result_2.set_x(x2);
            result_2.set_y(y2);
            result.template set_element<2>(result_2);
          }
        }
      }
    }

    if constexpr (RESULT_COORDINATES == Coordinates::SPHERICAL) {
      static_assert(!(INCOMING_COORDINATES == Coordinates::POLAR &&
                      RESULT_COORDINATES == Coordinates::SPHERICAL),
                    "Conversion from polar to spherical not implemented.");
    }
  }

  if constexpr (INCOMING_COORDINATES == Coordinates::SPHERICAL) {
    if constexpr (RESULT_COORDINATES == Coordinates::CARTESIAN) {
      if constexpr (CONVERSION_DEPTH >= 1) {
        const VectorT& incoming_vector{incoming.template element<0>()};
        const ScalarT x{incoming_vector.r() * cos(incoming_vector.phi()) *
                        sin(incoming_vector.theta())};
        const ScalarT y{incoming_vector.r() * sin(incoming_vector.phi()) *
                        sin(incoming_vector.theta())};
        const ScalarT z{incoming_vector.r() * cos(incoming_vector.theta())};

        VectorT result_vector{incoming_vector};
        result_vector.set_x(x);
        result_vector.set_y(y);
        result_vector.set_z(z);
        result.template set_element<0>(result_vector);

        if constexpr (CONVERSION_DEPTH >= 2) {
          // TODO(james): Handle derivative cases.

          if constexpr (CONVERSION_DEPTH >= 3) {
            // TODO(james): Handle derivative cases.
          }
        }
      }
    }

    if constexpr (RESULT_COORDINATES == Coordinates::POLAR) {
      static_assert(!(INCOMING_COORDINATES == Coordinates::SPHERICAL &&
                      RESULT_COORDINATES == Coordinates::POLAR),
                    "Conversion from spherical to polar not implemented.");
    }
  }

  return result;
}

}  // namespace ndyn::math
