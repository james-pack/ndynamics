#include <cmath>

#include "imgui.h"
#include "implot.h"
#include "ui/app.h"

class DemoApp : public App {
 public:
  using App::App;
  void Update() override {
    using std::sin;

    static float xs1[1001], ys1[1001];
    for (int i = 0; i < 1001; ++i) {
      xs1[i] = i * 0.001f;
      ys1[i] = 0.5f + 0.5f * sin(50 * (xs1[i] + (float)ImGui::GetTime() / 10));
    }
    static double xs2[20], ys2[20];
    for (int i = 0; i < 20; ++i) {
      xs2[i] = i * 1 / 19.0f;
      ys2[i] = xs2[i] * xs2[i];
    }

    if (ImPlot::BeginPlot("Line Plots")) {
      ImPlot::SetupAxes("x", "y");
      ImPlot::PlotLine("f(x)", xs1, ys1, 1001);
      ImPlot::SetNextMarkerStyle(ImPlotMarker_Circle);
      ImPlot::PlotLine("g(x)", xs2, ys2, 20, ImPlotLineFlags_Segments);
      ImPlot::EndPlot();
    }
  }
};

int main(int argc, const char* argv[]) {
  DemoApp app{"Demo"};
  app.Run();
  return 0;
}
