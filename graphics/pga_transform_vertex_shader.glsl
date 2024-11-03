#version 330 core

// PGA abstractions.
#define motor mat2x4    // [ [s, e23, e31, e12], [e01, e02, e03, e0123] ]
#define line mat2x3     // [ [e23, e31, e12], [e01, e02, e03] ]
#define point vec3      // [ e032, e013, e021 ] implied 1 e123
#define direction vec3  // [ e032, e013, e021 ] implied 0 e123

// Vertex position in model coordinates.
layout(location = 0) in point vertex;

// Forward to fragment shader.
layout(location = 1) in vec3 vertex_color;
out vec3 fragment_color;

// Gibbs vector-style matrices.
uniform mat4 projection_matrix;
uniform mat4 camera_matrix;

// PGA motor to transform vertex from model coordinates to world coordinates.
uniform motor element_transform;

point apply_transform(motor transform, point p) {
  direction t = cross(p, transform[0].yzw) - transform[1].xyz;
  return 2. *
             (transform[0].x * t + cross(t, transform[0].yzw) - transform[0].yzw * transform[1].w) +
         p;
}

void main() {
  vec3 world_coord_position = apply_transform(element_transform, vertex);
  // Transform vertex in world coordinates according to camera view followed by a projection.
  gl_Position = projection_matrix * camera_matrix * vec4(world_coord_position, 1);
  fragment_color = vertex_color;
}
