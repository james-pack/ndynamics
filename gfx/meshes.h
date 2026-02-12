#pragma once

#include "gfx/mesh.h"

namespace ndyn::gfx {

Mesh create_triangle(float side_length);
Mesh create_cube(float size);
Mesh create_cylinder(float radius, float height, unsigned int segments);

}  // namespace ndyn::gfx
