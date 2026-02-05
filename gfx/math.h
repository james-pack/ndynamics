#pragma once

#include <cmath>

namespace ndyn::gfx {

struct Vec3 final {
  float x;
  float y;
  float z;
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

  Quat conjugate() const { return {w, -x, -y, -z}; }
};

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

struct Mat4 final {
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

Mat4 quaternion_to_rotation_matrix(const Quat& q);
Mat4 create_model_matrix(const Transform& t);
Mat4 create_view_projection(const Camera& c);

}  // namespace ndyn::gfx
