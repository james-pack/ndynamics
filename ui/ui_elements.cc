// clang-format off
#include "glad/gl.h"
// clang-format on

#include "ui/ui_elements.h"

#include "glog/logging.h"
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "ui/imgui_utils.h"

namespace ndyn::ui {

void Window::update() {
  const ImGuiViewport* viewport = ImGui::GetMainViewport();

  ImVec2 work_pos = viewport->WorkPos;  // Use work area to avoid menu-bar/task-bar, if any!
  ImVec2 work_size = viewport->WorkSize;

  ImVec2 window_size, window_pos, window_pos_pivot;
  window_size.x = work_size.x * 0.35;  // Use a fraction of the available horizontal space.
  window_size.y = 0.f;                 // Auto-fit the y-axis.

  // We are going to right-justify this next window.
  window_pos.x = work_pos.x + work_size.x;
  window_pos.y = work_pos.y;

  // Use the top-right of the next window as the point to position the window.
  window_pos_pivot.x = 1.0f;
  window_pos_pivot.y = 0.0f;

  VLOG(6) << "work_pos: " << work_pos << ", work_size: " << work_size
          << ", window_pos: " << window_pos << ", window_pos_pivot: " << window_pos_pivot;

  ImGui::SetNextWindowPos(window_pos, ImGuiCond_Always, window_pos_pivot);
  ImGui::SetNextWindowSize(window_size, ImGuiCond_Always);

  ImGui::SetNextWindowBgAlpha(0.1f);  // Transparent background

  const ImGuiWindowFlags window_flags{
      ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_AlwaysAutoResize |
      ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoFocusOnAppearing |
      ImGuiWindowFlags_NoNav | ImGuiWindowFlags_NoMove};
  ImGui::Begin("##App", nullptr, window_flags);

  for (auto* child : children_) {
    child->update();
  }

  ImGui::End();
}

}  // namespace ndyn::ui
