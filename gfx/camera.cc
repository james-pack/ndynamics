#include "gfx/camera.h"

#include "gfx/math.h"

namespace ndyn::gfx {

Mat4 make_view(const Position& pose) { return pose.invert().as_matrix_transform(); }

CameraState PerspectiveCamera::make_camera_state() const {
  CameraState state{};
  state.view = make_view(pose).transpose();

  state.position = {pose.position.x, pose.position.y, pose.position.z, 0.f};

  state.projection_params = {vertical_fov, aspect_ratio};
  state.clip_params = {near_plane, far_plane};
  state.is_perspective = true;
  state.projection =
      make_perspective(vertical_fov, aspect_ratio, near_plane, far_plane).transpose();

  state.view_projection = state.view * state.projection;
  return state;
}

Mat4 PerspectiveCamera::make_perspective(float fov_y, float aspect, float n, float f) {
  const float tan_half = std::tan(0.5f * fov_y);
  Mat4 m{};
  m.m[0][0] = 1.f / (aspect * tan_half);
  m.m[1][1] = -1.f / tan_half;
  m.m[2][2] = f / (n - f);
  m.m[3][2] = -1.f;
  m.m[2][3] = (f * n) / (n - f);
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

CameraState CameraState::default_camera() {
  // The default camera is a perspective camera with an ~85 degree FOV at (0, 0, 3) looking down the
  // z-axis in the -z direction.
  PerspectiveCamera camera{Position{{0.f, 0.f, 3.f}, {}},
                           /* ~85 degrees in radians */ 1.5f,
                           /* square aspect ratio */ 1.f, /* near */ 1.f,
                           /* far */ 10.f};

  return camera.make_camera_state();
}

}  // namespace ndyn::gfx
