#pragma once

#include <cstddef>
#include <cstdint>

#include "gfx/math.h"

namespace ndyn::gfx {

/**
 * Light source description used by the fragment shader for surface illumination.
 * Supports point, directional, and spot lights evaluated with the Phong reflection model.
 * https://en.wikipedia.org/wiki/Phong_reflection_model
 */
struct alignas(16) Light {
  /**
   * World-space position of the light. Used by point and spot lights to determine
   * direction toward the surface and distance attenuation.
   */
  Vec4 position{};

  /**
   * Direction the light emits along. Used by directional lights and as the axis
   * of the emission cone for spot lights.
   */
  alignas(16) Vec4 direction{};

  /**
   * RGB emission color of the light, along with an intensity value in the alpha channel.
   */
  alignas(16) Vec4 color{};

  /**
   * Inner cone angle (radians). Surfaces inside this angle receive full illumination
   * from a spot light.
   */
  float spot_inner{};

  /**
   * Outer cone angle (radians). Illumination falls to zero at this boundary.
   */
  float spot_outer{};

  /**
   * Light classification: 0 = point, 1 = directional, 2 = spot.
   */
  uint32_t type{0};

  static Light no_light();
  static Light default_ambient_light();
  static Light default_directional_light();
};

static_assert(sizeof(Light) == 64, "size mismatch");
static_assert(alignof(Light) == 16, "align mismatch");
static_assert(offsetof(Light, position) == 0, "position offset mismatch");
static_assert(offsetof(Light, direction) == 16, "direction offset mismatch");

static_assert(offsetof(Light, color) == 32, "color offset mismatch");

static_assert(offsetof(Light, spot_inner) == 48, "spot_inner offset mismatch");
static_assert(offsetof(Light, spot_outer) == 52, "spot_outer offset mismatch");

static_assert(offsetof(Light, type) == 56, "type offset mismatch");

}  // namespace ndyn::gfx
