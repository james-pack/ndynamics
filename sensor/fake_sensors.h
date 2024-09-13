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
  std::function<ValueType(time::TimeT)> measure_fn_;
  SensorReadLimiter should_read_fn_;

  TemperatureMeasurementChannel temperature_{};

  bool should_read_temperature(time::TimeT t) {
    LOG(INFO) << "Sensor::should_read_temperature() -- t: " << t;
    if (should_read_fn_) {
      return should_read_fn_(t);
    } else {
      return true;
    }
  }

  void read_temperature(time::TimeT t) {
    LOG(INFO) << "Sensor::read_temperature() -- t: " << t;
    temperature_.set_value(t, measure_fn_(t));
  }

 public:
  Sensor(std::function<ValueType(time::TimeT)> measure_fn)
      : measure_fn_(std::forward<std::function<ValueType(time::TimeT)>>(measure_fn)) {}

  Sensor(std::function<ValueType(time::TimeT)> measure_fn,
         std::function<bool(time::TimeT)> should_read_fn)
      : measure_fn_(std::forward<std::function<ValueType(time::TimeT)>>(measure_fn)),
        should_read_fn_(std::forward<std::function<bool(time::TimeT)>>(should_read_fn)) {}

  void update(time::TimeT t) {
    LOG(INFO) << "Sensor::update() -- t: " << t;
    if (should_read_temperature(t)) {
      LOG(INFO) << "Sensor::update() -- reading temperature";
      read_temperature(t);
    }
    LOG(INFO) << "Sensor::update() -- exiting";
  }

  const TemperatureMeasurementChannel& measurement() const { return temperature_; }
};

template <>
class Sensor<SensorSku::FAKE_ACCELEROMETER, BusType::NO_BUS> final {
 public:
  using ValueType = MeasurementValueType<MeasurementType::ACCELEROMETER>::type;

 private:
  std::function<ValueType(time::TimeT)> measure_fn_;
  SensorReadLimiter should_read_fn_;

  AccelerometerMeasurementChannel accelerometer_{};

  bool should_read_accelerometer(time::TimeT t) {
    LOG(INFO) << "Sensor::should_read_accelerometer() -- t: " << t;
    if (should_read_fn_) {
      return should_read_fn_(t);
    } else {
      return true;
    }
  }

  void read_accelerometer(time::TimeT t) {
    LOG(INFO) << "Sensor::read_accelerometer() -- t: " << t;
    accelerometer_.set_value(t, measure_fn_(t));
  }

 public:
  Sensor(std::function<ValueType(time::TimeT)> measure_fn)
      : measure_fn_(std::forward<std::function<ValueType(time::TimeT)>>(measure_fn)) {}

  Sensor(std::function<ValueType(time::TimeT)> measure_fn,
         std::function<bool(time::TimeT)> should_read_fn)
      : measure_fn_(std::forward<std::function<ValueType(time::TimeT)>>(measure_fn)),
        should_read_fn_(std::forward<std::function<bool(time::TimeT)>>(should_read_fn)) {}

  void update(time::TimeT t) {
    LOG(INFO) << "Sensor::update() -- t: " << t;
    if (should_read_accelerometer(t)) {
      LOG(INFO) << "Sensor::update() -- reading accelerometer";
      read_accelerometer(t);
    }
    LOG(INFO) << "Sensor::update() -- exiting";
  }

  const AccelerometerMeasurementChannel& measurement() const { return accelerometer_; }
};

template <>
class Sensor<SensorSku::FAKE_GYROSCOPE, BusType::NO_BUS> final {
 public:
  using ValueType = MeasurementValueType<MeasurementType::GYROSCOPE>::type;

 private:
  std::function<ValueType(time::TimeT)> measure_fn_;
  SensorReadLimiter should_read_fn_;

  GyroscopeMeasurementChannel gyroscope_{};

  bool should_read_gyroscope(time::TimeT t) {
    LOG(INFO) << "Sensor::should_read_gyroscope() -- t: " << t;
    if (should_read_fn_) {
      return should_read_fn_(t);
    } else {
      return true;
    }
  }

  void read_gyroscope(time::TimeT t) {
    LOG(INFO) << "Sensor::read_gyroscope() -- t: " << t;
    gyroscope_.set_value(t, measure_fn_(t));
  }

 public:
  Sensor(std::function<ValueType(time::TimeT)> measure_fn)
      : measure_fn_(std::forward<std::function<ValueType(time::TimeT)>>(measure_fn)) {}

  Sensor(std::function<ValueType(time::TimeT)> measure_fn,
         std::function<bool(time::TimeT)> should_read_fn)
      : measure_fn_(std::forward<std::function<ValueType(time::TimeT)>>(measure_fn)),
        should_read_fn_(std::forward<std::function<bool(time::TimeT)>>(should_read_fn)) {}

  void update(time::TimeT t) {
    LOG(INFO) << "Sensor::update() -- t: " << t;
    if (should_read_gyroscope(t)) {
      LOG(INFO) << "Sensor::update() -- reading gyroscope";
      read_gyroscope(t);
    }
    LOG(INFO) << "Sensor::update() -- exiting";
  }

  const GyroscopeMeasurementChannel& measurement() const { return gyroscope_; }
};

}  // namespace ndyn::sensor
