#include <algorithm>
#include <array>
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

namespace ndyn::simulation {

template <typename T, size_t NUM_POINTS, size_t NUM_DIMENSIONS = 1>
class DataSeries final {
 private:
  std::array<std::array<T, NUM_POINTS>, NUM_DIMENSIONS + 1> data_{};

 public:
  void update(T x, const std::array<T, NUM_DIMENSIONS>& y) {
    using std::copy_n;
    for (size_t i = 0; i < NUM_DIMENSIONS + 1; ++i) {
      copy_n(data_[i].begin() + 1, data_[i].size() - 1, data_[i].begin());
    }
    data_[0][NUM_POINTS - 1] = x;
    for (size_t i = 0; i < NUM_DIMENSIONS; ++i) {
      data_.at(i + 1).at(NUM_POINTS - 1) = y.at(i);
    }
  }

  const T* x_data() const { return data_[0].data(); }

  template <size_t DIMENSION = 0>
  const T* y_data() const {
    return data_.at(DIMENSION + 1).data();
  }

  size_t size() const { return NUM_POINTS; }
};

class PendulumGraph : public ui::App {
 private:
  using AccelerometerTypes = sensor::MeasurementValueType<sensor::MeasurementType::ACCELEROMETER>;
  using FloatT = AccelerometerTypes::scalar_type;
  using T = AccelerometerTypes::type;
  using TemperatureType = sensor::MeasurementValueType<sensor::MeasurementType::TEMPERATURE>::type;

  static constexpr FloatT GRAVITY_ACCELERATION{1};
  static constexpr FloatT LENGTH{1};
  static constexpr FloatT ANGLE{pi / 32};

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

  DataSeries<FloatT, NUM_POINTS, 4> position_series{};
  DataSeries<FloatT, NUM_POINTS, 3> acceleration_series{};
  DataSeries<FloatT, NUM_POINTS, 1> theta_series{};
  DataSeries<FloatT, NUM_POINTS, 3> energy_series{};

  FloatT previous_time{};

 protected:
  void Update() override {
    using std::exp;
    using std::sin;

    // Every 5 seconds, we display cycle through one period of the pendulum.
    const FloatT current_time{static_cast<FloatT>(ImGui::GetTime()) * pendulum.period() / 5};

    pendulum.goto_time(current_time);
    previous_time = current_time;

    {
      const auto position{pendulum.position()};
      position_series.update(current_time, {position.component(1), position.component(2),
                                            position.component(4), pendulum.height()});
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
      acceleration_series.update(
          current_time,
          {acceleration.component(1), acceleration.component(2), acceleration.component(4)});
    }

    auto size{ImGui::GetContentRegionAvail()};
    size.y /= 4;

    if (ImPlot::BeginPlot("Position", size)) {
      ImPlot::SetupAxes("t", "position", ImPlotAxisFlags_AutoFit, ImPlotAxisFlags_AutoFit);
      ImPlot::SetNextMarkerStyle(ImPlotMarker_Circle);
      // ImPlot::PlotScatter("x", position_series.x_data(), position_series.y_data<0>(),
      //                     position_series.size());
      // ImPlot::PlotScatter("y", position_series.x_data(), position_series.y_data<1>(),
      //                     position_series.size());
      // ImPlot::PlotScatter("z", position_series.x_data(), position_series.y_data<2>(),
      //                     position_series.size());
      ImPlot::PlotScatter("height", position_series.x_data(), position_series.y_data<3>(),
                          position_series.size());
      ImPlot::EndPlot();
    }

    if (ImPlot::BeginPlot("Energy", size)) {
      ImPlot::SetupAxes("t", "Energy", ImPlotAxisFlags_AutoFit, ImPlotAxisFlags_AutoFit);
      ImPlot::SetNextMarkerStyle(ImPlotMarker_Circle);
      ImPlot::PlotScatter("Kinetic", energy_series.x_data(), energy_series.y_data<0>(),
                          energy_series.size());
      ImPlot::PlotScatter("Potential", energy_series.x_data(), energy_series.y_data<1>(),
                          energy_series.size());
      ImPlot::SetNextMarkerStyle(ImPlotMarker_Cross);
      ImPlot::PlotScatter("Total", energy_series.x_data(), energy_series.y_data<2>(),
                          energy_series.size());
      ImPlot::EndPlot();
    }

    if (ImPlot::BeginPlot("Angle", size)) {
      ImPlot::SetupAxes("t", "Theta", ImPlotAxisFlags_AutoFit, ImPlotAxisFlags_AutoFit);
      ImPlot::SetNextMarkerStyle(ImPlotMarker_Circle);
      ImPlot::PlotScatter("theta", theta_series.x_data(), theta_series.y_data(),
                          theta_series.size());
      ImPlot::EndPlot();
    }

    if (ImPlot::BeginPlot("Accelerometer Measurements", size)) {
      ImPlot::SetupAxes("t", "Acceleration", ImPlotAxisFlags_AutoFit, ImPlotAxisFlags_AutoFit);
      ImPlot::SetNextMarkerStyle(ImPlotMarker_Circle);
      ImPlot::PlotScatter("x", acceleration_series.x_data(), acceleration_series.y_data<0>(),
                          acceleration_series.size());
      ImPlot::PlotScatter("y", acceleration_series.x_data(), acceleration_series.y_data<1>(),
                          acceleration_series.size());
      ImPlot::PlotScatter("z", acceleration_series.x_data(), acceleration_series.y_data<2>(),
                          acceleration_series.size());
      ImPlot::EndPlot();
    }
  }

 public:
  using ui::App::App;
};

}  // namespace ndyn::simulation

int main(int argc, char* argv[]) {
  FLAGS_logtostderr = true;
  ndyn::initialize(&argc, &argv);
  ndyn::simulation::PendulumGraph app{"Pendulum Graph", 1920, 1080};
  app.Run();
  return 0;
}
