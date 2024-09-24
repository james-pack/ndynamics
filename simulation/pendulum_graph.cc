// clang-format off
#include "glad/gl.h"
// clang-format on

#include "base/initializer.h"
#include "base/pi.h"
#include "gflags/gflags.h"
#include "glog/logging.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "simulation/cube_ui.h"
#include "simulation/pendulum.h"
#include "simulation/pendulum_graph_ui.h"
#include "simulation/sensor_measurement_graph_ui.h"
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
  ui.add_left_child(statistics);
  SensorMeasurementGraph sensor_measurements{pendulum};
  ui.add_right_child(sensor_measurements);
  app.set_root_ui_element(ui);

  CubePositionFn cube_as_pendulum{[&pendulum]() {
    const auto x{pendulum.position().component(1)};
    const auto y{pendulum.position().component(2) + FLAGS_length / 2};
    const auto z{pendulum.position().component(4)};
    const auto theta{pendulum.theta()};
    return glm::rotate(glm::translate(glm::mat4{1.f}, glm::vec3{x, y, z}), theta,
                       glm::vec3{0, 0, 1});
  }};

  Cube cube{app.gl_window(), cube_as_pendulum};
  app.add_direct_render_element(cube);

  app.run();

  return 0;
}
