#pragma once

#include <functional>

#include "sensor/bus.h"
#include "sensor/sensors.h"
#include "time/time.h"

namespace ndyn::sensor {

// Most sensors should accept a read limiting function at construction, possibly a separate one for
// each measurement capability. They may also include limiting functionality internally, but that
// makes it more difficult to test the sensor.
using SensorReadLimiter = std::function<bool(time::TimeT)>;

// TODO(james): Make this class actually useful or delete it. Currently, it has very little value.
template <SensorSku SKU, BusType BUS_TYPE>
class Sensor final {
 public:
  void update(time::TimeT t);
};

}  // namespace ndyn::sensor
