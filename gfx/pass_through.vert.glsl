#version 450

layout(location = 0) in vec3 in_pos;
layout(location = 1) in vec3 in_normal;

layout(std430, set = 0, binding = 0) readonly buffer InstanceTransforms
{
    mat4 model[];
};

void main()
{
    mat4 M = model[gl_InstanceIndex];
    gl_Position = M * vec4(in_pos, 1.0);
}
