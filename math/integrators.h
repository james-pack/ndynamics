#pragma once

#include <functional>

#include "glog/logging.h"
#include "math/state.h"

namespace ndyn::math {

template <typename StateType>
using ComputePartials = std::function<StateType(const StateType&)>;

/**
 * Implementation of the forward Euler algorithm for integrating the state of a system according to
 * a differential equation. This implementation makes no assumptions about the relationship between
 * the elements in the state. Typically, the elements of the state are derivatives of each other,
 * but this implementation does not assume that.
 */
template <typename StateT>
class ForwardEuler final {
 public:
  using StateType = StateT;
  using ScalarType = typename StateType::ScalarType;

 private:
  ComputePartials<StateType> compute_partials_{};

 public:
  ForwardEuler(const ComputePartials<StateType>& compute_partials)
      : compute_partials_(compute_partials) {}

  StateType operator()(ScalarType interval, const StateType& s1) {
    const auto f1{compute_partials_(s1)};

    StateType result{};
    for (size_t i = 0; i < StateType::depth(); ++i) {
      result.set_element(i, s1.element(i) + interval * f1.element(i));
    }

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
  using StateType = StateT;
  using ScalarType = typename StateType::ScalarType;

 private:
  ComputePartials<StateType> compute_partials_{};

 public:
  RungeKutta2(const ComputePartials<StateType>& compute_partials)
      : compute_partials_(compute_partials) {}

  StateType operator()(ScalarType interval, const StateType& s1) {
    const auto f1{compute_partials_(s1)};

    StateType s2{};
    for (size_t i = 0; i < StateType::depth(); ++i) {
      s2.set_element(i, s1.element(i) + interval / 2 * f1.element(i));
    }
    const auto f2{compute_partials_(s2)};

    StateType result{};
    for (size_t i = 0; i < StateType::depth(); ++i) {
      result.set_element(i, s1.element(i) + interval * f2.element(i));
    }

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
  using StateType = StateT;
  using ScalarType = typename StateType::ScalarType;

 private:
  ComputePartials<StateType> compute_partials_{};

 public:
  RungeKutta4(const ComputePartials<StateType>& compute_partials)
      : compute_partials_(compute_partials) {}

  StateType operator()(ScalarType interval, const StateType& s1) {
    const auto f1{compute_partials_(s1)};

    StateType s2{};
    for (size_t i = 0; i < StateType::depth(); ++i) {
      s2.set_element(i, s1.element(i) + interval / 2 * f1.element(i));
    }
    const auto f2{compute_partials_(s2)};

    StateType s3{};
    for (size_t i = 0; i < StateType::depth(); ++i) {
      s3.set_element(i, s1.element(i) + interval / 2 * f2.element(i));
    }
    const auto f3{compute_partials_(s3)};

    StateType s4{};
    for (size_t i = 0; i < StateType::depth(); ++i) {
      s4.set_element(i, s1.element(i) + interval * f3.element(i));
    }
    const auto f4{compute_partials_(s4)};

    static constexpr ScalarType TWO{2};
    StateType result{};
    for (size_t i = 0; i < StateType::depth(); ++i) {
      result.set_element(i, s1.element(i) + interval / 6 *
                                                (f1.element(i) + TWO * f2.element(i) +
                                                 TWO * f3.element(i) + f4.element(i)));
    }

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
