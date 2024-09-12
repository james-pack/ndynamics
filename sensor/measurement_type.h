/**
 * Types for the readings from different types of measurements from sensors. The goal is to
 * centralize the types of the values that are produced when reading from sensors. This
 * centralization allows us to make assumptions downstream about how to consume these data. In
 * particular, this gives structure to the design for sensor fusion.
 */
#pragma once

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
  using type = float;
};

template <>
class MeasurementValueType<MeasurementType::ACCELEROMETER> final {
 public:
  using type = math::Multivector<float, 3, 0, 0>;
};

template <>
class MeasurementValueType<MeasurementType::GYROSCOPE> final {
 public:
  using type = math::Multivector<float, 3, 0, 0>;
};

template <>
class MeasurementValueType<MeasurementType::MAGNETOMETER> final {
 public:
  using type = math::Multivector<float, 3, 0, 0>;
};

}  // namespace ndyn::sensor
