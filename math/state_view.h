#pragma once

#include <functional>

#include "math/convert.h"
#include "math/state.h"

namespace ndyn::math {

/**
 * View of a state that is transformed from the original, usually with different units or a
 * different coordinate system.
 */
template <typename StateT, typename IncomingStateT>
class StateView final {
 public:
  using StateType = StateT;
  using IncomingStateType = IncomingStateT;
  using VectorType = typename StateType::VectorType;

 private:
  std::function<StateType(const IncomingStateType&)> convert_;

  const IncomingStateType* state_;
  mutable StateType view_{};
  mutable bool is_dirty_{true};

  constexpr void transform() const {
    view_ = convert_(*state_);
    is_dirty_ = false;
  }

 public:
  StateView(const IncomingStateType& state,
            std::function<StateType(const IncomingStateType&)> convert)
      : convert_(convert), state_(&state) {}

  template <size_t INDEX>
  constexpr const VectorType& element() const {
    static_assert(INDEX < StateType::depth(), "Attempt to reference out of bounds index");
    if (is_dirty_) {
      transform();
    }
    return view_.template element<INDEX>();
  }

  constexpr const VectorType& element(size_t index) const {
    if (is_dirty_) {
      transform();
    }

    return view_.at(index);
  }

  void clear_cache() const { is_dirty_ = true; }
};

}  // namespace ndyn::math
