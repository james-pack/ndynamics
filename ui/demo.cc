#include <algorithm>
#include <array>
#include <cmath>

#include "base/initializer.h"
#include "glog/logging.h"
#include "imgui.h"
#include "implot.h"
#include "ui/app.h"

namespace ndyn::ui {

class DemoApp : public App {
 private:
  static constexpr size_t NUM_POINTS_LARGE{1024};

  std::array<float, NUM_POINTS_LARGE> x1{};
  std::array<float, NUM_POINTS_LARGE> y1{};

  std::array<float, NUM_POINTS_LARGE> x2{};
  std::array<float, NUM_POINTS_LARGE> y2{};

  std::array<float, NUM_POINTS_LARGE> x3{};
  std::array<float, NUM_POINTS_LARGE> y3{};

  float previous_time{};

 protected:
  void update() override {
    using std::copy_n;
    using std::exp;
    using std::sin;

    const float current_time{static_cast<float>(ImGui::GetTime())};

    if (current_time - previous_time > 0.02) {
      copy_n(x1.begin() + 1, x1.size(), x1.begin());
      copy_n(y1.begin() + 1, y1.size(), y1.begin());
      copy_n(x2.begin() + 1, x2.size(), x2.begin());
      copy_n(y2.begin() + 1, y2.size(), y2.begin());
      copy_n(x3.begin() + 1, x3.size(), x3.begin());
      copy_n(y3.begin() + 1, y3.size(), y3.begin());

      x1[x1.size() - 1] = current_time / static_cast<float>(1024);
      y1[x1.size() - 1] = 0.5f + 0.5f * sin(2048 * x1[x1.size() - 1]);

      x2[x2.size() - 1] = current_time / static_cast<float>(128);
      y2[x2.size() - 1] = 0.5f + 0.5f * sin(64 * x2[x2.size() - 1]);

      x3[x3.size() - 1] = current_time / static_cast<float>(32);
      y3[x3.size() - 1] = exp(x3[x3.size() - 1]) / x3[x3.size() - 1];

      previous_time = current_time;
    }

    auto size{ImGui::GetContentRegionAvail()};
    size.y /= 3;

    if (ImPlot::BeginPlot("Position", size)) {
      ImPlot::SetupAxesLimits(0, 1, -0.1, 1.1);
      ImPlot::SetupAxes("x", "y", ImPlotAxisFlags_AutoFit, ImPlotAxisFlags_None);
      ImPlot::PlotLine("f(x)", x1.data(), y1.data(), x1.size());
      ImPlot::EndPlot();
    }

    if (ImPlot::BeginPlot("Theta", size)) {
      ImPlot::SetupAxesLimits(0, 1, -0.1, 1.1);
      ImPlot::SetupAxes("x", "y", ImPlotAxisFlags_AutoFit, ImPlotAxisFlags_None);
      ImPlot::PlotLine("theta(x)", x2.data(), y2.data(), x2.size());
      ImPlot::EndPlot();
    }

    if (ImPlot::BeginPlot("Energy", size)) {
      ImPlot::SetupAxesLimits(0, 1, -0.1, 1.1);
      ImPlot::SetupAxes("x", "y", ImPlotAxisFlags_PanStretch | ImPlotAxisFlags_AutoFit,
                        ImPlotAxisFlags_AutoFit);
      ImPlot::SetNextMarkerStyle(ImPlotMarker_Circle);
      ImPlot::PlotLine("h(x)", x3.data(), y3.data(), x3.size());
      ImPlot::EndPlot();
    }
  }

 public:
  using App::App;
};

}  // namespace ndyn::ui

int main(int argc, char* argv[]) {
  FLAGS_logtostderr = true;
  ndyn::initialize(&argc, &argv);
  ndyn::ui::DemoApp app{"Demo", 1920, 1080};
  app.run();
  return 0;
}
