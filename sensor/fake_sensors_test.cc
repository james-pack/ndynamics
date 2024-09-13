#include "sensor/fake_sensors.h"

#include <cstdint>

#include "gtest/gtest.h"
#include "sensor/bus.h"
#include "sensor/measurement_channel.h"
#include "sensor/sensor.h"
#include "sensor/sensors.h"
#include "time/time.h"

namespace ndyn::sensor {

class FakeTemperatureTest : public ::testing::Test {
 public:
  using ValueType = MeasurementValueType<MeasurementType::TEMPERATURE>::type;
  using ScalarType = ValueType;

  static ValueType return_time(time::TimeT t) { return ValueType{static_cast<ScalarType>(t)}; }
};

TEST(FakeTemperatureTest, CanCompile) {
  Sensor<SensorSku::FAKE_TEMPERATURE_SENSOR, BusType::NO_BUS> sensor{
      FakeTemperatureTest::return_time};

  const TemperatureMeasurementChannel& temperature{sensor.temperature_measurements()};

  EXPECT_FALSE(temperature.has_value_available());
}

TEST(FakeTemperatureTest, CanUpdate) {
  Sensor<SensorSku::FAKE_TEMPERATURE_SENSOR, BusType::NO_BUS> sensor{
      FakeTemperatureTest::return_time};

  const TemperatureMeasurementChannel& temperature{sensor.temperature_measurements()};

  sensor.update(1);

  EXPECT_TRUE(temperature.has_value_available());
}

class FakeAccelerometerTest : public ::testing::Test {
 public:
  using ValueType = MeasurementValueType<MeasurementType::ACCELEROMETER>::type;
  using ScalarType = ValueType::ScalarType;

  static ValueType return_time(time::TimeT t) { return ValueType{static_cast<ScalarType>(t)}; }
};

TEST(FakeAccelerometerTest, CanCompile) {
  Sensor<SensorSku::FAKE_ACCELEROMETER, BusType::NO_BUS> sensor{FakeAccelerometerTest::return_time};

  const AccelerometerMeasurementChannel& accelerometer{sensor.accelerometer_measurements()};

  EXPECT_FALSE(accelerometer.has_value_available());
}

TEST(FakeAccelerometerTest, CanUpdate) {
  Sensor<SensorSku::FAKE_ACCELEROMETER, BusType::NO_BUS> sensor{FakeAccelerometerTest::return_time};

  const AccelerometerMeasurementChannel& accelerometer{sensor.accelerometer_measurements()};

  sensor.update(1);

  EXPECT_TRUE(accelerometer.has_value_available());
}

class FakeGyroscopeTest : public ::testing::Test {
 public:
  using ValueType = MeasurementValueType<MeasurementType::GYROSCOPE>::type;
  using ScalarType = ValueType::ScalarType;

  static ValueType return_time(time::TimeT t) { return ValueType{static_cast<ScalarType>(t)}; }
};

TEST(FakeGyroscopeTest, CanCompile) {
  Sensor<SensorSku::FAKE_GYROSCOPE, BusType::NO_BUS> sensor{FakeGyroscopeTest::return_time};

  const GyroscopeMeasurementChannel& gyroscope{sensor.gyroscope_measurements()};

  EXPECT_FALSE(gyroscope.has_value_available());
}

TEST(FakeGyroscopeTest, CanUpdate) {
  Sensor<SensorSku::FAKE_GYROSCOPE, BusType::NO_BUS> sensor{FakeGyroscopeTest::return_time};

  const GyroscopeMeasurementChannel& gyroscope{sensor.gyroscope_measurements()};

  sensor.update(1);

  EXPECT_TRUE(gyroscope.has_value_available());
}

}  // namespace ndyn::sensor
