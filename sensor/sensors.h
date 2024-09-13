#pragma once

#include <cstdint>

namespace ndyn::sensor {

enum class SensorSku : uint8_t {
  FAKE_ACCELEROMETER,
  IMU_BMI323,  // IMU that also has a temperature sensor.
};

}  // namespace ndyn::sensor
