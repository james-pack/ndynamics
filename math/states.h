#pragma once

#include "math/multivector.h"
#include "math/state.h"

namespace ndyn::math {

template <typename T>
using State = StateT<VgaMultivector<T>, 3>;

template <typename T>
using State2d = StateT<Vga2dMultivector<T>, 3>;

template <typename T>
using RelativisticState = StateT<SpacetimeMultivector<T>, 3>;

}  // namespace ndyn::math
