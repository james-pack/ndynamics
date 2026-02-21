#pragma once

#include <cstddef>
#include <cstdint>

#include "gfx/alignment.h"
#include "gfx/math.h"

namespace ndyn::gfx {

struct Material final {
  // Feature mask: each bit enables a specific shading feature or texture lookup.
  // The renderer sets bits once after loading a material. The shader tests bits
  // to decide which BRDF terms and texture samples to evaluate.

  // TODO(james): Audit and regroup these. Make sure they are consistent with the shader values.
  static constexpr uint32_t FEATURE_BASE_COLOR_TEXTURE = 1u << 0;
  static constexpr uint32_t FEATURE_METALLIC_ROUGHNESS_TEXTURE = 1u << 1;
  static constexpr uint32_t FEATURE_NORMAL_TEXTURE = 1u << 2;
  static constexpr uint32_t FEATURE_OCCLUSION_TEXTURE = 1u << 3;
  static constexpr uint32_t FEATURE_EMISSIVE_TEXTURE = 1u << 4;
  static constexpr uint32_t FEATURE_CLEARCOAT = 1u << 5;            // secondary dielectric layer
  static constexpr uint32_t FEATURE_TRANSMISSION = 1u << 6;         // refractive transport
  static constexpr uint32_t FEATURE_SHEEN = 1u << 7;                // cloth grazing lobe
  static constexpr uint32_t FEATURE_SHEEN_COLOR_TEXTURE = 1u << 8;  // cloth grazing lobe
  static constexpr uint32_t FEATURE_SPECULAR = 1u << 9;             // specular color control
  static constexpr uint32_t FEATURE_VOLUME = 1u << 10;              // absorption inside medium

  // OR of FEATURE_* bits controlling which shading paths the GPU executes.
  uint32_t feature_mask = 0;

  // Alpha pipeline behavior: 0=opaque, 1=cutout(alpha test), 2=blend.
  uint32_t alpha_mode = 0;

  // Threshold used when alpha_mode == 1.
  float alpha_cutoff = 0.5f;

  // 0=cull backfaces, 1=render both sides of the surface.
  float double_sided = 0.0f;

  // Base color and final surface alpha.
  Vec4 base_color{1.f, 1.f, 1.f, 1.f};

  // Self-illumination added after lighting evaluation.
  Vec3 emissive_factor{0.f, 0.f, 0.f};

  // 0=dielectric response, 1=conductor response.
  float metallic_factor = 1.0f;

  // Microfacet distribution width controlling highlight spread.
  float roughness_factor = 1.0f;

  // Scales perturbation produced by the normal map.
  float normal_scale = 1.0f;

  // Multiplier applied to ambient/indirect lighting.
  float occlusion_strength = 1.0f;

  // Intensity of the outer clearcoat layer.
  float clearcoat_factor = 0.0f;

  // Roughness of the clearcoat layer.
  float clearcoat_roughness = 0.0f;

  // Fraction of light transmitted through the surface.
  float transmission_factor = 0.0f;

  // Index of refraction used for Fresnel and transmission.
  float ior = 1.5f;

  // Sheen lobe color for fabrics and velvet-like materials.
  alignas(16) Vec3 sheen_color{0.f, 0.f, 0.f};

  // Sheen lobe roughness.
  float sheen_roughness = 0.0f;

  // Additional specular intensity control.
  float specular_factor = 1.0f;

  // Specular reflectance color.
  alignas(16) Vec3 specular_color{1.f, 1.f, 1.f};

  // Thickness of participating medium inside the object.
  float thickness_factor = 0.0f;

  // Mean free path distance before absorption.
  float attenuation_distance = 0.0f;

  // Absorption tint for transmitted light.
  alignas(16) Vec3 attenuation_color{1.f, 1.f, 1.f};

  // Index of base color texture (UINT32_MAX = not bound).
  uint32_t base_color_tex = 0xffffffffu;

  // Index of packed metallic-roughness texture.
  uint32_t metallic_roughness_tex = 0xffffffffu;

  // Index of tangent-space normal map.
  uint32_t normal_tex = 0xffffffffu;

  // Index of ambient occlusion texture.
  uint32_t occlusion_tex = 0xffffffffu;

  // Index of emissive texture.
  uint32_t emissive_tex = 0xffffffffu;

  // Index of clearcoat factor texture.
  uint32_t clearcoat_tex = 0xffffffffu;

  // Index of clearcoat roughness texture.
  uint32_t clearcoat_roughness_tex = 0xffffffffu;

  // Index of clearcoat normal map.
  uint32_t clearcoat_normal_tex = 0xffffffffu;

  // Index of transmission texture.
  uint32_t transmission_tex = 0xffffffffu;

  // Index of sheen color texture.
  uint32_t sheen_color_tex = 0xffffffffu;

  // Index of sheen roughness texture.
  uint32_t sheen_roughness_tex = 0xffffffffu;

  // Index of specular intensity texture.
  uint32_t specular_tex = 0xffffffffu;

  // Index of specular color texture.
  uint32_t specular_color_tex = 0xffffffffu;

  // Index of thickness texture.
  uint32_t thickness_tex = 0xffffffffu;
};

using MaterialId = uint32_t;

template <>
inline constexpr bool SsboLayoutCheck<Material>::is_valid() {
  static_assert(offsetof(Material, feature_mask) == 0);
  static_assert(offsetof(Material, alpha_mode) == 4);
  static_assert(offsetof(Material, alpha_cutoff) == 8);
  static_assert(offsetof(Material, double_sided) == 12);
  static_assert(offsetof(Material, base_color) == 16);
  static_assert(offsetof(Material, emissive_factor) == 32);
  static_assert(offsetof(Material, metallic_factor) == 44);
  static_assert(offsetof(Material, roughness_factor) == 48);
  static_assert(offsetof(Material, normal_scale) == 52);
  static_assert(offsetof(Material, occlusion_strength) == 56);
  static_assert(offsetof(Material, clearcoat_factor) == 60);
  static_assert(offsetof(Material, clearcoat_roughness) == 64);
  static_assert(offsetof(Material, transmission_factor) == 68);
  static_assert(offsetof(Material, ior) == 72);
  static_assert(offsetof(Material, sheen_color) == 80);
  static_assert(offsetof(Material, sheen_roughness) == 92);
  static_assert(offsetof(Material, specular_factor) == 96);
  static_assert(offsetof(Material, specular_color) == 112);
  static_assert(offsetof(Material, thickness_factor) == 124);
  static_assert(offsetof(Material, attenuation_distance) == 128);
  static_assert(offsetof(Material, attenuation_color) == 144);
  static_assert(offsetof(Material, base_color_tex) == 156);
  static_assert(offsetof(Material, metallic_roughness_tex) == 160);
  static_assert(offsetof(Material, normal_tex) == 164);
  static_assert(offsetof(Material, occlusion_tex) == 168);
  static_assert(offsetof(Material, emissive_tex) == 172);
  static_assert(offsetof(Material, clearcoat_tex) == 176);
  static_assert(offsetof(Material, clearcoat_roughness_tex) == 180);
  static_assert(offsetof(Material, clearcoat_normal_tex) == 184);
  static_assert(offsetof(Material, transmission_tex) == 188);
  static_assert(offsetof(Material, sheen_color_tex) == 192);
  static_assert(offsetof(Material, sheen_roughness_tex) == 196);
  static_assert(offsetof(Material, specular_tex) == 200);
  static_assert(offsetof(Material, specular_color_tex) == 204);
  static_assert(offsetof(Material, thickness_tex) == 208);
  static_assert(SsboLayoutCheck<Vec4>::value);
  return true;
}

}  // namespace ndyn::gfx
