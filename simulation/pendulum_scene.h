#pragma once

#include <string>

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "sensor/measurement_type.h"
#include "simulation/accelerometer_sensor_model.h"
#include "simulation/cube_ui.h"
#include "simulation/pendulum.h"
#include "simulation/pendulum_graph_ui.h"
#include "simulation/sensor_measurement_graph_ui.h"
#include "ui/app.h"
#include "ui/scene.h"
#include "ui/ui_elements.h"

namespace ndyn::simulation {

class PendulumScene final : public ui::Scene {
 private:
  static constexpr size_t NUM_POINTS{1024};
  using PendulumGraphType = PendulumGraph<NUM_POINTS>;
  using FloatT = PendulumGraphType::FloatT;
  using PendulumType = Pendulum<PendulumGraphType::T>;
  using PendulumConfiguratorType = PendulumConfigurator<PendulumGraphType::T>;
  using AccelerometerType =
      sensor::MeasurementValueType<sensor::MeasurementType::ACCELEROMETER>::type;
  using TemperatureType = sensor::MeasurementValueType<sensor::MeasurementType::TEMPERATURE>::type;

  PendulumType pendulum;

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

  Characterization<AccelerometerType, FloatT> gyroscope_1_characterization{
      Characteristic<FloatT>{.temperature{TEMPERATURE}, .offset_average{1.01}, .offset_std{.1}}};

  AccelerometerSensorModel<PendulumType, NUM_POINTS> gyroscope_1{pendulum,
                                                                 gyroscope_1_characterization};

  Characterization<AccelerometerType, FloatT> gyroscope_2_characterization{
      Characteristic<FloatT>{.temperature{TEMPERATURE}, .offset_average{1.15}, .offset_std{.5}}};

  AccelerometerSensorModel<PendulumType, NUM_POINTS> gyroscope_2{pendulum,
                                                                 gyroscope_2_characterization};

  ui::LeftRightPane ui{};

  PendulumGraphType statistics{pendulum_model, position_model};

  SensorMeasurementGraph<PendulumType, NUM_POINTS> sensor_measurements{};

  CubePositionFn cube_as_pendulum{[this]() {
    const auto x{pendulum.position().x()};
    const auto y{pendulum.position().y() + pendulum.length() / 2};
    const auto z{pendulum.position().z()};
    const auto theta{pendulum.theta()};
    return glm::rotate(glm::translate(glm::mat4{1.f}, glm::vec3{x, y, z}), theta,
                       glm::vec3{0, 0, 1});
  }};
  Cube cube;

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