#include "gfx/light_strings.h"

#include <ostream>
#include <string>

#include "gfx/math_strings.h"

namespace ndyn::gfx {

std::string to_string(const Light& light) {
  using std::to_string;
  return "position: " + to_string(light.position) + ", direction: " + to_string(light.direction) +
         ", color: " + to_string(light.color) + ", spot_inner: " + to_string(light.spot_inner) +
         ", spot_outer: " + to_string(light.spot_outer) + ", type: " + to_string(light.type);
}

std::ostream& operator<<(std::ostream& os, const Light& light) { return os << to_string(light); }

}  // namespace ndyn::gfx
