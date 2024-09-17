#pragma once

#include <algorithm>
#include <random>
#include <stdexcept>
#include <vector>

#include "sensor/measurement_type.h"

namespace ndyn::simulation {

// The default temperature is 25C. This is the most common temperature used in datasheets when
// quoting specifications of a device.
static constexpr sensor::MeasurementValueType<sensor::MeasurementType::TEMPERATURE>::type
    DEFAULT_CHARACTERIZATION_TEMPERATURE{25};

template <typename ScalarType>
class Characteristic final {
 public:
  using TemperatureType = sensor::MeasurementValueType<sensor::MeasurementType::TEMPERATURE>::type;

  // This Characteristic is most applicable at this temperature. Usually, this is the temperature
  // where the measurement capability was specified or measured.
  TemperatureType temperature{DEFAULT_CHARACTERIZATION_TEMPERATURE};

  ScalarType offset_average;
  ScalarType offset_std;
};

template <typename ValueType, typename ScalarType = ValueType>
class Characterization final {
 public:
  using TemperatureType = sensor::MeasurementValueType<sensor::MeasurementType::TEMPERATURE>::type;

 private:
  std::mt19937 random_generator_{};
  std::vector<Characteristic<ScalarType>> characteristics_{};

 public:
  constexpr Characterization(std::initializer_list<Characteristic<ScalarType>> characteristics)
      : characteristics_(characteristics) {
    if (characteristics_.empty()) {
      throw std::domain_error("Cannot create a characterization without any characteristics");
    }
    std::sort(
        characteristics_.begin(), characteristics_.end(),
        [](const Characteristic<ScalarType>& lhs, const Characteristic<ScalarType>& rhs) -> bool {
          return lhs.temperature < rhs.temperature;
        });
  }

  // Allow for explicit seed value for repeatable simulations and testing.
  constexpr Characterization(uint32_t seed,
                             std::initializer_list<Characteristic<ScalarType>> characteristics)
      : random_generator_(seed), characteristics_(characteristics) {
    if (characteristics_.empty()) {
      throw std::domain_error("Cannot create a characterization without any characteristics");
    }
    std::sort(
        characteristics_.begin(), characteristics_.end(),
        [](const Characteristic<ScalarType>& lhs, const Characteristic<ScalarType>& rhs) -> bool {
          return lhs.temperature < rhs.temperature;
        });
  }

  ValueType inject_noise(TemperatureType temperature, ValueType value) {
    const Characteristic<ScalarType>* lower{nullptr};
    const Characteristic<ScalarType>* upper{nullptr};
    for (const auto& c : characteristics_) {
      if (c.temperature <= temperature &&
          (lower == nullptr || c.temperature > lower->temperature)) {
        lower = &c;
      } else if (c.temperature > temperature && upper == nullptr) {
        upper = &c;
        break;
      }
    }

    ScalarType noise_factor{};
    if (lower != nullptr && upper != nullptr) {
      // Interpolate between the two values.
      TemperatureType full_distance{upper->temperature - lower->temperature};
      TemperatureType distance_to_lower{temperature - lower->temperature};
      TemperatureType distance_to_upper{upper->temperature - temperature};

      const ScalarType average{
          (distance_to_upper * lower->offset_average + distance_to_lower * upper->offset_average) /
          full_distance};
      const ScalarType std{
          (distance_to_upper * lower->offset_std + distance_to_lower * upper->offset_std) /
          full_distance};

      std::normal_distribution<ScalarType> distribution{average, std};
      noise_factor = distribution(random_generator_);
    } else if (lower != nullptr) {
      std::normal_distribution<ScalarType> distribution{lower->offset_average, lower->offset_std};
      noise_factor = distribution(random_generator_);
    } else if (upper != nullptr) {
      std::normal_distribution<ScalarType> distribution{upper->offset_average, upper->offset_std};
      noise_factor = distribution(random_generator_);
    } else {
      throw std::runtime_error(
          "Could not find any characteristics for the given temperature and value");
    }
    return noise_factor * value;
  }
};

}  // namespace ndyn::simulation
