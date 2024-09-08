#include <cmath>

#include "base/initializer.h"
#include "glog/logging.h"
#include "imgui.h"
#include "implot.h"
#include "ui/app.h"

class DemoApp : public App {
 private:
  static constexpr size_t NUM_POINTS_LARGE{1024};
  static constexpr size_t NUM_POINTS_SMALL{32};

  float x1[NUM_POINTS_LARGE], y1[NUM_POINTS_LARGE];
  float x2[NUM_POINTS_LARGE], y2[NUM_POINTS_LARGE];
  float x3[NUM_POINTS_SMALL], y3[NUM_POINTS_SMALL];

 public:
  using App::App;

  void Update() override {
    using std::exp;
    using std::sin;

    for (size_t i = 0; i < NUM_POINTS_LARGE; ++i) {
      x1[i] = i / static_cast<float>(NUM_POINTS_LARGE - 1);
      y1[i] = 0.5f + 0.5f * sin(50 * (x1[i] + ImGui::GetTime() / static_cast<float>(10)));
    }

    for (size_t i = 0; i < NUM_POINTS_LARGE; ++i) {
      x2[i] = i / static_cast<float>(NUM_POINTS_LARGE - 1);
      y2[i] = 0.5f + 0.5f * sin(15 * (x2[i] + ImGui::GetTime() / static_cast<float>(32) + 150));
    }

    for (size_t i = 0; i < NUM_POINTS_SMALL; ++i) {
      x3[i] = i / static_cast<float>(NUM_POINTS_SMALL - 1);
      y3[i] = exp(x3[i]) / 3;
    }

    auto size{ImGui::GetContentRegionAvail()};
    size.y /= 3;

    if (ImPlot::BeginPlot("Position", size)) {
      ImPlot::SetupAxes("x", "y");
      ImPlot::SetupAxesLimits(0, 1, -0.1, 1.1);
      ImPlot::PlotLine("f(x)", x1, y1, NUM_POINTS_LARGE);
      ImPlot::SetNextMarkerStyle(ImPlotMarker_Circle);
      ImPlot::PlotLine("g(x)", x3, y3, NUM_POINTS_SMALL, ImPlotLineFlags_Segments);
      ImPlot::EndPlot();
    }

    if (ImPlot::BeginPlot("Theta", size)) {
      ImPlot::SetupAxes("x", "y");
      ImPlot::SetupAxesLimits(0, 1, -0.1, 1.1);
      ImPlot::PlotLine("theta(x)", x2, y2, NUM_POINTS_LARGE);
      ImPlot::EndPlot();
    }

    if (ImPlot::BeginPlot("Energy", size)) {
      ImPlot::SetupAxes("x", "y");
      ImPlot::SetupAxesLimits(0, 1, -0.1, 1.1);
      ImPlot::SetNextMarkerStyle(ImPlotMarker_Circle);
      ImPlot::PlotScatter("h(x)", x3, y3, NUM_POINTS_SMALL);
      ImPlot::EndPlot();
    }
  }
};

int main(int argc, char* argv[]) {
  FLAGS_logtostderr = true;
  ndyn::initialize(&argc, &argv);
  DemoApp app{"Demo", 1920, 1080};
  app.Run();
  return 0;
}
