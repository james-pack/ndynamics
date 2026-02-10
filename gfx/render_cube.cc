#include <chrono>
#include <cmath>
#include <thread>

#include "base/initializer.h"
#include "gfx/material.h"
#include "gfx/math.h"
#include "gfx/mesh.h"
#include "gfx/vulkan_renderer.h"
#include "glog/logging.h"

using namespace ndyn::gfx;
using namespace std::chrono_literals;

namespace {

Quat axis_angle(const Vec3& axis, float angle) {
  const float s{std::sin(0.5f * angle)};
  return Quat{std::cos(0.5f * angle), s * axis.x, s * axis.y, s * axis.z}.normalize();
}

}  // namespace

static constexpr Material BLUE{.diffuse_color = {0.f, 0.f, 1.f, 1.f},
                               .specular_color = {1.f, 1.f, 1.f, 1.f}};

static constexpr Material GREEN{.diffuse_color = {0.f, 1.f, 0.f, 1.f},
                                .specular_color = {1.f, 1.f, 1.f, 1.f}};

int main(int argc, char* argv[]) {
  ndyn::initialize(&argc, &argv);

  VulkanRenderer renderer{};

  const MeshId mesh{renderer.add_mesh(create_cube(0.7f))};
  const MaterialId blue{renderer.add_material(BLUE)};
  const MaterialId green{renderer.add_material(GREEN)};

  const InstanceId instance1{
      renderer.add_instance(Instance{Mat4::identity(), BLUE.diffuse_color, mesh, blue})};

  const InstanceId instance2{
      renderer.add_instance(Instance{Mat4::identity(), GREEN.diffuse_color, mesh, green})};

  // {
  //   Camera camera{};
  //   camera.position = {0.f, 0.f, 5.f};
  //   renderer.set_camera(camera);
  // }

  Transform instance1_position{};
  Transform instance2_position{};
  const auto start{std::chrono::high_resolution_clock::now()};

  while (true) {
    const auto now{std::chrono::high_resolution_clock::now()};
    float t = std::chrono::duration<float>(now - start).count();

    instance1_position.position = {-0.2f, -0.2f, -0.2f};
    instance1_position.rotation = axis_angle({0.f, 0.f, 1.f}, 0.1f * t);
    Mat4 model_matrix1{create_model_matrix(instance1_position)};
    renderer.update_position(instance1, model_matrix1);

    instance2_position.position = {0.5f, 0.5f, 0.5f};
    instance2_position.rotation = axis_angle({0.f, 0.f, 1.f}, 0.1f * t);
    Mat4 model_matrix2{create_model_matrix(instance2_position)};
    renderer.update_position(instance2, model_matrix2);

    renderer.render_frame();

    std::this_thread::sleep_for(5ms);
  }
}
