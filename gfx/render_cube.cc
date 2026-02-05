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
  float s = std::sin(angle * 0.5f);
  return {std::cos(angle * 0.5f), axis.x * s, axis.y * s, axis.z * s};
}

}  // namespace

int main(int argc, char* argv[]) {
  ndyn::initialize(&argc, &argv);

  VulkanRenderer renderer{};
  // const std::size_t cube_id{renderer.add_mesh(create_cube(1.f))};

  // {
  //   Camera camera{};
  //   camera.position = {0.f, 0.f, 5.f};
  //   renderer.set_camera(camera);
  // }

  auto start = std::chrono::high_resolution_clock::now();

  while (true) {
    auto now = std::chrono::high_resolution_clock::now();
    float t = std::chrono::duration<float>(now - start).count();

    Transform tr{};
    tr.position = {std::sin(t) * 2.f, std::sin(t * 0.5f) * 1.f, 0.f};
    tr.rotation = axis_angle({0.f, 1.f, 0.f}, t) * axis_angle({1.f, 0.f, 0.f}, t * 0.7f);

    // renderer.update_position(cube_id, tr);
    renderer.render_frame();

    std::this_thread::sleep_for(std::chrono::milliseconds(16));
  }
}
