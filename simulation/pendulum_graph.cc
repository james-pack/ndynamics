#include <algorithm>
#include <array>
#include <cmath>

#include "base/initializer.h"
#include "base/pi.h"
#include "glog/logging.h"
#include "imgui.h"
#include "implot.h"
#include "math/multivector.h"
#include "simulation/pendulum.h"
#include "ui/app.h"

namespace ndyn::simulation {

class PendulumGraph : public ui::App {
 private:
  using FloatT = float;
  using T = math::Multivector<FloatT, 3, 0, 0, math::InnerProduct::LEFT_CONTRACTION>;

  static constexpr FloatT GRAVITY_ACCELERATION{1};
  static constexpr FloatT LENGTH{1};
  static constexpr FloatT ANGLE{pi / 32};

  Pendulum<T> pendulum{PendulumConfigurator<T>{}
                           .set_length(LENGTH)
                           .set_g(GRAVITY_ACCELERATION)
                           .set_theta(ANGLE)
                           .create()};

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

    pendulum.goto_time(current_time);

    x1[x1.size() - 1] = current_time;
    y1[x1.size() - 1] = pendulum.height();

    x2[x2.size() - 1] = current_time;
    y2[x2.size() - 1] = pendulum.compute_potential_energy();

    x3[x3.size() - 1] = current_time;
    y3[x3.size() - 1] = pendulum.compute_kinetic_energy();

    x4[x4.size() - 1] = current_time;
    y4[x4.size() - 1] = pendulum.compute_kinetic_energy() + pendulum.compute_potential_energy();

    x5[x5.size() - 1] = current_time;
    y5[x5.size() - 1] = pendulum.theta();

    VLOG(2) << "current_time: " << current_time << ", theta: " << y5[x5.size() - 1]
            << ", height: " << y1[x1.size() - 1];

    previous_time = current_time;

    auto size{ImGui::GetContentRegionAvail()};
    size.y /= 3;

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
      ImPlot::SetupAxes("t", "Energy", ImPlotAxisFlags_AutoFit, ImPlotAxisFlags_AutoFit);
      ImPlot::SetNextMarkerStyle(ImPlotMarker_Circle);
      ImPlot::PlotScatter("theta", x5.data(), y5.data(), x5.size());
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
