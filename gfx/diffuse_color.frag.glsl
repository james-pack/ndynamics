#version 450

layout(location = 0) in vec3 frag_pos;
layout(location = 1) in vec3 frag_normal;
layout(location = 2) in vec3 frag_uv;
layout(location = 3) in flat uint material_id;

layout(location = 0) out vec4 out_color;

struct Material {
  // Feature mask: each bit enables a specific shading feature or texture lookup.
  uint feature_mask;

  // Alpha pipeline behavior: 0=opaque, 1=cutout(alpha test), 2=blend.
  uint alpha_mode;

  // Threshold used when alpha_mode == 1.
  float alpha_cutoff;

  // 0=cull backfaces, 1=render both sides of the surface.
  float double_sided;

  // Base color and final surface alpha.
  vec4 base_color;

  // Self-illumination added after lighting evaluation.
  vec3 emissive_factor;

  // 0=dielectric response, 1=conductor response.
  float metallic_factor;

  // Microfacet distribution width controlling highlight spread.
  float roughness_factor;

  // Scales perturbation produced by the normal map.
  float normal_scale;

  // Multiplier applied to ambient/indirect lighting.
  float occlusion_strength;

  // Intensity of the outer clearcoat layer.
  float clearcoat_factor;

  // Roughness of the clearcoat layer.
  float clearcoat_roughness;

  // Fraction of light transmitted through the surface.
  float transmission_factor;

  // Index of refraction used for Fresnel and transmission.
  float ior;

  // Sheen lobe color for fabrics and velvet-like materials.
  vec3 sheen_color;

  // Sheen lobe roughness.
  float sheen_roughness;

  // Additional specular intensity control.
  float specular_factor;

  // Specular reflectance color.
  vec3 specular_color;

  // Thickness of participating medium inside the object.
  float thickness_factor;

  // Mean free path distance before absorption.
  float attenuation_distance;

  // Absorption tint for transmitted light.
  vec3 attenuation_color;

  // Index of base color texture (UINT_MAX = not bound).
  uint base_color_tex;

  // Index of packed metallic-roughness texture.
  uint metallic_roughness_tex;

  // Index of tangent-space normal map.
  uint normal_tex;

  // Index of ambient occlusion texture.
  uint occlusion_tex;

  // Index of emissive texture.
  uint emissive_tex;

  // Index of clearcoat factor texture.
  uint clearcoat_tex;

  // Index of clearcoat roughness texture.
  uint clearcoat_roughness_tex;

  // Index of clearcoat normal map.
  uint clearcoat_normal_tex;

  // Index of transmission texture.
  uint transmission_tex;

  // Index of sheen color texture.
  uint sheen_color_tex;

  // Index of sheen roughness texture.
  uint sheen_roughness_tex;

  // Index of specular intensity texture.
  uint specular_tex;

  // Index of specular color texture.
  uint specular_color_tex;

  // Index of thickness texture.
  uint thickness_tex;
};

// Feature mask: each bit enables a specific shading feature or texture lookup.
// The shader tests bits to decide which BRDF terms and texture samples to evaluate.

// TODO(james): Audit and regroup these. Make sure they are consistent with the C++ values.
const uint FEATURE_BASE_COLOR_TEXTURE = 1u << 0u;
const uint FEATURE_METALLIC_ROUGHNESS_TEXTURE = 1u << 1u;
const uint FEATURE_NORMAL_TEXTURE = 1u << 2u;
const uint FEATURE_OCCLUSION_TEXTURE = 1u << 3u;
const uint FEATURE_EMISSIVE_TEXTURE = 1u << 4u;
const uint FEATURE_CLEARCOAT = 1u << 5u;            // secondary dielectric layer
const uint FEATURE_TRANSMISSION = 1u << 6u;         // refractive transport
const uint FEATURE_SHEEN = 1u << 7u;                // cloth grazing lobe
const uint FEATURE_SHEEN_COLOR_TEXTURE = 1u << 8u;  // cloth grazing lobe
const uint FEATURE_SPECULAR = 1u << 9u;             // specular color control
const uint FEATURE_VOLUME = 1u << 10u;              // absorption inside medium

layout(std430, set = 1, binding = 0) readonly buffer MaterialBuffer { Material materials[]; };

void main() {
  Material mat = materials[material_id];
  out_color = mat.base_color;
}
