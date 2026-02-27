#include "gtest/gtest.h"
#include "math/cayley_table_entry.h"

namespace ndyn::math {

TEST(GeometricProductCayleyEntryCalculatorTest, CorrectCommutativeOrderForSplitComplexNumbers) {
  static constexpr CayleyEntryCalculator<1, 0, 0> calculator{};
  EXPECT_EQ(1, calculator.compute_structure_constant(0, 0));
  EXPECT_EQ(1, calculator.compute_structure_constant(0, 1));
  EXPECT_EQ(1, calculator.compute_structure_constant(1, 0));
  EXPECT_EQ(1, calculator.compute_structure_constant(1, 1));
}

TEST(GeometricProductCayleyEntryCalculatorTest, CorrectCommutativeOrderForComplexNumbers) {
  static constexpr CayleyEntryCalculator<0, 1, 0> calculator{};
  EXPECT_EQ(1, calculator.compute_structure_constant(0, 0));
  EXPECT_EQ(1, calculator.compute_structure_constant(0, 1));
  EXPECT_EQ(1, calculator.compute_structure_constant(1, 0));
  EXPECT_EQ(-1, calculator.compute_structure_constant(1, 1));
}

TEST(GeometricProductCayleyEntryCalculatorTest, CorrectCommutativeOrderForDualNumbers) {
  static constexpr CayleyEntryCalculator<0, 0, 1> calculator{};
  EXPECT_EQ(1, calculator.compute_structure_constant(0, 0));
  EXPECT_EQ(1, calculator.compute_structure_constant(0, 1));
  EXPECT_EQ(1, calculator.compute_structure_constant(1, 0));
  EXPECT_EQ(0, calculator.compute_structure_constant(1, 1));
}

TEST(GeometricProductCayleyEntryCalculatorTest, CorrectCommutativeOrderForVgaNumbers) {
  static constexpr CayleyEntryCalculator<3, 0, 0> calculator{};
  EXPECT_EQ(1, calculator.compute_structure_constant(0, 0));
  EXPECT_EQ(1, calculator.compute_structure_constant(0, 1));
  EXPECT_EQ(1, calculator.compute_structure_constant(1, 0));

  EXPECT_EQ(1, calculator.compute_structure_constant(4, 0));
  EXPECT_EQ(1, calculator.compute_structure_constant(4, 4));

  // 1 commute.
  EXPECT_EQ(-1, calculator.compute_structure_constant(4, 1));

  // 1 commute.
  EXPECT_EQ(-1, calculator.compute_structure_constant(5, 2));

  // 2 commutes.
  EXPECT_EQ(1, calculator.compute_structure_constant(6, 1));

  // 3 commutes.
  EXPECT_EQ(-1, calculator.compute_structure_constant(7, 7));
}

TEST(GeometricProductCayleyEntryCalculatorTest, CorrectCommutativeOrderForStaNumbers) {
  static constexpr CayleyEntryCalculator<1, 3, 0> calculator{};
  EXPECT_EQ(1, calculator.compute_structure_constant(0, 0));
  EXPECT_EQ(1, calculator.compute_structure_constant(0, 1));
  EXPECT_EQ(1, calculator.compute_structure_constant(1, 0));

  EXPECT_EQ(1, calculator.compute_structure_constant(2, 0));
  EXPECT_EQ(1, calculator.compute_structure_constant(2, 4));
  EXPECT_EQ(-1, calculator.compute_structure_constant(2, 2));

  // 1 commute.
  EXPECT_EQ(-1, calculator.compute_structure_constant(2, 1));

  EXPECT_EQ(-1, calculator.compute_structure_constant(2, 6));

  // 1 commute.
  EXPECT_EQ(1, calculator.compute_structure_constant(2, 7));

  // 2 commutes.
  EXPECT_EQ(-1, calculator.compute_structure_constant(4, 7));

  EXPECT_EQ(1, calculator.compute_structure_constant(4, 0));
  EXPECT_EQ(-1, calculator.compute_structure_constant(4, 4));

  // 1 commute.
  EXPECT_EQ(-1, calculator.compute_structure_constant(4, 1));

  // 1 commute.
  EXPECT_EQ(-1, calculator.compute_structure_constant(5, 2));

  // 2 commutes.
  EXPECT_EQ(1, calculator.compute_structure_constant(6, 1));

  // 3 commutes.
  EXPECT_EQ(-1, calculator.compute_structure_constant(7, 7));
}

TEST(GeometricProductCayleyEntryCalculatorTest, CorrectResultGradeForSplitComplexNumbers) {
  static constexpr CayleyEntryCalculator<1, 0, 0> calculator{};
  EXPECT_EQ(0, calculator.compute_result_component(0, 0));
  EXPECT_EQ(1, calculator.compute_result_component(0, 1));
  EXPECT_EQ(1, calculator.compute_result_component(1, 0));
  EXPECT_EQ(0, calculator.compute_result_component(1, 1));
}

TEST(GeometricProductCayleyEntryCalculatorTest, CorrectResultGradeForComplexNumbers) {
  static constexpr CayleyEntryCalculator<0, 1, 0> calculator{};
  EXPECT_EQ(0, calculator.compute_result_component(0, 0));
  EXPECT_EQ(1, calculator.compute_result_component(0, 1));
  EXPECT_EQ(1, calculator.compute_result_component(1, 0));
  EXPECT_EQ(0, calculator.compute_result_component(1, 1));
}

TEST(GeometricProductCayleyEntryCalculatorTest, CorrectResultGradeForDualNumbers) {
  static constexpr CayleyEntryCalculator<0, 0, 1> calculator{};
  EXPECT_EQ(0, calculator.compute_result_component(0, 0));
  EXPECT_EQ(1, calculator.compute_result_component(0, 1));
  EXPECT_EQ(1, calculator.compute_result_component(1, 0));
  EXPECT_EQ(0, calculator.compute_result_component(1, 1));
}

TEST(GeometricProductCayleyEntryCalculatorTest, CorrectResultGradeForVgaNumbers) {
  static constexpr CayleyEntryCalculator<3, 0, 0> calculator{};
  EXPECT_EQ(0, calculator.compute_result_component(0, 0));
  EXPECT_EQ(1, calculator.compute_result_component(0, 1));
  EXPECT_EQ(1, calculator.compute_result_component(1, 0));

  EXPECT_EQ(4, calculator.compute_result_component(4, 0));
  EXPECT_EQ(0, calculator.compute_result_component(4, 4));

  EXPECT_EQ(5, calculator.compute_result_component(4, 1));
  EXPECT_EQ(7, calculator.compute_result_component(5, 2));
  EXPECT_EQ(7, calculator.compute_result_component(6, 1));
  EXPECT_EQ(0, calculator.compute_result_component(7, 7));
}

TEST(GeometricProductCayleyEntryCalculatorTest, CorrectResultGradeForStaNumbers) {
  static constexpr CayleyEntryCalculator<1, 3, 0> calculator{};
  EXPECT_EQ(0, calculator.compute_result_component(0, 0));
  EXPECT_EQ(1, calculator.compute_result_component(0, 1));
  EXPECT_EQ(1, calculator.compute_result_component(1, 0));

  EXPECT_EQ(2, calculator.compute_result_component(2, 0));
  EXPECT_EQ(6, calculator.compute_result_component(2, 4));
  EXPECT_EQ(0, calculator.compute_result_component(2, 2));

  EXPECT_EQ(3, calculator.compute_result_component(2, 1));

  EXPECT_EQ(4, calculator.compute_result_component(2, 6));

  EXPECT_EQ(5, calculator.compute_result_component(2, 7));

  EXPECT_EQ(3, calculator.compute_result_component(4, 7));

  EXPECT_EQ(4, calculator.compute_result_component(4, 0));
  EXPECT_EQ(0, calculator.compute_result_component(4, 4));

  EXPECT_EQ(5, calculator.compute_result_component(4, 1));

  EXPECT_EQ(7, calculator.compute_result_component(5, 2));

  EXPECT_EQ(7, calculator.compute_result_component(6, 1));

  EXPECT_EQ(0, calculator.compute_result_component(7, 7));
}

}  // namespace ndyn::math
