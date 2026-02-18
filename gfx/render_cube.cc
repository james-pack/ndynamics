#include <array>
#include <chrono>
#include <cmath>
#include <numbers>
#include <thread>

#include "base/initializer.h"
#include "gflags/gflags.h"
#include "gfx/camera.h"
#include "gfx/material.h"
#include "gfx/math.h"
#include "gfx/mesh.h"
#include "gfx/meshes.h"
#include "gfx/vulkan_renderer.h"
#include "glog/logging.h"

DEFINE_uint32(num_cubes, 3, "Number of cubes to display.");

using namespace ndyn::gfx;
using namespace std::chrono_literals;

static constexpr float DEFAULT_SHININESS{0.75f};
static constexpr Vec4 BRASSY_COLOR{0.7961f, 0.4275f, 0.3176f, 1.f};
static constexpr Vec4 WHITE_COLOR{1.f, 1.f, 1.f, 1.f};
static constexpr Vec4 DEFAULT_SPECULAR_COLOR{WHITE_COLOR};
static constexpr float DEFAULT_OPACITY{1.f};

static constexpr Material WHITE{
    .diffuse_color = {1.f, 1.f, 1.f, 1.f},
    .specular_color = DEFAULT_SPECULAR_COLOR,
    .shininess = DEFAULT_SHININESS,
    .opacity = DEFAULT_OPACITY,
};

static constexpr Material RED{
    .diffuse_color = {1.f, 0.f, 0.f, 1.f},
    .specular_color = DEFAULT_SPECULAR_COLOR,
    .shininess = DEFAULT_SHININESS,
    .opacity = DEFAULT_OPACITY,
};

static constexpr Material GREEN{
    .diffuse_color = {0.f, 1.f, 0.f, 1.f},
    .specular_color = DEFAULT_SPECULAR_COLOR,
    .shininess = DEFAULT_SHININESS,
    .opacity = DEFAULT_OPACITY,
};

static constexpr Material BLUE{
    .diffuse_color = {0.f, 0.f, 1.f, 1.f},
    .specular_color = DEFAULT_SPECULAR_COLOR,
    .shininess = DEFAULT_SHININESS,
    .opacity = DEFAULT_OPACITY,
};

static constexpr Material YELLOW{
    .diffuse_color = {1.f, 1.f, 0.f, 1.f},
    .specular_color = DEFAULT_SPECULAR_COLOR,
    .shininess = DEFAULT_SHININESS,
    .opacity = DEFAULT_OPACITY,
};

static constexpr Material CYAN{
    .diffuse_color = {0.f, 1.f, 1.f, 1.f},
    .specular_color = DEFAULT_SPECULAR_COLOR,
    .shininess = DEFAULT_SHININESS,
    .opacity = DEFAULT_OPACITY,
};

static constexpr Material MAGENTA{
    .diffuse_color = {1.f, 0.f, 1.f, 1.f},
    .specular_color = DEFAULT_SPECULAR_COLOR,
    .shininess = DEFAULT_SHININESS,
    .opacity = DEFAULT_OPACITY,
};

int main(int argc, char* argv[]) {
  ndyn::initialize(&argc, &argv);

  VulkanRenderer renderer{};

  const MeshId mesh{renderer.add_mesh(create_cube(0.25f))};
  const MaterialId white{renderer.add_material(WHITE)};
  std::array materials{
      renderer.add_material(CYAN), renderer.add_material(MAGENTA), renderer.add_material(YELLOW),
      renderer.add_material(RED),  renderer.add_material(GREEN),   renderer.add_material(BLUE),
  };

  std::vector<InstanceId> instances{};
  instances.reserve(FLAGS_num_cubes + 1);

  // Add an instance at the origin to make the origin easier to find.
  const Position origin{};
  instances.push_back(renderer.add_instance(Instance{origin.as_matrix_transform(), mesh, white}));

  for (size_t i = 0; i < FLAGS_num_cubes; ++i) {
    instances.push_back(
        renderer.add_instance(Instance{Mat4::identity(), mesh, materials[i % materials.size()]}));
  }

  PerspectiveCamera camera{Position{{0.f, 0.f, 3.f}, {}},
                           /* ~85 degrees in radians */ 1.5f,
                           /* square aspect ratio */ 1.f, /* near */ 1.f,
                           /* far */ 10.f};

  const auto start{std::chrono::high_resolution_clock::now()};
  while (true) {
    const auto now{std::chrono::high_resolution_clock::now()};
    float t = std::chrono::duration<float>(now - start).count();

    camera.pose = {{3.f * std::sin(t), 0.f, 3.f * std::cos(t)}, Quat::axis_angle({0, 1, 0}, t)};
    renderer.update_camera(camera.make_camera_state());

    for (size_t i = 0; i < FLAGS_num_cubes; ++i) {
      Position instance_position{};
      instance_position.position = {1.f /** std::cos(t - i / 25.f)*/,  //
                                    1.f /** std::sin(t - i / 25.f)*/,  //
                                    0.9f - 0.5f * i};
      // instance_position.orientation = Quat::axis_angle({1.f, 1.f, 1.f}, t);
      Mat4 model_matrix{instance_position.as_matrix_transform()};
      renderer.update_position(instances[i + 1], model_matrix);
    }

    renderer.render_frame();

    std::this_thread::sleep_for(5ms);
  }
}
