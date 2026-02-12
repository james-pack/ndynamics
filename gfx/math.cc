#include "gfx/math.h"

namespace ndyn::gfx {

Mat4 create_model_matrix(const Position& p) {
  const Quat& q = p.orientation;

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

  // Rotation.
  m.m[0][0] = 1.f - 2.f * (yy + zz);
  m.m[0][1] = 2.f * (xy + wz);
  m.m[0][2] = 2.f * (xz - wy);

  m.m[1][0] = 2.f * (xy - wz);
  m.m[1][1] = 1.f - 2.f * (xx + zz);
  m.m[1][2] = 2.f * (yz + wx);

  m.m[2][0] = 2.f * (xz + wy);
  m.m[2][1] = 2.f * (yz - wx);
  m.m[2][2] = 1.f - 2.f * (xx + yy);

  // Translation.
  m.m[0][3] = p.position.x;
  m.m[1][3] = p.position.y;
  m.m[2][3] = p.position.z;

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

}  // namespace ndyn::gfx
