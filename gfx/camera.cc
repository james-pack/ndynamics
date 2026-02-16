#include "gfx/camera.h"

#include "gfx/math.h"

namespace ndyn::gfx {

Mat4 make_view(const Position& pose) { return pose.invert().as_matrix_transform(); }

CameraState PerspectiveCamera::make_camera_state() const {
  CameraState state{};
  state.position = {pose.position.x, pose.position.y, pose.position.z, 0.f};

  state.projection_params = {vertical_fov, aspect_ratio};
  state.clip_params = {near_plane, far_plane};
  state.is_perspective = true;
  state.projection = make_perspective(vertical_fov, aspect_ratio, near_plane, far_plane);

  state.view = make_view(pose);
  state.view_projection = state.projection * state.view;
  return state;
}

Mat4 PerspectiveCamera::make_perspective(float fov_y, float aspect, float n, float f) {
  const float tan_half = std::tan(0.5f * fov_y);
  Mat4 m{};
  m.m[0][0] = 1.f / (aspect * tan_half);
  m.m[1][1] = -1.f / tan_half;
  m.m[2][2] = f / (f - n);
  m.m[2][3] = 1.f;
  m.m[3][2] = -(f * n) / (f - n);
  return m;
}

CameraState OrthographicCamera::make_camera_state() const {
  CameraState state{};
  state.position = {pose.position.x, pose.position.y, pose.position.z, 0.f};

  state.projection_params = {half_width, half_height};
  state.clip_params = {near_plane, far_plane};
  state.is_perspective = false;
  state.projection = make_orthographic(half_width, half_height, near_plane, far_plane);

  state.view = make_view(pose);
  state.view_projection = state.projection * state.view;
  return state;
}

}  // namespace ndyn::gfx
