// clang-format off
#include "glad/gl.h"
// clang-format on

#include "base/initializer.h"
#include "base/pi.h"
#include "gflags/gflags.h"
#include "glog/logging.h"
#include "simulation/cube_ui.h"
#include "simulation/pendulum.h"
#include "simulation/pendulum_graph_ui.h"
#include "ui/app.h"
#include "ui/ui_elements.h"

DEFINE_double(length, 1, "Length of the pendulum");
DEFINE_double(gravity, 1, "Acceleration due to gravity");
DEFINE_double(mass, 1, "Mass of the pendulum");
DEFINE_double(angle, ndyn::pi / 4, "Initial angle of the pendulum in radians. Defaults to pi/4.");

int main(int argc, char* argv[]) {
  using namespace ndyn::simulation;
  using namespace ndyn::ui;

  FLAGS_logtostderr = true;
  ndyn::initialize(&argc, &argv);

  Pendulum<PendulumGraph::T> pendulum{PendulumConfigurator<PendulumGraph::T>{}
                                          .set_length(FLAGS_length)
                                          .set_g(FLAGS_gravity)
                                          .set_mass(FLAGS_mass)
                                          .set_theta(FLAGS_angle)
                                          .create()};

  App app{"Pendulum Graph", 1920, 1080};

  Window ui{};
  PendulumGraph statistics{pendulum};
  ui.add_child(statistics);
  app.set_root_ui_element(ui);

  Cube cube{app.gl_window()};
  app.add_direct_render_element(cube);

  app.run();

  return 0;
}
