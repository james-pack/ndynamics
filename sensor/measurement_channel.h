#pragma once

#include <utility>

#include "sensor/measurement_type.h"
#include "time/time.h"

namespace ndyn::sensor {

template <MeasurementType TYPE>
class MeasurementChannel final {
 public:
  using ValueType = typename MeasurementValueType<TYPE>::type;

 private:
  time::TimeT time_{};
  ValueType value_{};
  mutable bool is_ready_{};

 public:
  static constexpr MeasurementType type() { return TYPE; }

  constexpr bool has_value_available() const { return is_ready_; }

  constexpr time::TimeT time() const { return time_; }

  constexpr const ValueType& value() const {
    is_ready_ = false;
    return value_;
  }

  constexpr std::pair<time::TimeT, ValueType> current() const {
    is_ready_ = false;
    return {time_, value_};
  }

  constexpr void set_value(time::TimeT time, ValueType value) {
    time_ = time;
    value_ = value;
    is_ready_ = true;
  }
};

using TemperatureMeasurementChannel = MeasurementChannel<MeasurementType::TEMPERATURE>;
using AccelerometerMeasurementChannel = MeasurementChannel<MeasurementType::ACCELEROMETER>;
using GyroscopeMeasurementChannel = MeasurementChannel<MeasurementType::GYROSCOPE>;
using MagnetometerMeasurementChannel = MeasurementChannel<MeasurementType::MAGNETOMETER>;

}  // namespace ndyn::sensor
