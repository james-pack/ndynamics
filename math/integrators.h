#pragma once

#include <cstring>
#include <functional>

#include "math/state.h"

namespace ndyn::math {

template <typename T, size_t SIZE>
using ComputePartials = std::function<StateT<T, SIZE>(const StateT<T, SIZE>&)>;

/**
 * Implementation of the forward Euler algorithm for integrating the state of a system according to
 * a differential equation.
 *
 * This implementation assumes that the state contains elements that are derivatives of each other.
 * The nth element can be approximated by multiplying the n+1st element by the interval.
 *
 * The state is designed so that elements of the previous state can be updated before each step.
 * This design supports approximating various differential equations, including harmonic
 * oscillators, pendulum motion, etc.
 */
template <typename ScalarT, typename T, size_t SIZE>
class ForwardEuler final {
 private:
  ComputePartials<T, SIZE> compute_partials_{};

 public:
  ForwardEuler(ComputePartials<T, SIZE>&& updater)
      : compute_partials_(std::forward<ComputePartials<T, SIZE>>(updater)) {}

  StateT<T, SIZE> operator()(ScalarT interval, StateT<T, SIZE> s0) {
    const auto partials{compute_partials_(s0)};
    for (size_t i = 0; i < SIZE - 1; ++i) {
      s0.set_element(i, s0.element(i) + interval * s0.element(i + 1));
    }
    s0.set_element(SIZE - 1, partials.element(SIZE - 1));
    return s0;
  }
};

template <typename ScalarT, typename T>
class ForwardEuler<ScalarT, T, 4> final {
 private:
  ComputePartials<T, 4> compute_partials_{};

 public:
  ForwardEuler(ComputePartials<T, 4>&& updater)
      : compute_partials_(std::forward<ComputePartials<T, 4>>(updater)) {}

  StateT<T, 4> operator()(ScalarT interval, StateT<T, 4> s0) {
    const auto partials{compute_partials_(s0)};
    s0.template set_element<0>(s0.template element<0>() + interval * s0.template element<1>());
    s0.template set_element<1>(s0.template element<1>() + interval * s0.template element<2>());
    s0.template set_element<2>(s0.template element<2>() + interval * s0.template element<3>());
    s0.template set_element<3>(partials.template element<3>());
    return s0;
  }
};

template <typename ScalarT, typename T>
class ForwardEuler<ScalarT, T, 3> final {
 private:
  ComputePartials<T, 3> compute_partials_{};

 public:
  ForwardEuler(ComputePartials<T, 3>&& updater)
      : compute_partials_(std::forward<ComputePartials<T, 3>>(updater)) {}

  StateT<T, 3> operator()(ScalarT interval, StateT<T, 3> s0) {
    const auto partials{compute_partials_(s0)};

    s0.template set_element<0>(s0.template element<0>() + interval * s0.template element<1>());
    s0.template set_element<1>(s0.template element<1>() + interval * s0.template element<2>());
    s0.template set_element<2>(partials.template element<2>());

    return s0;
  }
};

template <typename ScalarT, typename T>
class ForwardEuler<ScalarT, T, 2> final {
 private:
  ComputePartials<T, 2> compute_partials_{};

 public:
  ForwardEuler(ComputePartials<T, 2>&& updater)
      : compute_partials_(std::forward<ComputePartials<T, 2>>(updater)) {}

  StateT<T, 2> operator()(ScalarT interval, StateT<T, 2> s0) {
    const auto partials{compute_partials_(s0)};

    s0.template set_element<0>(s0.template element<0>() + interval * s0.template element<1>());
    s0.template set_element<1>(partials.template element<1>());

    return s0;
  }
};

/**
 * Implementation of the Runge-Kutta 2 algorithm for integrating the state of a system according to
 * a differential equation.
 *
 * This implementation assumes that the state contains elements that are derivatives of each other.
 * The nth element can be approximated by multiplying the n+1st element by the interval.
 *
 * The state is designed so that elements of the previous state can be updated before each step.
 * This design supports approximating various differential equations, including harmonic
 * oscillators, pendulum motion, etc.
 */
template <typename ScalarT, typename T, size_t SIZE>
class RungeKutta2 final {
 private:
  ComputePartials<T, SIZE> compute_partials_{};

 public:
  RungeKutta2(ComputePartials<T, SIZE>&& updater)
      : compute_partials_(std::forward<ComputePartials<T, SIZE>>(updater)) {}

  StateT<T, SIZE> operator()(ScalarT interval, StateT<T, SIZE> s0) {
    auto s1{compute_partials_(s0)};
    for (size_t i = 1; i < SIZE; ++i) {
      s1.set_element(i - 1, s0.element(i - 1) + interval / 2 * s0.element(i));
    }

    auto s2{compute_partials_(s1)};
    for (size_t i = 1; i < SIZE; ++i) {
      s2.set_element(i - 1, s0.element(i - 1) + interval * s1.element(i));
    }

    return s2;
  }
};

template <typename ScalarT, typename T>
class RungeKutta2<ScalarT, T, 3> final {
 private:
  ComputePartials<T, 3> compute_partials_{};

 public:
  RungeKutta2(ComputePartials<T, 3>&& updater)
      : compute_partials_(std::forward<ComputePartials<T, 3>>(updater)) {}

  StateT<T, 3> operator()(ScalarT interval, StateT<T, 3> s0) {
    auto s1{compute_partials_(s0)};
    s1.template set_element<0>(s0.template element<0>() + interval / 2 * s0.template element<1>());
    s1.template set_element<1>(s0.template element<1>() + interval / 2 * s0.template element<2>());

    auto s2{compute_partials_(s1)};
    s2.template set_element<0>(s0.template element<0>() + interval * s1.template element<1>());
    s2.template set_element<1>(s0.template element<1>() + interval * s1.template element<2>());

    return s2;
  }
};

template <typename ScalarT, typename T>
class RungeKutta2<ScalarT, T, 2> final {
 private:
  ComputePartials<T, 2> compute_partials_{};

 public:
  RungeKutta2(ComputePartials<T, 2>&& updater)
      : compute_partials_(std::forward<ComputePartials<T, 2>>(updater)) {}

  StateT<T, 2> operator()(ScalarT interval, StateT<T, 2> s0) {
    auto s1{compute_partials_(s0)};
    s1.template set_element<0>(s0.template element<0>() + interval / 2 * s0.template element<1>());

    auto s2{compute_partials_(s1)};
    s2.template set_element<0>(s0.template element<0>() + interval * s1.template element<1>());

    return s2;
  }
};

}  // namespace ndyn::math
