#pragma once

#include <string>
#include <string_view>

namespace ndyn::ui {

/**
 * A particular set of UI elements, including data models. See the App class for more details.
 */
class Scene {
 private:
  std::string scene_description_;

 public:
  Scene(std::string_view scene_description) : scene_description_(scene_description) {}
  virtual ~Scene() = default;

  const std::string& description() const { return scene_description_; }

  // Called when this Scene is loaded as the active scene.
  virtual void handle_loading() = 0;
  // Called when this Scene is being unloaded.
  virtual void handle_unloading() = 0;

  virtual void update_models() = 0;

  virtual void update_ui() = 0;

  virtual void update_direct_render_elements() = 0;
};

}  // namespace ndyn::ui
