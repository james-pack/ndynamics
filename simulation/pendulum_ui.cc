// clang-format off
#include "glad/gl.h"
// clang-format on

#include "base/initializer.h"
#include "base/pi.h"
#include "gflags/gflags.h"
#include "glog/logging.h"
#include "simulation/pendulum_scene.h"
#include "ui/app.h"

DEFINE_double(length, 3.5, "Length of the pendulum");
DEFINE_double(gravity, 9.8, "Acceleration due to gravity");
DEFINE_double(mass, 1, "Mass of the pendulum");
DEFINE_double(angle, ndyn::pi / 16, "Initial angle of the pendulum in radians. Defaults to pi/16.");

DEFINE_bool(fullscreen, false, "Run in fullscreen mode (windowless)");

int main(int argc, char* argv[]) {
  FLAGS_logtostderr = true;
  ndyn::initialize(&argc, &argv);

  size_t width{FLAGS_fullscreen ? 0UL : 1920UL};
  size_t height{FLAGS_fullscreen ? 0UL : 1080UL};

  ndyn::ui::App app{"Pendulum Simulation", width, height};

  ndyn::simulation::PendulumScene default_scene{
      app.gl_window(), static_cast<float>(FLAGS_length), static_cast<float>(FLAGS_gravity),
      static_cast<float>(FLAGS_mass), static_cast<float>(FLAGS_angle)};
  app.add_scene(default_scene);

  ndyn::simulation::PendulumScene scene1{app.gl_window(), 3.5, 9.8, 1., ndyn::pi / 4};
  app.add_scene(scene1);

  ndyn::simulation::PendulumScene scene3{app.gl_window(), 3.5, 9.8, 1., ndyn::pi / 2};
  app.add_scene(scene3);

  ndyn::simulation::PendulumScene scene4{app.gl_window(), 1, 9.8, 1., 3.};
  app.add_scene(scene4);

  ndyn::simulation::PendulumScene scene5{app.gl_window(), .25, 9.8, 1., 3.};
  app.add_scene(scene5);

  ndyn::simulation::PendulumScene scene6{app.gl_window(), 3.5, 50., 1., 3.};
  app.add_scene(scene6);

  ndyn::simulation::PendulumScene scene7{app.gl_window(), .5, 50., 1., 3.};
  app.add_scene(scene7);

  app.run();

  return 0;
}
