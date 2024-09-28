#pragma once

#include "imgui.h"
#include "sensor/measurement_type.h"
#include "simulation/pendulum.h"
#include "ui/data_series.h"
#include "ui/ui_model.h"

namespace ndyn::simulation {

template <size_t NUM_POINTS = 2048>
class PendulumUiModel final : public ui::UiModel {
 public:
  using AccelerometerTypes = sensor::MeasurementValueType<sensor::MeasurementType::ACCELEROMETER>;
  using FloatT = AccelerometerTypes::scalar_type;
  using T = AccelerometerTypes::type;

 private:
  Pendulum<T>* pendulum_;

 public:
  PendulumUiModel(Pendulum<T>& pendulum) : pendulum_(&pendulum) {}

  ui::DataSeries<FloatT, NUM_POINTS, 1> theta_series{"t", {"theta"}};
  ui::DataSeries<FloatT, NUM_POINTS, 1> height_series{"t", {"height"}};
  ui::DataSeries<FloatT, NUM_POINTS, 3> energy_series{"t", {"kinetic", "potential", "total"}};

  void update() override {
    const FloatT current_time{static_cast<FloatT>(ImGui::GetTime())};

    pendulum_->goto_time(current_time);

    theta_series.update(current_time, {pendulum_->theta()});
    height_series.update(current_time, {pendulum_->height()});

    const auto kinetic_energy{pendulum_->compute_kinetic_energy()};
    const auto potential_energy{pendulum_->compute_potential_energy()};
    const auto total_energy{kinetic_energy + potential_energy};
    energy_series.update(current_time, {kinetic_energy, potential_energy, total_energy});
  }
};

}  // namespace ndyn::simulation
