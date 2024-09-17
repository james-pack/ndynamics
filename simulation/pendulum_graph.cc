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
      static_cast<size_t>(128 * 4 * compute_period(LENGTH, GRAVITY_ACCELERATION, ANGLE))};

  std::array<FloatT, NUM_POINTS> x1{};
  std::array<FloatT, NUM_POINTS> y1{};

  std::array<FloatT, NUM_POINTS> x2{};
  std::array<FloatT, NUM_POINTS> y2{};

  std::array<FloatT, NUM_POINTS> x3{};
  std::array<FloatT, NUM_POINTS> y3{};

  std::array<FloatT, NUM_POINTS> x4{};
  std::array<FloatT, NUM_POINTS> y4{};

  std::array<FloatT, NUM_POINTS> x5{};
  std::array<FloatT, NUM_POINTS> y5{};

  std::array<FloatT, NUM_POINTS> x6{};
  std::array<FloatT, NUM_POINTS> y6{};

  std::array<FloatT, NUM_POINTS> x7{};
  std::array<FloatT, NUM_POINTS> y7{};

  std::array<FloatT, NUM_POINTS> x8{};
  std::array<FloatT, NUM_POINTS> y8{};

  FloatT previous_time{};

 protected:
  void Update() override {
    using std::copy_n;
    using std::exp;
    using std::sin;

    // Every 5 seconds, we display cycle through one period of the pendulum.
    const FloatT current_time{static_cast<FloatT>(ImGui::GetTime()) * pendulum.period() / 5};

    copy_n(x1.begin() + 1, x1.size(), x1.begin());
    copy_n(y1.begin() + 1, y1.size(), y1.begin());
    copy_n(x2.begin() + 1, x2.size(), x2.begin());
    copy_n(y2.begin() + 1, y2.size(), y2.begin());

    copy_n(x3.begin() + 1, x3.size(), x3.begin());
    copy_n(y3.begin() + 1, y3.size(), y3.begin());

    copy_n(x4.begin() + 1, x4.size(), x4.begin());
    copy_n(y4.begin() + 1, y4.size(), y4.begin());

    copy_n(x5.begin() + 1, x5.size(), x5.begin());
    copy_n(y5.begin() + 1, y5.size(), y5.begin());

    copy_n(x6.begin() + 1, x6.size(), x6.begin());
    copy_n(y6.begin() + 1, y6.size(), y6.begin());

    copy_n(x7.begin() + 1, x7.size(), x7.begin());
    copy_n(y7.begin() + 1, y7.size(), y7.begin());

    copy_n(x8.begin() + 1, x8.size(), x8.begin());
    copy_n(y8.begin() + 1, y8.size(), y8.begin());

    pendulum.goto_time(current_time);

    x1[x1.size() - 1] = current_time;
    y1[x1.size() - 1] = pendulum.height();

    x2[x2.size() - 1] = current_time;
    y2[x2.size() - 1] = pendulum.compute_potential_energy();

    x3[x3.size() - 1] = current_time;
    y3[x3.size() - 1] = pendulum.compute_kinetic_energy();

    x4[x4.size() - 1] = current_time;
    y4[x4.size() - 1] = pendulum.compute_kinetic_energy() + pendulum.compute_potential_energy();

    const auto theta{pendulum.theta()};
    x5[x5.size() - 1] = current_time;
    y5[x5.size() - 1] = theta;

    const auto& acceleration{pendulum.acceleration()};
    const auto fuzzed_acceleration{characterization.inject_noise(TEMPERATURE, acceleration)};
    x6[x6.size() - 1] = current_time;
    y6[x6.size() - 1] = fuzzed_acceleration.component(1);
    x7[x7.size() - 1] = current_time;
    y7[x7.size() - 1] = fuzzed_acceleration.component(2);
    x8[x8.size() - 1] = current_time;
    y8[x8.size() - 1] = fuzzed_acceleration.component(4);

    VLOG(2) << "current_time: " << current_time << ", theta: " << y5[x5.size() - 1]
            << ", height: " << y1[x1.size() - 1];

    previous_time = current_time;

    auto size{ImGui::GetContentRegionAvail()};
    size.y /= 4;

    if (ImPlot::BeginPlot("Position", size)) {
      ImPlot::SetupAxes("t", "h", ImPlotAxisFlags_AutoFit, ImPlotAxisFlags_AutoFit);
      ImPlot::SetNextMarkerStyle(ImPlotMarker_Circle);
      ImPlot::PlotScatter("Height", x1.data(), y1.data(), x1.size());
      ImPlot::EndPlot();
    }

    if (ImPlot::BeginPlot("Energy", size)) {
      ImPlot::SetupAxes("t", "Energy", ImPlotAxisFlags_AutoFit, ImPlotAxisFlags_AutoFit);
      ImPlot::SetNextMarkerStyle(ImPlotMarker_Circle);
      ImPlot::PlotScatter("Potential", x2.data(), y2.data(), x2.size());
      ImPlot::SetNextMarkerStyle(ImPlotMarker_Cross);
      ImPlot::PlotScatter("Kinetic", x3.data(), y3.data(), x3.size());
      ImPlot::PlotScatter("Total", x4.data(), y4.data(), x4.size());
      ImPlot::EndPlot();
    }

    if (ImPlot::BeginPlot("Angle", size)) {
      ImPlot::SetupAxes("t", "Theta", ImPlotAxisFlags_AutoFit, ImPlotAxisFlags_AutoFit);
      ImPlot::SetNextMarkerStyle(ImPlotMarker_Circle);
      ImPlot::PlotScatter("theta", x5.data(), y5.data(), x5.size());
      ImPlot::EndPlot();
    }

    if (ImPlot::BeginPlot("Accelerometer Measurements", size)) {
      ImPlot::SetupAxes("t", "Acceleration", ImPlotAxisFlags_AutoFit, ImPlotAxisFlags_AutoFit);
      ImPlot::SetNextMarkerStyle(ImPlotMarker_Circle);
      ImPlot::PlotScatter("x", x6.data(), y6.data(), x6.size());
      ImPlot::PlotScatter("y", x7.data(), y7.data(), x7.size());
      ImPlot::PlotScatter("z", x8.data(), y8.data(), x8.size());
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
