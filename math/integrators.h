#pragma once

#include <functional>

#include "glog/logging.h"
#include "math/state.h"

namespace ndyn::math {

template <typename T, size_t DEPTH>
using ComputePartials = std::function<StateT<T, DEPTH>(const StateT<T, DEPTH>&)>;

/**
 * Implementation of the forward Euler algorithm for integrating the state of a system according to
 * a differential equation. This implementation makes no assumptions about the relationship between
 * the elements in the state. Typically, the elements of the state are derivatives of each other,
 * but this implementation does not assume that.
 */
template <typename ScalarT, typename T, size_t DEPTH>
class ForwardEuler final {
 private:
  ComputePartials<T, DEPTH> compute_partials_{};

 public:
  ForwardEuler(const ComputePartials<T, DEPTH>& compute_partials)
      : compute_partials_(compute_partials) {}

  StateT<T, DEPTH> operator()(ScalarT interval, const StateT<T, DEPTH>& s1) {
    const auto f1{compute_partials_(s1)};

    StateT<T, DEPTH> result{};
    for (size_t i = 0; i < DEPTH; ++i) {
      result.set_element(i, s1.element(i) + interval * f1.element(i));
    }

    VLOG(6) << "interval: " << interval;
    VLOG(6) << "s1: " << s1;
    VLOG(6) << "f1: " << f1;
    VLOG(5) << "result: " << result;

    return result;
  }
};

template <typename ScalarT, typename T>
class ForwardEuler<ScalarT, T, 2> final {
 private:
  ComputePartials<T, 2> compute_partials_{};

 public:
  ForwardEuler(const ComputePartials<T, 2>& compute_partials)
      : compute_partials_(compute_partials) {}

  StateT<T, 2> operator()(ScalarT interval, const StateT<T, 2>& s1) {
    const auto f1{compute_partials_(s1)};

    StateT<T, 2> result{};
    result.template set_element<0>(s1.template element<0>() + interval * f1.template element<0>());
    result.template set_element<1>(s1.template element<1>() + interval * f1.template element<1>());

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
template <typename ScalarT, typename T, size_t DEPTH>
class RungeKutta2 final {
 private:
  ComputePartials<T, DEPTH> compute_partials_{};

 public:
  RungeKutta2(const ComputePartials<T, DEPTH>& compute_partials)
      : compute_partials_(compute_partials) {}

  StateT<T, DEPTH> operator()(ScalarT interval, const StateT<T, DEPTH>& s1) {
    const auto f1{compute_partials_(s1)};

    StateT<T, DEPTH> s2{};
    for (size_t i = 0; i < DEPTH; ++i) {
      s2.set_element(i, s1.element(i) + interval / 2 * f1.element(i));
    }
    const auto f2{compute_partials_(s2)};

    StateT<T, DEPTH> result{};
    for (size_t i = 0; i < DEPTH; ++i) {
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

template <typename ScalarT, typename T>
class RungeKutta2<ScalarT, T, 2> final {
 private:
  ComputePartials<T, 2> compute_partials_{};

 public:
  RungeKutta2(const ComputePartials<T, 2>& compute_partials)
      : compute_partials_(compute_partials) {}

  StateT<T, 2> operator()(ScalarT interval, const StateT<T, 2>& s1) {
    const auto f1{compute_partials_(s1)};

    StateT<T, 2> s2{};
    s2.template set_element<0>(s1.template element<0>() + interval / 2 * f1.template element<0>());
    s2.template set_element<1>(s1.template element<1>() + interval / 2 * f1.template element<1>());
    const auto f2{compute_partials_(s2)};

    StateT<T, 2> result{};
    result.template set_element<0>(s1.template element<0>() + interval * f2.template element<0>());
    result.template set_element<1>(s1.template element<1>() + interval * f2.template element<1>());

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
template <typename ScalarT, typename T, size_t DEPTH>
class RungeKutta4 final {
 private:
  ComputePartials<T, DEPTH> compute_partials_{};

 public:
  RungeKutta4(const ComputePartials<T, DEPTH>& compute_partials)
      : compute_partials_(compute_partials) {}

  StateT<T, DEPTH> operator()(ScalarT interval, const StateT<T, DEPTH>& s1) {
    const auto f1{compute_partials_(s1)};

    StateT<T, DEPTH> s2{};
    for (size_t i = 0; i < DEPTH; ++i) {
      s2.set_element(i, s1.element(i) + interval / 2 * f1.element(i));
    }
    const auto f2{compute_partials_(s2)};

    StateT<T, DEPTH> s3{};
    for (size_t i = 0; i < DEPTH; ++i) {
      s3.set_element(i, s1.element(i) + interval / 2 * f2.element(i));
    }
    const auto f3{compute_partials_(s3)};

    StateT<T, DEPTH> s4{};
    for (size_t i = 0; i < DEPTH; ++i) {
      s4.set_element(i, s1.element(i) + interval * f3.element(i));
    }
    const auto f4{compute_partials_(s4)};

    static constexpr ScalarT TWO{2};
    StateT<T, DEPTH> result{};
    for (size_t i = 0; i < DEPTH; ++i) {
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

template <typename ScalarT, typename T>
class RungeKutta4<ScalarT, T, 2> final {
 private:
  ComputePartials<T, 2> compute_partials_{};

 public:
  RungeKutta4(const ComputePartials<T, 2>& compute_partials)
      : compute_partials_(compute_partials) {}

  StateT<T, 2> operator()(ScalarT interval, const StateT<T, 2>& s1) {
    const auto f1{compute_partials_(s1)};

    StateT<T, 2> s2{};
    s2.template set_element<0>(s1.template element<0>() + interval / 2 * f1.template element<0>());
    s2.template set_element<1>(s1.template element<1>() + interval / 2 * f1.template element<1>());
    const auto f2{compute_partials_(s2)};

    StateT<T, 2> s3{};
    s3.template set_element<0>(s1.template element<0>() + interval / 2 * f2.template element<0>());
    s3.template set_element<1>(s1.template element<1>() + interval / 2 * f2.template element<1>());
    const auto f3{compute_partials_(s3)};

    StateT<T, 2> s4{};
    s4.template set_element<0>(s1.template element<0>() + interval * f3.template element<0>());
    s4.template set_element<1>(s1.template element<1>() + interval * f3.template element<1>());
    const auto f4{compute_partials_(s4)};

    static constexpr ScalarT TWO{2};
    StateT<T, 2> result{};
    result.template set_element<0>(s1.template element<0>() +
                                   interval / 6 *
                                       (f1.template element<0>() + TWO * f2.template element<0>() +
                                        TWO * f3.template element<0>() + f4.template element<0>()));
    result.template set_element<1>(s1.template element<1>() +
                                   interval / 6 *
                                       (f1.template element<1>() + TWO * f2.template element<1>() +
                                        TWO * f3.template element<1>() + f4.template element<1>()));

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
