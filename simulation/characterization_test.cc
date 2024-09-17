#include "simulation/characterization.h"

#include <cmath>

#include "gtest/gtest.h"

namespace ndyn::simulation {

using TemperatureType = sensor::MeasurementValueType<sensor::MeasurementType::TEMPERATURE>::type;

TEST(CharacterizationTest, CanDoNoiselessInjection) {
  using ValueType = float;
  Characteristic<ValueType> exact_measurement{.offset_average{1}, .offset_std{0}};
  Characterization<ValueType> characterization{exact_measurement};

  static constexpr ValueType VALUE{17};
  EXPECT_EQ(VALUE, characterization.inject_noise(DEFAULT_CHARACTERIZATION_TEMPERATURE, VALUE));
}

TEST(CharacterizationTest, CanDoSimpleStdInjection) {
  using ValueType = float;
  static constexpr ValueType DEVIATION{0.1};
  static constexpr ValueType VALUE{17};

  Characteristic<ValueType> fuzzer{.offset_average{1}, .offset_std{DEVIATION}};
  Characterization<ValueType> characterization{fuzzer};

  const ValueType result{
      characterization.inject_noise(DEFAULT_CHARACTERIZATION_TEMPERATURE, VALUE)};

  // Probability of a 5 sigma event (ie, this test failing due to randomness) is less than 1 in 3
  // million.
  EXPECT_LT(VALUE * (1 - 5 * DEVIATION), result);
  EXPECT_GT(VALUE * (1 + 5 * DEVIATION), result);
}

TEST(CharacterizationTest, CanDoAverageInjection) {
  using ValueType = float;
  static constexpr ValueType AVERAGE{2};
  static constexpr ValueType DEVIATION{0.01};
  static constexpr ValueType VALUE{17};

  Characteristic<ValueType> fuzzer{.offset_average{AVERAGE}, .offset_std{DEVIATION}};
  Characterization<ValueType> characterization{fuzzer};

  const ValueType result{
      characterization.inject_noise(DEFAULT_CHARACTERIZATION_TEMPERATURE, VALUE)};

  // Probability of a 5 sigma event (ie, this test failing due to randomness) is less than 1 in 3
  // million.
  EXPECT_LT(VALUE * (AVERAGE - 5 * DEVIATION), result);
  EXPECT_GT(VALUE * (AVERAGE + 5 * DEVIATION), result);
}

TEST(CharacterizationTest, CanInterpolateAcrossCharacteristicsByTemperature) {
  using ValueType = float;
  using std::abs;
  static constexpr ValueType AVERAGE_1{2};
  static constexpr ValueType AVERAGE_2{3};
  static constexpr TemperatureType TEMP_1{0};
  static constexpr TemperatureType TEMP_2{100};
  static constexpr TemperatureType MEASUREMENT_TEMP{25};

  static constexpr ValueType VALUE{17};

  Characteristic<ValueType> fuzzer_1{
      .temperature{TEMP_1}, .offset_average{AVERAGE_1}, .offset_std{0}};
  Characteristic<ValueType> fuzzer_2{
      .temperature{TEMP_2}, .offset_average{AVERAGE_2}, .offset_std{0}};

  Characterization<ValueType> characterization{fuzzer_1, fuzzer_2};

  const ValueType result{characterization.inject_noise(MEASUREMENT_TEMP, VALUE)};

  // Since the std deviations are both zero, result will be a weighted average between the two
  // characterics.
  EXPECT_EQ(VALUE *
                (abs(TEMP_2 - MEASUREMENT_TEMP) * AVERAGE_1 +
                 abs(TEMP_1 - MEASUREMENT_TEMP) * AVERAGE_2) /
                abs(TEMP_2 - TEMP_1),
            result);
}

}  // namespace ndyn::simulation
