#include "gfx/material.h"

#include "gfx/materials.h"
#include "gfx/math.h"

namespace ndyn::gfx {

Material make_matte(const Vec3& color) {
  Material m{};
  m.base_color = Vec4{color.x, color.y, color.z, 1.f};
  m.metallic_factor = 0.0f;
  m.roughness_factor = 1.0f;
  return m;
}

Material make_glossy(const Vec3& color) {
  Material m{};
  m.base_color = Vec4{color.x, color.y, color.z, 1.f};
  m.metallic_factor = 0.0f;
  m.roughness_factor = 0.1f;
  return m;
}

Material make_metallic(const Vec3& color, float roughness) {
  Material m{};
  m.base_color = Vec4{color.x, color.y, color.z, 1.0f};
  m.metallic_factor = 1.0f;
  m.roughness_factor = roughness;
  m.specular_factor = 1.0f;
  return m;
}

Material make_plastic(const Vec3& color, float roughness) {
  Material m{};
  m.base_color = Vec4{color.x, color.y, color.z, 1.0f};
  m.metallic_factor = 0.0f;
  m.roughness_factor = roughness;
  m.specular_factor = 1.0f;
  return m;
}

Material make_diffuse(const Vec3& color) {
  Material m{};
  m.base_color = Vec4{color.x, color.y, color.z, 1.0f};
  m.metallic_factor = 0.0f;
  m.roughness_factor = 0.9f;
  m.specular_factor = 0.5f;
  return m;
}

Material make_glass(const Vec3& tint, float ior, float roughness) {
  Material m{};
  m.feature_mask |= Material::FEATURE_TRANSMISSION | Material::FEATURE_VOLUME;

  m.base_color = Vec4{tint.x, tint.y, tint.z, 1.0f};
  m.metallic_factor = 0.0f;
  m.transmission_factor = 1.0f;
  m.ior = ior;
  m.roughness_factor = roughness;

  m.attenuation_color = tint;
  m.attenuation_distance = 1.0f;

  m.alpha_mode = 2;
  return m;
}

Material make_emissive(const Vec3& color, float intensity) {
  Material m{};
  m.emissive_factor = Vec3{color.x * intensity, color.y * intensity, color.z * intensity};
  m.base_color = Vec4{0.f, 0.f, 0.f, 1.f};
  m.metallic_factor = 0.0f;
  m.roughness_factor = 1.0f;
  return m;
}

Material make_clearcoat_paint(const Vec3& color, float roughness) {
  Material m{};
  m.feature_mask |= Material::FEATURE_CLEARCOAT;

  m.base_color = Vec4{color.x, color.y, color.z, 1.0f};
  m.metallic_factor = 0.0f;
  m.roughness_factor = roughness;

  m.clearcoat_factor = 1.0f;
  m.clearcoat_roughness = 0.03f;
  return m;
}

}  // namespace ndyn::gfx
