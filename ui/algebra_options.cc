#include "ui/algebra_options.h"

#include <algorithm>
#include <array>
#include <optional>
#include <string>
#include <string_view>

#include "gflags/gflags.h"
#include "glog/logging.h"

namespace ndyn::ui {

struct AlgebraName {
  const char* const name;
  const Algebras algebra;
};

/**
 * Mapping of the name of the Algebra to its enum value.
 */
static constexpr std::array ALGEBRA_NAMES{
    AlgebraName{"scalar", Algebras::Scalar},
    AlgebraName{"complex", Algebras::Complex},
    AlgebraName{"euclid2d", Algebras::Euclid2d},
    AlgebraName{"euclid3d", Algebras::Euclid3d},
};

std::string_view algebra_to_string(Algebras algebra) {
  for (const auto& entry : ALGEBRA_NAMES) {
    if (entry.algebra == algebra) {
      return entry.name;
    }
  }
  return "<unknown>";
}

std::optional<Algebras> algebra_from_string(std::string_view s) {
  for (const auto& entry : ALGEBRA_NAMES) {
    if (entry.name == s) {
      return entry.algebra;
    }
  }
  return std::nullopt;
}

// Helper method to generate list of valid algebra names from the name mapping.
static constexpr auto algebra_valid_values_string() {
  std::array<char, 128> valid{};
  auto out = valid.begin();

  auto append = [&](std::string_view s) constexpr {
    for (char c : s) {
      *out++ = c;
    }
  };

  for (size_t i = 0; i < ALGEBRA_NAMES.size(); ++i) {
    if (i != 0) {
      append(", ");
    }
    append("'");
    append(ALGEBRA_NAMES[i].name);
    append("'");
  }

  *out = '\0';
  return valid;
}

constexpr auto ALGEBRA_VALID_VALUES_TEXT = algebra_valid_values_string();

// Helper method to generate list of valid algebra names from the name mapping.
static constexpr auto algebra_help_string() {
  std::array<char, 128> help{};
  auto out = help.begin();

  auto append = [&](std::string_view s) constexpr {
    for (char c : s) {
      *out++ = c;
    }
  };

  append("Algebra to use for calculations. Valid values: ");
  append(ALGEBRA_VALID_VALUES_TEXT.data());

  *out = '\0';
  return help;
}

constexpr auto ALGEBRA_HELP_TEXT = algebra_help_string();

bool validate_algebra_name(const char* flagname, const std::string& value) {
  if (algebra_from_string(value).has_value()) {
    return true;
  }

  std::string msg{};
  msg += "Invalid value for --";
  msg += flagname;
  msg += ": '";
  msg += value;
  msg += "'. Valid values: ";
  msg += ALGEBRA_VALID_VALUES_TEXT.data();

  LOG(ERROR) << msg;
  return false;
}

}  // namespace ndyn::ui

DEFINE_string(algebra, "euclid3d", ndyn::ui::ALGEBRA_HELP_TEXT.data());

DEFINE_validator(algebra, &ndyn::ui::validate_algebra_name);
