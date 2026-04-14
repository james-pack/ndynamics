#pragma once

#include <functional>

#include "glog/logging.h"
#include "math/state.h"

namespace ndyn::math {

template <typename State>
using ComputePartials = std::function<State(const State&)>;

/**
 * Implementation of the forward Euler algorithm for integrating the state of a system according to
 * a differential equation. This implementation makes no assumptions about the relationship between
 * the elements in the state. Typically, the elements of the state are derivatives of each other,
 * but this implementation does not assume that.
 */
template <typename StateT>
class ForwardEuler final {
 public:
  using State = StateT;
  using Vector = typename State::VectorType;
  using Scalar = typename State::ScalarType;

 private:
  ComputePartials<State> compute_partials_{};

 public:
  ForwardEuler(const ComputePartials<State>& compute_partials)
      : compute_partials_(compute_partials) {}

  State operator()(Scalar interval, const State& s1) {
    const auto f1{compute_partials_(s1)};
    State result = s1.advance(f1, interval);

    VLOG(6) << "interval: " << interval;
    VLOG(6) << "s1: " << s1;
    VLOG(6) << "f1: " << f1;
    VLOG(5) << "result: " << result;

    return result;
  }
};

/**
 * Implementation of the 2nd order Runge-Kutta algorithm for integrating the state of a system
 * according to a differential equation. This implementation makes no assumptions about the
 * relationship between the elements in the state. Typically, the elements of the state are
 * derivatives of each other, but this implementation does not assume that.
 */
template <typename StateT>
class RungeKutta2 final {
 public:
  using State = StateT;
  using Vector = typename State::VectorType;
  using Scalar = typename State::ScalarType;

 private:
  ComputePartials<State> compute_partials_{};

 public:
  RungeKutta2(const ComputePartials<State>& compute_partials)
      : compute_partials_(compute_partials) {}

  State operator()(Scalar interval, const State& s1) {
    static constexpr Scalar TWO{static_cast<Scalar>(2)};
    const Scalar half_interval = interval / TWO;

    // Stage 1
    const auto f1{compute_partials_(s1)};

    // Stage 2
    State s2 = s1.advance(f1, half_interval);
    const auto f2{compute_partials_(s2)};

    State result = s1.advance(f2, interval);

    VLOG(6) << "interval: " << interval;
    VLOG(6) << "s1: " << s1;
    VLOG(6) << "f1: " << f1;
    VLOG(6) << "s2: " << s2;
    VLOG(6) << "f2: " << f2;
    VLOG(5) << "result: " << result;

    return result;
  }
};

/**
 * Implementation of the 4th order Runge-Kutta algorithm for integrating the state of a system
 * according to a differential equation. This implementation makes no assumptions about the
 * relationship between the elements in the state. Typically, the elements of the state are
 * derivatives of each other, but this implementation does not assume that.
 */
template <typename StateT>
class RungeKutta4 final {
 public:
  using State = StateT;
  using Vector = typename State::VectorType;
  using Scalar = typename State::ScalarType;

 private:
  ComputePartials<State> compute_partials_{};

 public:
  RungeKutta4(const ComputePartials<State>& compute_partials)
      : compute_partials_(compute_partials) {}

  State operator()(Scalar interval, const State& s1) const noexcept {
    static constexpr Scalar TWO{static_cast<Scalar>(2)};
    static constexpr Scalar SIX{static_cast<Scalar>(6)};
    const Scalar half_interval = interval / TWO;

    // Stage 1
    const State f1 = compute_partials_(s1);

    // Stage 2
    const State s2 = s1.advance(f1, half_interval);
    const State f2 = compute_partials_(s2);

    // Stage 3
    const State s3 = s1.advance(f2, half_interval);
    const State f3 = compute_partials_(s3);

    // Stage 4
    const State s4 = s1.advance(f3, interval);
    const State f4 = compute_partials_(s4);

    State blended_delta{};

    for (size_t i = 0; i < State::depth(); ++i) {
      blended_delta.set_element(
          i, (f1.element(i) + TWO * f2.element(i) + TWO * f3.element(i) + f4.element(i)) / SIX);
    }

    State result{s1.advance(blended_delta, interval)};

    VLOG(6) << "interval: " << interval;
    VLOG(6) << "s1: " << s1;
    VLOG(6) << "f1: " << f1;
    VLOG(6) << "s2: " << s2;
    VLOG(6) << "f2: " << f2;
    VLOG(6) << "s3: " << s3;
    VLOG(6) << "f3: " << f3;
    VLOG(6) << "s4: " << s4;
    VLOG(6) << "f4: " << f4;
    VLOG(5) << "result: " << result;

    return result;
  }
};

}  // namespace ndyn::math
