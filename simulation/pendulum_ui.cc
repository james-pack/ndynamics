// clang-format off
#include "glad/gl.h"
// clang-format on

#include "base/initializer.h"
#include "base/pi.h"
#include "gflags/gflags.h"
#include "glog/logging.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "sensor/measurement_type.h"
#include "simulation/accelerometer_sensor_model.h"
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

DEFINE_bool(fullscreen, false, "Run in fullscreen mode (windowless)");

int main(int argc, char* argv[]) {
  using namespace ndyn::sensor;
  using namespace ndyn::simulation;
  using namespace ndyn::ui;

  FLAGS_logtostderr = true;
  ndyn::initialize(&argc, &argv);

  static constexpr size_t NUM_POINTS{1024};
  using PendulumGraphType = PendulumGraph<NUM_POINTS>;
  using FloatT = PendulumGraphType::FloatT;
  using PendulumType = Pendulum<PendulumGraphType::T>;
  using PendulumConfiguratorType = PendulumConfigurator<PendulumGraphType::T>;
  using AccelerometerType = MeasurementValueType<MeasurementType::ACCELEROMETER>::type;
  using TemperatureType = MeasurementValueType<MeasurementType::TEMPERATURE>::type;

  size_t width{FLAGS_fullscreen ? 0UL : 1920UL};
  size_t height{FLAGS_fullscreen ? 0UL : 1080UL};

  App app{"Pendulum Simulation", width, height};

  PendulumType pendulum{PendulumConfiguratorType{}
                            .set_length(FLAGS_length)
                            .set_g(FLAGS_gravity)
                            .set_mass(FLAGS_mass)
                            .set_theta(FLAGS_angle)
                            .create()};

  PendulumModel<NUM_POINTS> pendulum_model{pendulum};
  PositionModel<PendulumType, FloatT, NUM_POINTS> position_model{pendulum};

  static constexpr TemperatureType TEMPERATURE{25};
  Characterization<AccelerometerType, FloatT> accelerometer_1_characterization{
      Characteristic<FloatT>{.temperature{TEMPERATURE}, .offset_average{1.025}, .offset_std{.05}}};

  AccelerometerSensorModel<PendulumType, NUM_POINTS> accelerometer_1{
      pendulum, accelerometer_1_characterization};

  Characterization<AccelerometerType, FloatT> accelerometer_2_characterization{
      Characteristic<FloatT>{.temperature{TEMPERATURE}, .offset_average{1.05}, .offset_std{.25}}};

  AccelerometerSensorModel<PendulumType, NUM_POINTS> accelerometer_2{
      pendulum, accelerometer_2_characterization};

  Characterization<AccelerometerType, FloatT> accelerometer_3_characterization{
      Characteristic<FloatT>{.temperature{TEMPERATURE}, .offset_average{1.15}, .offset_std{.5}}};

  AccelerometerSensorModel<PendulumType, NUM_POINTS> accelerometer_3{
      pendulum, accelerometer_3_characterization};

  app.add_model(pendulum_model);
  app.add_model(position_model);
  app.add_model(accelerometer_1);
  app.add_model(accelerometer_2);
  app.add_model(accelerometer_3);

  Window ui{};
  app.set_root_ui_element(ui);

  PendulumGraphType statistics{pendulum_model, position_model};
  ui.add_left_child(statistics);

  SensorMeasurementGraph<PendulumType, NUM_POINTS> sensor_measurements{};
  sensor_measurements.add_accelerometer(accelerometer_1);
  sensor_measurements.add_accelerometer(accelerometer_2);
  sensor_measurements.add_accelerometer(accelerometer_3);
  ui.add_right_child(sensor_measurements);

  CubePositionFn cube_as_pendulum{[&pendulum]() {
    const auto x{pendulum.position().x()};
    const auto y{pendulum.position().y() + FLAGS_length / 2};
    const auto z{pendulum.position().z()};
    const auto theta{pendulum.theta()};
    return glm::rotate(glm::translate(glm::mat4{1.f}, glm::vec3{x, y, z}), theta,
                       glm::vec3{0, 0, 1});
  }};
  Cube cube{app.gl_window(), cube_as_pendulum};
  app.add_direct_render_element(cube);

  app.run();

  return 0;
}
