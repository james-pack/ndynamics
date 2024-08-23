#pragma once

#include <cmath>

namespace ndyn::control {

/**
 * Simple utility that simulates the state of a pendulum at any time. The main purpose of this
 * utility is to provide a physical model for testing control systems.
 *
 * Note that this simulation does NOT use a small angle approximation; it numerically approximates
 * the full 2nd order differential equation of motion.
 */
template <typename T>
class Pendulum final {
 private:
  const T g_;   // Acceleration due to gravity in m / s^2 with negative values consistent with
                // pointing in the direction of zero radians.
  const T mu_;  // Dampening factor applied to the angular velocity. Unitless.

  const T mass_;    // kg
  const T length_;  // m

  T t_{};          // seconds
  T theta_{};      // Angle in radians with zero pointing in the direction of gravity.
  T theta_dot_{};  // Angular velocity in radians/s.

  T get_theta_double_dot() { return -mu_ * theta_dot_ + (g_ / length_) * std::sin(theta_); }

 public:
  // Initialize a pendulum of a certain mass (kg) and length (m) at some initial angle theta
  // (radians) and initial velocity theta_dot (radians/s) at time t (seconds).
  Pendulum(T g, T mu, T mass, T length, T t, T theta, T theta_dot)
      : g_(g), mu_(mu), mass_(mass), length_(length), t_(t), theta_(theta), theta_dot_(theta_dot) {}

  T g() const { return g_; }
  T mu() const { return mu_; }

  T mass() const { return mass_; }
  T length() const { return length_; }

  T current_time() const { return t_; }

  /**
   * Evolve the state of the system to the new_time using the given step_size. A default step_size
   * is provided; see code below for details. Note that it is possible to evolve to a time before
   * the current time.
   */
  void goto_time(T new_time, T step_size = 0) {
    if (step_size == 0) {
      step_size = std::abs(g_) / 1000;
    }

    if (new_time < t_ && step_size > 0) {
      step_size = -step_size;
    }

    while (t_ < new_time) {
      T theta_double_dot = get_theta_double_dot();
      theta_ += theta_dot_ * step_size;
      theta_dot_ += theta_double_dot * step_size;
      t_ += step_size;
    }
  }

  void evolve(T time_increment, T step_size = 0) { goto_time(t_ + time_increment, step_size); }

  T theta() const { return theta_; }
  T theta_dot() const { return theta_dot_; }
};

template <typename T = float>
class PendulumConfigurator final {
 private:
  T mass_{1};
  T length_{1};
  T initial_time_{0};
  T theta_{};
  T theta_dot_{};
  T mu_{0};
  T g_{-9.8f};

 public:
  /**
   * Set the mass of the weight at the end of the pendulum.
   * Defaults to one.
   */
  T get_mass() const { return mass_; }
  PendulumConfigurator& set_mass(T mass) {
    mass_ = mass;
    return *this;
  }

  /**
   * Set the length of the pendulum.
   * Defaults to one.
   */
  T get_length() const { return length_; }
  PendulumConfigurator& set_length(T length) {
    length_ = length;
    return *this;
  }

  /**
   * The time of the initial conditions.
   * Defaults to zero.
   */
  T get_initial_time() const { return initial_time_; }
  PendulumConfigurator& set_initial_time(T initial_time) {
    initial_time_ = initial_time;
    return *this;
  }

  /**
   * Initial angular position in radians, with zero meaning "down" and pi radians meaning "up".
   * Defaults to zero.
   */
  T get_theta() const { return theta_; }
  PendulumConfigurator& set_theta(T theta) {
    theta_ = theta;
    return *this;
  }

  /**
   * Initial angular velocity in radians/second.
   * Defaults to zero.
   */
  T get_theta_dot() const { return theta_dot_; }
  PendulumConfigurator& set_theta_dot(T theta_dot) {
    theta_dot_ = theta_dot;
    return *this;
  }

  /**
   * Set a dampening factor. This dampening is modelled as being this factor times the angular
   * velocity. Useful for modelling air resistance or friction in the system.
   * Defaults to zero, giving no dampening.
   */
  T get_mu() const { return mu_; }
  PendulumConfigurator& set_mu(T mu) {
    mu_ = mu;
    return *this;
  }

  /**
   * Set the acceleration due to gravity. Negative values are "down" in that they will pull theta
   * towards zero. Defaults to -9.8, ostensibly in m / s^2.
   */
  T get_g() const { return g_; }
  PendulumConfigurator& set_g(T g) {
    g_ = g;
    return *this;
  }

  /**
   * Create the pendulum as configured. Note that the PendulumConfigurator object can be reused, if
   * desired, and its state is not changed after calling this method.
   */
  Pendulum<T> create() const {
    return Pendulum<T>{g_, mu_, mass_, length_, initial_time_, theta_, theta_dot_};
  }
};

}  // namespace ndyn::control
