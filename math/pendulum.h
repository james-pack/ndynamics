#pragma once

#include <cmath>
#include <cstring>

#include "glog/logging.h"
#include "math/integrators.h"
#include "math/multivector.h"
#include "math/multivector_utils.h"
#include "math/state.h"

namespace ndyn::math {

/**
 * Simple utility that simulates the state of a pendulum at any time. The main purpose of this
 * utility is to provide a model for testing integrator routines (Runge-Kutta, Euler's method,
 * etc.).
 *
 * Note that this simulation does NOT use a small angle approximation; it numerically approximates
 * the full 2nd order differential equation of motion.
 */
template <typename T>
class ClassicPendulum final {
 private:
  using StateType = math::StateT<T, 2>;

  const T g_;  // Acceleration due to gravity.

  const T mass_;    // kg
  const T length_;  // m

  T t_;  // seconds
  StateType state_;

  math::RungeKutta4<T, T, 2> integrator_{[this](const StateType& state) -> StateType {
    using std::sin;
    StateType result{state.shift()};
    result.template set_element<1>((g_ / length_) * sin(state.template element<0>()));
    return result;
  }};

 public:
  // Initialize a pendulum of a certain mass (kg) and length (m) at some initial angle theta
  // (radians) and initial velocity theta_dot (radians/s) at time t (seconds).
  ClassicPendulum(T g, T mass, T length, T t, T theta, T theta_dot)
      : g_(g), mass_(mass), length_(length), t_(t), state_({theta, theta_dot}) {}

  T g() const { return g_; }

  T mass() const { return mass_; }
  T length() const { return length_; }

  T current_time() const { return t_; }

  /**
   * Evolve the state of the system to the new_time using the given step_size. A default step_size
   * is provided; see code below for details. Note that it is possible to evolve to a time before
   * the current time.
   */
  void goto_time(T new_time, T step_size = 0) {
    using std::abs;

    if (step_size == 0) {
      step_size = abs(g_) / 1000;
    }

    if (new_time < t_ && step_size > 0) {
      step_size = -step_size;
    }

    if (abs(t_ - new_time) > abs(step_size)) {
      do {
        t_ += step_size;
        state_ = integrator_(step_size, state_);
      } while (abs(t_ - new_time) > abs(step_size));
    }
  }

  void evolve(T time_increment, T step_size = 0) { goto_time(t_ + time_increment, step_size); }

  T theta() const { return state_.template element<0>(); }
  T theta_dot() const { return state_.template element<1>(); }
};

template <typename T = float>
class ClassicPendulumConfigurator final {
 private:
  T mass_{1};
  T length_{1};
  T initial_time_{0};
  T theta_{};
  T theta_dot_{};
  T g_{-1};

 public:
  /**
   * Set the mass of the weight at the end of the pendulum.
   * Defaults to one.
   */
  T get_mass() const { return mass_; }
  ClassicPendulumConfigurator& set_mass(T mass) {
    mass_ = mass;
    return *this;
  }

  /**
   * Set the length of the pendulum.
   * Defaults to one.
   */
  T get_length() const { return length_; }
  ClassicPendulumConfigurator& set_length(T length) {
    length_ = length;
    return *this;
  }

  /**
   * The time of the initial conditions.
   * Defaults to zero.
   */
  T get_initial_time() const { return initial_time_; }
  ClassicPendulumConfigurator& set_initial_time(T initial_time) {
    initial_time_ = initial_time;
    return *this;
  }

  /**
   * Initial angular position in radians, with zero meaning "down" and pi radians meaning "up".
   * Defaults to zero.
   */
  T get_theta() const { return theta_; }
  ClassicPendulumConfigurator& set_theta(T theta) {
    theta_ = theta;
    return *this;
  }

  /**
   * Initial angular velocity in radians/second.
   * Defaults to zero.
   */
  T get_theta_dot() const { return theta_dot_; }
  ClassicPendulumConfigurator& set_theta_dot(T theta_dot) {
    theta_dot_ = theta_dot;
    return *this;
  }

  /**
   * Set the acceleration due to gravity. Negative values are "down" in that they will pull theta
   * towards zero. Defaults to -9.8, ostensibly in m / s^2.
   */
  T get_g() const { return g_; }
  ClassicPendulumConfigurator& set_g(T g) {
    using std::abs;
    if (g > 0) {
      g_ = -g;
    } else {
      g_ = g;
    }
    return *this;
  }

  /**
   * Create the pendulum as configured. Note that the ClassicPendulumConfigurator object can be
   * reused, if desired, and its state is not changed after calling this method.
   */
  ClassicPendulum<T> create() const {
    return ClassicPendulum<T>{g_, mass_, length_, initial_time_, theta_, theta_dot_};
  }
};

/**
 * Multivector form of ClassicPendulum above.
 */
template <typename MultivectorT>
class GAPendulum final {
 public:
  using ScalarType = typename MultivectorT::ScalarType;
  using StateType = math::StateT<MultivectorT, 2>;

 private:
  const ScalarType mass_;
  const MultivectorT gravitational_acceleration_;  // Acceleration due to gravity.

  ScalarType t_;

  StateType state_{};

  math::RungeKutta4<ScalarType, MultivectorT, 2> integrator_{
      [this](const StateType& state) -> StateType {
        StateType result{state.shift()};
        result.template set_element<1>(
            decompose(gravitational_acceleration_, state.template element<0>()).second);
        return result;
      }};

 public:
  GAPendulum(ScalarType mass, ScalarType t, MultivectorT position, MultivectorT velocity,
             MultivectorT gravitational_acceleration)
      : mass_(mass),
        gravitational_acceleration_(gravitational_acceleration),
        t_(t),
        state_({position, velocity}) {}

  ScalarType current_time() const { return t_; }

  ScalarType length() const {
    using std::abs;
    return abs(position());
  }

  const MultivectorT& position() const { return state_.template element<0>(); }
  const MultivectorT& velocity() const { return state_.template element<1>(); }
  const MultivectorT& acceleration() const { return state_.template element<2>(); }

  ScalarType theta() const {
    using std::abs;
    using std::acos;

    // Unit vector in the direction of gravity.
    const auto g_hat{gravitational_acceleration_ / abs(gravitational_acceleration_)};

    // This quadrant selector computes whether the position and gravity vectors are in the same, or
    // different, orientation from e0 and e1. If the two sets of vectors are in the same
    // orientation, the quadrant_selector variable will be positive; if the position is aligned with
    // gravity, it will be zero; otherwise, it will be negative.
    // We use this selector to compute theta. The std::acos() function only returns values on [0,
    // pi]. We use this selector to expand that return value to [-pi, pi].
    const auto quadrant_selector{
        state_.template element<0>()
            .outer(g_hat)
            .left_contraction(MultivectorT::template e<0>() * MultivectorT::template e<1>())
            .scalar()};

    const ScalarType sign{(quadrant_selector < 0) ? static_cast<ScalarType>(-1)
                                                  : static_cast<ScalarType>(1)};

    return sign * acos(state_.template element<0>().left_contraction(g_hat).scalar() /
                       abs(state_.template element<0>()));
  }

  /**
   * Evolve the state of the system to the new_time using the given step_size. A default step_size
   * is provided; see code below for details. Note that it is possible to evolve to a time before
   * the current time.
   */
  void goto_time(ScalarType new_time, ScalarType step_size = 0) {
    using std::abs;

    if (step_size == 0) {
      step_size = abs(gravitational_acceleration_) / 1000;

      VLOG(5) << "Default step size used. step_size: " << step_size;
    }

    if (new_time < t_ && step_size > 0) {
      step_size = -step_size;
    }

    if (abs(t_ - new_time) > abs(step_size)) {
      do {
        t_ += step_size;
        state_ = integrator_(step_size, state_);

        VLOG(4) << "t_: " << t_ << ", theta(): " << theta();
      } while (abs(t_ - new_time) > abs(step_size));
    }
  }

  void evolve(ScalarType time_increment, ScalarType step_size = 0) {
    goto_time(t_ + time_increment, step_size);
  }
};

template <typename MultivectorT>
class GAPendulumConfigurator final {
 public:
  using ScalarType = typename MultivectorT::ScalarType;

 private:
  ScalarType mass_{1};
  ScalarType length_{1};
  ScalarType speed_{0};
  ScalarType initial_time_{0};
  ScalarType theta_{};
  ScalarType g_{1};

  static constexpr auto e0 = MultivectorT::template e<0>();
  static constexpr auto e1 = MultivectorT::template e<1>();

 public:
  /**
   * Set the mass of the weight at the end of the pendulum.
   * Defaults to one.
   */
  ScalarType mass() const { return mass_; }
  GAPendulumConfigurator& set_mass(ScalarType mass) {
    mass_ = mass;
    return *this;
  }

  /**
   * Set the length of the pendulum.
   * Defaults to one.
   */
  ScalarType length() const { return length_; }
  GAPendulumConfigurator& set_length(ScalarType length) {
    length_ = length;
    return *this;
  }

  ScalarType speed() const { return speed_; }
  GAPendulumConfigurator& set_speed(ScalarType speed) {
    speed_ = speed;
    return *this;
  }

  /**
   * The time of the initial conditions.
   * Defaults to zero.
   */
  ScalarType initial_time() const { return initial_time_; }
  GAPendulumConfigurator& set_initial_time(ScalarType initial_time) {
    initial_time_ = initial_time;
    return *this;
  }

  /**
   * Initial angular position in radians, with zero meaning "down" and pi radians meaning "up".
   * Defaults to zero.
   */
  ScalarType theta() const { return theta_; }
  GAPendulumConfigurator& set_theta(ScalarType theta) {
    theta_ = theta;
    return *this;
  }

  /**
   * Set the acceleration due to gravity.
   */
  ScalarType g() const { return g_; }
  GAPendulumConfigurator& set_g(ScalarType g) {
    if (g < 0) {
      g_ = -g;
    } else {
      g_ = g;
    }
    return *this;
  }

  MultivectorT calculate_position() const {
    using std::cos;
    using std::sin;

    return length_ * sin(theta_) * e0 - length_ * cos(theta_) * e1;
  }

  MultivectorT calculate_velocity() const {
    using std::cos;
    using std::sin;
    return speed_ * cos(theta_) * e0 - speed_ * sin(theta_) * e1;
  }

  MultivectorT gravitational_acceleration() const { return -g_ * e1; }

  /**
   * Create the pendulum as configured. Note that the GAPendulumConfigurator object can be
   * reused, if desired, and its state is not changed after calling this method.
   */
  GAPendulum<MultivectorT> create() const {
    return GAPendulum<MultivectorT>{mass_, initial_time_, calculate_position(),
                                    calculate_velocity(), gravitational_acceleration()};
  }
};

}  // namespace ndyn::math
