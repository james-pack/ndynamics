#version 450

struct Instance {
  mat4 pose;
  uint mesh_id;
  uint material_id;
};
layout(std430, set = 0, binding = 0) readonly buffer InstanceBuffer { Instance instances[]; };

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

layout(location = 0) in vec3 in_pos;
layout(location = 1) in vec3 in_normal;

layout(location = 0) out vec3 frag_pos;
layout(location = 1) out vec3 frag_normal;
layout(location = 2) out flat uint material_id;

void main() {
  mat4 model_pose = instances[gl_InstanceIndex].pose;
  vec4 world_pos = model_pose * vec4(in_pos, 1.0);
  gl_Position = camera.view_projection * world_pos;
  frag_pos = vec3(world_pos.x, world_pos.y, world_pos.z);
  frag_normal = in_normal;
  material_id = instances[gl_InstanceIndex].material_id;
}
