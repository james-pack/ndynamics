#include <cmath>

#include "base/initializer.h"
#include "base/pi.h"
#include "glog/logging.h"
#include "imgui.h"
#include "implot.h"
#include "math/multivector.h"
#include "sensor/measurement_type.h"
#include "simulation/characterization.h"
#include "simulation/pendulum.h"
#include "ui/app.h"
#include "ui/data_series.h"

namespace ndyn::simulation {

class PendulumGraph : public ui::App {
 private:
  using AccelerometerTypes = sensor::MeasurementValueType<sensor::MeasurementType::ACCELEROMETER>;
  using FloatT = AccelerometerTypes::scalar_type;
  using T = AccelerometerTypes::type;
  using TemperatureType = sensor::MeasurementValueType<sensor::MeasurementType::TEMPERATURE>::type;

  static constexpr FloatT GRAVITY_ACCELERATION{1};
  static constexpr FloatT LENGTH{1};
  static constexpr FloatT ANGLE{pi / 4};

  Pendulum<T> pendulum{PendulumConfigurator<T>{}
                           .set_length(LENGTH)
                           .set_g(GRAVITY_ACCELERATION)
                           .set_theta(ANGLE)
                           .create()};

  static constexpr TemperatureType TEMPERATURE{25};

  Characterization<T, FloatT> characterization{
      Characteristic<FloatT>{.temperature{TEMPERATURE}, .offset_average{1.01}, .offset_std{0.1}}};

  // Show 7 full periods at a time with 128 points per period.
  static constexpr size_t NUM_POINTS{
      static_cast<size_t>(64 * 4 * compute_period(LENGTH, GRAVITY_ACCELERATION, ANGLE))};

  ui::DataSeries<FloatT, NUM_POINTS, 3> position_series{"t", {"x", "y", "height"}};
  ui::DataSeries<FloatT, NUM_POINTS, 2> velocity_series{"t", {"x", "y"}};
  ui::DataSeries<FloatT, NUM_POINTS, 2> acceleration_series{"t", {"x", "y"}};
  ui::DataSeries<FloatT, NUM_POINTS, 1> theta_series{"t", {"theta"}};
  ui::DataSeries<FloatT, NUM_POINTS, 3> energy_series{"t", {"kinetic", "potential", "total"}};

  FloatT previous_time{};

 protected:
  void update() override {
    using std::exp;
    using std::sin;

    // Every 5 seconds, we display cycle through one period of the pendulum.
    const FloatT current_time{static_cast<FloatT>(ImGui::GetTime()) * pendulum.period() / 5};

    pendulum.goto_time(current_time);
    previous_time = current_time;

    {
      const auto position{pendulum.position()};
      position_series.update(current_time,
                             {position.component(1), position.component(2), pendulum.height()});
    }

    {
      const auto velocity{pendulum.velocity()};
      velocity_series.update(current_time, {velocity.component(1), velocity.component(2)});
    }

    theta_series.update(current_time, {pendulum.theta()});

    {
      const auto kinetic_energy{pendulum.compute_kinetic_energy()};
      const auto potential_energy{pendulum.compute_potential_energy()};
      const auto total_energy{kinetic_energy + potential_energy};
      energy_series.update(current_time, {kinetic_energy, potential_energy, total_energy});
    }

    {
      const auto acceleration{pendulum.acceleration()};
      const auto fuzzed_acceleration{characterization.inject_noise(TEMPERATURE, acceleration)};
      const auto& graphed_acceleration{acceleration};
      acceleration_series.update(
          current_time, {graphed_acceleration.component(1), graphed_acceleration.component(2)});
    }

    auto size{ImGui::GetContentRegionAvail()};
    size.y /= 5;

    if (ImPlot::BeginPlot("Position", size)) {
      ImPlot::SetupAxes(position_series.x_clabel(), "Position", ImPlotAxisFlags_AutoFit,
                        ImPlotAxisFlags_AutoFit);
      ImPlot::SetNextMarkerStyle(ImPlotMarker_Circle);

      for (size_t i = 0; i < position_series.num_functions(); ++i) {
        ImPlot::PlotScatter(position_series.y_clabel(i), position_series.x_data(),
                            position_series.y_data(i), position_series.size());
      }

      ImPlot::EndPlot();
    }

    if (ImPlot::BeginPlot("Velocity", size)) {
      ImPlot::SetupAxes(velocity_series.x_clabel(), "Velocity", ImPlotAxisFlags_AutoFit,
                        ImPlotAxisFlags_AutoFit);
      ImPlot::SetNextMarkerStyle(ImPlotMarker_Circle);

      for (size_t i = 0; i < velocity_series.num_functions(); ++i) {
        ImPlot::PlotScatter(velocity_series.y_clabel(i), velocity_series.x_data(),
                            velocity_series.y_data(i), velocity_series.size());
      }

      ImPlot::EndPlot();
    }

    if (ImPlot::BeginPlot("Accelerometer Measurements", size)) {
      ImPlot::SetupAxes(acceleration_series.x_clabel(), "Acceleration", ImPlotAxisFlags_AutoFit,
                        ImPlotAxisFlags_AutoFit);
      ImPlot::SetNextMarkerStyle(ImPlotMarker_Circle);

      for (size_t i = 0; i < acceleration_series.num_functions(); ++i) {
        ImPlot::PlotScatter(acceleration_series.y_clabel(i), acceleration_series.x_data(),
                            acceleration_series.y_data(i), acceleration_series.size());
      }

      ImPlot::EndPlot();
    }

    if (ImPlot::BeginPlot("Energy", size)) {
      ImPlot::SetupAxes(energy_series.x_clabel(), "Energy", ImPlotAxisFlags_AutoFit,
                        ImPlotAxisFlags_AutoFit);
      ImPlot::SetNextMarkerStyle(ImPlotMarker_Circle);

      for (size_t i = 0; i < energy_series.num_functions(); ++i) {
        ImPlot::PlotScatter(energy_series.y_clabel(i), energy_series.x_data(),
                            energy_series.y_data(i), energy_series.size());
      }

      ImPlot::EndPlot();
    }

    if (ImPlot::BeginPlot("Angle", size)) {
      ImPlot::SetupAxes(theta_series.x_clabel(), "Theta", ImPlotAxisFlags_AutoFit,
                        ImPlotAxisFlags_AutoFit);
      ImPlot::SetNextMarkerStyle(ImPlotMarker_Circle);

      for (size_t i = 0; i < theta_series.num_functions(); ++i) {
        ImPlot::PlotScatter(theta_series.y_clabel(i), theta_series.x_data(), theta_series.y_data(i),
                            theta_series.size());
      }

      ImPlot::EndPlot();
    }
  }

  void handle_unpause() override {
    position_series.clear();
    velocity_series.clear();
    acceleration_series.clear();
    theta_series.clear();
    energy_series.clear();
  }

 public:
  using ui::App::App;
};

}  // namespace ndyn::simulation

int main(int argc, char* argv[]) {
  FLAGS_logtostderr = true;
  ndyn::initialize(&argc, &argv);
  ndyn::simulation::PendulumGraph app{"Pendulum Graph", 1920, 1080};
  app.run();
  return 0;
}
