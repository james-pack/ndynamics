#include <cmath>

#include "base/initializer.h"
#include "glog/logging.h"
#include "imgui.h"
#include "implot.h"
#include "ui/app.h"

class DemoApp : public App {
 public:
  using App::App;
  void Update() override {
    using std::sin;
    static constexpr size_t NUM_POINTS_LARGE{1024};
    static constexpr size_t NUM_POINTS_SMALL{32};

    static float x1[NUM_POINTS_LARGE], y1[NUM_POINTS_LARGE];
    for (size_t i = 0; i < NUM_POINTS_LARGE; ++i) {
      x1[i] = i / static_cast<float>(NUM_POINTS_LARGE - 1);
      y1[i] = 0.5f + 0.5f * sin(50 * (x1[i] + (float)ImGui::GetTime() / 10));
    }

    static float x2[NUM_POINTS_LARGE], y2[NUM_POINTS_LARGE];
    for (size_t i = 0; i < NUM_POINTS_LARGE; ++i) {
      x2[i] = i / static_cast<float>(NUM_POINTS_LARGE - 1);
      y2[i] = 0.5f + 0.5f * sin(50 * (x2[i] + (float)ImGui::GetTime() / 32 + 150));
    }

    static double x3[NUM_POINTS_SMALL], y3[NUM_POINTS_SMALL];
    for (size_t i = 0; i < NUM_POINTS_SMALL; ++i) {
      x3[i] = i / static_cast<float>(NUM_POINTS_SMALL - 1);
      y3[i] = x3[i] * x3[i];
    }

    auto size{ImGui::GetContentRegionAvail()};
    size.y /= 3;

    if (ImPlot::BeginPlot("Position", size)) {
      ImPlot::SetupAxes("x", "y");
      ImPlot::PlotLine("f(x)", x1, y1, NUM_POINTS_LARGE);
      ImPlot::SetNextMarkerStyle(ImPlotMarker_Circle);
      ImPlot::PlotLine("g(x)", x3, y3, NUM_POINTS_SMALL, ImPlotLineFlags_Segments);
      ImPlot::EndPlot();
    }

    if (ImPlot::BeginPlot("Theta", size)) {
      ImPlot::SetupAxes("x", "y");
      ImPlot::PlotLine("f(x)", x2, y2, NUM_POINTS_LARGE);
      ImPlot::EndPlot();
    }

    if (ImPlot::BeginPlot("Energy", size)) {
      ImPlot::SetupAxes("x", "y");
      ImPlot::SetNextMarkerStyle(ImPlotMarker_Circle);
      ImPlot::PlotLine("g(x)", x3, y3, NUM_POINTS_SMALL, ImPlotLineFlags_Segments);
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
