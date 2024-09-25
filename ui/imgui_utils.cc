#include "ui/imgui_utils.h"

#include <ostream>
#include <string>

#include "imgui.h"

std::string to_string(const ImVec2 &v) {
  using std::to_string;
  std::string result{};
  result.append("(").append(to_string(v.x)).append(", ").append(to_string(v.y)).append(")");
  return result;
}

std::ostream &operator<<(std::ostream &os, const ImVec2 &v) {
  os << to_string(v);
  return os;
}

namespace ndyn::ui {

std::string key_chord_to_string(ImGuiKeyChord chord) {
  std::string result{};

  if (chord & ImGuiMod_Ctrl) {
    result.append("Ctrl+");
  }
  if (chord & ImGuiMod_Alt) {
    result.append("Alt+");
  }
  if (chord & ImGuiMod_Shift) {
    result.append("Shift+");
  }
  if (chord & ImGuiMod_Super) {
    result.append("Super+");
  }

  const ImGuiKey key_code{chord & ~ImGuiMod_Mask_};
  const auto *name{ImGui::GetKeyName(key_code)};
  if (name && name[0] != '\0') {
    result.append(name);
  } else {
    result.append("(unknown key)");
  }

  return result;
}

}  // namespace ndyn::ui
