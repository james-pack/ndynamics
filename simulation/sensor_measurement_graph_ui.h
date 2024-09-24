#pragma once

#include <cmath>

#include "base/pi.h"
#include "imgui.h"
#include "implot.h"
#include "math/multivector.h"
#include "sensor/measurement_type.h"
#include "simulation/characterization.h"
#include "simulation/pendulum.h"
#include "ui/app.h"
#include "ui/data_series.h"
#include "ui/ui_elements.h"

namespace ndyn::simulation {

class SensorMeasurementGraph final : public ui::UiElement {
 public:
  using AccelerometerTypes = sensor::MeasurementValueType<sensor::MeasurementType::ACCELEROMETER>;
  using FloatT = AccelerometerTypes::scalar_type;
  using T = AccelerometerTypes::type;
  using TemperatureType = sensor::MeasurementValueType<sensor::MeasurementType::TEMPERATURE>::type;

 private:
  Pendulum<T>* pendulum_;

  TemperatureType TEMPERATURE{25};

  Characterization<T, FloatT> accelerometer_1_characterization{
      Characteristic<FloatT>{.temperature{TEMPERATURE}, .offset_average{1.025}, .offset_std{.05}}};

  Characterization<T, FloatT> accelerometer_2_characterization{
      Characteristic<FloatT>{.temperature{TEMPERATURE}, .offset_average{1.05}, .offset_std{.25}}};

  static constexpr size_t NUM_POINTS{2048};
  ui::DataSeries<FloatT, NUM_POINTS, 2> accelerometer_1_series{"t", {"x", "y"}};
  ui::DataSeries<FloatT, NUM_POINTS, 2> accelerometer_2_series{"t", {"x", "y"}};

  FloatT previous_time{};

 protected:
  void update() override {
    using std::exp;
    using std::sin;

    // Every 5 seconds, we display cycle through one period of the pendulum.
    const FloatT current_time{static_cast<FloatT>(ImGui::GetTime()) * pendulum_->period() / 5};

    pendulum_->goto_time(current_time);
    previous_time = current_time;

    {
      const auto acceleration{pendulum_->acceleration()};

      const auto fuzzed_acceleration_1{
          accelerometer_1_characterization.inject_noise(TEMPERATURE, acceleration)};
      accelerometer_1_series.update(
          current_time, {fuzzed_acceleration_1.component(1), fuzzed_acceleration_1.component(2)});

      const auto fuzzed_acceleration_2{
          accelerometer_2_characterization.inject_noise(TEMPERATURE, acceleration)};
      accelerometer_2_series.update(
          current_time, {fuzzed_acceleration_2.component(1), fuzzed_acceleration_2.component(2)});
    }

    auto size{ImGui::GetContentRegionAvail()};
    size.y /= 2;

    if (ImPlot::BeginPlot("Accelerometer 1", size)) {
      ImPlot::SetupAxes(accelerometer_1_series.x_clabel(), "Accelerometer", ImPlotAxisFlags_AutoFit,
                        ImPlotAxisFlags_AutoFit);
      ImPlot::SetNextMarkerStyle(ImPlotMarker_Circle);

      for (size_t i = 0; i < accelerometer_1_series.num_functions(); ++i) {
        ImPlot::PlotScatter(accelerometer_1_series.y_clabel(i), accelerometer_1_series.x_data(),
                            accelerometer_1_series.y_data(i), accelerometer_1_series.size());
      }

      ImPlot::EndPlot();
    }

    if (ImPlot::BeginPlot("Accelerometer 2", size)) {
      ImPlot::SetupAxes(accelerometer_2_series.x_clabel(), "Accelerometer", ImPlotAxisFlags_AutoFit,
                        ImPlotAxisFlags_AutoFit);
      ImPlot::SetNextMarkerStyle(ImPlotMarker_Circle);

      for (size_t i = 0; i < accelerometer_2_series.num_functions(); ++i) {
        ImPlot::PlotScatter(accelerometer_2_series.y_clabel(i), accelerometer_2_series.x_data(),
                            accelerometer_2_series.y_data(i), accelerometer_2_series.size());
      }

      ImPlot::EndPlot();
    }
  }

 public:
  SensorMeasurementGraph(Pendulum<T>& pendulum) : pendulum_(&pendulum) {}
};

}  // namespace ndyn::simulation
