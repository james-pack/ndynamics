#pragma once

#include <optional>
#include <string_view>

#include "gflags/gflags.h"

// Declare the new "Stage 1" flags
DECLARE_string(model);
DECLARE_string(metric);
DECLARE_string(convention);
DECLARE_string(interpretation);
DECLARE_int32(dims);

namespace ndyn::calculator {

enum class Model {
  Vga,
  Pga,
  Cga,
};

/**
 * The fundamental geometry of the base manifold.
 */
enum class Metric {
  Euclidean,
  Minkowski,
};

/**
 * Minkowski metric signature convention (East Coast vs West Coast).
 */
enum class MinkowskiConvention {
  EastCoast,  // -+++
  WestCoast,  // +---
};

/**
 * The geometric interpretation of multivectors.
 */
enum class GeometricInterpretation {
  Opns,
  Ipns,
};

[[nodiscard]] std::string_view model_to_string(Model model);
[[nodiscard]] std::optional<Model> model_from_string(std::string_view s);

[[nodiscard]] std::string_view metric_to_string(Metric metric);
[[nodiscard]] std::optional<Metric> metric_from_string(std::string_view s);

[[nodiscard]] std::string_view convention_to_string(MinkowskiConvention convention);
[[nodiscard]] std::optional<MinkowskiConvention> convention_from_string(std::string_view s);

[[nodiscard]] std::string_view interpretation_to_string(GeometricInterpretation interpretation);
[[nodiscard]] std::optional<GeometricInterpretation> interpretation_from_string(std::string_view s);

[[nodiscard]] inline Model get_model_flag() { return model_from_string(FLAGS_model).value(); }

[[nodiscard]] inline Metric get_metric_flag() { return metric_from_string(FLAGS_metric).value(); }

[[nodiscard]] inline MinkowskiConvention get_convention_flag() {
  return convention_from_string(FLAGS_convention).value();
}

[[nodiscard]] inline GeometricInterpretation get_interpretation_flag() {
  return interpretation_from_string(FLAGS_interpretation).value();
}

[[nodiscard]] inline int get_dims_flag() { return FLAGS_dims; }

}  // namespace ndyn::calculator
