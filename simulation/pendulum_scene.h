#pragma once

#include <string>

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "math/state.h"
#include "sensor/measurement_type.h"
#include "simulation/accelerometer_sensor_model.h"
#include "simulation/old_cube_ui.h"
#include "simulation/pendulum.h"
#include "simulation/pendulum_graph_ui.h"
#include "simulation/pendulum_ui_model.h"
#include "simulation/position_ui_model.h"
#include "simulation/sensor_measurement_graph_ui.h"
#include "ui/app.h"
#include "ui/scene.h"
#include "ui/ui_elements.h"

namespace ndyn::simulation {

class PendulumScene final : public ui::Scene {
 private:
  static constexpr size_t NUM_POINTS{1024};

  // We base all of the types off of what accelerometers provide, since this is primarily a UI to
  // visualize accelerometer readings, especially as compared to actual motion.
  using AccelerometerTypes = sensor::MeasurementValueType<sensor::MeasurementType::ACCELEROMETER>;

  using VectorType = typename AccelerometerTypes::type;
  using AccelerometerType = VectorType;
  using FloatT = typename AccelerometerTypes::scalar_type;

  using PendulumConfiguratorType = PendulumConfigurator<VectorType>;
  using PendulumType = typename PendulumConfiguratorType::PendulumType;
  using PendulumStateType = typename PendulumConfiguratorType::StateType;

  using TemperatureType = sensor::MeasurementValueType<sensor::MeasurementType::TEMPERATURE>::type;

  /**
   * Numeric simulation of the motion of a pendulum.
   */
  PendulumType pendulum;

  /**
   * UI models of this motion. These save major motion parameters over time and make them available
   * to graph.
   */
  PendulumUiModel<PendulumType, NUM_POINTS> pendulum_model{pendulum};
  PositionUiModel<PendulumType, NUM_POINTS> position_model{pendulum};

  /**
   * UI that displays graphs of the pendulum's state over time. This graph unifies the data from the
   * pendulum-specific UI model and the generic position model.
   */
  PendulumGraph<PendulumUiModel<PendulumType, NUM_POINTS>,
                PositionUiModel<PendulumType, NUM_POINTS>>
      statistics{pendulum_model, position_model};

  /**
   * Sensor simulations and their accompanying characterizations.
   */
  static constexpr TemperatureType TEMPERATURE{25};
  Characterization<AccelerometerType, FloatT> accelerometer_1_characterization{
      Characteristic<FloatT>{.temperature{TEMPERATURE}, .offset_average{1.025}, .offset_std{.05}}};

  AccelerometerSensorModel<PendulumType, NUM_POINTS> accelerometer_1{
      pendulum, accelerometer_1_characterization};

  Characterization<AccelerometerType, FloatT> accelerometer_2_characterization{
      Characteristic<FloatT>{.temperature{TEMPERATURE}, .offset_average{1.05}, .offset_std{.25}}};

  AccelerometerSensorModel<PendulumType, NUM_POINTS> accelerometer_2{
      pendulum, accelerometer_2_characterization};

  Characterization<AccelerometerType, FloatT> gyroscope_1_characterization{
      Characteristic<FloatT>{.temperature{TEMPERATURE}, .offset_average{1.01}, .offset_std{.1}}};

  AccelerometerSensorModel<PendulumType, NUM_POINTS> gyroscope_1{pendulum,
                                                                 gyroscope_1_characterization};

  Characterization<AccelerometerType, FloatT> gyroscope_2_characterization{
      Characteristic<FloatT>{.temperature{TEMPERATURE}, .offset_average{1.15}, .offset_std{.5}}};

  AccelerometerSensorModel<PendulumType, NUM_POINTS> gyroscope_2{pendulum,
                                                                 gyroscope_2_characterization};
  /**
   * UI to display graphs of these sensor simulations.
   */
  SensorMeasurementGraph<PendulumType, NUM_POINTS> sensor_measurements{};

  /**
   * Top-level UI pane.
   */
  ui::LeftRightPane ui{};

  /**
   * Directly render a cube to visualize the motion of the mass at the end of the pendulum.
   */
  CubePositionFn cube_as_pendulum{[this]() {
    const auto x{position_model.position_x};
    const auto y{position_model.position_z + pendulum.length() / 2};
    const auto z{position_model.position_y};
    const auto theta{position_model.theta};
    const auto phi{position_model.phi};
    return glm::rotate(
        glm::rotate(glm::translate(glm::mat4{1.f}, glm::vec3{x, y, z}), phi, glm::vec3{1, 0, 0}),
        theta, glm::vec3{0, 0, 1});
  }};
  Cube cube;

  /**
   * Construct a description of the scene based on the pendulum's parameters. This description gets
   * rendered as the window title, if using a windowing system.
   */
  static std::string build_scene_description(float length, float gravity, float mass, float theta) {
    using std::to_string;
    std::string result{"Pendulum -- "};
    result.append("length: ").append(to_string(length));
    result.append(", g: ").append(to_string(gravity));
    result.append(", mass: ").append(to_string(mass));
    result.append(", theta: ").append(to_string(theta));
    return result;
  }

 public:
  PendulumScene(GLFWwindow& gl_window, float length, float gravity, float mass, float theta)
      : Scene(build_scene_description(length, gravity, mass, theta)),
        pendulum(PendulumConfiguratorType{}
                     .set_length(length)
                     .set_g(gravity)
                     .set_mass(mass)
                     .set_theta(theta)
                     .create()),
        cube(gl_window, cube_as_pendulum) {
    ui.add_left_child(statistics);

    sensor_measurements.add_accelerometer(accelerometer_1);
    sensor_measurements.add_accelerometer(accelerometer_2);
    sensor_measurements.add_gyroscope(gyroscope_1);
    sensor_measurements.add_gyroscope(gyroscope_2);
    ui.add_right_child(sensor_measurements);
  }

  void handle_loading() override {
    pendulum_model.handle_loading();
    position_model.handle_loading();
    accelerometer_1.handle_loading();
    accelerometer_2.handle_loading();
    gyroscope_1.handle_loading();
    gyroscope_2.handle_loading();
    cube.handle_loading();
    ui.handle_loading();
  }

  void handle_unloading() override {
    ui.handle_unloading();
    cube.handle_unloading();
    pendulum_model.handle_unloading();
    position_model.handle_unloading();
    accelerometer_1.handle_unloading();
    accelerometer_2.handle_unloading();
    gyroscope_1.handle_unloading();
    gyroscope_2.handle_unloading();
  }

  void update_models() override {
    pendulum_model.update();
    position_model.update();
    accelerometer_1.update();
    accelerometer_2.update();
    gyroscope_1.update();
    gyroscope_2.update();
  }

  void update_ui() override { ui.update(); }

  void update_direct_render_elements() override { cube.update(); }
};

}  // namespace ndyn::simulation
