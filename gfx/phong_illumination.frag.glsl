#version 450

layout(location = 0) in vec3 frag_pos;
layout(location = 1) in vec3 frag_normal;
layout(location = 2) in flat uint material_id;

layout(location = 0) out vec4 out_color;

struct Material {
  vec4 diffuse;
  vec4 specular;
  float shininess;
  float opacity;
  uint texture_index;
};

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
layout(std140, set = 2, binding = 1) readonly uniform LightBuffer { Light lights[16]; };

void main() {
  Material mat = materials[material_id];

  vec3 N = normalize(frag_normal);
  vec3 V = camera.position.xyz;
  vec3 L;
  float attenuation = 1.0;

  vec3 color = vec3(0.0, 0.0, 0.0);
  for (uint i = 0; i < 3; ++i) {
    Light light = lights[i];
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

    float diff = max(dot(N, L), 0.0);
    vec3 diffuse = diff * mat.diffuse.rgb * light.color.rgb * light.color.a * attenuation;

    vec3 R = reflect(-L, N);
    float spec = pow(max(dot(R, V), 0.0), mat.shininess);
    vec3 specular = spec * mat.specular.rgb * light.color.rgb * light.color.a * attenuation;

    color += diffuse + specular;
  }
  out_color = vec4(color, mat.opacity);
}
