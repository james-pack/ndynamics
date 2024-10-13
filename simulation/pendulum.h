#pragma once

#include <cmath>

#include "base/pi.h"
#include "glog/logging.h"
#include "math/convert.h"
#include "math/integrators.h"
#include "math/multivector.h"
#include "math/multivector_utils.h"
#include "math/state.h"
#include "math/unit_set.h"
#include "units.h"

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
  using std::abs;
  using std::cos;
  using std::fmod;
  using std::log;
  using std::pow;
  using std::sin;
  using std::sqrt;

  // Force the angle to be on the interval [0, 2*pi).
  angle = fmod(abs(angle), 2 * pi);

  const ScalarType T0{static_cast<ScalarType>(2 * pi * sqrt(abs(length / g)))};
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
template <typename StateT>
class Pendulum final {
 public:
  using StateType = StateT;
  using MultivectorType = typename StateType::VectorType;
  using ScalarType = typename StateType::ScalarType;

 private:
  // Precomputed magnitude of the position vector. The length of the pendulum.
  const ScalarType length_;

  // Precomputed magnitude of the gravitational acceleration.
  const ScalarType g_;

  // Initial position of the pendulum. We restrict the initial velocity to zero, so this vector also
  // gives the maximum angle for the pendulum.
  const MultivectorType initial_angular_position_;

  // Acceleration due to gravity as a vector.
  const MultivectorType gravitational_acceleration_;

  // Precomputed period of the pendulum.
  const ScalarType period_;

  const ScalarType initial_time_;

  ScalarType t_;
  StateType angular_state_;
  mutable StateType cartesian_state_{};
  mutable bool cartesian_state_dirty_{true};

  constexpr MultivectorType compute_angular_acceleration(const StateType& state) const {
    using std::sin;
    const MultivectorType& angular_position{state.template element<0>()};
    return -g_ / angular_position.r() * sin(angular_position.theta()) *
           MultivectorType::template e<1>();
  }

  math::RungeKutta4<StateType> integrator_{[this](const StateType& state) -> StateType {
    using std::sin;
    StateType partials{state.shift()};
    partials.template set_element<1>(compute_angular_acceleration(state));
    return partials;
  }};

  const ScalarType mass_;

  // Initial energy of the system. Sum of potential and kinetic energy at construction.
  const ScalarType initial_energy_;

 public:
  constexpr Pendulum(ScalarType mass, MultivectorType gravitational_acceleration, ScalarType t,
                     MultivectorType angular_position)
      : length_(angular_position.r()),
        g_(abs(gravitational_acceleration)),
        initial_angular_position_(angular_position),
        gravitational_acceleration_(gravitational_acceleration),
        period_(compute_period(length_, g_, angular_position.theta())),
        initial_time_(t),
        t_(t),
        angular_state_({initial_angular_position_}),
        mass_(mass),
        initial_energy_(compute_potential_energy() + compute_kinetic_energy()) {}

  ScalarType mass() const { return mass_; }

  // Magnitude of the acceleration due to gravity.
  constexpr ScalarType g() const { return g_; }

  constexpr ScalarType period() const { return period_; }

  // Rectilinear state. These are transformations of the actual state.
  constexpr const StateType& state() const {
    if (cartesian_state_dirty_) {
      StateType angular_state_external{angular_state_};
      {
        MultivectorType angular_position{angular_state_external.template element<0>()};
        angular_position.set_theta(pi_v<ScalarType> - angular_position.theta());
        angular_state_external.template set_element<0>(angular_position);
      }
      {
        MultivectorType angular_velocity{angular_state_external.template element<1>()};
        angular_velocity.set_theta(-angular_velocity.theta());
        angular_state_external.template set_element<1>(angular_velocity);
      }
      {
        MultivectorType angular_acceleration{angular_state_external.template element<2>()};
        angular_acceleration.set_theta(-angular_acceleration.theta());
        angular_state_external.template set_element<2>(angular_acceleration);
      }
      cartesian_state_ =
          math::convert_spherical_to_cartesian<StateType, math::State<MultivectorType, 3>>(
              angular_state_external);
      cartesian_state_dirty_ = false;
    }
    return cartesian_state_;
  }

  constexpr MultivectorType position() const { return state().template element<0>(); }
  constexpr MultivectorType velocity() const { return state().template element<1>(); }
  constexpr MultivectorType acceleration() const { return state().template element<2>(); }

  constexpr const MultivectorType& graviational_acceleration() const {
    return gravitational_acceleration_;
  }

  constexpr ScalarType height() const { return length() + position().z(); }

  constexpr ScalarType compute_potential_energy() const { return mass_ * g_ * height(); }

  constexpr ScalarType compute_kinetic_energy() const {
    return mass_ / 2 * square_magnitude(velocity());
  }

  constexpr ScalarType length() const { return angular_state_.template element<0>().r(); }
  constexpr ScalarType theta() const { return angular_state_.template element<0>().theta(); }
  constexpr ScalarType phi() const { return angular_state_.template element<0>().phi(); }

  constexpr ScalarType current_time() const { return t_; }

  /**
   * Set the state of the system to the new_time using the given step_size. A default step_size
   * is provided; see code below for details. Note that it is possible to evolve to a time before
   * the current time.
   */
  void goto_time(ScalarType new_time, ScalarType step_size = 0) {
    using std::abs;
    using std::remainder;
    using std::round;

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

        // if (abs(remainder(t_, period_)) < abs(step_size)) {
        //   // If we are going to a time that is a multiple of the period, just set the state to
        //   the
        //   // initial position. We do this to zero out any accumulated errors in the integration.
        //   angular_state_ = {initial_angular_position_};
        // } else {
        angular_state_ = integrator_(step_size, angular_state_);
        // }

        VLOG(4) << "t_: " << t_ << ", theta(): " << theta();
      } while (abs(t_ - new_time) > abs(step_size));
    }

    // Update the acceleration component of the state, if it is expected to be tracked.
    if constexpr (StateType::depth() >= 3) {
      angular_state_.template set_element<2>(compute_angular_acceleration(angular_state_));
    }
    cartesian_state_dirty_ = true;
  }

  void evolve(ScalarType time_increment, ScalarType step_size = 0) {
    goto_time(t_ + time_increment, step_size);
  }
};

template <typename VectorT, typename Unit = units::length::meter_t>
class PendulumConfigurator final {
 public:
  using VectorType = VectorT;
  using ScalarType = typename VectorType::ScalarType;
  using Units = math::UnitSet<Unit, units::angle::radian_t>;
  using StateType = math::State<VectorType, 3>;

  // Type of the resulting pendulum.
  using PendulumType = Pendulum<StateType>;

 private:
  ScalarType mass_{1};
  ScalarType length_{1};
  ScalarType initial_time_{0};
  ScalarType theta_{};
  ScalarType g_{1};

  static constexpr auto e0 = VectorType::template e<0>();
  static constexpr auto e1 = VectorType::template e<1>();

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

  VectorType calculate_position() const { return length_ * e0 + theta_ * e1; }

  VectorType gravitational_acceleration() const { return -g_ * e1; }

  /**
   * Create the pendulum as configured. Note that the PendulumConfigurator object can be
   * reused, if desired, and its state is not changed after calling this method.
   */
  PendulumType create() const {
    return PendulumType{mass_, gravitational_acceleration(), initial_time_, calculate_position()};
  }
};

}  // namespace ndyn::simulation
