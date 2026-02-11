#version 450

struct Instance {
  mat4 position;
  uint mesh_id;
  uint material_id;
};
layout(std430, set = 0, binding = 0) readonly buffer InstanceBuffer { Instance instances[]; };

layout(location = 0) in vec3 in_pos;
layout(location = 1) in vec3 in_normal;

layout(location = 0) out flat uint material_id;

void main() {
  mat4 M = instances[gl_InstanceIndex].position;
  gl_Position = M * vec4(in_pos, 1.0);
  material_id = instances[gl_InstanceIndex].material_id;
}
