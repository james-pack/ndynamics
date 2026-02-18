#pragma once

#include <ostream>
#include <string>

#include "gfx/light.h"

namespace ndyn::gfx {

std::string to_string(const Light& state);
std::ostream& operator<<(std::ostream& os, const Light& state);

}  // namespace ndyn::gfx
