#pragma once

#include <cstring>
#include <functional>

#include "math/state.h"

namespace ndyn::math {

template <typename T, size_t SIZE>
using ComputePartials = std::function<StateT<T, SIZE>(const StateT<T, SIZE>&)>;

/**
 * Implementation of the forward Euler algorithm for integrating the state of a system according to
 * a differential equation. This implementation makes no assumptions about the relationship between
 * the elements in the state. Typically, the elements of the state are derivatives of each other,
 * but this implementation does not assume that.
 */
template <typename ScalarT, typename T, size_t SIZE>
class ForwardEuler final {
 private:
  ComputePartials<T, SIZE> compute_partials_{};

 public:
  ForwardEuler(ComputePartials<T, SIZE>&& compute_partials)
      : compute_partials_(std::forward<ComputePartials<T, SIZE>>(compute_partials)) {}

  StateT<T, SIZE> operator()(ScalarT interval, StateT<T, SIZE> s0) {
    const auto partials{compute_partials_(s0)};
    for (size_t i = 0; i < SIZE; ++i) {
      s0.set_element(i, s0.element(i) + interval * partials.element(i));
    }
    return s0;
  }
};

template <typename ScalarT, typename T>
class ForwardEuler<ScalarT, T, 3> final {
 private:
  ComputePartials<T, 3> compute_partials_{};

 public:
  ForwardEuler(ComputePartials<T, 3>&& compute_partials)
      : compute_partials_(std::forward<ComputePartials<T, 3>>(compute_partials)) {}

  StateT<T, 3> operator()(ScalarT interval, StateT<T, 3> s0) {
    const auto partials{compute_partials_(s0)};
    s0.template set_element<0>(s0.template element<0>() +
                               interval * partials.template element<0>());
    s0.template set_element<1>(s0.template element<1>() +
                               interval * partials.template element<1>());
    s0.template set_element<2>(s0.template element<2>() +
                               interval * partials.template element<2>());
    return s0;
  }
};

template <typename ScalarT, typename T>
class ForwardEuler<ScalarT, T, 2> final {
 private:
  ComputePartials<T, 2> compute_partials_{};

 public:
  ForwardEuler(ComputePartials<T, 2>&& compute_partials)
      : compute_partials_(std::forward<ComputePartials<T, 2>>(compute_partials)) {}

  StateT<T, 2> operator()(ScalarT interval, StateT<T, 2> s0) {
    const auto partials{compute_partials_(s0)};
    s0.template set_element<0>(s0.template element<0>() +
                               interval * partials.template element<0>());
    s0.template set_element<1>(s0.template element<1>() +
                               interval * partials.template element<1>());
    return s0;
  }
};

template <typename ScalarT, typename T>
class ForwardEuler<ScalarT, T, 1> final {
 private:
  ComputePartials<T, 1> compute_partials_{};

 public:
  ForwardEuler(ComputePartials<T, 1>&& compute_partials)
      : compute_partials_(std::forward<ComputePartials<T, 1>>(compute_partials)) {}

  StateT<T, 1> operator()(ScalarT interval, StateT<T, 1> s0) {
    const auto partials{compute_partials_(s0)};
    s0.template set_element<0>(s0.template element<0>() +
                               interval * partials.template element<0>());
    return s0;
  }
};

/**
 * Implementation of the 2nd order Runge-Kutta algorithm for integrating the state of a system
 * according to a differential equation. This implementation makes no assumptions about the
 * relationship between the elements in the state. Typically, the elements of the state are
 * derivatives of each other, but this implementation does not assume that.
 */
template <typename ScalarT, typename T, size_t SIZE>
class RungeKutta2 final {
 private:
  ComputePartials<T, SIZE> compute_partials_{};

 public:
  RungeKutta2(ComputePartials<T, SIZE>&& compute_partials)
      : compute_partials_(std::forward<ComputePartials<T, SIZE>>(compute_partials)) {}

  StateT<T, SIZE> operator()(ScalarT interval, StateT<T, SIZE> s0) {
    const auto f1{compute_partials_(s0)};

    StateT<T, SIZE> s1{};
    for (size_t i = 0; i < SIZE; ++i) {
      s1.set_element(i, s0.element(i) + interval / 2 * f1.element(i));
    }
    const auto f2{compute_partials_(s1)};

    StateT<T, SIZE> result{};
    for (size_t i = 0; i < SIZE; ++i) {
      result.set_element(i, s0.element(i) + interval * f2.element(i));
    }

    return result;
  }
};

template <typename ScalarT, typename T>
class RungeKutta2<ScalarT, T, 3> final {
 private:
  ComputePartials<T, 3> compute_partials_{};

 public:
  RungeKutta2(ComputePartials<T, 3>&& compute_partials)
      : compute_partials_(std::forward<ComputePartials<T, 3>>(compute_partials)) {}

  StateT<T, 3> operator()(ScalarT interval, StateT<T, 3> s0) {
    const auto f1{compute_partials_(s0)};

    StateT<T, 3> s1{};
    s1.template set_element<0>(s0.template element<0>() + interval / 2 * f1.template element<0>());
    s1.template set_element<1>(s0.template element<1>() + interval / 2 * f1.template element<1>());
    s1.template set_element<2>(s0.template element<2>() + interval / 2 * f1.template element<2>());
    const auto f2{compute_partials_(s1)};

    StateT<T, 3> result{};
    result.template set_element<0>(s0.template element<0>() + interval * f2.template element<0>());
    result.template set_element<1>(s0.template element<1>() + interval * f2.template element<1>());
    result.template set_element<2>(s0.template element<2>() + interval * f2.template element<2>());

    return result;
  }
};

template <typename ScalarT, typename T>
class RungeKutta2<ScalarT, T, 2> final {
 private:
  ComputePartials<T, 2> compute_partials_{};

 public:
  RungeKutta2(ComputePartials<T, 2>&& compute_partials)
      : compute_partials_(std::forward<ComputePartials<T, 2>>(compute_partials)) {}

  StateT<T, 2> operator()(ScalarT interval, StateT<T, 2> s0) {
    const auto f1{compute_partials_(s0)};

    StateT<T, 2> s1{};
    s1.template set_element<0>(s0.template element<0>() + interval / 2 * f1.template element<0>());
    s1.template set_element<1>(s0.template element<1>() + interval / 2 * f1.template element<1>());
    const auto f2{compute_partials_(s1)};

    StateT<T, 2> result{};
    result.template set_element<0>(s0.template element<0>() + interval * f2.template element<0>());
    result.template set_element<1>(s0.template element<1>() + interval * f2.template element<1>());

    LOG(INFO) << "interval: " << interval;
    LOG(INFO) << "s0: " << s0;
    LOG(INFO) << "f1: " << f1;
    LOG(INFO) << "s1: " << s1;
    LOG(INFO) << "f2: " << f2;
    LOG(INFO) << "result: " << result;

    return result;
  }
};

template <typename ScalarT, typename T>
class RungeKutta2<ScalarT, T, 1> final {
 private:
  ComputePartials<T, 1> compute_partials_{};

 public:
  RungeKutta2(ComputePartials<T, 1>&& compute_partials)
      : compute_partials_(std::forward<ComputePartials<T, 1>>(compute_partials)) {}

  StateT<T, 1> operator()(ScalarT interval, StateT<T, 1> s0) {
    const auto f1{compute_partials_(s0)};

    StateT<T, 1> s1{};
    s1.template set_element<0>(s0.template element<0>() + interval / 2 * f1.template element<0>());
    const auto f2{compute_partials_(s1)};

    StateT<T, 1> result{};
    result.template set_element<0>(s0.template element<0>() + interval * f2.template element<0>());

    return result;
  }
};

/**
 * Implementation of the 4th order Runge-Kutta algorithm for integrating the state of a system
 * according to a differential equation. This implementation makes no assumptions about the
 * relationship between the elements in the state. Typically, the elements of the state are
 * derivatives of each other, but this implementation does not assume that.
 */
template <typename ScalarT, typename T, size_t SIZE>
class RungeKutta4 final {
 private:
  ComputePartials<T, SIZE> compute_partials_{};

 public:
  RungeKutta4(ComputePartials<T, SIZE>&& compute_partials)
      : compute_partials_(std::forward<ComputePartials<T, SIZE>>(compute_partials)) {}

  StateT<T, SIZE> operator()(ScalarT interval, StateT<T, SIZE> s1) {
    const auto f1{compute_partials_(s1)};

    StateT<T, SIZE> s2{};
    for (size_t i = 0; i < SIZE; ++i) {
      s2.set_element(i, s1.element(i) + interval / 2 * f1.element(i));
    }
    const auto f2{compute_partials_(s2)};

    StateT<T, SIZE> s3{};
    for (size_t i = 0; i < SIZE; ++i) {
      s3.set_element(i, s1.element(i) + interval / 2 * f2.element(i));
    }
    const auto f3{compute_partials_(s3)};

    StateT<T, SIZE> s4{};
    for (size_t i = 0; i < SIZE; ++i) {
      s4.set_element(i, s1.element(i) + interval * f3.element(i));
    }
    const auto f4{compute_partials_(s4)};

    static constexpr ScalarT TWO{2};
    StateT<T, SIZE> result{};
    for (size_t i = 0; i < SIZE; ++i) {
      result.set_element(i, s1.element(i) + interval / 6 *
                                                (f1.element(i) + TWO * f2.element(i) +
                                                 TWO * f3.element(i) + f4.element(i)));
    }

    LOG(INFO) << "interval: " << interval;
    LOG(INFO) << "s1: " << s1;
    LOG(INFO) << "f1: " << f1;
    LOG(INFO) << "s2: " << s2;
    LOG(INFO) << "f2: " << f2;
    LOG(INFO) << "s3: " << s3;
    LOG(INFO) << "f3: " << f3;
    LOG(INFO) << "s4: " << s4;
    LOG(INFO) << "f4: " << f4;
    LOG(INFO) << "result: " << result;

    return result;
  }
};

template <typename ScalarT, typename T>
class RungeKutta4<ScalarT, T, 3> final {
 private:
  ComputePartials<T, 3> compute_partials_{};

 public:
  RungeKutta4(ComputePartials<T, 3>&& compute_partials)
      : compute_partials_(std::forward<ComputePartials<T, 3>>(compute_partials)) {}

  StateT<T, 3> operator()(ScalarT interval, StateT<T, 3> s1) {
    const auto f1{compute_partials_(s1)};

    StateT<T, 3> s2{};
    s2.template set_element<0>(s1.template element<0>() + interval / 2 * f1.template element<0>());
    s2.template set_element<1>(s1.template element<1>() + interval / 2 * f1.template element<1>());
    s2.template set_element<2>(s1.template element<2>() + interval / 2 * f1.template element<2>());
    const auto f2{compute_partials_(s2)};

    StateT<T, 3> s3{};
    s3.template set_element<0>(s1.template element<0>() + interval / 2 * f2.template element<0>());
    s3.template set_element<1>(s1.template element<1>() + interval / 2 * f2.template element<1>());
    s3.template set_element<2>(s1.template element<2>() + interval / 2 * f2.template element<2>());
    const auto f3{compute_partials_(s3)};

    StateT<T, 3> s4{};
    s4.template set_element<0>(s1.template element<0>() + interval * f3.template element<0>());
    s4.template set_element<1>(s1.template element<1>() + interval * f3.template element<1>());
    s4.template set_element<2>(s1.template element<2>() + interval * f3.template element<2>());
    const auto f4{compute_partials_(s4)};

    static constexpr ScalarT TWO{2};
    StateT<T, 3> result{};
    result.template set_element<0>(s1.template element<0>() +
                                   interval / 6 *
                                       (f1.template element<0>() + TWO * f2.template element<0>() +
                                        TWO * f3.template element<0>() + f4.template element<0>()));
    result.template set_element<1>(s1.template element<1>() +
                                   interval / 6 *
                                       (f1.template element<1>() + TWO * f2.template element<1>() +
                                        TWO * f3.template element<1>() + f4.template element<1>()));
    result.template set_element<2>(s1.template element<2>() +
                                   interval / 6 *
                                       (f1.template element<2>() + TWO * f2.template element<2>() +
                                        TWO * f3.template element<2>() + f4.template element<2>()));

    LOG(INFO) << "interval: " << interval;
    LOG(INFO) << "s1: " << s1;
    LOG(INFO) << "f1: " << f1;
    LOG(INFO) << "s2: " << s2;
    LOG(INFO) << "f2: " << f2;
    LOG(INFO) << "s3: " << s3;
    LOG(INFO) << "f3: " << f3;
    LOG(INFO) << "s4: " << s4;
    LOG(INFO) << "f4: " << f4;
    LOG(INFO) << "result: " << result;

    return result;
  }
};

template <typename ScalarT, typename T>
class RungeKutta4<ScalarT, T, 2> final {
 private:
  ComputePartials<T, 2> compute_partials_{};

 public:
  RungeKutta4(ComputePartials<T, 2>&& compute_partials)
      : compute_partials_(std::forward<ComputePartials<T, 2>>(compute_partials)) {}

  StateT<T, 2> operator()(ScalarT interval, StateT<T, 2> s1) {
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

    LOG(INFO) << "interval: " << interval;
    LOG(INFO) << "s1: " << s1;
    LOG(INFO) << "f1: " << f1;
    LOG(INFO) << "s2: " << s2;
    LOG(INFO) << "f2: " << f2;
    LOG(INFO) << "s3: " << s3;
    LOG(INFO) << "f3: " << f3;
    LOG(INFO) << "s4: " << s4;
    LOG(INFO) << "f4: " << f4;
    LOG(INFO) << "result: " << result;

    return result;
  }
};

template <typename ScalarT, typename T>
class RungeKutta4<ScalarT, T, 1> final {
 private:
  ComputePartials<T, 1> compute_partials_{};

 public:
  RungeKutta4(ComputePartials<T, 1>&& compute_partials)
      : compute_partials_(std::forward<ComputePartials<T, 1>>(compute_partials)) {}

  StateT<T, 1> operator()(ScalarT interval, StateT<T, 1> s1) {
    const auto f1{compute_partials_(s1)};

    StateT<T, 1> s2{};
    s2.template set_element<0>(s1.template element<0>() + interval / 2 * f1.template element<0>());
    const auto f2{compute_partials_(s2)};

    StateT<T, 1> s3{};
    s3.template set_element<0>(s1.template element<0>() + interval / 2 * f2.template element<0>());
    const auto f3{compute_partials_(s3)};

    StateT<T, 1> s4{};
    s4.template set_element<0>(s1.template element<0>() + interval * f3.template element<0>());
    const auto f4{compute_partials_(s4)};

    static constexpr ScalarT TWO{2};
    StateT<T, 1> result{};
    result.template set_element<0>(s1.template element<0>() +
                                   interval / 6 *
                                       (f1.template element<0>() + TWO * f2.template element<0>() +
                                        TWO * f3.template element<0>() + f4.template element<0>()));

    LOG(INFO) << "interval: " << interval;
    LOG(INFO) << "s1: " << s1;
    LOG(INFO) << "f1: " << f1;
    LOG(INFO) << "s2: " << s2;
    LOG(INFO) << "f2: " << f2;
    LOG(INFO) << "s3: " << s3;
    LOG(INFO) << "f3: " << f3;
    LOG(INFO) << "s4: " << s4;
    LOG(INFO) << "f4: " << f4;
    LOG(INFO) << "result: " << result;

    return result;
  }
};

}  // namespace ndyn::math
