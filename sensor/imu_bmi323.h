#pragma once

#include <cstdint>
#include <memory>

#include "sensor/bus.h"
#include "sensor/measurement_channel.h"
#include "sensor/measurement_type.h"
#include "sensor/sensor.h"
#include "sensor/sensors.h"

namespace ndyn::sensor {

enum class ImuBmi323_AllowedI2CAddress : uint8_t {
  LOWER_ADDRESS = 0x68,
  UPPER_ADDRESS = 0x69,
};

template <BusType BUS_TYPE>
class Sensor<SensorSku::IMU_BMI323, BUS_TYPE> final {
 private:
  Bus<BUS_TYPE, uint8_t, uint8_t>* bus_;

  TemperatureMeasurementChannel temperature_{};
  AccelerometerMeasurementChannel accelerometer_{};
  GyroscopeMeasurementChannel gyroscope_{};

 public:
  constexpr Sensor(Bus<BUS_TYPE, uint8_t, uint8_t>& bus, ImuBmi323_AllowedI2CAddress i2c_address)
      : bus_(&bus) {}

  constexpr const TemperatureMeasurementChannel& temperature_measurements() const {
    return temperature_;
  }

  constexpr const AccelerometerMeasurementChannel& accelerometer_measurements() const {
    return accelerometer_;
  }

  constexpr const GyroscopeMeasurementChannel& gyroscope_measurements() const { return gyroscope_; }
};

}  // namespace ndyn::sensor
