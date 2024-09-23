#include <algorithm>
#include <array>
#include <cmath>

#include "base/initializer.h"
#include "glog/logging.h"
#include "imgui.h"
#include "implot.h"
#include "ui/app.h"
#include "ui/ui_elements.h"
#include "ui/ui_model.h"

namespace ndyn::ui {

class DemoModel final : public UiModel {
 public:
  static constexpr size_t NUM_POINTS_LARGE{1024};

  std::array<float, NUM_POINTS_LARGE> x1{};
  std::array<float, NUM_POINTS_LARGE> y1{};

  std::array<float, NUM_POINTS_LARGE> x2{};
  std::array<float, NUM_POINTS_LARGE> y2{};

  std::array<float, NUM_POINTS_LARGE> x3{};
  std::array<float, NUM_POINTS_LARGE> y3{};

  float previous_time{};

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
  }
};

class DemoUi final : public UiElement {
 private:
  const DemoModel* model_;

 public:
  DemoUi(const DemoModel& model) : model_(&model) {}

  void update() override {
    auto size{ImGui::GetContentRegionAvail()};
    size.y /= 3;

    if (ImPlot::BeginPlot("Position", size)) {
      ImPlot::SetupAxesLimits(0, 1, -0.1, 1.1);
      ImPlot::SetupAxes("x", "y", ImPlotAxisFlags_AutoFit, ImPlotAxisFlags_None);
      ImPlot::PlotLine("f(x)", model_->x1.data(), model_->y1.data(), model_->x1.size());
      ImPlot::EndPlot();
    }

    if (ImPlot::BeginPlot("Theta", size)) {
      ImPlot::SetupAxesLimits(0, 1, -0.1, 1.1);
      ImPlot::SetupAxes("x", "y", ImPlotAxisFlags_AutoFit, ImPlotAxisFlags_None);
      ImPlot::PlotLine("theta(x)", model_->x2.data(), model_->y2.data(), model_->x2.size());
      ImPlot::EndPlot();
    }

    if (ImPlot::BeginPlot("Energy", size)) {
      ImPlot::SetupAxesLimits(0, 1, -0.1, 1.1);
      ImPlot::SetupAxes("x", "y", ImPlotAxisFlags_PanStretch | ImPlotAxisFlags_AutoFit,
                        ImPlotAxisFlags_AutoFit);
      ImPlot::SetNextMarkerStyle(ImPlotMarker_Circle);
      ImPlot::PlotLine("h(x)", model_->x3.data(), model_->y3.data(), model_->x3.size());
      ImPlot::EndPlot();
    }
  }
};

}  // namespace ndyn::ui

int main(int argc, char* argv[]) {
  using namespace ndyn::ui;

  FLAGS_logtostderr = true;
  ndyn::initialize(&argc, &argv);
  App app{"Demo", 1920, 1080};
  DemoModel model{};
  DemoUi demo{model};
  Window ui{};
  ui.add_child(demo);
  app.add_model(model);
  app.set_root_ui_element(ui);
  app.run();
  return 0;
}
