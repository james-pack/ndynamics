#pragma once

#include <cmath>
#include <cstring>

#include "base/pi.h"
#include "glog/logging.h"
#include "math/integrators.h"
#include "math/multivector.h"
#include "math/multivector_utils.h"
#include "math/state.h"

namespace ndyn::simulation {

/**
 * This method computes the period of the pendulum using the small angle approximation, but
 * corrected for circular error. See https://en.wikipedia.org/wiki/Pendulum#Period_of_oscillation
 * https://en.wikipedia.org/wiki/Pendulum_(mechanics)#Power_series_solution_for_the_elliptic_integral
 * and
 * https://en.wikipedia.org/wiki/Pendulum_(mechanics)#Approximate_formulae_for_the_nonlinear_pendulum_period
 * for more details.
 */
template <typename ScalarType>
inline constexpr ScalarType compute_period(ScalarType length, ScalarType g, ScalarType angle) {
  using std::cos;
  using std::log;
  using std::pow;
  using std::sin;
  using std::sqrt;

  const ScalarType T0{static_cast<ScalarType>(2 * pi * sqrt(length / g))};
  if (angle < pi / 4) {
    return T0 * (1 + pow(angle, 2) / 16 +                                    //
                 11 * pow(angle, 4) / 3072 + 173 * pow(angle, 6) / 737280 +  //
                 22931 * pow(angle, 8) / 1'321'205'760 +                     //
                 1'319'183 * pow(angle, 10) / 951'268'147'200);
  } else {
    const ScalarType a{cos(angle / 2)};
    const ScalarType k{sin(angle / 2)};

    const ScalarType Tlima{-T0 * log(a) / (1 - a)};

    const ScalarType Tcromer{static_cast<ScalarType>(2 / pi * T0 * log(4 / a))};

    constexpr ScalarType r{7.17};  // Averaging constant determined by Lima in 2008.

    return (r * a * a * Tlima + k * k * Tcromer) / (r * a * a + k * k);
  }
}

/**
 * Simple utility that simulates the state of a pendulum at any time.
 *
 * Note that this simulation does NOT use a small angle approximation; it numerically approximates
 * the full 2nd order differential equation of motion.
 */
template <typename MultivectorT>
class Pendulum final {
 public:
  using MultivectorType = MultivectorT;
  using ScalarType = typename MultivectorType::ScalarType;
  using StateType = math::StateT<MultivectorType, 2>;

 private:
  // Precomputed magnitude of the position vector. The length of the pendulum.
  const ScalarType length_;

  // Precomputed magnitude of the gravitational acceleration.
  const ScalarType g_;

  // Initial position of the pendulum. We restrict the initial velocity to zero, so this vector also
  // gives the maximum angle for the pendulum.
  const MultivectorType initial_position_;

  // Acceleration due to gravity as a vector.
  const MultivectorType gravitational_acceleration_;

  // Precomputed period of the pendulum.
  const ScalarType period_;

  const ScalarType initial_time_;

  ScalarType t_;
  StateType state_;

  math::RungeKutta4<ScalarType, MultivectorType, 2> integrator_{
      [this](const StateType& state) -> StateType {
        StateType result{state.shift()};
        result.template set_element<1>(
            decompose(gravitational_acceleration_, state.template element<0>()).second);
        return result;
      }};

  // Mass isn't used directly in calculations. It is available for reference in the API.
  const ScalarType mass_;

  constexpr static ScalarType compute_max_angle(const MultivectorType& position,
                                                const MultivectorType& gravitational_acceleration) {
    using std::abs;
    using std::acos;

    // Unit vector in the direction of gravity.
    const auto g_hat{gravitational_acceleration / abs(gravitational_acceleration)};

    // Return the angle computed directly from acos(). This angle will be on [0, pi]. In this
    // method, we are concerned with computing the maximum angle; we do not care whether the
    // pendulum is swung to the left or right.
    return acos(position.left_contraction(g_hat).scalar() / abs(position));
  }

 public:
  constexpr Pendulum(ScalarType mass, MultivectorType gravitational_acceleration, ScalarType t,
                     MultivectorType position)
      : length_(abs(position)),
        g_(abs(gravitational_acceleration)),
        initial_position_(position),
        gravitational_acceleration_(gravitational_acceleration),
        period_(
            compute_period(length_, g_, compute_max_angle(position, gravitational_acceleration_))),
        initial_time_(t),
        t_(t),
        state_({position}),
        mass_(mass) {
    LOG(INFO) << "Pendulum::Pendulum() -- period_: " << period_;
  }

  ScalarType mass() const { return mass_; }
  // Magnitude of the acceleration due to gravity.
  ScalarType g() const { return g_; }
  ScalarType period() const { return period_; }
  ScalarType length() const { return length_; }
  ScalarType current_time() const { return t_; }

  const StateType& state() const { return state_; }
  const MultivectorType& position() const { return state_.template element<0>(); }
  const MultivectorType& velocity() const { return state_.template element<1>(); }
  const MultivectorType& acceleration() const {
    return decompose(gravitational_acceleration_, position()).second;
  }

  const MultivectorType& graviational_acceleration() const { return gravitational_acceleration_; }

  ScalarType height() const {
    // Extract the aspect of the position vector that is parallel to the gravitational acceleration
    // direction.
    const auto parallel{math::parallel(position(), gravitational_acceleration_)};
    // Construct the vector that represents the position of the pendulum at the bottom of its swing.
    const auto bottom_of_swing{length_ / g_ * gravitational_acceleration_};

    return abs(parallel - bottom_of_swing);
  }

  ScalarType compute_potential_energy() const { return mass_ * g_ * height(); }

  ScalarType compute_kinetic_energy() const { return mass_ / 2 * square_magnitude(velocity()); }

  ScalarType theta() const {
    using std::abs;
    using std::acos;

    // Unit vector in the direction of gravity.
    const auto g_hat{gravitational_acceleration_ / g_};

    // This quadrant selector computes whether the position and gravity vectors are in the same, or
    // different, orientation from e0 and e1. If the two sets of vectors are in the same
    // orientation, the quadrant_selector variable will be positive; if the position is aligned with
    // gravity, it will be zero; otherwise, it will be negative.
    // We use this selector to compute theta. The std::acos() function only returns values on [0,
    // pi]. We use this selector to expand that return value to [-pi, pi].
    const auto quadrant_selector{
        position()
            .outer(gravitational_acceleration_)
            .left_contraction(MultivectorType::template e<0>() * MultivectorType::template e<1>())
            .scalar()};

    const ScalarType sign{(quadrant_selector < 0) ? static_cast<ScalarType>(-1)
                                                  : static_cast<ScalarType>(1)};

    VLOG(5) << "theta() -- position(): " << position();
    VLOG(5) << "theta() -- gravitational_acceleration_: " << gravitational_acceleration_;
    VLOG(5) << "theta() -- length_: " << length_;
    VLOG(5) << "theta() -- abs(position()): " << abs(position());

    VLOG(5) << "theta() -- value for acos(): "
            << position().left_contraction(gravitational_acceleration_).scalar() / abs(position());

    return sign * acos(position().left_contraction(g_hat).scalar() / abs(position()));
  }

  /**
   * Set the state of the system to the new_time using the given step_size. A default step_size
   * is provided; see code below for details. Note that it is possible to evolve to a time before
   * the current time.
   */
  void goto_time(ScalarType new_time, ScalarType step_size = 0) {
    using std::abs;
    using std::remainder;

    if (step_size == 0) {
      step_size = g_ / 1000;

      VLOG(5) << "Default step size used. step_size: " << step_size;
    }

    if (new_time < t_ && step_size > 0) {
      step_size = -step_size;
    }

    if (abs(t_ - new_time) > abs(step_size)) {
      do {
        t_ += step_size;

        if (abs(remainder(t_, period_)) < abs(step_size)) {
          state_ = {initial_position_};
        } else if (abs(remainder(t_, period_ / 2)) < abs(step_size)) {
          state_ = {reflect(initial_position_, gravitational_acceleration_)};
        } else {
          state_ = integrator_(step_size, state_);

          // Renomalize length after each step integration step. This helps stabilize the overall
          // system by enforcing the constraint that the pendulum's length is constant.
          state_.template set_element<0>(length_ / abs(state_.template element<0>()) *
                                         state_.template element<0>());
        }
        VLOG(4) << "t_: " << t_ << ", theta(): " << theta();
      } while (abs(t_ - new_time) > abs(step_size));
    }
  }

  void evolve(ScalarType time_increment, ScalarType step_size = 0) {
    goto_time(t_ + time_increment, step_size);
  }
};

template <typename MultivectorT>
class PendulumConfigurator final {
 public:
  using MultivectorType = MultivectorT;
  using ScalarType = typename MultivectorType::ScalarType;

 private:
  ScalarType mass_{1};
  ScalarType length_{1};
  ScalarType initial_time_{0};
  ScalarType theta_{};
  ScalarType g_{1};

  static constexpr auto e0 = MultivectorType::template e<0>();
  static constexpr auto e1 = MultivectorType::template e<1>();

 public:
  /**
   * Set the mass of the weight at the end of the pendulum.
   * Defaults to one.
   */
  ScalarType mass() const { return mass_; }
  PendulumConfigurator& set_mass(ScalarType mass) {
    mass_ = mass;
    return *this;
  }

  /**
   * Set the length of the pendulum.
   * Defaults to one.
   */
  ScalarType length() const { return length_; }
  PendulumConfigurator& set_length(ScalarType length) {
    length_ = length;
    return *this;
  }

  /**
   * The time of the initial conditions.
   * Defaults to zero.
   */
  ScalarType initial_time() const { return initial_time_; }
  PendulumConfigurator& set_initial_time(ScalarType initial_time) {
    initial_time_ = initial_time;
    return *this;
  }

  /**
   * Initial angular position in radians, with zero meaning "down" and pi radians meaning "up".
   * Defaults to zero.
   */
  ScalarType theta() const { return theta_; }
  PendulumConfigurator& set_theta(ScalarType theta) {
    theta_ = theta;
    return *this;
  }

  /**
   * Set the acceleration due to gravity.
   * Defaults to one.
   */
  ScalarType g() const { return g_; }
  PendulumConfigurator& set_g(ScalarType g) {
    if (g < 0) {
      g_ = -g;
    } else {
      g_ = g;
    }
    return *this;
  }

  MultivectorType calculate_position() const {
    using std::cos;
    using std::sin;

    return length_ * sin(theta_) * e0 - length_ * cos(theta_) * e1;
  }

  MultivectorType gravitational_acceleration() const { return -g_ * e1; }

  /**
   * Create the pendulum as configured. Note that the PendulumConfigurator object can be
   * reused, if desired, and its state is not changed after calling this method.
   */
  Pendulum<MultivectorType> create() const {
    return Pendulum<MultivectorType>{mass_, gravitational_acceleration(), initial_time_,
                                     calculate_position()};
  }
};

}  // namespace ndyn::simulation
