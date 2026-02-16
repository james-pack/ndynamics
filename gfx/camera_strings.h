#pragma once

#include <ostream>
#include <string>

#include "gfx/camera.h"

namespace ndyn::gfx {

std::string to_string(const CameraState& state);
std::ostream& operator<<(std::ostream& os, const CameraState& state);

std::string to_string(const PerspectiveCamera& cam);
std::ostream& operator<<(std::ostream& os, const PerspectiveCamera& cam);

std::string to_string(const OrthographicCamera& cam);
std::ostream& operator<<(std::ostream& os, const OrthographicCamera& cam);

}  // namespace ndyn::gfx
