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

// Matrix to transform model-space coordinates into Vulkan view space.
// Model coordinates x,y,z on [-1,1] with +x pointing to the right, +y pointing up, and +z pointing
// out of the screen. Vulkan coordinates for the x and y axes are the same, but Vulkan has the z
// axis on [0,1] with +z pointing into the screen. This transform flips and scales the z axis and
// translates it to achieve the desired coordinate system.
// Also, the y-axis is flipped to align Vulkan’s NDC with the windowing system's coordinates.
const mat4 model_to_view_transform = mat4(  //
    1.0, 0.0, 0.0, 0.0,                     //
    0.0, -1.0, 0.0, 0.0,                    //
    0.0, 0.0, 0.5, 0.0,                     //
    0.0, 0.0, 0.5, 1.0                      //
);

void main() {
  mat4 model_position_orientation = instances[gl_InstanceIndex].position;
  gl_Position = model_to_view_transform * model_position_orientation * vec4(in_pos, 1.0);
  material_id = instances[gl_InstanceIndex].material_id;
}
