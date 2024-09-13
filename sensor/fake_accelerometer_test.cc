#include "sensor/fake_accelerometer.h"

#include <cstdint>

#include "gtest/gtest.h"
#include "sensor/bus.h"
#include "sensor/measurement_channel.h"
#include "sensor/sensor.h"
#include "sensor/sensors.h"
#include "time/time.h"

namespace ndyn::sensor {

using ValueType = MeasurementValueType<MeasurementType::ACCELEROMETER>::type;
using ScalarType = ValueType::ScalarType;

ValueType return_time(time::TimeT t) { return ValueType{static_cast<ScalarType>(t)}; }

TEST(FakeAccelerometerTest, CanCompile) {
  Sensor<SensorSku::FAKE_ACCELEROMETER, BusType::NO_BUS> sensor{return_time};

  const AccelerometerMeasurementChannel& accelerometer{sensor.accelerometer_measurements()};

  EXPECT_FALSE(accelerometer.has_value_available());
}

TEST(FakeAccelerometerTest, CanUpdate) {
  Sensor<SensorSku::FAKE_ACCELEROMETER, BusType::NO_BUS> sensor{return_time};

  const AccelerometerMeasurementChannel& accelerometer{sensor.accelerometer_measurements()};

  sensor.update(1);

  EXPECT_TRUE(accelerometer.has_value_available());
}

}  // namespace ndyn::sensor
