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
