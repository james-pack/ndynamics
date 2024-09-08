#pragma once

#include <ostream>
#include <string>

#include "imgui.h"

std::string to_string(const ImVec2&);
std::ostream& operator<<(std::ostream&, const ImVec2&);
