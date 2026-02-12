#include <array>
#include <chrono>
#include <cmath>
#include <thread>

#include "base/initializer.h"
#include "gflags/gflags.h"
#include "gfx/material.h"
#include "gfx/math.h"
#include "gfx/mesh.h"
#include "gfx/vulkan_renderer.h"
#include "glog/logging.h"

DEFINE_uint32(num_cubes, 3, "Number of cubes to display.");

using namespace ndyn::gfx;
using namespace std::chrono_literals;

namespace {

Quat axis_angle(const Vec3& axis, float angle) {
  const float s{std::sin(0.5f * angle)};
  return Quat{std::cos(0.5f * angle), s * axis.x, s * axis.y, s * axis.z}.normalize();
}

}  // namespace

static constexpr Material RED{.diffuse_color = {1.f, 0.f, 0.f, 1.f},
                              .specular_color = {1.f, 1.f, 1.f, 1.f}};

static constexpr Material GREEN{.diffuse_color = {0.f, 1.f, 0.f, 1.f},
                                .specular_color = {1.f, 1.f, 1.f, 1.f}};

static constexpr Material BLUE{.diffuse_color = {0.f, 0.f, 1.f, 1.f},
                               .specular_color = {1.f, 1.f, 1.f, 1.f}};

static constexpr Material YELLOW{.diffuse_color = {1.f, 1.f, 0.f, 1.f},
                                 .specular_color = {1.f, 1.f, 1.f, 1.f}};

static constexpr Material CYAN{.diffuse_color = {0.f, 1.f, 1.f, 1.f},
                               .specular_color = {1.f, 1.f, 1.f, 1.f}};

static constexpr Material MAGENTA{.diffuse_color = {1.f, 0.f, 1.f, 1.f},
                                  .specular_color = {1.f, 1.f, 1.f, 1.f}};

int main(int argc, char* argv[]) {
  ndyn::initialize(&argc, &argv);

  VulkanRenderer renderer{};

  const MeshId mesh{renderer.add_mesh(create_cube(0.5f))};

  std::array materials{
      renderer.add_material(CYAN), renderer.add_material(MAGENTA), renderer.add_material(YELLOW),
      renderer.add_material(RED),  renderer.add_material(GREEN),   renderer.add_material(BLUE),
  };

  std::vector<InstanceId> instances{};
  instances.reserve(FLAGS_num_cubes);
  for (size_t i = 0; i < FLAGS_num_cubes; ++i) {
    instances.push_back(
        renderer.add_instance(Instance{Mat4::identity(), mesh, materials[i % materials.size()]}));
  }

  // {
  //   Camera camera{};
  //   camera.position = {0.f, 0.f, 5.f};
  //   renderer.set_camera(camera);
  // }

  const auto start{std::chrono::high_resolution_clock::now()};

  while (true) {
    const auto now{std::chrono::high_resolution_clock::now()};
    float t = std::chrono::duration<float>(now - start).count();

    for (size_t i = 0; i < FLAGS_num_cubes; ++i) {
      Position instance_position{};
      instance_position.position = {-2.8f + 1.7f * i, -0.8f + 0.7f * i, 0.2f + 1.7f * i};
      instance_position.orientation = axis_angle({1.f, 0.f, 1.f}, 0.1f * t);
      Mat4 model_matrix{create_model_matrix(instance_position)};
      renderer.update_position(instances[i], model_matrix);
    }

    renderer.render_frame();

    std::this_thread::sleep_for(5ms);
  }
}
