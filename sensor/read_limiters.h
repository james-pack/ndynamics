#pragma once

#include "sensor/sensor.h"
#include "time/time.h"

namespace ndyn::sensor {

template <time::TimeT READ_DELAY_MS>
class ReadRateLimiter final {
 private:
  time::TimeT last_read_{};
  SensorReadLimiter next_{};

 public:
  bool operator()(time::TimeT t) {
    bool result{false};
    if (t - last_read_ > READ_DELAY_MS) {
      result = true;
    }

    // If it has been long enough between reads, we ask the next read limiter.
    if (result && next_) {
      result = next_(t);
    }

    // Update the last read timing if we are going to allow a read.
    if (result) {
      last_read_ = t;
    }

    return result;
  }
};

}  // namespace ndyn::sensor
