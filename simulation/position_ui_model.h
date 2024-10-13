#pragma once

#include "ui/data_series.h"
#include "ui/ui_model.h"

namespace ndyn::simulation {

template <typename ObjectT, size_t NUM_POINTS = 2048>
class PositionUiModel final : public ui::UiModel {
 public:
  using ObjectType = ObjectT;
  using ScalarType = typename ObjectType::ScalarType;

 private:
  const ObjectType* object_;

 public:
  PositionUiModel(const ObjectType& object) : object_(&object) {}

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
    const ScalarType current_time{static_cast<ScalarType>(ImGui::GetTime())};

    const auto& position{object_->position()};
    position_x = position.x();
    position_y = position.y();
    position_z = position.z();
    position_series.update(current_time, {position.x(), position.y(), position.z()});

    const auto& velocity{object_->velocity()};
    velocity_x = velocity.x();
    velocity_y = velocity.y();
    velocity_z = velocity.z();
    velocity_series.update(current_time, {velocity.x(), velocity.y(), velocity.z()});

    const auto& acceleration{object_->acceleration()};
    acceleration_x = acceleration.x();
    acceleration_y = acceleration.y();
    acceleration_z = acceleration.z();
    acceleration_series.update(current_time,
                               {acceleration.x(), acceleration.y(), acceleration.z()});
  }
};

}  // namespace ndyn::simulation
