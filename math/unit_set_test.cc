#include "math/unit_set.h"

#include <type_traits>

#include "gtest/gtest.h"

namespace ndyn::math {

template <typename UnitsT>
struct ValuesWithUnits {
  using Units = UnitsT;
  float a;
  float b;

  template <size_t INDEX>
  auto get() const {
    if constexpr (INDEX == 0) {
      return with_unit<Units, INDEX>(a);
    }
    if constexpr (INDEX == 1) {
      return with_unit<Units, INDEX>(b);
    }
  }
};

TEST(UnitSetTest, CanGetUnitsFromUnitSet) {
  using Units = UnitSet<units::length::meter_t, units::velocity::miles_per_hour_t>;

  bool result{std::is_same_v<units::length::meter_t, Units::type<0>>};
  EXPECT_TRUE(result);

  result = std::is_same_v<units::velocity::miles_per_hour_t, Units::type<1>>;
  EXPECT_TRUE(result);
}

TEST(UnitSetTest, CanGetUnitsFromUnitSetWhenRepeatingUnits) {
  using Units = UnitSet<units::length::meter_t>;

  bool result{std::is_same_v<units::length::meter_t, Units::type<0>>};
  EXPECT_TRUE(result);

  result = std::is_same_v<units::length::meter_t, Units::type<1>>;
  EXPECT_TRUE(result);

  // Just keeps repeating the last unit...
  result = std::is_same_v<units::length::meter_t, Units::type<1024>>;
  EXPECT_TRUE(result);
}

TEST(UnitSetTest, CanGetUnits) {
  ValuesWithUnits<UnitSet<units::length::meter_t, units::velocity::miles_per_hour_t>> values{1, 2};

  bool result{std::is_same_v<units::length::meter_t, decltype(values)::Units::type<0>>};
  EXPECT_TRUE(result);

  result = std::is_same_v<units::velocity::miles_per_hour_t, decltype(values)::Units::type<1>>;
  EXPECT_TRUE(result);
}

TEST(UnitSetTest, CanValuesWithUnits) {
  ValuesWithUnits<UnitSet<units::length::meter_t, units::velocity::miles_per_hour_t>> values{1, 2};

  {
    auto unit_value_0{values.get<0>()};
    bool result{std::is_same_v<units::length::meter_t, decltype(unit_value_0)>};
    EXPECT_TRUE(result);
  }
  {
    auto unit_value_1{values.get<1>()};
    bool result{std::is_same_v<units::velocity::miles_per_hour_t, decltype(unit_value_1)>};
    EXPECT_TRUE(result);
  }
}

TEST(UnitSetTest, CanGetUnitsWhenRepeatingUnits) {
  ValuesWithUnits<UnitSet<units::length::meter_t>> values{1, 2};

  bool result{std::is_same_v<units::length::meter_t, decltype(values)::Units::type<0>>};
  EXPECT_TRUE(result);

  result = std::is_same_v<units::length::meter_t, decltype(values)::Units::type<1>>;
  EXPECT_TRUE(result);
}

TEST(UnitSetTest, CanValuesWithUnitsWhenRepeatingUnits) {
  ValuesWithUnits<UnitSet<units::length::meter_t>> values{1, 2};

  {
    auto unit_value_0{values.get<0>()};
    bool result{std::is_same_v<units::length::meter_t, decltype(unit_value_0)>};
    EXPECT_TRUE(result);
  }
  {
    auto unit_value_1{values.get<1>()};
    bool result{std::is_same_v<units::length::meter_t, decltype(unit_value_1)>};
    EXPECT_TRUE(result);
  }
}

}  // namespace ndyn::math
