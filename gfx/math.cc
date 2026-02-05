#include "gfx/math.h"

namespace ndyn::gfx {

Mat4 quaternion_to_rotation_matrix(const Quat& q) {
  const float xx = q.x * q.x;
  const float yy = q.y * q.y;
  const float zz = q.z * q.z;
  const float xy = q.x * q.y;
  const float xz = q.x * q.z;
  const float yz = q.y * q.z;
  const float wx = q.w * q.x;
  const float wy = q.w * q.y;
  const float wz = q.w * q.z;

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

  return r;
}

Mat4 create_model_matrix(const Transform& t) {
  const Quat& q = t.rotation;

  const float xx = q.x * q.x;
  const float yy = q.y * q.y;
  const float zz = q.z * q.z;
  const float xy = q.x * q.y;
  const float xz = q.x * q.z;
  const float yz = q.y * q.z;
  const float wx = q.w * q.x;
  const float wy = q.w * q.y;
  const float wz = q.w * q.z;

  Mat4 m = Mat4::identity();

  // Rotation
  m.m[0][0] = 1.f - 2.f * (yy + zz);
  m.m[0][1] = 2.f * (xy - wz);
  m.m[0][2] = 2.f * (xz + wy);

  m.m[1][0] = 2.f * (xy + wz);
  m.m[1][1] = 1.f - 2.f * (xx + zz);
  m.m[1][2] = 2.f * (yz - wx);

  m.m[2][0] = 2.f * (xz - wy);
  m.m[2][1] = 2.f * (yz + wx);
  m.m[2][2] = 1.f - 2.f * (xx + yy);

  // Translation
  m.m[0][3] = t.position.x;
  m.m[1][3] = t.position.y;
  m.m[2][3] = t.position.z;

  return m;
}

Mat4 Mat4::operator*(const Mat4& rhs) const {
  Mat4 result{};
  for (int i = 0; i < 4; ++i) {
    for (int j = 0; j < 4; ++j) {
      for (int k = 0; k < 4; ++k) {
        result.m[i][j] += m[i][k] * rhs.m[k][j];
      }
    }
  }
  return result;
}

namespace {

Mat4 create_view(const Camera& cam) {
  const Mat4 rotate{quaternion_to_rotation_matrix(cam.rotation.conjugate())};

  Mat4 translate = Mat4::identity();
  translate.m[0][3] = -cam.position.x;
  translate.m[1][3] = -cam.position.y;
  translate.m[2][3] = -cam.position.z;

  return rotate * translate;
}

Mat4 create_projection(float fov_y, float aspect, float z_near, float z_far) {
  const float f = 1.f / std::tan(fov_y * 0.5f);

  Mat4 projection{};
  projection.m[0][0] = f / aspect;
  projection.m[1][1] = f;
  projection.m[2][2] = z_far / (z_far - z_near);
  projection.m[2][3] = 1.f;
  projection.m[3][2] = (-z_near * z_far) / (z_far - z_near);
  return projection;
}

}  // namespace

Mat4 create_view_projection(const Camera& camera) {
  constexpr float kAspect = 16.f / 9.f;
  constexpr float kNear = 0.1f;
  constexpr float kFar = 1000.f;

  Mat4 view = create_view(camera);
  Mat4 proj = create_projection(camera.fov_y_rad, kAspect, kNear, kFar);
  return proj * view;
}

}  // namespace ndyn::gfx
