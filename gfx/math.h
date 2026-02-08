#pragma once

#include <cmath>
#include <cstddef>

#include "gfx/alignment.h"

namespace ndyn::gfx {

struct Vec3 final {
  float x;
  float y;
  float z;
};

struct Vec4 final {
  float x;
  float y;
  float z;
  float a;
};

struct Quat final {
  float w;
  float x;
  float y;
  float z;

  Quat operator*(const Quat& rhs) const {
    return {
        w * rhs.w - x * rhs.x - y * rhs.y - z * rhs.z,
        w * rhs.x + x * rhs.w + y * rhs.z - z * rhs.y,
        w * rhs.y - x * rhs.z + y * rhs.w + z * rhs.x,
        w * rhs.z + x * rhs.y - y * rhs.x + z * rhs.w,
    };
  }

  Quat normalize() const {
    auto scale{std::hypot(w, std::hypot(x, y, z))};
    return {w / scale, x / scale, y / scale, z / scale};
  }
  Quat conjugate() const { return {w, -x, -y, -z}; }
};

inline Quat operator*(float scale, const Quat& q) {
  return {
      scale * q.w,
      scale * q.x,
      scale * q.y,
      scale * q.z,
  };
}

inline Quat operator*(const Quat& q, float scale) {
  return {
      scale * q.w,
      scale * q.x,
      scale * q.y,
      scale * q.z,
  };
}

struct Vertex final {
  float px, py, pz;
  float nx, ny, nz;
};

// TODO(james): Rename the Transform class as it is really a combination of position and
// orientation.
struct Transform final {
  Vec3 position{0.f, 0.f, 0.f};
  Quat rotation{1.f, 0.f, 0.f, 0.f};
};

struct Camera final {
  Vec3 position{0.f, 0.f, 5.f};
  Quat rotation{1.f, 0.f, 0.f, 0.f};
  float fov_y_rad{1.0f};
};

struct Mat4 final : GpuStdAlign {
  float m[4][4]{};

  Mat4 operator*(const Mat4& rhs) const;

  static Mat4 identity() {
    Mat4 r{};
    r.m[0][0] = 1.f;
    r.m[1][1] = 1.f;
    r.m[2][2] = 1.f;
    r.m[3][3] = 1.f;
    return r;
  }
};

template <>
inline constexpr bool SsboLayoutCheck<Mat4>::is_valid() {
  static_assert(offsetof(Mat4, m) % 16 == 0);
  return true;
};

Mat4 quaternion_to_rotation_matrix(const Quat& q);
Mat4 create_model_matrix(const Transform& t);
Mat4 create_view_projection(const Camera& c);

}  // namespace ndyn::gfx
