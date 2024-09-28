#pragma once

#include "ui/data_series.h"
#include "ui/ui_model.h"

namespace ndyn::simulation {

template <typename DataSourceT, typename FloatT = float, size_t NUM_POINTS = 2048>
class PositionUiModel final : public ui::UiModel {
 private:
  const DataSourceT* source_;

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

    {
      const auto position{source_->position()};
      position_x = position.x();
      position_y = position.y();
      position_series.update(current_time, {position.x(), position.y()});
    }

    {
      const auto velocity{source_->velocity()};
      velocity_x = velocity.x();
      velocity_y = velocity.y();
      velocity_series.update(current_time, {velocity.x(), velocity.y()});
    }

    {
      const auto acceleration{source_->acceleration()};
      acceleration_x = acceleration.x();
      acceleration_y = acceleration.y();
      acceleration_series.update(current_time, {acceleration.x(), acceleration.y()});
    }
  }
};

}  // namespace ndyn::simulation
