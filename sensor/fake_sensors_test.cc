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

  ValueType next_measure_value{};
};

TEST_F(FakeTemperatureTest, CanCompile) {
  Sensor<SensorSku::FAKE_TEMPERATURE_SENSOR, BusType::NO_BUS> sensor{
      FakeTemperatureTest::return_time};

  const TemperatureMeasurementChannel& temperature{sensor.measurement()};

  EXPECT_FALSE(temperature.has_value_available());
}

TEST_F(FakeTemperatureTest, CanUpdate) {
  Sensor<SensorSku::FAKE_TEMPERATURE_SENSOR, BusType::NO_BUS> sensor{
      FakeTemperatureTest::return_time};

  const TemperatureMeasurementChannel& temperature{sensor.measurement()};

  sensor.update(1);

  EXPECT_TRUE(temperature.has_value_available());
}

TEST_F(FakeTemperatureTest, CanSetNextValueThroughMeasureFunction) {
  Sensor<SensorSku::FAKE_TEMPERATURE_SENSOR, BusType::NO_BUS> sensor(
      [this](time::TimeT t) { return this->next_measure_value; });

  const TemperatureMeasurementChannel& measurement{sensor.measurement()};

  static constexpr time::TimeT EXPECTED_TIME{static_cast<time::TimeT>(10)};
  static constexpr ValueType EXPECTED_VALUE{ValueType{static_cast<ScalarType>(12)}};
  next_measure_value = EXPECTED_VALUE;

  sensor.update(EXPECTED_TIME);

  EXPECT_EQ(EXPECTED_TIME, measurement.time());
  EXPECT_EQ(EXPECTED_VALUE, measurement.value());
}

class FakeAccelerometerTest : public ::testing::Test {
 public:
  using ValueType = MeasurementValueType<MeasurementType::ACCELEROMETER>::type;
  using ScalarType = ValueType::ScalarType;

  static ValueType return_time(time::TimeT t) { return ValueType{static_cast<ScalarType>(t)}; }

  ValueType next_measure_value{};
};

TEST_F(FakeAccelerometerTest, CanCompile) {
  Sensor<SensorSku::FAKE_ACCELEROMETER, BusType::NO_BUS> sensor{FakeAccelerometerTest::return_time};

  const AccelerometerMeasurementChannel& accelerometer{sensor.measurement()};

  EXPECT_FALSE(accelerometer.has_value_available());
}

TEST_F(FakeAccelerometerTest, CanUpdate) {
  Sensor<SensorSku::FAKE_ACCELEROMETER, BusType::NO_BUS> sensor{FakeAccelerometerTest::return_time};

  const AccelerometerMeasurementChannel& accelerometer{sensor.measurement()};

  sensor.update(1);

  EXPECT_TRUE(accelerometer.has_value_available());
}

TEST_F(FakeAccelerometerTest, CanSetNextValueThroughMeasureFunction) {
  Sensor<SensorSku::FAKE_ACCELEROMETER, BusType::NO_BUS> sensor(
      [this](time::TimeT t) { return this->next_measure_value; });

  const AccelerometerMeasurementChannel& measurement{sensor.measurement()};

  static constexpr time::TimeT EXPECTED_TIME{static_cast<time::TimeT>(10)};
  static constexpr ValueType EXPECTED_VALUE{ValueType{static_cast<ScalarType>(12)}};
  next_measure_value = EXPECTED_VALUE;

  sensor.update(EXPECTED_TIME);

  EXPECT_EQ(EXPECTED_TIME, measurement.time());
  EXPECT_EQ(EXPECTED_VALUE, measurement.value());
}

class FakeGyroscopeTest : public ::testing::Test {
 public:
  using ValueType = MeasurementValueType<MeasurementType::GYROSCOPE>::type;
  using ScalarType = ValueType::ScalarType;

  static ValueType return_time(time::TimeT t) { return ValueType{static_cast<ScalarType>(t)}; }

  ValueType next_measure_value{};
};

TEST_F(FakeGyroscopeTest, CanCompile) {
  Sensor<SensorSku::FAKE_GYROSCOPE, BusType::NO_BUS> sensor{FakeGyroscopeTest::return_time};

  const GyroscopeMeasurementChannel& gyroscope{sensor.measurement()};

  EXPECT_FALSE(gyroscope.has_value_available());
}

TEST_F(FakeGyroscopeTest, CanUpdate) {
  Sensor<SensorSku::FAKE_GYROSCOPE, BusType::NO_BUS> sensor{FakeGyroscopeTest::return_time};

  const GyroscopeMeasurementChannel& gyroscope{sensor.measurement()};

  sensor.update(1);

  EXPECT_TRUE(gyroscope.has_value_available());
}

TEST_F(FakeGyroscopeTest, CanSetNextValueThroughMeasureFunction) {
  Sensor<SensorSku::FAKE_GYROSCOPE, BusType::NO_BUS> sensor(
      [this](time::TimeT t) { return this->next_measure_value; });

  const GyroscopeMeasurementChannel& measurement{sensor.measurement()};

  static constexpr time::TimeT EXPECTED_TIME{static_cast<time::TimeT>(10)};
  static constexpr ValueType EXPECTED_VALUE{ValueType{static_cast<ScalarType>(12)}};
  next_measure_value = EXPECTED_VALUE;

  sensor.update(EXPECTED_TIME);

  EXPECT_EQ(EXPECTED_TIME, measurement.time());
  EXPECT_EQ(EXPECTED_VALUE, measurement.value());
}

}  // namespace ndyn::sensor