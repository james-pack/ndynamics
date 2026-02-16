#include "gfx/math.h"

namespace ndyn::gfx {

Mat4 Position::as_matrix_transform() const {
  // The general approach would be to create a transform that will apply a rotation and translation
  // to a Vec3.
  //   1) Embed the incoming x (a Vec3) into a quaternion: r = (0, x)
  //   2) Rotate using quaternion conjugation: r' = q * r * q⁻¹
  //   3) Extract the rotated Vec3 from r' and apply the translation:
  //        t' = {r'.x, r'.y, r'.z} + position
  //
  // Following that algorithm would be difficult in C++ and glsl, but the entire process can be
  // encoded as a 4x4 matrix multiplication. This function generates that matrix. To apply this
  // matrix to a Vec3, the Vec3 gets embedded in homogeneous coordinates:
  //   x' = (x, 1)
  // and then can be left multiplied by this matrix:
  //   y' = A x'
  // The result can be left in homogeneous coordinates, or extracted back to a Vec3 as:
  //   Vec3 y{y'.x, y'.y, y'.z};

  const Quat& q = orientation;

  const float xx = q.x * q.x;
  const float yy = q.y * q.y;
  const float zz = q.z * q.z;
  const float xy = q.x * q.y;
  const float xz = q.x * q.z;
  const float yz = q.y * q.z;
  const float wx = q.w * q.x;
  const float wy = q.w * q.y;
  const float wz = q.w * q.z;

  Mat4 m{};

  // Rotation.
  m.m[0][0] = 1.f - 2.f * (yy + zz);
  m.m[1][0] = 2.f * (xy + wz);
  m.m[2][0] = 2.f * (xz - wy);

  m.m[0][1] = 2.f * (xy - wz);
  m.m[1][1] = 1.f - 2.f * (xx + zz);
  m.m[2][1] = 2.f * (yz + wx);

  m.m[0][2] = 2.f * (xz + wy);
  m.m[1][2] = 2.f * (yz - wx);
  m.m[2][2] = 1.f - 2.f * (xx + yy);

  // Translation.
  m.m[0][3] = position.x;
  m.m[1][3] = position.y;
  m.m[2][3] = position.z;
  m.m[3][3] = 1.f;

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

Vec4 Mat4::operator*(const Vec4& rhs) const {
  Vec4 result{};
  result.x = m[0][0] * rhs.x + m[0][1] * rhs.y + m[0][2] * rhs.z + m[0][3] * rhs.w;
  result.y = m[1][0] * rhs.x + m[1][1] * rhs.y + m[1][2] * rhs.z + m[1][3] * rhs.w;
  result.z = m[2][0] * rhs.x + m[2][1] * rhs.y + m[2][2] * rhs.z + m[2][3] * rhs.w;
  result.w = m[3][0] * rhs.x + m[3][1] * rhs.y + m[3][2] * rhs.z + m[3][3] * rhs.w;
  return result;
}

}  // namespace ndyn::gfx
