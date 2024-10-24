#pragma once

#include <tuple>

#include "math/coordinates.h"
#include "units.h"

namespace ndyn::math {

namespace {

/**
 * Create a class to access the type at position INDEX of a list of types. If INDEX is greater than
 * the number of types, create a class that gives access to the last type. In essence, this allows
 * us access to the list of types by index but with the last type repeating as necessary.
 */
template <size_t INDEX, size_t NUM_TYPES, typename Type, typename... Types>
class RepeatingTypeSelector final {
 public:
  using type = typename RepeatingTypeSelector<INDEX - 1, NUM_TYPES - 1, Types...>::type;
};

template <size_t NUM_TYPES, typename Type, typename... Types>
class RepeatingTypeSelector<0, NUM_TYPES, Type, Types...> final {
 public:
  using type = Type;
};

template <size_t INDEX, typename Type, typename... Types>
class RepeatingTypeSelector<INDEX, 1, Type, Types...> final {
 public:
  using type = Type;
};

template <typename Type, typename... Types>
class RepeatingTypeSelector<0, 1, Type, Types...> final {
 public:
  using type = Type;
};

}  // namespace

template <typename Unit, typename... Units>
class UnitSet final {
 public:
  static constexpr size_t size() { return 1 /* For the Unit parameter */ + sizeof...(Units); }

  // If the number of units is fewer than the index, we assume that the last unit is repeated. This
  // saves a lot of work when dealing with vectors and other tuple-like structures when the types
  // are the same. For example, for a vector in Cartesian coordinates where all of the bases are in
  // meters, we could just use UnitSet<CARTESIAN, unit::length::meter_t>, regardless of the size of
  // the vector.
  template <size_t INDEX>
  using type = typename RepeatingTypeSelector<INDEX, size(), Unit, Units...>::type;
};

template <typename UnitSetT, size_t INDEX, typename T>
auto with_unit(const T& value) {
  using ResultT = typename UnitSetT::template type<INDEX>;
  return ResultT(value);
}

}  // namespace ndyn::math
