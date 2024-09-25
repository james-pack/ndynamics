#pragma once

#include "ui/data_series.h"
#include "ui/ui_model.h"

namespace ndyn::simulation {

template <typename DataSourceT, typename FloatT = float, size_t NUM_POINTS = 2048>
class PositionModel final : public ui::UiModel {
 private:
  const DataSourceT* source_;

 public:
  PositionModel(const DataSourceT& source) : source_(&source) {}

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

    {
      const auto position{source_->position()};
      position_x = position.component(1);
      position_y = position.component(2);
      position_series.update(current_time, {position.component(1), position.component(2)});
    }

    {
      const auto velocity{source_->velocity()};
      velocity_x = velocity.component(1);
      velocity_y = velocity.component(2);
      velocity_series.update(current_time, {velocity.component(1), velocity.component(2)});
    }

    {
      const auto acceleration{source_->acceleration()};
      acceleration_x = acceleration.component(1);
      acceleration_y = acceleration.component(2);
      acceleration_series.update(current_time,
                                 {acceleration.component(1), acceleration.component(2)});
    }
  }
};

}  // namespace ndyn::simulation
