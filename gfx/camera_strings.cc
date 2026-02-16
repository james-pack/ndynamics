#include "gfx/camera_strings.h"

#include <ostream>
#include <string>

#include "gfx/math_strings.h"

namespace ndyn::gfx {

std::string to_string(const CameraState& state) {
  using std::to_string;
  return "view: " + to_string(state.view) + ", projection: " + to_string(state.projection) +
         ", view_projection: " + to_string(state.view_projection) +
         ", position: " + to_string(state.position) +
         ", projection_params: " + to_string(state.projection_params) +
         ", clip_params: " + to_string(state.clip_params) +
         ", is_perspective: " + to_string(state.is_perspective);
}

std::ostream& operator<<(std::ostream& os, const CameraState& state) {
  return os << to_string(state);
}

std::string to_string(const PerspectiveCamera& cam) {
  using std::to_string;
  return "pose: " + to_string(cam.pose) + ", vertical_fov: " + to_string(cam.vertical_fov) +
         ", aspect_ratio: " + to_string(cam.aspect_ratio) +
         ", near_plane: " + to_string(cam.near_plane) + ", far_plane: " + to_string(cam.far_plane);
}

std::ostream& operator<<(std::ostream& os, const PerspectiveCamera& cam) {
  return os << to_string(cam);
}

std::string to_string(const OrthographicCamera& cam) {
  using std::to_string;
  return "pose: " + to_string(cam.pose) + ", half_width: " + to_string(cam.half_width) +
         ", half_height: " + to_string(cam.half_height) +
         ", near_plane: " + to_string(cam.near_plane) + ", far_plane: " + to_string(cam.far_plane);
}

std::ostream& operator<<(std::ostream& os, const OrthographicCamera& cam) {
  return os << to_string(cam);
}

}  // namespace ndyn::gfx
