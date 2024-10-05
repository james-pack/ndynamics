#pragma once

#include <cmath>
#include <type_traits>

#include "math/multivector.h"
#include "math/multivector_utils.h"
#include "math/state.h"
#include "math/unit_set.h"

namespace ndyn::math {

/**
 * Conversion that just converts the units of a vector. It requires
 * that the incoming and resulting coordinate systems are the same.
 */
template <typename VectorType, typename IncomingUnits, typename ResultUnits>
VectorType convert_vector_units(const VectorType& incoming) {
  // TODO(james): Implement convert_units(VectorType).
  return incoming;
}

/**
 * Conversion that just converts the units of each vector in the state independently. It requires
 * that the incoming and resulting coordinate systems are the same.
 */
template <typename VectorType, size_t DEPTH, typename IncomingUnits, typename ResultUnits>
State<VectorType, DEPTH, ResultUnits> convert_units(
    const State<VectorType, DEPTH, IncomingUnits>& incoming) {
  static_assert(IncomingUnits::coordinates() == ResultUnits::coordinates(),
                "Can only convert units, not coordinates");

  State<VectorType, DEPTH, ResultUnits> result{};
  for (size_t i = 0; i < DEPTH; ++i) {
    result.set_element(
        i, convert_vector_units<VectorType, IncomingUnits, ResultUnits>(incoming.element(i)));
  }
  return result;
}

/**
 * Conversion that converts the coordinates of a state.
 */
template <typename IncomingStateType, typename ResultStateType>
void convert_coordinates(const IncomingStateType& incoming, ResultStateType& result) {
  using std::acos;
  using std::atan2;
  using std::cos;
  using std::hypot;
  using std::sin;

  static constexpr Coordinates INCOMING_COORDINATES{IncomingStateType::Units::coordinates()};
  static constexpr Coordinates RESULT_COORDINATES{ResultStateType::Units::coordinates()};
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
}

template <typename IncomingStateT, typename ResultStateT>
class ConvertState final {
 public:
  using IncomingStateType = IncomingStateT;
  using ResultStateType = ResultStateT;

  using IncomingVectorType = typename IncomingStateType::VectorType;
  using ResultVectorType = typename ResultStateType::VectorType;

  static constexpr size_t INCOMING_DEPTH{IncomingStateType::depth()};
  static constexpr size_t RESULT_DEPTH{ResultStateType::depth()};

  using IncomingUnits = typename IncomingStateType::Units;
  using ResultUnits = typename ResultStateType::Units;

  static constexpr Coordinates INCOMING_COORDINATES{IncomingUnits::coordinates()};
  static constexpr Coordinates RESULT_COORDINATES{ResultUnits::coordinates()};

 private:
 public:
  ResultStateType operator()(const IncomingStateType& incoming) const {
    // Change the vector type.
    using IncomingStateWithResultVectorType =
        State<ResultVectorType, INCOMING_DEPTH, IncomingUnits>;

    IncomingStateWithResultVectorType with_result_vector_type{};

    if constexpr (!std::is_same_v<IncomingVectorType, ResultVectorType>) {
      static_assert(std::is_same_v<IncomingVectorType, ResultVectorType>,
                    "TODO(james): Implement vector type conversion");
    } else {
      with_result_vector_type = incoming;
    }

    // Change coordinates.
    using IncomingUnitsWithResultCoordinates =
        typename IncomingUnits::template with_changed_coordinates<RESULT_COORDINATES>;

    using IncomingStateWithResultCoordinates =
        State<ResultVectorType, INCOMING_DEPTH, IncomingUnitsWithResultCoordinates>;

    IncomingStateWithResultCoordinates with_result_coordinates{};
    if constexpr (INCOMING_COORDINATES != RESULT_COORDINATES) {
      convert_coordinates(with_result_vector_type, with_result_coordinates);
    } else {
      with_result_coordinates = with_result_vector_type;
    }

    // Change the other units.
    ResultStateType result{};
    if constexpr (!std::is_same_v<IncomingUnitsWithResultCoordinates, ResultUnits>) {
      result =
          convert_units<ResultVectorType, std::min(INCOMING_DEPTH, RESULT_DEPTH),
                        IncomingUnitsWithResultCoordinates, ResultUnits>(with_result_coordinates);
    } else {
      result = with_result_coordinates;
    }

    return result;
  }
};

}  // namespace ndyn::math
