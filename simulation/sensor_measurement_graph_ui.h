#pragma once

#include <string>

#include "imgui.h"
#include "implot.h"
#include "simulation/accelerometer_sensor_model.h"
#include "ui/ui_elements.h"

namespace ndyn::simulation {

template <typename DataSourceT, size_t NUM_POINTS = 2048>
class SensorMeasurementGraph final : public ui::UiElement {
 private:
  std::vector<AccelerometerSensorModel<DataSourceT, NUM_POINTS>*> accelerometers_{};
  std::vector<AccelerometerSensorModel<DataSourceT, NUM_POINTS>*> gyroscopes_{};
  std::vector<std::string> plot_names_{};

 protected:
  void update() override {
    auto size{ImGui::GetContentRegionAvail()};
    size.y /= accelerometers_.size() + gyroscopes_.size();

    size_t i{0};
    for (const auto* accelerometer : accelerometers_) {
      if (ImPlot::BeginPlot(plot_names_[i].c_str(), size)) {
        ImPlot::SetupAxes(accelerometer->acceleration_series.x_clabel(), "Accelerometer",
                          ImPlotAxisFlags_AutoFit, ImPlotAxisFlags_AutoFit);
        ImPlot::SetNextMarkerStyle(ImPlotMarker_Circle);

        for (size_t j = 0; j < accelerometer->acceleration_series.num_functions(); ++j) {
          ImPlot::PlotScatter(accelerometer->acceleration_series.y_clabel(j),
                              accelerometer->acceleration_series.x_data(),
                              accelerometer->acceleration_series.y_data(j),
                              accelerometer->acceleration_series.size());
        }

        ImPlot::EndPlot();
      }
      ++i;
    }

    for (const auto* gyroscope : gyroscopes_) {
      if (ImPlot::BeginPlot(plot_names_[i].c_str(), size)) {
        ImPlot::SetupAxes(gyroscope->acceleration_series.x_clabel(), "Gyroscope",
                          ImPlotAxisFlags_AutoFit, ImPlotAxisFlags_AutoFit);
        ImPlot::SetNextMarkerStyle(ImPlotMarker_Circle);

        for (size_t j = 0; j < gyroscope->acceleration_series.num_functions(); ++j) {
          ImPlot::PlotScatter(
              gyroscope->acceleration_series.y_clabel(j), gyroscope->acceleration_series.x_data(),
              gyroscope->acceleration_series.y_data(j), gyroscope->acceleration_series.size());
        }

        ImPlot::EndPlot();
      }
      ++i;
    }
  }

 public:
  void add_accelerometer(AccelerometerSensorModel<DataSourceT, NUM_POINTS>& accelerometer) {
    using std::to_string;
    accelerometers_.push_back(&accelerometer);
    plot_names_.push_back(std::string{"Accelerometer "}.append(to_string(accelerometers_.size())));
  }
  void add_gyroscope(AccelerometerSensorModel<DataSourceT, NUM_POINTS>& gyro) {
    using std::to_string;
    gyroscopes_.push_back(&gyro);
    plot_names_.push_back(std::string{"Gyroscope "}.append(to_string(gyroscopes_.size())));
  }
};

}  // namespace ndyn::simulation
