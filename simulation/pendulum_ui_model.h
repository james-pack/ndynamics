#pragma once

#include "imgui.h"
#include "simulation/pendulum.h"
#include "ui/data_series.h"
#include "ui/ui_model.h"

namespace ndyn::simulation {

template <typename PendulumT, size_t NUM_POINTS = 2048>
class PendulumUiModel final : public ui::UiModel {
 public:
  using PendulumType = PendulumT;
  using StateType = typename PendulumType::StateType;
  using ScalarType = typename StateType::ScalarType;

 private:
  PendulumType* pendulum_;

 public:
  PendulumUiModel(PendulumType& pendulum) : pendulum_(&pendulum) {}
  ui::DataSeries<ScalarType, NUM_POINTS, 2> angle_series{"t", {"theta", "phi"}};
  ui::DataSeries<ScalarType, NUM_POINTS, 1> height_series{"t", {"height"}};
  ui::DataSeries<ScalarType, NUM_POINTS, 3> energy_series{"t", {"kinetic", "potential", "total"}};

  ScalarType theta{};
  ScalarType phi{};

  void update() override {
    const ScalarType current_time{static_cast<ScalarType>(ImGui::GetTime())};

    pendulum_->goto_time(current_time);

    theta = pendulum_->theta();
    phi = pendulum_->phi();
    angle_series.update(current_time, {theta, phi});

    height_series.update(current_time, {pendulum_->height()});

    const auto kinetic_energy{pendulum_->compute_kinetic_energy()};
    const auto potential_energy{pendulum_->compute_potential_energy()};
    const auto total_energy{kinetic_energy + potential_energy};
    energy_series.update(current_time, {kinetic_energy, potential_energy, total_energy});
  }
};

}  // namespace ndyn::simulation
