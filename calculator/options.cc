#include "options.h"

#include <algorithm>
#include <array>
#include <optional>
#include <string>
#include <string_view>

#include "gflags/gflags.h"
#include "glog/logging.h"

namespace ndyn::calculator {

struct OptionDescriptor {
  const char* const name;
  const int value;
};

template <typename EnumType>
[[nodiscard]] constexpr int as_int(EnumType v) noexcept {
  return static_cast<int>(static_cast<std::underlying_type_t<EnumType>>(v));
}

static constexpr std::array MODEL_OPTIONS{
    OptionDescriptor{"vga", as_int(Model::Vga)},
    OptionDescriptor{"hga", as_int(Model::Hga)},
    OptionDescriptor{"cga", as_int(Model::Cga)},
};

static constexpr std::array METRIC_OPTIONS{
    OptionDescriptor{"euclidean", as_int(Metric::Euclidean)},
    OptionDescriptor{"minkowski", as_int(Metric::Minkowski)},
};

static constexpr std::array CONVENTION_OPTIONS{
    OptionDescriptor{"east_coast", as_int(MinkowskiConvention::EastCoast)},  // -+++
    OptionDescriptor{"west_coast", as_int(MinkowskiConvention::WestCoast)},  // +---
};

static constexpr std::array INTERPRET_OPTIONS{
    OptionDescriptor{"opns", as_int(GeometricInterpretation::Opns)},
    OptionDescriptor{"ipns", as_int(GeometricInterpretation::Ipns)},
};

static constexpr std::array SCALAR_OPTIONS{
    OptionDescriptor{"short", as_int(Scalar::Short)},
    OptionDescriptor{"int", as_int(Scalar::Int)},
    OptionDescriptor{"long", as_int(Scalar::Long)},
    OptionDescriptor{"float", as_int(Scalar::Float)},
    OptionDescriptor{"double", as_int(Scalar::Double)},
};

/**
 * Internal helper to find a string name from an enum value using OptionDescriptors.
 */
template <typename EnumType, size_t N>
[[nodiscard]] static std::string_view to_string_impl(
    EnumType value, const std::array<OptionDescriptor, N>& options) {
  const int target = as_int(value);
  auto it = std::find_if(options.begin(), options.end(),
                         [target](const auto& opt) { return opt.value == target; });
  if (it != options.end()) {
    return it->name;
  }
  return "unknown";
}

/**
 * Internal helper to find an enum value from a string name using OptionDescriptors.
 */
template <typename EnumType, size_t N>
[[nodiscard]] static std::optional<EnumType> from_string_impl(
    std::string_view s, const std::array<OptionDescriptor, N>& options) {
  auto it =
      std::find_if(options.begin(), options.end(), [s](const auto& opt) { return opt.name == s; });
  if (it != options.end()) {
    return static_cast<EnumType>(it->value);
  }
  return std::nullopt;
}

std::string_view model_to_string(Model model) { return to_string_impl(model, MODEL_OPTIONS); }

std::optional<Model> model_from_string(std::string_view s) {
  return from_string_impl<Model>(s, MODEL_OPTIONS);
}

std::string_view metric_to_string(Metric metric) { return to_string_impl(metric, METRIC_OPTIONS); }

std::optional<Metric> metric_from_string(std::string_view s) {
  return from_string_impl<Metric>(s, METRIC_OPTIONS);
}

std::string_view convention_to_string(MinkowskiConvention convention) {
  return to_string_impl(convention, CONVENTION_OPTIONS);
}

std::optional<MinkowskiConvention> convention_from_string(std::string_view s) {
  return from_string_impl<MinkowskiConvention>(s, CONVENTION_OPTIONS);
}

std::string_view interpretation_to_string(GeometricInterpretation interpretation) {
  return to_string_impl(interpretation, INTERPRET_OPTIONS);
}

std::optional<GeometricInterpretation> interpretation_from_string(std::string_view s) {
  return from_string_impl<GeometricInterpretation>(s, INTERPRET_OPTIONS);
}

std::string_view scalar_to_string(Scalar scalar) { return to_string_impl(scalar, SCALAR_OPTIONS); }

std::optional<Scalar> scalar_from_string(std::string_view s) {
  return from_string_impl<Scalar>(s, SCALAR_OPTIONS);
}

/**
 * Generates a comma-separated list of valid values at compile-time.
 */
template <size_t N>
[[nodiscard]] static constexpr auto generate_valid_values_text(
    const std::array<OptionDescriptor, N>& options) {
  std::array<char, 256> buffer{};
  auto out = buffer.begin();
  auto append = [&](std::string_view s) {
    for (char c : s) *out++ = c;
  };

  for (size_t i = 0; i < N; ++i) {
    if (i != 0) append(", ");
    append("'");
    append(options[i].name);
    append("'");
  }
  *out = '\0';
  return buffer;
}

/**
 * Generates the full help string for a flag at compile-time.
 */
template <size_t N>
[[nodiscard]] static constexpr auto generate_help_text(
    std::string_view prefix, const std::array<OptionDescriptor, N>& options) {
  std::array<char, 512> buffer{};
  auto out = buffer.begin();
  auto append = [&](std::string_view s) {
    for (char c : s) *out++ = c;
  };

  append(prefix);
  append(". Valid values: ");

  auto valid_vals = generate_valid_values_text(options);
  append(valid_vals.data());

  *out = '\0';
  return buffer;
}

static constexpr auto MODEL_HELP = generate_help_text("Geometric model type", MODEL_OPTIONS);
static constexpr auto METRIC_HELP =
    generate_help_text("Metric signature of the base space", METRIC_OPTIONS);
static constexpr auto CONVENTION_HELP =
    generate_help_text("Minkowski sign convention", CONVENTION_OPTIONS);
static constexpr auto INTERPRET_HELP =
    generate_help_text("Null-space mapping interpretation", INTERPRET_OPTIONS);
static constexpr auto SCALAR_HELP = generate_help_text("Scalar type", SCALAR_OPTIONS);

template <size_t N>
[[nodiscard]] bool validate_option(const char* flagname, const std::string& value,
                                   const std::array<OptionDescriptor, N>& options) {
  auto it = std::find_if(options.begin(), options.end(),
                         [&](const auto& opt) { return opt.name == value; });
  if (it != options.end()) {
    return true;
  }

  LOG(ERROR) << "Invalid value for --" << flagname << ": '" << value
             << "'. Valid values: " << generate_valid_values_text(options).data();
  return false;
}

}  // namespace ndyn::calculator

DEFINE_string(model, "cga", ndyn::calculator::MODEL_HELP.data());
DEFINE_validator(model, [](const char* fn, const std::string& v) {
  return ndyn::calculator::validate_option(fn, v, ndyn::calculator::MODEL_OPTIONS);
});

DEFINE_string(metric, "euclidean", ndyn::calculator::METRIC_HELP.data());
DEFINE_validator(metric, [](const char* fn, const std::string& v) {
  return ndyn::calculator::validate_option(fn, v, ndyn::calculator::METRIC_OPTIONS);
});

DEFINE_string(convention, "west_coast", ndyn::calculator::CONVENTION_HELP.data());
DEFINE_validator(convention, [](const char* fn, const std::string& v) {
  return ndyn::calculator::validate_option(fn, v, ndyn::calculator::CONVENTION_OPTIONS);
});

DEFINE_string(interpretation, "opns", ndyn::calculator::INTERPRET_HELP.data());
DEFINE_validator(interpretation, [](const char* fn, const std::string& v) {
  return ndyn::calculator::validate_option(fn, v, ndyn::calculator::INTERPRET_OPTIONS);
});

DEFINE_string(scalar, "float", ndyn::calculator::SCALAR_HELP.data());
DEFINE_validator(scalar, [](const char* fn, const std::string& v) {
  return ndyn::calculator::validate_option(fn, v, ndyn::calculator::SCALAR_OPTIONS);
});

DEFINE_int32(dims, 3, "Number of physical/spatial dimensions.");
