// clang-format off
#include "glad/gl.h"
#include "GLFW/glfw3.h"
// clang-format on

#include "graphics/gpu_element.h"
#include "graphics/model.h"
#include "graphics/part_builder.h"
#include "graphics/graphics_test_utils.h"
#include "gtest/gtest.h"
#include "math/algebra.h"
#include "math/geometry.h"

namespace ndyn::graphics {

using ScalarType = float;
using AlgebraType = math::Algebra<ScalarType, 3, 0, 1>;
using GeometryType = math::Geometry3D<AlgebraType>;
using PartBuilderType = PartBuilder<GeometryType>;
using ElementType = GpuElement<GeometryType>;
using ModelType = Model<GeometryType>;

class PartBuilderTest : public GraphicsTest {};

TEST_F(PartBuilderTest, CanCompile) {
  ElementType *revolute_joint{};
  PartBuilderType builder{};
  builder  //
      .add_revolute_joint(
          GeometryType::z_axis)  // Add a revolute joint that rotates around the z_axis.
      .bind(&revolute_joint)     // Bind the revolute joint to a pointer.
      .down()         // Indicate that we are going to add children to the revolute joint.
      .add_sphere(2)  // Add a sphere that graphically displays the rotation of the revolute joint.
      .add_offset(2, GeometryType::y_axis)     // Add an offset of 2 in the y direction.
      .down()                                  // The next items will be children of the offset.
      .add_rod(0.25, 3, GeometryType::y_axis)  // Add a rod (cylinder) of length 3 oriented in the y
                                               // direction with a radius of 0.25.
      .down()                                  // The next items will be children of the rod.
      .add_offset(1, GeometryType::y_axis)     // Add an offset of 2 in the y direction to the rod.
      .down()                                  // The next items will be children of the offset.
      .add_cube(1);                            // Add a cube at the end of the offset.
  ModelType model{};
  model.add_element(builder.build());
}

}  // namespace ndyn::graphics
