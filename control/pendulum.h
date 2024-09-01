#pragma once

#include <cmath>
#include <cstring>

#include "glog/logging.h"

namespace ndyn::control {

/**
 * Simple utility that simulates the state of a pendulum at any time. The main purpose of this
 * utility is to provide a physical model for testing control systems.
 *
 * Note that this simulation does NOT use a small angle approximation; it numerically approximates
 * the full 2nd order differential equation of motion.
 */
template <typename T>
class ClassicPendulum final {
 private:
  const T g_;   // Acceleration due to gravity.
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
  ClassicPendulum(T g, T mu, T mass, T length, T t, T theta, T theta_dot)
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
    using std::abs;

    if (step_size == 0) {
      step_size = abs(g_) / 1000;
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
class ClassicPendulumConfigurator final {
 private:
  T mass_{1};
  T length_{1};
  T initial_time_{0};
  T theta_{};
  T theta_dot_{};
  T mu_{0};
  T g_{-1.f};

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
   * Set a dampening factor. This dampening is modelled as being this factor times the angular
   * velocity. Useful for modelling air resistance or friction in the system.
   * Defaults to zero, giving no dampening.
   */
  T get_mu() const { return mu_; }
  ClassicPendulumConfigurator& set_mu(T mu) {
    mu_ = mu;
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
    return ClassicPendulum<T>{g_, mu_, mass_, length_, initial_time_, theta_, theta_dot_};
  }
};

/**
 * Multivector form of ClassicPendulum above.
 */
template <typename MultivectorT>
class GAPendulum final {
 public:
  using ScalarType = typename MultivectorT::ScalarType;

 private:
  const ScalarType mass_;
  const MultivectorT gravitational_acceleration_;  // Acceleration due to gravity.

  ScalarType t_;

  MultivectorT position_;
  MultivectorT velocity_;
  MultivectorT acceleration_{};

  void update_acceleration() {
    using std::abs;
    const auto pos_hat{position_ / abs(position_)};
    acceleration_ = pos_hat * pos_hat.outer(gravitational_acceleration_);
  }

 public:
  GAPendulum(ScalarType mass, ScalarType t, MultivectorT position, MultivectorT velocity,
             MultivectorT gravitational_acceleration)
      : mass_(mass),
        gravitational_acceleration_(gravitational_acceleration),
        t_(t),
        position_(position),
        velocity_(velocity) {
    update_acceleration();
  }

  ScalarType current_time() const { return t_; }

  ScalarType length() const {
    using std::abs;
    return abs(position_);
  }

  const MultivectorT& position() const { return position_; }
  const MultivectorT& velocity() const { return velocity_; }
  const MultivectorT& acceleration() const { return acceleration_; }

  ScalarType theta() const {
    using std::abs;
    using std::acos;

    // Unit vectors in the direction of gravity and the position of the pendulum.
    const auto g_hat{gravitational_acceleration_ / abs(gravitational_acceleration_)};
    const auto pos_hat{position_ / abs(position_)};

    const auto quadrant_selector{
        pos_hat.outer(g_hat)
            .left_contraction(MultivectorT::template e<0>() * MultivectorT::template e<1>())
            .scalar()};

    const ScalarType sign{(quadrant_selector < 0) ? static_cast<ScalarType>(-1)
                                                  : static_cast<ScalarType>(1)};

    return sign * acos(pos_hat.left_contraction(g_hat).scalar());
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

    for (t_ += step_size; abs(t_ - new_time) > abs(step_size); t_ += step_size) {
      update_acceleration();
      velocity_ += step_size * acceleration_;
      position_ += step_size * velocity_;

      VLOG(4) << "t_: " << t_ << ", theta(): " << theta();

      VLOG(5) << ", position_: " << position_  //
              << ", velocity_: " << velocity_  //
              << ", acceleration_: " << acceleration_;
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

}  // namespace ndyn::control
