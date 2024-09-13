#pragma once

#include <cstdint>
#include <memory>

#include "sensor/bus.h"
#include "sensor/measurement_channel.h"
#include "sensor/measurement_type.h"
#include "sensor/sensor.h"
#include "sensor/sensors.h"
#include "time/time.h"

namespace ndyn::sensor {

enum class ImuBmi323_AllowedI2CAddress : uint8_t {
  LOWER_ADDRESS = 0x68,
  UPPER_ADDRESS = 0x69,
};

template <BusType BUS_TYPE>
class Sensor<SensorSku::IMU_BMI323, BUS_TYPE> final {
 public:
  using AddressType = uint8_t;
  using DeviceAddressType = uint8_t;

 private:
  Bus<BUS_TYPE, AddressType, DeviceAddressType>* bus_;
  DeviceAddressType device_address_;

  SensorReadLimiter temperature_read_limiter_{};
  SensorReadLimiter accelerometer_read_limiter_{};
  SensorReadLimiter gyroscope_read_limiter_{};

  TemperatureMeasurementChannel temperature_{};
  AccelerometerMeasurementChannel accelerometer_{};
  GyroscopeMeasurementChannel gyroscope_{};

  bool should_read_temperature(time::TimeT t) {
    if (temperature_read_limiter_) {
      return temperature_read_limiter_(t);
    } else {
      return true;
    }
  }

  bool should_read_accelerometer(time::TimeT t) {
    if (accelerometer_read_limiter_) {
      return accelerometer_read_limiter_(t);
    } else {
      return true;
    }
  }

  bool should_read_gyroscope(time::TimeT t) {
    if (gyroscope_read_limiter_) {
      return gyroscope_read_limiter_(t);
    } else {
      return true;
    }
  }

  void read_temperature(time::TimeT t) {
    // For details on this implementation, see page 73 of the datasheet:
    // https://www.bosch-sensortec.com/media/boschsensortec/downloads/datasheets/bst-bmi323-ds000.pdf
    using ValueType = TemperatureMeasurementChannel::ValueType;
    static constexpr AddressType REGISTER_TEMPERATURE{0x09};
    uint16_t read_value{};
    {
      auto transmission{bus_->initiate(device_address_)};
      transmission.read(REGISTER_TEMPERATURE, read_value);
    }
    ValueType temperature = read_value / static_cast<ValueType>(512) + static_cast<ValueType>(23);
    temperature_.set_value(t, temperature);
  }

  void read_accelerometer(time::TimeT t) {
    // For details on this implementation, see pages 67-69 of the datasheet:
    // https://www.bosch-sensortec.com/media/boschsensortec/downloads/datasheets/bst-bmi323-ds000.pdf
    using ValueType = AccelerometerMeasurementChannel::ValueType;
    using ScalarType = AccelerometerMeasurementChannel::ScalarType;
    static constexpr AddressType REGISTER_ACCELEROMETER_X{0x03};
    static constexpr AddressType REGISTER_ACCELEROMETER_Y{0x04};
    static constexpr AddressType REGISTER_ACCELEROMETER_Z{0x05};

    uint16_t read_value_x{};
    uint16_t read_value_y{};
    uint16_t read_value_z{};
    {
      auto transmission{bus_->initiate(device_address_)};
      transmission.read(REGISTER_ACCELEROMETER_X, read_value_x);
      transmission.read(REGISTER_ACCELEROMETER_Y, read_value_y);
      transmission.read(REGISTER_ACCELEROMETER_Z, read_value_z);
    }
    ValueType accelerometer = {static_cast<ScalarType>(read_value_x),
                               static_cast<ScalarType>(read_value_y),
                               static_cast<ScalarType>(read_value_z)};
    accelerometer_.set_value(t, accelerometer);
  }

  void read_gyroscope(time::TimeT t) {
    // For details on this implementation, see pages 70-72 of the datasheet:
    // https://www.bosch-sensortec.com/media/boschsensortec/downloads/datasheets/bst-bmi323-ds000.pdf
    using ValueType = GyroscopeMeasurementChannel::ValueType;
    using ScalarType = GyroscopeMeasurementChannel::ScalarType;
    static constexpr AddressType REGISTER_GYROSCOPE_X{0x06};
    static constexpr AddressType REGISTER_GYROSCOPE_Y{0x07};
    static constexpr AddressType REGISTER_GYROSCOPE_Z{0x08};

    uint16_t read_value_x{};
    uint16_t read_value_y{};
    uint16_t read_value_z{};
    {
      auto transmission{bus_->initiate(device_address_)};
      transmission.read(REGISTER_GYROSCOPE_X, read_value_x);
      transmission.read(REGISTER_GYROSCOPE_Y, read_value_y);
      transmission.read(REGISTER_GYROSCOPE_Z, read_value_z);
    }
    ValueType gyroscope = {static_cast<ScalarType>(read_value_x),
                           static_cast<ScalarType>(read_value_y),
                           static_cast<ScalarType>(read_value_z)};
    gyroscope_.set_value(t, gyroscope);
  }

 public:
  Sensor(Bus<BUS_TYPE, AddressType, DeviceAddressType>& bus,
         ImuBmi323_AllowedI2CAddress i2c_address)
      : bus_(&bus), device_address_(static_cast<DeviceAddressType>(i2c_address)) {}

  Sensor(Bus<BUS_TYPE, AddressType, DeviceAddressType>& bus,
         ImuBmi323_AllowedI2CAddress i2c_address, SensorReadLimiter temperature_read_limiter,
         SensorReadLimiter accelerometer_read_limiter, SensorReadLimiter gyroscope_read_limiter)
      : bus_(&bus),
        device_address_(static_cast<DeviceAddressType>(i2c_address)),
        temperature_read_limiter_(temperature_read_limiter),
        accelerometer_read_limiter_(accelerometer_read_limiter),
        gyroscope_read_limiter_(gyroscope_read_limiter) {}

  void update(time::TimeT t) {
    if (should_read_temperature(t)) {
      read_temperature(t);
    } else if (should_read_accelerometer(t)) {
      read_accelerometer(t);
    } else if (should_read_gyroscope(t)) {
      read_gyroscope(t);
    }
  }

  const TemperatureMeasurementChannel& temperature_measurements() const { return temperature_; }

  const AccelerometerMeasurementChannel& accelerometer_measurements() const {
    return accelerometer_;
  }

  const GyroscopeMeasurementChannel& gyroscope_measurements() const { return gyroscope_; }
};

}  // namespace ndyn::sensor
