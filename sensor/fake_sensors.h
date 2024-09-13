#pragma once

#include <cstdint>
#include <functional>
#include <memory>

#include "glog/logging.h"
#include "sensor/bus.h"
#include "sensor/measurement_channel.h"
#include "sensor/measurement_type.h"
#include "sensor/sensor.h"
#include "sensor/sensors.h"
#include "time/time.h"

namespace ndyn::sensor {

template <>
class Sensor<SensorSku::FAKE_TEMPERATURE_SENSOR, BusType::NO_BUS> final {
 public:
  using ValueType = MeasurementValueType<MeasurementType::TEMPERATURE>::type;

 private:
  std::function<ValueType(time::TimeT)> binding_fn_;

  TemperatureMeasurementChannel temperature_{};

  bool should_read_temperature(time::TimeT t) {
    LOG(INFO) << "Sensor::should_read_temperature() -- t: " << t;
    return true;
  }

  void read_temperature(time::TimeT t) {
    LOG(INFO) << "Sensor::read_temperature() -- t: " << t;
    temperature_.set_value(t, binding_fn_(t));
  }

 public:
  Sensor(std::function<ValueType(time::TimeT)> binding_fn)
      : binding_fn_(std::forward<std::function<ValueType(time::TimeT)>>(binding_fn)) {}

  void update(time::TimeT t) {
    LOG(INFO) << "Sensor::update() -- t: " << t;
    if (should_read_temperature(t)) {
      LOG(INFO) << "Sensor::update() -- reading temperature";
      read_temperature(t);
    }
    LOG(INFO) << "Sensor::update() -- exiting";
  }

  const TemperatureMeasurementChannel& temperature_measurements() const { return temperature_; }
};

template <>
class Sensor<SensorSku::FAKE_ACCELEROMETER, BusType::NO_BUS> final {
 public:
  using ValueType = MeasurementValueType<MeasurementType::ACCELEROMETER>::type;

 private:
  std::function<ValueType(time::TimeT)> binding_fn_;

  AccelerometerMeasurementChannel accelerometer_{};

  bool should_read_accelerometer(time::TimeT t) {
    LOG(INFO) << "Sensor::should_read_accelerometer() -- t: " << t;
    return true;
  }

  void read_accelerometer(time::TimeT t) {
    LOG(INFO) << "Sensor::read_accelerometer() -- t: " << t;
    accelerometer_.set_value(t, binding_fn_(t));
  }

 public:
  Sensor(std::function<ValueType(time::TimeT)> binding_fn)
      : binding_fn_(std::forward<std::function<ValueType(time::TimeT)>>(binding_fn)) {}

  void update(time::TimeT t) {
    LOG(INFO) << "Sensor::update() -- t: " << t;
    if (should_read_accelerometer(t)) {
      LOG(INFO) << "Sensor::update() -- reading accelerometer";
      read_accelerometer(t);
    }
    LOG(INFO) << "Sensor::update() -- exiting";
  }

  const AccelerometerMeasurementChannel& accelerometer_measurements() const {
    return accelerometer_;
  }
};

template <>
class Sensor<SensorSku::FAKE_GYROSCOPE, BusType::NO_BUS> final {
 public:
  using ValueType = MeasurementValueType<MeasurementType::GYROSCOPE>::type;

 private:
  std::function<ValueType(time::TimeT)> binding_fn_;

  GyroscopeMeasurementChannel gyroscope_{};

  bool should_read_gyroscope(time::TimeT t) {
    LOG(INFO) << "Sensor::should_read_gyroscope() -- t: " << t;
    return true;
  }

  void read_gyroscope(time::TimeT t) {
    LOG(INFO) << "Sensor::read_gyroscope() -- t: " << t;
    gyroscope_.set_value(t, binding_fn_(t));
  }

 public:
  Sensor(std::function<ValueType(time::TimeT)> binding_fn)
      : binding_fn_(std::forward<std::function<ValueType(time::TimeT)>>(binding_fn)) {}

  void update(time::TimeT t) {
    LOG(INFO) << "Sensor::update() -- t: " << t;
    if (should_read_gyroscope(t)) {
      LOG(INFO) << "Sensor::update() -- reading gyroscope";
      read_gyroscope(t);
    }
    LOG(INFO) << "Sensor::update() -- exiting";
  }

  const GyroscopeMeasurementChannel& gyroscope_measurements() const { return gyroscope_; }
};

}  // namespace ndyn::sensor
