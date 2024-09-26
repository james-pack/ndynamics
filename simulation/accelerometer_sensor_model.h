#pragma once

#include "imgui.h"
#include "sensor/measurement_type.h"
#include "simulation/characterization.h"
#include "ui/data_series.h"
#include "ui/ui_model.h"

namespace ndyn::simulation {

template <typename DataSourceT, size_t NUM_POINTS = 2048>
class AccelerometerSensorModel final : public ui::UiModel {
 public:
  using AccelerometerTypes = sensor::MeasurementValueType<sensor::MeasurementType::ACCELEROMETER>;
  using FloatT = AccelerometerTypes::scalar_type;
  using T = AccelerometerTypes::type;
  using TemperatureType = sensor::MeasurementValueType<sensor::MeasurementType::TEMPERATURE>::type;

 private:
  static constexpr TemperatureType TEMPERATURE{25};

  const DataSourceT* source_;
  Characterization<T, FloatT>* characterization_;

 public:
  AccelerometerSensorModel(const DataSourceT& source, Characterization<T, FloatT>& characterization)
      : source_(&source), characterization_(&characterization) {}

  ui::DataSeries<FloatT, NUM_POINTS, 2> acceleration_series{"t", {"x", "y"}};

  void update() override {
    const FloatT current_time{static_cast<FloatT>(ImGui::GetTime())};

    const auto fuzzed_acceleration{
        characterization_->inject_noise(TEMPERATURE, source_->acceleration())};

    acceleration_series.update(current_time, {fuzzed_acceleration.x(), fuzzed_acceleration.y()});
  }
};

}  // namespace ndyn::simulation
