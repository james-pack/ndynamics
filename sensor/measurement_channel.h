#pragma once

#include <utility>

#include "glog/logging.h"
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
  constexpr MeasurementChannel() = default;
  constexpr MeasurementChannel(MeasurementChannel&&) = default;
  constexpr MeasurementChannel& operator=(MeasurementChannel&&) = default;

  constexpr MeasurementChannel(const MeasurementChannel&) = delete;
  constexpr MeasurementChannel& operator=(const MeasurementChannel&) = delete;

  static constexpr MeasurementType type() { return TYPE; }

  constexpr bool has_value_available() const {
    LOG(INFO) << "MeasurementChannel::has_value_available() -- is_ready_: " << is_ready_
              << ", time_: " << time_ << ", value_: " << value_;
    return is_ready_;
  }

  constexpr time::TimeT time() const { return time_; }

  constexpr const ValueType& value() const {
    is_ready_ = false;
    return value_;
  }

  constexpr std::pair<time::TimeT, ValueType> current() const {
    is_ready_ = false;
    return {time_, value_};
  }

  constexpr void set_value(time::TimeT t, ValueType value) {
    LOG(INFO) << "MeasurementChannel::set_value() -- t: " << t << ", value: " << value;
    time_ = t;
    value_ = value;
    is_ready_ = true;
    LOG(INFO) << "MeasurementChannel::set_value() -- is_ready_: " << is_ready_
              << ", time_: " << time_ << ", value_: " << value_;
  }
};

using TemperatureMeasurementChannel = MeasurementChannel<MeasurementType::TEMPERATURE>;
using AccelerometerMeasurementChannel = MeasurementChannel<MeasurementType::ACCELEROMETER>;
using GyroscopeMeasurementChannel = MeasurementChannel<MeasurementType::GYROSCOPE>;
using MagnetometerMeasurementChannel = MeasurementChannel<MeasurementType::MAGNETOMETER>;

}  // namespace ndyn::sensor
