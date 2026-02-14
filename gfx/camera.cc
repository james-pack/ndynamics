#include "gfx/camera.h"

#include "gfx/math.h"

namespace ndyn::gfx {

Mat4 Camera::make_view(const Position& pose) const {
  const Quat q = pose.orientation.normalize();
  const Quat qc = q.conjugate();

  const float xx = qc.x * qc.x;
  const float yy = qc.y * qc.y;
  const float zz = qc.z * qc.z;
  const float xy = qc.x * qc.y;
  const float xz = qc.x * qc.z;
  const float yz = qc.y * qc.z;
  const float wx = qc.w * qc.x;
  const float wy = qc.w * qc.y;
  const float wz = qc.w * qc.z;

  Mat4 r = Mat4::identity();
  r.m[0][0] = 1.f - 2.f * (yy + zz);
  r.m[0][1] = 2.f * (xy - wz);
  r.m[0][2] = 2.f * (xz + wy);
  r.m[1][0] = 2.f * (xy + wz);
  r.m[1][1] = 1.f - 2.f * (xx + zz);
  r.m[1][2] = 2.f * (yz - wx);
  r.m[2][0] = 2.f * (xz - wy);
  r.m[2][1] = 2.f * (yz + wx);
  r.m[2][2] = 1.f - 2.f * (xx + yy);

  Mat4 t = Mat4::identity();
  t.m[3][0] = -pose.position.x;
  t.m[3][1] = -pose.position.y;
  t.m[3][2] = -pose.position.z;

  return r * t;
}

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
