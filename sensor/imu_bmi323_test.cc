#include "sensor/imu_bmi323.h"

#include <cstdint>

#include "gtest/gtest.h"
#include "sensor/bus.h"
#include "sensor/fake_bus.h"
#include "sensor/measurement_channel.h"
#include "sensor/sensor.h"
#include "sensor/sensors.h"

namespace ndyn::sensor {

TEST(ImuBmi323Test, CanCompile) {
  Bus<BusType::FAKE_BUS, uint8_t, uint8_t> bus{};
  Sensor<SensorSku::IMU_BMI323, BusType::FAKE_BUS> bmi323{
      bus, ImuBmi323_AllowedI2CAddress::LOWER_ADDRESS};
  TemperatureMeasurementChannel temperature{bmi323.temperature_measurements()};

  EXPECT_FALSE(temperature.has_value_available());
}

}  // namespace ndyn::sensor
