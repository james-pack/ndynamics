#include <chrono>
#include <cmath>
#include <thread>

#include "base/initializer.h"
#include "gfx/math.h"
#include "gfx/mesh.h"
#include "gfx/vulkan_renderer.h"

using namespace ndyn::gfx;

namespace {

Quat axis_angle(const Vec3& axis, float angle) {
  const float s{std::sin(angle * 0.5f)};
  return Quat{std::cos(angle * 0.5f), axis.x * s, axis.y * s, axis.z * s}.normalize();
}

}  // namespace

int main(int argc, char* argv[]) {
  ndyn::initialize(&argc, &argv);

  VulkanRenderer renderer{};
  const MeshId mesh{renderer.add_mesh(create_cube(0.2f))};
  const InstanceId id1{renderer.add_instance(Instance{mesh})};

  // {
  //   Camera camera{};
  //   camera.position = {0.f, 0.f, 5.f};
  //   renderer.set_camera(camera);
  // }

  const auto start{std::chrono::high_resolution_clock::now()};

  while (true) {
    const auto now{std::chrono::high_resolution_clock::now()};
    float t = std::chrono::duration<float>(now - start).count();

    Transform position{};
    position.position = {std::sin(t) * 2.f, std::sin(t * 0.5f) * 1.f, 0.f};
    position.rotation = axis_angle({1.f, 1.f, 0.f}, 0.1f * t);

    renderer.update_position(id1, position);
    renderer.render_frame();
  }
}
