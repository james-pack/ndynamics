#pragma once

#include <cstdint>

namespace ndyn::time {

// Type for representing times (usually a count of seconds, milliseconds, or microseconds) as
// scalars.
using TimeType = uint64_t;
// Retained for backward compatibility.
using TimeT = TimeType;

}  // namespace ndyn::time
