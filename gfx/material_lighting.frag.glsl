#version 450

layout(location = 0) in vec3 frag_pos;
layout(location = 1) in vec3 frag_normal;
layout(location = 2) in vec3 frag_uv;
layout(location = 3) in flat uint material_id;

layout(location = 0) out vec4 out_color;

layout(constant_id = 0) const uint MAX_NUM_LIGHTS = 16;

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

struct CameraState {
  mat4 view;
  mat4 projection;
  mat4 view_projection;
  vec4 position;
  vec2 projection_params;
  vec2 clip_params;
  bool is_perspective;
};
layout(std140, set = 2, binding = 0) readonly uniform CameraStateBuffer { CameraState camera; };

struct Light {
  vec4 position;
  vec4 direction;

  vec4 color;

  float spot_inner;
  float spot_outer;

  uint type;
};
layout(std140, set = 2, binding = 1) readonly uniform LightBuffer { Light lights[MAX_NUM_LIGHTS]; };

vec4 sample_texture(uint index, vec3 uv) {
  // Implement textures using an SSBO.
  return vec4(0.0);
}

void main() {
  Material mat = materials[material_id];

  vec3 N = normalize(frag_normal);
  vec3 V = normalize(camera.position.xyz - frag_pos);

  vec3 color = vec3(0.0);

  for (uint i = 0; i < MAX_NUM_LIGHTS; ++i) {
    Light light = lights[i];

    vec3 L = vec3(0.0);
    float attenuation = 1.0;

    // Determine light direction and attenuation
    if (light.type == 0) {  // point light
      vec3 to_light = light.position.xyz - frag_pos;
      float dist = length(to_light);
      L = normalize(to_light);
      attenuation = clamp(1.0 / (dist * dist), 0.0, 1.0);
    } else if (light.type == 1) {  // directional
      L = normalize(-light.direction.xyz);
    } else if (light.type == 2) {  // spot
      vec3 to_light = light.position.xyz - frag_pos;
      float dist = length(to_light);
      L = normalize(to_light);
      float cos_theta = dot(-L, normalize(light.direction.xyz));
      float spot_factor = smoothstep(light.spot_outer, light.spot_inner, cos_theta);
      attenuation = clamp(spot_factor / (dist * dist), 0.0, 1.0);
    }

    // Apply ambient occlusion
    float ao = 1.0;
    if ((mat.feature_mask & FEATURE_OCCLUSION_TEXTURE) != 0u) {
      ao = sample_texture(mat.occlusion_tex, frag_uv).r * mat.occlusion_strength;
    }

    // Base color + metallic/roughness
    vec3 base_color = mat.base_color.rgb;
    if ((mat.feature_mask & FEATURE_BASE_COLOR_TEXTURE) != 0u) {
      base_color *= sample_texture(mat.base_color_tex, frag_uv).rgb;
    }

    float metallic = mat.metallic_factor;
    float roughness = mat.roughness_factor;
    if ((mat.feature_mask & FEATURE_METALLIC_ROUGHNESS_TEXTURE) != 0u) {
      vec4 mr = sample_texture(mat.metallic_roughness_tex, frag_uv);
      roughness *= mr.r;
      metallic *= mr.g;
    }

    // Normal mapping
    vec3 NN = N;
    if ((mat.feature_mask & FEATURE_NORMAL_TEXTURE) != 0u) {
      vec3 normal_map = sample_texture(mat.normal_tex, frag_uv).xyz * 2.0 - 1.0;
      NN = normalize(N + normal_map * mat.normal_scale);
    }

    // Fresnel-Schlick
    vec3 F0 = mix(vec3(0.04), base_color, metallic);

    // Light vector
    vec3 H = normalize(L + V);
    float NdotL = max(dot(NN, L), 0.0);
    float NdotV = max(dot(NN, V), 0.0);
    float NdotH = max(dot(NN, H), 0.0);
    float VdotH = max(dot(V, H), 0.0);

    // Microfacet specular (GGX)
    float alpha = roughness * roughness;
    float alpha2 = alpha * alpha;
    float denom = NdotH * NdotH * (alpha2 - 1.0) + 1.0;
    float D = alpha2 / (3.14159265 * denom * denom);

    float k = alpha / 2.0;
    float G1V = NdotV / (NdotV * (1.0 - k) + k);
    float G1L = NdotL / (NdotL * (1.0 - k) + k);
    float G = G1V * G1L;

    vec3 F = F0 + (1.0 - F0) * pow(1.0 - VdotH, 5.0);
    vec3 specular = (D * G * F) / (4.0 * NdotV * NdotL + 0.001);

    // Diffuse term (Lambert, energy-conserving)
    vec3 kD = (1.0 - F) * (1.0 - metallic);
    vec3 diffuse = kD * base_color / 3.14159265;

    // Clearcoat
    if ((mat.feature_mask & FEATURE_CLEARCOAT) != 0u) {
      float cc_roughness = mat.clearcoat_roughness;
      vec3 cc_F0 = vec3(0.04);
      float cc_alpha2 = cc_roughness * cc_roughness;
      float cc_denom = NdotH * NdotH * (cc_alpha2 - 1.0) + 1.0;
      float cc_D = cc_alpha2 / (3.14159265 * cc_denom * cc_denom);
      float cc_k = cc_roughness / 2.0;
      float cc_G1V = NdotV / (NdotV * (1.0 - cc_k) + cc_k);
      float cc_G1L = NdotL / (NdotL * (1.0 - cc_k) + cc_k);
      float cc_G = cc_G1V * cc_G1L;
      vec3 cc_F = cc_F0 + (1.0 - cc_F0) * pow(1.0 - VdotH, 5.0);
      specular += mat.clearcoat_factor * (cc_D * cc_G * cc_F) / (4.0 * NdotV * NdotL + 0.001);
    }

    // Sheen
    if ((mat.feature_mask & FEATURE_SHEEN) != 0u) {
      vec3 sheen_color = mat.sheen_color;
      if ((mat.feature_mask & FEATURE_SHEEN_COLOR_TEXTURE) != 0u) {
        sheen_color *= sample_texture(mat.sheen_color_tex, frag_uv).rgb;
      }
      float sheen_roughness = mat.sheen_roughness;
      diffuse += sheen_color * pow(1.0 - NdotL, 5.0) * sheen_roughness;
    }

    // Transmission
    if ((mat.feature_mask & FEATURE_TRANSMISSION) != 0u) {
      vec3 trans_color = mat.base_color.rgb;
      if ((mat.feature_mask & FEATURE_VOLUME) != 0u) {
        trans_color *= exp(-mat.attenuation_color * mat.attenuation_distance);
      }
      diffuse = mix(diffuse, trans_color, mat.transmission_factor);
    }

    // Emissive
    vec3 emissive = mat.emissive_factor;
    if ((mat.feature_mask & FEATURE_EMISSIVE_TEXTURE) != 0u) {
      emissive *= sample_texture(mat.emissive_tex, frag_uv).rgb;
    }

    color += (diffuse + specular) * NdotL * attenuation * ao + emissive;
  }

  out_color = vec4(color, mat.base_color.a);
}
