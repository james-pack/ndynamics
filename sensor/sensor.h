#pragma once

#include "sensor/bus.h"
#include "sensor/sensors.h"

namespace ndyn::sensor {

// TODO(james): Make this class actually useful or delete it. Currently, it has very little value.
  template <SensorSku SKU, BusType BUS_TYPE>
class Sensor final {};

}  // namespace ndyn::sensor
