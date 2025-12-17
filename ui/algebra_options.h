#pragma once

#include <optional>
#include <string_view>

#include "gflags/gflags.h"

DECLARE_string(algebra);

namespace ndyn::ui {

enum class Algebras {
  Euclid2d,
  Euclid3d,
};

std::string_view algebra_to_string(Algebras algebra);

std::optional<Algebras> algebra_from_string(std::string_view s);

inline Algebras get_algebra_flag() { return algebra_from_string(FLAGS_algebra).value(); }

}  // namespace ndyn::ui
