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

  Vec3 operator+(const Vec3& rhs) const { return {x + rhs.x, y + rhs.y, z + rhs.z}; }

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
  float x{};
  float y{};
  float z{};
  float w{1.f};
};
template <>
inline constexpr bool SsboLayoutCheck<Vec4>::is_valid() {
  static_assert(offsetof(Vec4, x) == 0);
  static_assert(offsetof(Vec4, y) == 4);
  static_assert(offsetof(Vec4, z) == 8);
  static_assert(offsetof(Vec4, w) == 12);
  static_assert(alignof(Vec4) == 16);
  static_assert(sizeof(Vec4) == 16);
  return true;
}

struct Quat final {
  float w{1.f};
  float x{};
  float y{};
  float z{};

  Quat operator*(const Quat& rhs) const {
    return {
        w * rhs.w - x * rhs.x - y * rhs.y - z * rhs.z,
        w * rhs.x + x * rhs.w + y * rhs.z - z * rhs.y,
        w * rhs.y - x * rhs.z + y * rhs.w + z * rhs.x,
        w * rhs.z + x * rhs.y - y * rhs.x + z * rhs.w,
    };
  }

  Quat normalize() const {
    static constexpr float epsilon{1e-18};
    auto scale{std::hypot(w, std::hypot(x, y, z))};
    if (scale < epsilon) {
      return {1.f, 0.f, 0.f, 0.f};
    } else {
      return {w / scale, x / scale, y / scale, z / scale};
    }
  }

  Quat conjugate() const { return {w, -x, -y, -z}; }

  static Quat axis_angle(const Vec3& axis, float angle_radians) {
    Vec3 normed{axis.normalize()};
    const float s{std::sin(0.5f * angle_radians)};
    return {std::cos(0.5f * angle_radians), s * normed.x, s * normed.y, s * normed.z};
  }
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

struct Mat4;

/**
 * Position can be thought of as both a state vector of an object or as a transform.
 *
 * As a state vector, it just encodes the current position and orientation. Combined these are often
 * referred to as the pose of an object.
 *
 * As a transform, it can be used to rotate a vector, according to the orientation, and then
 * translate it, according to the position.
 */
struct Position final {
 private:
  // Helper method to rotate translations/offsets in other methods.
  static Vec3 rotate(const Quat& orientation, const Vec3& v) {
    Quat qv{0.f, v.x, v.y, v.z};
    Quat res = orientation * qv * orientation.conjugate();
    return {res.x, res.y, res.z};
  }

 public:
  Vec3 position{0.f, 0.f, 0.f};
  Quat orientation{1.f, 0.f, 0.f, 0.f};

  Position operator*(const Position& rhs) const {
    return {position + rotate(orientation, rhs.position), orientation * rhs.orientation};
  }

  Position invert() const {
    const Quat inverse_orientation{orientation.conjugate()};
    return {rotate(inverse_orientation, {-position.x, -position.y, -position.z}),
            inverse_orientation};
  }

  /**
   * Function to generate a matrix that can perform this transform.
   */
  Mat4 as_matrix_transform() const;

  static Position translation(const Vec3& t) { return Position{.position = t}; }

  static Position rotation(const Vec3& axis, float angle_radians) {
    return Position{.orientation = Quat::axis_angle(axis, angle_radians)};
  }
};

struct alignas(16) Mat4 final {
  float m[4][4]{};

  Mat4 operator*(const Mat4& rhs) const;
  Vec4 operator*(const Vec4& rhs) const;

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

  static Mat4 scaling(const Vec3& s) {
    Mat4 result{};
    result.m[0][0] = s.x;
    result.m[1][1] = s.y;
    result.m[2][2] = s.z;
    result.m[3][3] = 1.f;
    return result;
  }
  static Mat4 translation(const Vec3& t) { return Position::translation(t).as_matrix_transform(); }
  static Mat4 rotation(const Vec3& axis, float angle_radians) {
    return Position::rotation(axis, angle_radians).as_matrix_transform();
  }
};

template <>
inline constexpr bool SsboLayoutCheck<Mat4>::is_valid() {
  static_assert(offsetof(Mat4, m) % 16 == 0);
  static_assert(alignof(Mat4) == 16);
  static_assert(sizeof(Mat4) == 64);
  return true;
};

}  // namespace ndyn::gfx
