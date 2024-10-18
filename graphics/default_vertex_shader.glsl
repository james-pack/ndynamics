#version 330 core

// Vertex position in model coordinates.
layout(location = 0) in vec3 vertex_position;
layout(location = 1) in vec3 vertex_color;

// Forward to fragment shader.
out vec3 fragment_color;

uniform mat4 projection_matrix;
uniform mat4 camera_matrix;

// Matrix to transform from model coordinates to world coordinates.
uniform mat4 element_matrix;

void main() {
  // Transform vertex from model coordinates to world coordinates then view through a camera
  // followed by a projection.
  gl_Position = projection_matrix * camera_matrix * element_matrix * vec4(vertex_position, 1);
  fragment_color = vertex_color;
}
