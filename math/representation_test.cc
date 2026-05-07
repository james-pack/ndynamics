#include "math/representation.h"

#include <array>
#include <string_view>
#include <utility>

#include "gtest/gtest.h"
#include "math/algebra.h"
#include "string/static_string.h"
#include "string/string_utils.h"

namespace ndyn::math {

template <typename T, size_t size>
auto print_array(const std::array<T, size>& arr) {
  std::string result{"["};
  for (size_t i = 0; i < arr.size(); ++i) {
    if (i > 0) {
      result.append(", ");
    }
    result.append(string::as_string(arr[i]));
  }
  result.append("]");
  return result;
}

TEST(RepresentationTest, CanGenerateSingleBasisBladeNameOutsideOfConstexpr) {
  static constexpr std::string_view BASIS_PREFIX{"e"};
  static constexpr size_t BASIS_NAME_OFFSET = 0;
  static constexpr size_t basis_blade_index{1};
  auto build_name = []() {
    return generate_basis_blade_name(BASIS_PREFIX, BASIS_NAME_OFFSET, basis_blade_index);
  };

  const auto basis_name = build_name();

  static constexpr std::string_view EXPECTED{"e0"};
  EXPECT_EQ(basis_name, EXPECTED);
}

TEST(RepresentationTest, CanGenerateShortSingleBasisBladeName) {
  static constexpr std::string_view BASIS_PREFIX{"e"};
  static constexpr size_t BASIS_NAME_OFFSET = 1;
  static constexpr size_t basis_blade_index{1};
  static constexpr auto build_name = []() {
    return generate_basis_blade_name(BASIS_PREFIX, BASIS_NAME_OFFSET, basis_blade_index);
  };

  static constexpr auto basis_name = string::to_static_string(build_name);

  static constexpr std::string_view EXPECTED{"e1"};
  static_assert(basis_name == EXPECTED);
  EXPECT_EQ(basis_name, EXPECTED);
}

TEST(RepresentationTest, CanGenerateSingleBasisBladeName) {
  static constexpr std::string_view BASIS_PREFIX{"gamma"};
  static constexpr size_t BASIS_NAME_OFFSET = 0;
  static constexpr size_t basis_blade_index{1};
  static constexpr auto build_name = []() {
    return generate_basis_blade_name(BASIS_PREFIX, BASIS_NAME_OFFSET, basis_blade_index);
  };

  static constexpr auto basis_name = string::to_static_string(build_name);

  static constexpr std::string_view EXPECTED{"gamma0"};
  static_assert(basis_name == EXPECTED);
  EXPECT_EQ(basis_name, EXPECTED);
}

TEST(RepresentationTest, CanGenerateMultipleBasisBladeNames) {
  static constexpr char BASIS_PREFIX[] = "e";
  static constexpr size_t BASIS_NAME_OFFSET{0};
  static constexpr size_t NUMBER_NAMES{4};

  static constexpr std::array<std::string_view, NUMBER_NAMES> basis_names =
      generate_basis_blade_names<BASIS_PREFIX, BASIS_NAME_OFFSET, NUMBER_NAMES>();

  static constexpr std::string_view EXPECTED{"e0"};

  static_assert(basis_names[1] == EXPECTED);
  EXPECT_EQ(basis_names[1], EXPECTED) << print_array(basis_names);
}

TEST(RepresentationTest, CanBuildSmallBasisArray) {
  using Algebra = Csta<>;
  using Multivector = Algebra::VectorType;
  using Scalar = Algebra::ScalarType;

  static constexpr char BASIS_PREFIX[] = "e";
  constexpr auto bases = generate_representation<Algebra, BASIS_PREFIX, 1>();

  ASSERT_EQ(Algebra::NUM_BASIS_BLADES, bases.size()) << print_array(bases);

  static constexpr Multivector EXPECTED_SCALAR_BASIS{Scalar{1}};
  static_assert(bases[0].basis == EXPECTED_SCALAR_BASIS);

  static constexpr std::string_view EXPECTED_NAME{"e1"};
  static constexpr Multivector EXPECTED_VECTOR_BASIS{Multivector::template e<0>()};
  static_assert(bases[1].name == EXPECTED_NAME);
  static_assert(bases[1].basis == EXPECTED_VECTOR_BASIS);

  EXPECT_EQ(bases[1].name, EXPECTED_NAME) << print_array(bases);
}

TEST(RepresentationTest, CanBuildBasisArray) {
  using Algebra = Csta<>;
  using Multivector = Algebra::VectorType;
  using Scalar = Algebra::ScalarType;

  static constexpr char BASIS_PREFIX[] = "gamma";
  constexpr auto bases = generate_representation<Algebra, BASIS_PREFIX>();

  ASSERT_EQ(Algebra::NUM_BASIS_BLADES, bases.size()) << print_array(bases);

  static constexpr Multivector EXPECTED_SCALAR_BASIS{Scalar{1}};
  static_assert(bases[0].basis == EXPECTED_SCALAR_BASIS);

  static constexpr std::string_view EXPECTED_NAME{"gamma0"};
  static constexpr Multivector EXPECTED_VECTOR_BASIS{Multivector::template e<0>()};
  static_assert(bases[1].name == EXPECTED_NAME);
  static_assert(bases[1].basis == EXPECTED_VECTOR_BASIS);

  EXPECT_EQ(bases[1].name, EXPECTED_NAME) << print_array(bases);
}

TEST(GenericRepresentationTest, CanPrintBasisVectors) {
  using Algebra = Vga<>;
  using Multivector = Algebra::VectorType;

  EXPECT_TRUE(to_string(Multivector{1}).starts_with("1.00")) << to_string(Multivector{1});
  EXPECT_TRUE(to_string(Multivector{1}).ends_with("00")) << to_string(Multivector{1});

  EXPECT_TRUE(to_string(Multivector::template e<0>()).starts_with("1.00"))
      << to_string(Multivector::template e<0>());
  EXPECT_TRUE(to_string(Multivector::template e<0>()).ends_with("e1"))
      << to_string(Multivector::template e<0>());

  EXPECT_TRUE(to_string(Multivector::template e<1>()).starts_with("1.00"))
      << to_string(Multivector::template e<1>());
  EXPECT_TRUE(to_string(Multivector::template e<1>()).ends_with("e2"))
      << to_string(Multivector::template e<1>());

  EXPECT_TRUE(to_string(Multivector::template e<2>()).starts_with("1.00"))
      << to_string(Multivector::template e<2>());
  EXPECT_TRUE(to_string(Multivector::template e<2>()).ends_with("e3"))
      << to_string(Multivector::template e<2>());
}

TEST(GenericRepresentationTest, CanPrintBasisBlades) {
  using Algebra = Vga<>;
  using Multivector = Algebra::VectorType;

  ASSERT_EQ(Multivector::template blade<0>(), Multivector{1});

  EXPECT_TRUE(to_string(Multivector{1}).starts_with("1.00")) << to_string(Multivector{1});
  EXPECT_TRUE(to_string(Multivector{1}).ends_with("00")) << to_string(Multivector{1});

  EXPECT_TRUE(to_string(Multivector::template blade<0>()).starts_with("1.00"))
      << to_string(Multivector::template blade<0>());
  EXPECT_TRUE(to_string(Multivector::template blade<0>()).ends_with("00"))
      << to_string(Multivector::template blade<0>());

  EXPECT_TRUE(to_string(Multivector::template blade<1>()).starts_with("1.00"))
      << to_string(Multivector::template blade<1>());
  EXPECT_TRUE(to_string(Multivector::template blade<1>()).ends_with("e1"))
      << to_string(Multivector::template blade<1>());

  EXPECT_TRUE(to_string(Multivector::template blade<2>()).starts_with("1.00"))
      << to_string(Multivector::template blade<2>());
  EXPECT_TRUE(to_string(Multivector::template blade<2>()).ends_with("e2"))
      << to_string(Multivector::template blade<2>());

  EXPECT_TRUE(to_string(Multivector::template blade<3>()).starts_with("1.00"))
      << to_string(Multivector::template blade<3>());
  EXPECT_TRUE(to_string(Multivector::template blade<3>()).ends_with("e12"))
      << to_string(Multivector::template blade<3>());

  EXPECT_TRUE(to_string(Multivector::template blade<7>()).starts_with("1.00"))
      << to_string(Multivector::template blade<7>());
  EXPECT_TRUE(to_string(Multivector::template blade<7>()).ends_with("e123"))
      << to_string(Multivector::template blade<7>());
}

}  // namespace ndyn::math
