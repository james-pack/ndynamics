#pragma once

#include <cstdint>
#include <string>

namespace ndyn::math {

enum class Coordinates : uint8_t {
  CARTESIAN,
  POLAR,
  SPHERICAL,
};

std::string to_string(Coordinates coord) {
  switch (coord) {
    case Coordinates::CARTESIAN:
      return "cartesian";
    case Coordinates::POLAR:
      return "polar";
    case Coordinates::SPHERICAL:
      return "spherical";
    default:
      return "<unknown coordinate system>";
  }
}

}  // namespace ndyn::math
