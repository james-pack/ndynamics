#pragma once

#include <tuple>

#include "units.h"

namespace ndyn::math {

enum class Coordinates : uint8_t {
  CARTESIAN,
  POLAR,
  SPHERICAL,
};

namespace internal {

template <size_t INDEX, size_t NUM_TYPES, typename T, typename... Ts>
class TypeSelector final {
 public:
  using type = typename TypeSelector<INDEX - 1, NUM_TYPES - 1, Ts...>::type;
};

template <size_t NUM_TYPES, typename T, typename... Ts>
class TypeSelector<0, NUM_TYPES, T, Ts...> final {
 public:
  using type = T;
};

template <size_t INDEX, typename T, typename... Ts>
class TypeSelector<INDEX, 0, T, Ts...> final {
 public:
  using type = T;
};

template <typename T, typename... Ts>
class TypeSelector<0, 0, T, Ts...> final {
 public:
  using type = T;
};

}  // namespace internal

template <Coordinates COORDINATES, typename Unit, typename... Units>
class UnitSet final {
 public:
  static constexpr Coordinates coordinates() { return COORDINATES; }

  static constexpr size_t size() { return 1 /* For the Unit parameter */ + sizeof...(Units); }

  // If the number of units is fewer than the index, we assume that the last unit is repeated. This
  // saves a lot of work when dealing with vectors and other tuple-like structures when the types
  // are the same. For example for a vector in Cartesian coordinates where all of the bases are in
  // meters, we could just use UnitSet<CARTESIAN, unit::length::meter_t>, regardless of the size of
  // the vector.
  template <size_t INDEX>
  using type = typename internal::TypeSelector<INDEX, sizeof...(Units), Unit, Units...>::type;
};

template <typename UnitSetT, size_t INDEX, typename T>
auto with_unit(const T& value) {
  using ResultT = typename UnitSetT::template type<INDEX>;
  return ResultT(value);
}

}  // namespace ndyn::math
