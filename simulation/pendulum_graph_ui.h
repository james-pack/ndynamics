#pragma once

#include "imgui.h"
#include "implot.h"
#include "simulation/pendulum.h"
#include "simulation/pendulum_ui_model.h"
#include "simulation/position_ui_model.h"
#include "ui/ui_elements.h"

namespace ndyn::simulation {

template <typename PendulumModelT, typename PositionModelT>
class PendulumGraph : public ui::UiElement {
 private:
  PendulumModelT* pendulum_;
  PositionModelT* position_;

 protected:
  void update() override {
    auto size{ImGui::GetContentRegionAvail()};
    size.y /= 5;

    if (ImPlot::BeginPlot("Position", size)) {
      ImPlot::SetupAxes(position_->position_series.x_clabel(), "Position", ImPlotAxisFlags_AutoFit,
                        ImPlotAxisFlags_AutoFit);
      ImPlot::SetNextMarkerStyle(ImPlotMarker_Circle);

      for (size_t i = 0; i < position_->position_series.num_functions(); ++i) {
        ImPlot::PlotScatter(
            position_->position_series.y_clabel(i), position_->position_series.x_data(),
            position_->position_series.y_data(i), position_->position_series.size());
      }

      for (size_t i = 0; i < pendulum_->height_series.num_functions(); ++i) {
        ImPlot::PlotScatter(pendulum_->height_series.y_clabel(i), pendulum_->height_series.x_data(),
                            pendulum_->height_series.y_data(i), pendulum_->height_series.size());
      }

      ImPlot::EndPlot();
    }

    if (ImPlot::BeginPlot("Velocity", size)) {
      ImPlot::SetupAxes(position_->velocity_series.x_clabel(), "Velocity", ImPlotAxisFlags_AutoFit,
                        ImPlotAxisFlags_AutoFit);
      ImPlot::SetNextMarkerStyle(ImPlotMarker_Circle);

      for (size_t i = 0; i < position_->velocity_series.num_functions(); ++i) {
        ImPlot::PlotScatter(
            position_->velocity_series.y_clabel(i), position_->velocity_series.x_data(),
            position_->velocity_series.y_data(i), position_->velocity_series.size());
      }

      ImPlot::EndPlot();
    }

    if (ImPlot::BeginPlot("Acceleration", size)) {
      ImPlot::SetupAxes(position_->acceleration_series.x_clabel(), "Acceleration",
                        ImPlotAxisFlags_AutoFit, ImPlotAxisFlags_AutoFit);
      ImPlot::SetNextMarkerStyle(ImPlotMarker_Circle);

      for (size_t i = 0; i < position_->acceleration_series.num_functions(); ++i) {
        ImPlot::PlotScatter(
            position_->acceleration_series.y_clabel(i), position_->acceleration_series.x_data(),
            position_->acceleration_series.y_data(i), position_->acceleration_series.size());
      }

      ImPlot::EndPlot();
    }

    if (ImPlot::BeginPlot("Energy", size)) {
      ImPlot::SetupAxes(pendulum_->energy_series.x_clabel(), "Energy", ImPlotAxisFlags_AutoFit,
                        ImPlotAxisFlags_AutoFit);
      ImPlot::SetNextMarkerStyle(ImPlotMarker_Circle);

      for (size_t i = 0; i < pendulum_->energy_series.num_functions(); ++i) {
        ImPlot::PlotScatter(pendulum_->energy_series.y_clabel(i), pendulum_->energy_series.x_data(),
                            pendulum_->energy_series.y_data(i), pendulum_->energy_series.size());
      }

      ImPlot::EndPlot();
    }

    if (ImPlot::BeginPlot("Angle", size)) {
      ImPlot::SetupAxes(pendulum_->theta_series.x_clabel(), "Theta", ImPlotAxisFlags_AutoFit,
                        ImPlotAxisFlags_AutoFit);
      ImPlot::SetNextMarkerStyle(ImPlotMarker_Circle);

      for (size_t i = 0; i < pendulum_->theta_series.num_functions(); ++i) {
        ImPlot::PlotScatter(pendulum_->theta_series.y_clabel(i), pendulum_->theta_series.x_data(),
                            pendulum_->theta_series.y_data(i), pendulum_->theta_series.size());
      }

      ImPlot::EndPlot();
    }
  }

 public:
  PendulumGraph(PendulumModelT& pendulum, PositionModelT& position)
      : pendulum_(&pendulum), position_(&position) {}
};

}  // namespace ndyn::simulation
