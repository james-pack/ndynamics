#pragma once

namespace ndyn::ui {

/**
 * Mostly standard class hierarchy for UI data models.
 */
class UiModel {
 public:
  virtual ~UiModel() = default;

  // Called when this model is loaded as part of the active scene.
  virtual void handle_loading() {}
  // Called when this model is being unloaded.
  virtual void handle_unloading() {}

  // Update, called once per frame to update the data model. This update is called when the app is
  // paused.
  virtual void update() {}
};

}  // namespace ndyn::ui
