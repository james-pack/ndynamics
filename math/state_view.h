#pragma once

#include <array>

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
  static constexpr Convert<VectorType, IncomingUnits, Units> convert{};

  const IncomingStateType* state_;
  mutable StateType view_{};
  mutable std::array<bool, StateType::depth()> is_dirty_{};

  template <size_t INDEX>
  constexpr void transform() const {
    view_.template set_element<INDEX>(convert(state_->template element<INDEX>()));
  }

  constexpr void transform(size_t index) const {
    view_.set_element(index, convert(state_->element(index)));
  }

 public:
  StateView(const IncomingStateType& state) : state_(&state) { is_dirty_.fill(true); }

  template <size_t INDEX>
  constexpr const VectorType& element() const {
    static_assert(INDEX < StateType::depth(), "Attempt to reference out of bounds index");
    if (is_dirty_[INDEX]) {
      transform<INDEX>();
    }
    return view_.template element<INDEX>();
  }

  constexpr const VectorType& element(size_t index) const {
    if (is_dirty_.at(index)) {
      transform(index);
    }

    return view_.at(index);
  }
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
  const IncomingStateType* view_;

 public:
  StateView(const IncomingStateType& state) : view_(&state) {}

  template <size_t INDEX>
  constexpr const VectorType& element() const {
    static_assert(INDEX < StateType::depth(), "Attempt to reference out of bounds index");
    return view_.template element<INDEX>();
  }

  constexpr const VectorType& element(size_t index) const { return view_.at(index); }
};

}  // namespace ndyn::math
