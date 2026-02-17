#pragma once

#include <vulkan/vulkan.h>

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <stdexcept>

#include "gfx/math.h"

namespace ndyn::gfx {

struct alignas(16) CameraState final {
  Mat4 view;
  Mat4 projection;
  Mat4 view_projection;
  Vec4 position;
  Vec2 projection_params;
  Vec2 clip_params;
  bool is_perspective;

  static CameraState default_camera();
};

static_assert(offsetof(CameraState, view) == 0, "view offset incorrect");
static_assert(offsetof(CameraState, projection) == 64, "projection offset incorrect");
static_assert(offsetof(CameraState, view_projection) == 128, "view_projection offset incorrect");
static_assert(offsetof(CameraState, position) == 192, "position offset incorrect");
static_assert(offsetof(CameraState, projection_params) == 208,
              "projection_params offset incorrect");
static_assert(offsetof(CameraState, clip_params) == 216, "clip_params offset incorrect");
static_assert(offsetof(CameraState, is_perspective) == 224, "is_perspective offset incorrect");
static_assert(sizeof(CameraState) % 16 == 0, "CameraState size must be multiple of 16");
static_assert(alignof(CameraState) % 16 == 0, "CameraState alignment must be multiple of 16");

class Camera {
 public:
  virtual ~Camera() = default;

  virtual CameraState make_camera_state() const = 0;
};

class PerspectiveCamera final : public Camera {
 public:
  Position pose{};

  float vertical_fov{1.f};
  float aspect_ratio{1.f};

  float near_plane{0.1f};
  float far_plane{100.f};

  PerspectiveCamera(const Position& pose, float vertical_fov_rad, float aspect_ratio,
                    float near_plane, float far_plane)
      : pose(pose),
        vertical_fov(vertical_fov_rad),
        aspect_ratio(aspect_ratio),
        near_plane(near_plane),
        far_plane(far_plane) {}

  CameraState make_camera_state() const override;

 private:
  static Mat4 make_perspective(float fov_y, float aspect, float n, float f);
};

class OrthographicCamera final : public Camera {
 public:
  Position pose{};

  float half_width{1.f};
  float half_height{1.f};

  float near_plane{0.1f};
  float far_plane{100.f};

  OrthographicCamera(const Position& pose, float half_width, float half_height, float near_plane,
                     float far_plane)
      : pose(pose),
        half_width(half_width),
        half_height(half_height),
        near_plane(near_plane),
        far_plane(far_plane) {}

  CameraState make_camera_state() const override;

 private:
  static Mat4 make_orthographic(float hw, float hh, float n, float f) {
    Mat4 m = Mat4::identity();
    m.m[0][0] = 1.f / hw;
    m.m[1][1] = -1.f / hh;
    m.m[2][2] = 1.f / (f - n);
    m.m[3][2] = -n / (f - n);
    return m;
  }
};

}  // namespace ndyn::gfx
