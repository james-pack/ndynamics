#version 450

struct Material {
  vec4 diffuse;
  vec4 specular;
  float shininess;
  float opacity;
  uint texture_index;
};
layout(std430, set = 1, binding = 0) readonly buffer MaterialBuffer { Material materials[]; };

layout(location = 0) in flat uint material_id;
layout(location = 0) out vec4 out_color;

void main() {
  Material mat = materials[material_id];
  out_color = mat.diffuse;
}
