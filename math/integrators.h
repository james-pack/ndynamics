#pragma once

#include <cstring>
#include <functional>

#include "math/state.h"

namespace ndyn::math {

template <typename T, size_t SIZE>
using UpdateFn = std::function<StateT<T, SIZE>(const StateT<T, SIZE>&)>;

/**
 * Implementations of the forward Euler algorithm for integrating the state of a system according to
 * a differential equation.
 *
 * These implementations assume that the state contains elements that are derivatives of each other.
 * The nth element can be approximated by multiplying the n+1st element by the interval.
 *
 * The state is designed so that elements of the previous state can be updated before each step.
 * This design supports approximating various differential equations, including harmonic
 * oscillators, pendulum motion, etc.
 */
template <typename ScalarT, typename T, size_t SIZE>
class ForwardEuler final {
 private:
  UpdateFn<T, SIZE> updater_{};

 public:
  ForwardEuler() = default;
  ForwardEuler(UpdateFn<T, SIZE>&& updater) : updater_(std::forward<UpdateFn<T, SIZE>>(updater)) {}

  StateT<T, SIZE> operator()(ScalarT interval, const StateT<T, SIZE>& s0) {
    auto s1{s0};
    if (updater_) {
      s1 = updater_(s1);
    }
    for (size_t i = 1; i < SIZE; ++i) {
      s1.set_element(SIZE - i - 1, s1.element(SIZE - i - 1) + interval * s1.element(SIZE - i));
    }
    return s1;
  }
};

template <typename ScalarT, typename T>
class ForwardEuler<ScalarT, T, 2> final {
 private:
  UpdateFn<T, 2> updater_{};

 public:
  ForwardEuler() = default;
  ForwardEuler(UpdateFn<T, 2>&& updater) : updater_(std::forward<UpdateFn<T, 2>>(updater)) {}

  StateT<T, 2> operator()(ScalarT interval, const StateT<T, 2>& s0) {
    auto s1{s0};
    if (updater_) {
      s1 = updater_(s1);
    }
    s1.template set_element<0>(s1.template element<0>() + interval * s1.template element<1>());
    return s1;
  }
};

template <typename ScalarT, typename T>
class ForwardEuler<ScalarT, T, 3> final {
 private:
  UpdateFn<T, 3> updater_{};

 public:
  ForwardEuler() = default;
  ForwardEuler(UpdateFn<T, 3>&& updater) : updater_(std::forward<UpdateFn<T, 3>>(updater)) {}

  StateT<T, 3> operator()(ScalarT interval, const StateT<T, 3>& s0) {
    auto s1{s0};
    if (updater_) {
      s1 = updater_(s1);
    }
    s1.template set_element<1>(s1.template element<1>() + interval * s1.template element<2>());
    s1.template set_element<0>(s1.template element<0>() + interval * s1.template element<1>());
    return s1;
  }
};

}  // namespace ndyn::math
