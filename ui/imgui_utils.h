#pragma once

#include <ostream>
#include <string>

#include "imgui.h"

std::string to_string(const ImVec2&);
std::ostream& operator<<(std::ostream&, const ImVec2&);

namespace ndyn::ui {

std::string key_chord_to_string(ImGuiKeyChord chord);

}  // namespace ndyn::ui
