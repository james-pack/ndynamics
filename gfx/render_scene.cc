#include <array>
#include <chrono>
#include <cmath>
#include <numbers>
#include <thread>

#include "base/initializer.h"
#include "gflags/gflags.h"
#include "gfx/camera.h"
#include "gfx/material.h"
#include "gfx/materials.h"
#include "gfx/math.h"
#include "gfx/mesh.h"
#include "gfx/meshes.h"
#include "gfx/vulkan_renderer.h"
#include "glog/logging.h"

DEFINE_uint32(num_objects, 3, "Number of different objects to display.");

using namespace ndyn::gfx;
using namespace std::chrono_literals;

int main(int argc, char* argv[]) {
  ndyn::initialize(&argc, &argv);

  VulkanRenderer renderer{};

  const MeshId center_mesh{renderer.add_mesh(create_icosphere<3>(0.1f))};
  std::vector<MeshId> meshes{
      renderer.add_mesh(create_icosphere<0>(0.25f)),
      renderer.add_mesh(create_icosphere<1>(0.25f)),
      renderer.add_mesh(create_icosphere<2>(0.25f)),
  };
  const MaterialId white{renderer.add_material(MATTE_WHITE)};
  std::array materials{
      renderer.add_material(GLOSSY_CYAN),   renderer.add_material(GLOSSY_MAGENTA),
      renderer.add_material(GLOSSY_YELLOW), renderer.add_material(GLOSSY_RED),
      renderer.add_material(GLOSSY_GREEN),  renderer.add_material(GLOSSY_BLUE),
  };

  std::vector<InstanceId> instances{};
  instances.reserve(FLAGS_num_objects + 1);

  // Add an instance at the origin to make the origin easier to find.
  const Position origin{};
  instances.push_back(
      renderer.add_instance(Instance{origin.as_matrix_transform(), center_mesh, white}));

  for (size_t i = 0; i < FLAGS_num_objects; ++i) {
    instances.push_back(renderer.add_instance(
        Instance{Mat4::identity(), meshes.at(i % meshes.size()), materials[i % materials.size()]}));
  }

  PerspectiveCamera camera{Position{{0.f, 0.f, 3.f}, {}},
                           /* ~85 degrees in radians */ 1.5f,
                           /* square aspect ratio */ 1.f, /* near */ 1.f,
                           /* far */ 10.f};

  size_t frame_count{0};
  const auto start{std::chrono::high_resolution_clock::now()};
  while (true) {
    const auto now{std::chrono::high_resolution_clock::now()};
    float t = std::chrono::duration<float>(now - start).count();

    camera.pose = {{3.f * std::sin(t), 0.f, 3.f * std::cos(t)}, Quat::axis_angle({0, 1, 0}, t)};
    renderer.update_camera(camera.make_camera_state());

    for (size_t i = 0; i < FLAGS_num_objects; ++i) {
      Position instance_position{};
      const float phase{static_cast<float>(i * 2.f * M_PI / FLAGS_num_objects)};
      instance_position.position = {1.f * std::cos(t - phase),  //
                                    1.f * std::sin(t - phase),  //
                                    1.f * std::sin(t / 4.f - phase)};
      instance_position.orientation = Quat::axis_angle({1.f, 1.f, 1.f}, 3.f * t - phase);
      Mat4 model_matrix{instance_position.as_matrix_transform()};
      renderer.update_position(instances[i + 1], model_matrix);
    }

    renderer.render_frame();
    ++frame_count;

    // Naive FPS calculation. Averages all frames over all time, rather than a weighted average of
    // the last N frames.
    const float avg_fps{frame_count / t};
    if (avg_fps > 200) {
      std::this_thread::sleep_for(5ms);
    }
  }
}
