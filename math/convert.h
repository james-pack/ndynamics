#pragma once

#include <cmath>
#include <type_traits>

#include "math/coordinates.h"
#include "math/multivector.h"
#include "math/multivector_utils.h"
#include "math/state.h"
#include "math/unit_set.h"

namespace ndyn::math {

template <typename IncomingStateType, typename ResultStateType>
ResultStateType convert_spherical_to_cartesian(const IncomingStateType& incoming) {
  using std::acos;
  using std::atan2;
  using std::cos;
  using std::hypot;
  using std::sin;

  using IncomingVectorType = typename IncomingStateType::VectorType;
  using VectorType = typename ResultStateType::VectorType;
  using ScalarType = typename ResultStateType::ScalarType;

  static constexpr size_t CONVERSION_DEPTH{
      std::min(IncomingStateType::depth(), ResultStateType::depth())};

  static_assert(
      CONVERSION_DEPTH < 4,
      "Spherical to Cartesian coordinate conversions are not implemented to the requested depth");

  ResultStateType result{};
  if constexpr (CONVERSION_DEPTH >= 1) {
    const IncomingVectorType& in0{incoming.template element<0>()};
    const ScalarType x0{in0.r() * cos(in0.phi()) * sin(in0.theta())};
    const ScalarType y0{in0.r() * sin(in0.phi()) * sin(in0.theta())};
    const ScalarType z0{in0.r() * cos(in0.theta())};

    result.template set_element<0>(x0 * VectorType::template e<0>() +
                                   y0 * VectorType::template e<1>() +
                                   z0 * VectorType::template e<2>());

    if constexpr (CONVERSION_DEPTH >= 2) {
      const IncomingVectorType& in1{incoming.template element<1>()};

      const ScalarType x1{in1.r() * cos(in0.phi()) * sin(in0.theta()) -
                          in0.r() * sin(in0.phi()) * in1.phi() * sin(in0.theta()) +
                          in0.r() * cos(in0.phi()) * cos(in0.theta()) * in1.theta()};

      const ScalarType y1{in1.r() * sin(in0.phi()) * sin(in0.theta()) +
                          in0.r() * cos(in0.phi()) * in1.phi() * sin(in0.theta()) +
                          in0.r() * sin(in0.phi()) * cos(in0.theta()) * in1.theta()};

      const ScalarType z1{in1.r() * cos(in0.theta()) - in0.r() * sin(in0.theta()) * in1.theta()};

      result.template set_element<1>(x1 * VectorType::template e<0>() +
                                     y1 * VectorType::template e<1>() +
                                     z1 * VectorType::template e<2>());

      if constexpr (CONVERSION_DEPTH >= 3) {
        const IncomingVectorType& in2{incoming.template element<2>()};

        const ScalarType x2{
            in2.r() * cos(in0.phi()) * sin(in0.theta()) -
            in1.r() * sin(in0.phi()) * in1.phi() * sin(in0.theta()) +
            in1.r() * cos(in0.phi()) * cos(in0.theta()) * in1.theta() +

            -(in1.r() * sin(in0.phi()) * in1.phi() * sin(in0.theta()) +
              in0.r() * cos(in0.phi()) * in1.phi() * in1.phi() * sin(in0.theta()) +
              in0.r() * sin(in0.phi()) * in2.phi() * sin(in0.theta()) +
              in0.r() * sin(in0.phi()) * in1.phi() * cos(in0.theta()) * in1.theta()) +

            in1.r() * cos(in0.phi()) * cos(in0.theta()) * in1.theta() -
            in0.r() * sin(in0.phi()) * in1.phi() * cos(in0.theta()) * in1.theta() -
            in0.r() * cos(in0.phi()) * sin(in0.theta()) * in1.theta() * in1.theta() +
            in0.r() * cos(in0.phi()) * cos(in0.theta()) * in2.theta()

        };

        const ScalarType y2{
            in2.r() * sin(in0.phi()) * sin(in0.theta()) +
            in1.r() * cos(in0.phi()) * in1.phi() * sin(in0.theta()) +
            in1.r() * sin(in0.phi()) * cos(in0.theta()) * in1.theta() +

            in1.r() * cos(in0.phi()) * in1.phi() * sin(in0.theta()) +
            in0.r() * sin(in0.phi()) * in1.phi() * in1.phi() * sin(in0.theta()) -
            in0.r() * cos(in0.phi()) * in2.phi() * sin(in0.theta()) +
            in0.r() * cos(in0.phi()) * in1.phi() * cos(in0.theta()) * in1.theta() +

            in1.r() * sin(in0.phi()) * cos(in0.theta()) * in1.theta() +
            in0.r() * cos(in0.phi()) * in1.phi() * cos(in0.theta()) * in1.theta() -
            in0.r() * sin(in0.phi()) * sin(in0.theta()) * in1.theta() * in1.theta() +
            in0.r() * sin(in0.phi()) * cos(in0.theta()) * in2.theta()

        };

        const ScalarType z2{in2.r() * cos(in0.theta())                                   //
                            - in1.r() * sin(in0.theta()) * in1.theta()                   //
                            - (in1.r() * sin(in0.theta()) * in1.theta() +                //
                               in0.r() * cos(in0.theta()) * in1.theta() * in1.theta() +  //
                               in0.r() * sin(in0.theta()) * in2.theta())};

        result.template set_element<2>(x2 * VectorType::template e<0>() +
                                       y2 * VectorType::template e<1>() +
                                       z2 * VectorType::template e<2>());
      }
    }
  }
  return result;
}

template <typename IncomingStateType, typename ResultStateType>
ResultStateType convert_cartesian_to_spherical(const IncomingStateType& incoming) {
  using std::acos;
  using std::atan2;
  using std::cos;
  using std::hypot;
  using std::sin;

  using IncomingVectorType = typename IncomingStateType::VectorType;
  using VectorType = typename ResultStateType::VectorType;
  using ScalarType = typename ResultStateType::ScalarType;

  static constexpr size_t CONVERSION_DEPTH{
      std::min(IncomingStateType::depth(), ResultStateType::depth())};

  static_assert(CONVERSION_DEPTH < 4,
                "Coordinate conversions are not implemented beyond a state depth of 3");

  ResultStateType result{};
  if constexpr (CONVERSION_DEPTH >= 1) {
    const IncomingVectorType& incoming_vector{incoming.template element<0>()};
    const ScalarType r{hypot(incoming_vector.x(), incoming_vector.y(), incoming_vector.z())};
    const ScalarType phi{atan2(incoming_vector.y(), incoming_vector.x())};
    const ScalarType theta{acos(incoming_vector.z() / r)};
    result.template set_element<0>(VectorType{r, theta, phi});

    if constexpr (CONVERSION_DEPTH >= 2) {
      // TODO(james): Handle derivative cases.

      if constexpr (CONVERSION_DEPTH >= 3) {
        // TODO(james): Handle derivative cases.
      }
    }
  }
  return result;
}

template <typename IncomingStateType, typename ResultStateType>
ResultStateType convert_cartesian_to_polar(const IncomingStateType& incoming) {
  using std::acos;
  using std::atan2;
  using std::cos;
  using std::hypot;
  using std::sin;

  using IncomingVectorType = typename IncomingStateType::VectorType;
  using VectorType = typename ResultStateType::VectorType;
  using ScalarType = typename ResultStateType::ScalarType;

  static constexpr size_t CONVERSION_DEPTH{
      std::min(IncomingStateType::depth(), ResultStateType::depth())};

  static_assert(CONVERSION_DEPTH < 4,
                "Coordinate conversions are not implemented beyond a state depth of 3");

  ResultStateType result{};
  if constexpr (CONVERSION_DEPTH >= 1) {
    const IncomingVectorType& incoming_vector{incoming.template element<0>()};
    const ScalarType r{hypot(incoming_vector.x(), incoming_vector.y())};
    const ScalarType theta{atan2(incoming_vector.y(), incoming_vector.x())};
    result.template set_element<0>(VectorType{r, theta});

    if constexpr (CONVERSION_DEPTH >= 2) {
      // TODO(james): Handle derivative cases.

      if constexpr (CONVERSION_DEPTH >= 3) {
        // TODO(james): Handle derivative cases.
      }
    }
  }
  return result;
}

template <typename IncomingStateType, typename ResultStateType>
ResultStateType convert_polar_to_cartesian(const IncomingStateType& incoming) {
  using std::cos;
  using std::sin;

  using IncomingVectorType = typename IncomingStateType::VectorType;
  using VectorType = typename ResultStateType::VectorType;
  using ScalarType = typename ResultStateType::ScalarType;

  static constexpr size_t CONVERSION_DEPTH{
      std::min(IncomingStateType::depth(), ResultStateType::depth())};

  static_assert(CONVERSION_DEPTH < 4,
                "Coordinate conversions are not implemented beyond a state depth of 3");

  ResultStateType result{};
  if constexpr (CONVERSION_DEPTH >= 1) {
    const IncomingVectorType& incoming_0{incoming.template element<0>()};
    const ScalarType x0{incoming_0.r() * cos(incoming_0.theta())};
    const ScalarType y0{incoming_0.r() * sin(incoming_0.theta())};
    result.template set_element<0>(VectorType{x0, y0});

    if constexpr (CONVERSION_DEPTH >= 2) {
      const IncomingVectorType& incoming_1{incoming.template element<1>()};

      const ScalarType x1{incoming_1.r() * cos(incoming_0.theta()) -
                          incoming_0.r() * sin(incoming_0.theta()) * incoming_1.theta()};

      const ScalarType y1{incoming_1.r() * sin(incoming_0.theta()) +
                          incoming_0.r() * cos(incoming_0.theta()) * incoming_1.theta()};

      result.template set_element<1>(VectorType{x1, y1});

      if constexpr (CONVERSION_DEPTH >= 3) {
        const IncomingVectorType& incoming_2{incoming.template element<2>()};

        const ScalarType x2{
            incoming_2.r() * cos(incoming_0.theta()) -
            incoming_1.r() * sin(incoming_0.theta()) * incoming_1.theta() -
            (incoming_1.r() * sin(incoming_0.theta()) * incoming_1.theta() +
             incoming_0.r() * cos(incoming_0.theta()) * incoming_1.theta() * incoming_1.theta() +
             incoming_0.r() * sin(incoming_0.theta()) * incoming_2.theta())};

        const ScalarType y2{incoming_2.r() * sin(incoming_0.theta()) +
                            incoming_1.r() * cos(incoming_0.theta()) * incoming_1.theta() +
                            incoming_1.r() * cos(incoming_0.theta()) * incoming_1.theta() -
                            incoming_0.r() * sin(incoming_0.theta()) * incoming_1.theta() *
                                incoming_1.theta() +
                            incoming_0.r() * cos(incoming_0.theta()) * incoming_2.theta()};

        result.template set_element<2>(VectorType{x2, y2});
      }
    }
  }

  return result;
}

template <typename IncomingStateType, typename ResultStateType>
ResultStateType convert_polar_to_spherical(const IncomingStateType& incoming) {
  using std::acos;
  using std::atan2;
  using std::cos;
  using std::hypot;
  using std::sin;

  using IncomingVectorType = typename IncomingStateType::VectorType;
  using VectorType = typename ResultStateType::VectorType;
  using ScalarType = typename ResultStateType::ScalarType;

  static constexpr size_t CONVERSION_DEPTH{
      std::min(IncomingStateType::depth(), ResultStateType::depth())};

  static_assert(CONVERSION_DEPTH < 4,
                "Coordinate conversions are not implemented beyond a state depth of 3");

  ResultStateType result{};
  return result;
}

template <typename IncomingStateType, typename ResultStateType>
ResultStateType convert_spherical_to_polar(const IncomingStateType& incoming) {
  using std::acos;
  using std::atan2;
  using std::cos;
  using std::hypot;
  using std::sin;

  using IncomingVectorType = typename IncomingStateType::VectorType;
  using VectorType = typename ResultStateType::VectorType;
  using ScalarType = typename ResultStateType::ScalarType;

  static constexpr size_t CONVERSION_DEPTH{
      std::min(IncomingStateType::depth(), ResultStateType::depth())};

  static_assert(CONVERSION_DEPTH < 4,
                "Coordinate conversions are not implemented beyond a state depth of 3");

  ResultStateType result{};
  return result;
}

/**
 * Conversion that converts the coordinates of a state.
 */
template <Coordinates INCOMING_COORDINATES, Coordinates RESULT_COORDINATES,
          typename IncomingStateType, typename ResultStateType>
ResultStateType convert_coordinates(const IncomingStateType& incoming) {
  // Assert that we have an implementation for the requested incoming and result coordinates.
  static_assert(INCOMING_COORDINATES == Coordinates::CARTESIAN ||
                    INCOMING_COORDINATES == Coordinates::POLAR ||
                    INCOMING_COORDINATES == Coordinates::SPHERICAL,
                "Incoming coordinates not implemented");
  static_assert(RESULT_COORDINATES == Coordinates::CARTESIAN ||
                    RESULT_COORDINATES == Coordinates::POLAR ||
                    RESULT_COORDINATES == Coordinates::SPHERICAL,
                "Result coordinates not implemented");

  ResultStateType result{};

  if constexpr (INCOMING_COORDINATES == Coordinates::CARTESIAN) {
    if constexpr (RESULT_COORDINATES == Coordinates::POLAR) {
      return convert_cartesian_to_polar<IncomingStateType, ResultStateType>(incoming);
    }

    if constexpr (RESULT_COORDINATES == Coordinates::SPHERICAL) {
      return convert_cartesian_to_spherical<IncomingStateType, ResultStateType>(incoming);
    }
  }

  if constexpr (INCOMING_COORDINATES == Coordinates::POLAR) {
    if constexpr (RESULT_COORDINATES == Coordinates::CARTESIAN) {
      return convert_polar_to_cartesian<IncomingStateType, ResultStateType>(incoming);
    }

    if constexpr (RESULT_COORDINATES == Coordinates::SPHERICAL) {
      return convert_polar_to_spherical<IncomingStateType, ResultStateType>(incoming);
    }
  }

  if constexpr (INCOMING_COORDINATES == Coordinates::SPHERICAL) {
    if constexpr (RESULT_COORDINATES == Coordinates::CARTESIAN) {
      return convert_spherical_to_cartesian<IncomingStateType, ResultStateType>(incoming);
    }

    if constexpr (RESULT_COORDINATES == Coordinates::POLAR) {
      return convert_spherical_to_polar<IncomingStateType, ResultStateType>(incoming);
    }
  }

  return result;
}

template <typename IncomingVectorType, typename ResultVectorType>
ResultVectorType convert_vector_spherical_to_cartesian(const IncomingVectorType& incoming) {
  using std::cos;
  using std::sin;

  using ScalarType = typename ResultVectorType::ScalarType;

  const ScalarType x{incoming.r() * cos(incoming.phi()) * sin(incoming.theta())};
  const ScalarType y{incoming.r() * sin(incoming.phi()) * sin(incoming.theta())};
  const ScalarType z{incoming.r() * cos(incoming.theta())};

  return ResultVectorType{x, y, z};
}

template <typename IncomingVectorType, typename ResultVectorType>
ResultVectorType convert_vector_cartesian_to_spherical(const IncomingVectorType& incoming) {
  using std::acos;
  using std::atan2;
  using std::hypot;

  using ScalarType = typename ResultVectorType::ScalarType;

  const ScalarType r{hypot(incoming.x(), incoming.y(), incoming.z())};
  const ScalarType phi{atan2(incoming.y(), incoming.x())};
  const ScalarType theta{acos(incoming.z() / r)};

  return ResultVectorType{r, theta, phi};
}

template <typename IncomingVectorType, typename ResultVectorType>
ResultVectorType convert_vector_cartesian_to_polar(const IncomingVectorType& incoming) {
  using std::atan2;
  using std::hypot;

  using ScalarType = typename ResultVectorType::ScalarType;

  const ScalarType r{hypot(incoming.x(), incoming.y())};
  const ScalarType theta{atan2(incoming.y(), incoming.x())};

  return ResultVectorType{r, theta};
}

template <typename IncomingVectorType, typename ResultVectorType>
ResultVectorType convert_vector_polar_to_cartesian(const IncomingVectorType& incoming) {
  using std::cos;
  using std::sin;

  using ScalarType = typename ResultVectorType::ScalarType;

  const ScalarType x{incoming.r() * cos(incoming.theta())};
  const ScalarType y{incoming.r() * sin(incoming.theta())};

  return ResultVectorType{x, y};
}

template <typename IncomingVectorType, typename ResultVectorType>
ResultVectorType convert_vector_polar_to_spherical(const IncomingVectorType& incoming) {
  return ResultVectorType{incoming.r(), pi / 2, incoming.theta()};
}

template <typename IncomingVectorType, typename ResultVectorType>
ResultVectorType convert_vector_spherical_to_polar(const IncomingVectorType& incoming) {
  return ResultVectorType{incoming.r(), incoming.phi()};
}

/**
 * Conversion that converts the coordinates of a single vector.
 */
template <Coordinates INCOMING_COORDINATES, Coordinates RESULT_COORDINATES,
          typename IncomingVectorType, typename ResultVectorType>
ResultVectorType convert_vector_coordinates(const IncomingVectorType& incoming) {
  // Assert that we have an implementation for the requested incoming and result coordinates.
  static_assert(INCOMING_COORDINATES == Coordinates::CARTESIAN ||
                    INCOMING_COORDINATES == Coordinates::POLAR ||
                    INCOMING_COORDINATES == Coordinates::SPHERICAL,
                "Incoming coordinates not implemented");
  static_assert(RESULT_COORDINATES == Coordinates::CARTESIAN ||
                    RESULT_COORDINATES == Coordinates::POLAR ||
                    RESULT_COORDINATES == Coordinates::SPHERICAL,
                "Result coordinates not implemented");

  ResultVectorType result{};

  if constexpr (INCOMING_COORDINATES == Coordinates::CARTESIAN) {
    if constexpr (RESULT_COORDINATES == Coordinates::POLAR) {
      return convert_vector_cartesian_to_polar<IncomingVectorType, ResultVectorType>(incoming);
    }

    if constexpr (RESULT_COORDINATES == Coordinates::SPHERICAL) {
      return convert_vector_cartesian_to_spherical<IncomingVectorType, ResultVectorType>(incoming);
    }
  }

  if constexpr (INCOMING_COORDINATES == Coordinates::POLAR) {
    if constexpr (RESULT_COORDINATES == Coordinates::CARTESIAN) {
      return convert_vector_polar_to_cartesian<IncomingVectorType, ResultVectorType>(incoming);
    }

    if constexpr (RESULT_COORDINATES == Coordinates::SPHERICAL) {
      return convert_vector_polar_to_spherical<IncomingVectorType, ResultVectorType>(incoming);
    }
  }

  if constexpr (INCOMING_COORDINATES == Coordinates::SPHERICAL) {
    if constexpr (RESULT_COORDINATES == Coordinates::CARTESIAN) {
      return convert_vector_spherical_to_cartesian<IncomingVectorType, ResultVectorType>(incoming);
    }

    if constexpr (RESULT_COORDINATES == Coordinates::POLAR) {
      return convert_vector_spherical_to_polar<IncomingVectorType, ResultVectorType>(incoming);
    }
  }

  return result;
}

}  // namespace ndyn::math
