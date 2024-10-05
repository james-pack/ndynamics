#pragma once

#include "math/state_view.h"
#include "math/unit_set.h"
#include "ui/data_series.h"
#include "ui/ui_model.h"

namespace ndyn::simulation {

template <typename ViewT, size_t NUM_POINTS = 2048>
class PositionUiModel final : public ui::UiModel {
 public:
  using ViewType = ViewT;
  using ViewStateType = typename ViewType::StateType;
  using Units = typename ViewStateType::Units;
  using ScalarType = typename ViewStateType::ScalarType;

 private:
  const ViewType* view_;

  static_assert(Units::coordinates() == math::Coordinates::CARTESIAN,
                "Only Cartesian coordinates are supported");

 public:
  PositionUiModel(const ViewType& view) : view_(&view) {}

  ui::DataSeries<ScalarType, NUM_POINTS, 3> position_series{"t", {"x", "y", "z"}};
  ui::DataSeries<ScalarType, NUM_POINTS, 3> velocity_series{"t", {"x", "y", "z"}};
  ui::DataSeries<ScalarType, NUM_POINTS, 3> acceleration_series{"t", {"x", "y", "z"}};

  ScalarType position_x{};
  ScalarType position_y{};
  ScalarType position_z{};

  ScalarType velocity_x{};
  ScalarType velocity_y{};
  ScalarType velocity_z{};

  ScalarType acceleration_x{};
  ScalarType acceleration_y{};
  ScalarType acceleration_z{};

  void update() override {
    view_->clear_cache();
    const ScalarType current_time{static_cast<ScalarType>(ImGui::GetTime())};

    if constexpr (ViewStateType::depth() > 0) {
      const auto& position{view_->template element<0>()};
      position_x = position.x();
      position_y = position.y();
      position_z = position.z();
      position_series.update(current_time, {position.x(), position.y(), position.z()});
    }

    if constexpr (ViewStateType::depth() > 1) {
      const auto& velocity{view_->template element<1>()};
      velocity_x = velocity.x();
      velocity_y = velocity.y();
      velocity_z = velocity.z();
      velocity_series.update(current_time, {velocity.x(), velocity.y(), velocity.z()});
    }

    if constexpr (ViewStateType::depth() > 2) {
      const auto& acceleration{view_->template element<2>()};
      acceleration_x = acceleration.x();
      acceleration_y = acceleration.y();
      acceleration_z = acceleration.z();
      acceleration_series.update(current_time,
                                 {acceleration.x(), acceleration.y(), acceleration.z()});
    }
  }
};

}  // namespace ndyn::simulation
