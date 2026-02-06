#version 450

layout(location = 0) in vec3 in_pos;
layout(location = 1) in vec3 in_normal;

void main() { gl_Position = vec4(in_pos.x, in_pos.y, in_pos.z, 1.0); }
