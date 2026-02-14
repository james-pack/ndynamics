#pragma once

#include <cmath>
#include <cstddef>

#include "gfx/alignment.h"

namespace ndyn::gfx {

struct Vec2 final {
  float x;
  float y;
};

struct Vec3 final {
  float x;
  float y;
  float z;
  Vec3 normalize() const {
    static constexpr float epsilon{1e-18};
    auto scale{std::hypot(x, y, z)};
    if (scale < epsilon) {
      return {0, 0, 0};
    } else {
      return {x / scale, y / scale, z / scale};
    }
  }
};

struct alignas(16) Vec4 final {
  float x;
  float y;
  float z;
  float a;
};
template <>
inline constexpr bool SsboLayoutCheck<Vec4>::is_valid() {
  static_assert(offsetof(Vec4, x) == 0);
  static_assert(offsetof(Vec4, y) == 4);
  static_assert(offsetof(Vec4, z) == 8);
  static_assert(offsetof(Vec4, a) == 12);
  static_assert(alignof(Vec4) == 16);
  static_assert(sizeof(Vec4) == 16);
  return true;
}

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

inline Quat axis_angle(const Vec3& axis, float angle) {
  Vec3 normed{axis.normalize()};
  const float s{std::sin(0.5f * angle)};
  return Quat{std::cos(0.5f * angle), s * normed.x, s * normed.y, s * normed.z};
}

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

struct Position final {
  Vec3 position{0.f, 0.f, 0.f};
  Quat orientation{1.f, 0.f, 0.f, 0.f};
};

struct alignas(16) Mat4 final {
  float m[4][4]{};

  Mat4 operator*(const Mat4& rhs) const;

  Mat4 transpose() const {
    Mat4 result;
    for (size_t i = 0; i < 4; ++i) {
      for (size_t j = 0; j < 4; ++j) {
        result.m[j][i] = m[i][j];
      }
    }
    return result;
  }

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
  static_assert(alignof(Mat4) == 16);
  static_assert(sizeof(Mat4) == 64);
  return true;
};

Mat4 create_model_matrix(const Position& t);

}  // namespace ndyn::gfx
