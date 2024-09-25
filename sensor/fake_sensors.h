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
  using MeasureFn = std::function<ValueType(time::TimeT)>;

 private:
  MeasureFn measure_fn_;
  SensorReadLimiter should_read_sensor_fn_;

  TemperatureMeasurementChannel temperature_{};

  bool should_read_temperature(time::TimeT t) {
    if (should_read_sensor_fn_) {
      return should_read_sensor_fn_(t);
    } else {
      return true;
    }
  }

  void read_temperature(time::TimeT t) { temperature_.set_value(t, measure_fn_(t)); }

 public:
  Sensor(const MeasureFn& measure_fn,
         const SensorReadLimiter& should_read_sensor_fn = SensorReadLimiter{})
      : measure_fn_(measure_fn), should_read_sensor_fn_(should_read_sensor_fn) {}

  void update(time::TimeT t) {
    if (should_read_temperature(t)) {
      read_temperature(t);
    }
  }

  const TemperatureMeasurementChannel& measurement() const { return temperature_; }
};

template <>
class Sensor<SensorSku::FAKE_ACCELEROMETER, BusType::NO_BUS> final {
 public:
  using ValueType = MeasurementValueType<MeasurementType::ACCELEROMETER>::type;
  using MeasureFn = std::function<ValueType(time::TimeT)>;

 private:
  MeasureFn measure_fn_;
  SensorReadLimiter should_read_sensor_fn_;

  AccelerometerMeasurementChannel accelerometer_{};

  bool should_read_accelerometer(time::TimeT t) {
    if (should_read_sensor_fn_) {
      return should_read_sensor_fn_(t);
    } else {
      return true;
    }
  }

  void read_accelerometer(time::TimeT t) { accelerometer_.set_value(t, measure_fn_(t)); }

 public:
  Sensor(const MeasureFn& measure_fn,
         const SensorReadLimiter& should_read_sensor_fn = SensorReadLimiter{})
      : measure_fn_(measure_fn), should_read_sensor_fn_(should_read_sensor_fn) {}

  void update(time::TimeT t) {
    if (should_read_accelerometer(t)) {
      read_accelerometer(t);
    }
  }

  const AccelerometerMeasurementChannel& measurement() const { return accelerometer_; }
};

template <>
class Sensor<SensorSku::FAKE_GYROSCOPE, BusType::NO_BUS> final {
 public:
  using ValueType = MeasurementValueType<MeasurementType::GYROSCOPE>::type;
  using MeasureFn = std::function<ValueType(time::TimeT)>;

 private:
  MeasureFn measure_fn_;
  SensorReadLimiter should_read_sensor_fn_;

  GyroscopeMeasurementChannel gyroscope_{};

  bool should_read_gyroscope(time::TimeT t) {
    if (should_read_sensor_fn_) {
      return should_read_sensor_fn_(t);
    } else {
      return true;
    }
  }

  void read_gyroscope(time::TimeT t) { gyroscope_.set_value(t, measure_fn_(t)); }

 public:
  Sensor(const MeasureFn& measure_fn,
         const SensorReadLimiter& should_read_sensor_fn = SensorReadLimiter{})
      : measure_fn_(measure_fn), should_read_sensor_fn_(should_read_sensor_fn) {}

  void update(time::TimeT t) {
    if (should_read_gyroscope(t)) {
      read_gyroscope(t);
    }
  }

  const GyroscopeMeasurementChannel& measurement() const { return gyroscope_; }
};

}  // namespace ndyn::sensor
