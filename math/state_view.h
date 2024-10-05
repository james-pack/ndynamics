#pragma once

#include "math/convert.h"
#include "math/state.h"

namespace ndyn::math {

/**
 * View of a state with different units or coordinate system.
 */
template <typename StateT, typename IncomingStateT>
class StateView final {
 public:
  using StateType = StateT;
  using IncomingStateType = IncomingStateT;
  using Units = typename StateType::Units;
  using IncomingUnits = typename IncomingStateType::Units;
  using VectorType = typename StateType::VectorType;

 private:
  static constexpr ConvertState<IncomingStateType, StateType> convert{};

  const IncomingStateType* state_;
  mutable StateType view_{};
  mutable bool is_dirty_{true};

  constexpr void transform() const {
    view_ = convert(*state_);
    is_dirty_ = false;
  }

 public:
  StateView(const IncomingStateType& state) : state_(&state) {}

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

/**
 * Specialization for when the incoming and view state types are the same. This class keeps the call
 * semantics of the full template but minimizes the overhead.
 */
template <typename StateT>
class StateView<StateT, StateT> final {
 public:
  using StateType = StateT;
  using IncomingStateType = StateT;
  using Units = typename StateType::Units;
  using IncomingUnits = typename IncomingStateType::Units;
  using VectorType = typename StateType::VectorType;

 private:
  const IncomingStateType* state_;

 public:
  StateView(const IncomingStateType& state) : state_(&state) {}

  template <size_t INDEX>
  constexpr const VectorType& element() const {
    static_assert(INDEX < StateType::depth(), "Attempt to reference out of bounds index");
    return state_->template element<INDEX>();
  }

  constexpr const VectorType& element(size_t index) const { return state_->at(index); }

  void clear_cache() const {}
};

}  // namespace ndyn::math
