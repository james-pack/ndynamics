#pragma once

namespace ndyn::ui {

/**
 * Mostly standard class hierarchy for UI data models.
 */
class UiModel {
 public:
  virtual ~UiModel() = default;

  // Update, called once per frame to update the data model. This update is called when the app is
  // paused.
  virtual void update() {}
};

}  // namespace ndyn::ui
