/**
 * Types for the readings from different types of measurements from sensors. The goal is to
 * centralize the types of the values that are produced when reading from sensors. This
 * centralization allows us to make assumptions downstream about how to consume these data. In
 * particular, this gives structure to the design for sensor fusion.
 */
#pragma once

#include "math/algebra.h"
#include "math/multivector.h"

namespace ndyn::sensor {

enum class MeasurementType : uint8_t {
  TEMPERATURE,
  ACCELEROMETER,
  GYROSCOPE,
  MAGNETOMETER,
};

template <MeasurementType TYPE>
class MeasurementValueType final {};

template <>
class MeasurementValueType<MeasurementType::TEMPERATURE> final {
 public:
  using scalar_type = float;
  using type = scalar_type;
};

template <>
class MeasurementValueType<MeasurementType::ACCELEROMETER> final {
 public:
  using scalar_type = float;
  using type = typename math::Algebra<scalar_type, 3, 0, 0>::VectorType;
};

template <>
class MeasurementValueType<MeasurementType::GYROSCOPE> final {
 public:
  using scalar_type = float;
  using type = typename math::Algebra<scalar_type, 3, 0, 0>::VectorType;
};

template <>
class MeasurementValueType<MeasurementType::MAGNETOMETER> final {
 public:
  using scalar_type = float;
  using type = typename math::Algebra<scalar_type, 3, 0, 0>::VectorType;
};

}  // namespace ndyn::sensor
