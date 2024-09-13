#include "sensor/imu_bmi323.h"

#include <cstdint>

#include "gtest/gtest.h"
#include "sensor/bus.h"
#include "sensor/fake_bus.h"
#include "sensor/measurement_channel.h"
#include "sensor/read_limiters.h"
#include "sensor/sensor.h"
#include "sensor/sensors.h"

namespace ndyn::sensor {

TEST(ImuBmi323Test, CanCompile) {
  Bus<BusType::FAKE_BUS, uint8_t, uint8_t> bus{};
  Sensor<SensorSku::IMU_BMI323, BusType::FAKE_BUS> bmi323{
      bus, ImuBmi323_AllowedI2CAddress::LOWER_ADDRESS};
  const TemperatureMeasurementChannel& temperature{bmi323.temperature_measurements()};

  EXPECT_FALSE(temperature.has_value_available());
}

TEST(ImuBmi323Test, CanLimitReadsThroughReadRateLimiter) {
  static constexpr time::TimeT TIME_BETWEEN_READS{10};
  Bus<BusType::FAKE_BUS, uint8_t, uint8_t> bus{};
  Sensor<SensorSku::IMU_BMI323, BusType::FAKE_BUS> sensor(  //
      bus, ImuBmi323_AllowedI2CAddress::UPPER_ADDRESS, ReadRateLimiter<TIME_BETWEEN_READS>{},
      ReadRateLimiter<TIME_BETWEEN_READS>{}, ReadRateLimiter<TIME_BETWEEN_READS>{});

  const auto& measurement{sensor.temperature_measurements()};

  // Note that the ReadRateLimiter starts at time zero. Here, we set up the first update to take
  // place before TIME_BETWEEN_READS after time zero so that we know what to expect.
  static constexpr time::TimeT EARLIER_TIME{TIME_BETWEEN_READS - 1};
  static constexpr time::TimeT EXPECTED_TIME{
      static_cast<time::TimeT>(TIME_BETWEEN_READS + EARLIER_TIME + 1)};

  sensor.update(EARLIER_TIME);

  EXPECT_FALSE(measurement.has_value_available());

  sensor.update(EXPECTED_TIME);

  EXPECT_TRUE(measurement.has_value_available());
  EXPECT_EQ(EXPECTED_TIME, measurement.time());
}

}  // namespace ndyn::sensor
