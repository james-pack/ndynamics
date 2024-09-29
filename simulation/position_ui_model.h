#pragma once

#include "math/unit_set.h"
#include "ui/data_series.h"
#include "ui/ui_model.h"

namespace ndyn::simulation {

template <typename DataSourceT, typename FloatT = float, size_t NUM_POINTS = 2048>
class PositionUiModel final : public ui::UiModel {
 public:
  using DataSourceType = DataSourceT;
  using StateType = typename DataSourceType::StateType;
  using UnitsType = typename StateType::UnitsType;

 private:
  const DataSourceT* source_;

  static_assert(UnitsType::coordinates() == math::Coordinates::CARTESIAN,
                "Only Cartesian coordinates are supported");

 public:
  PositionUiModel(const DataSourceT& source) : source_(&source) {}

  ui::DataSeries<FloatT, NUM_POINTS, 2> position_series{"t", {"x", "y"}};
  ui::DataSeries<FloatT, NUM_POINTS, 2> velocity_series{"t", {"x", "y"}};
  ui::DataSeries<FloatT, NUM_POINTS, 2> acceleration_series{"t", {"x", "y"}};

  FloatT position_x{};
  FloatT position_y{};

  FloatT velocity_x{};
  FloatT velocity_y{};

  FloatT acceleration_x{};
  FloatT acceleration_y{};

  void update() override {
    const FloatT current_time{static_cast<FloatT>(ImGui::GetTime())};

    const StateType& state{source_->state()};
    if constexpr (StateType::depth() > 0) {
      const auto& position{state.template element<0>()};
      position_x = position.x();
      position_y = position.y();
      position_series.update(current_time, {position.x(), position.y()});
    }

    if constexpr (StateType::depth() > 1) {
      const auto& velocity{state.template element<1>()};
      velocity_x = velocity.x();
      velocity_y = velocity.y();
      velocity_series.update(current_time, {velocity.x(), velocity.y()});
    }

    if constexpr (StateType::depth() > 2) {
      const auto& acceleration{state.template element<2>()};
      acceleration_x = acceleration.x();
      acceleration_y = acceleration.y();
      acceleration_series.update(current_time, {acceleration.x(), acceleration.y()});
    }
  }
};

}  // namespace ndyn::simulation
