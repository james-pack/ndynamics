#pragma once

#include <cmath>
#include <type_traits>

#include "math/integrators.h"
#include "math/state.h"

namespace ndyn::simulation {

/**
 * Generic representation of single particles or objects that evolve according to a differential
 * equation.
 *
 * Areas for future abstraction:
 * - Include the concept of an external potential. The problem is that an external potential's frame
 * of reference is different from the Body's. For now, this concept is implicit in the definition of
 * the compute_partials functions.
 * - We should include the ability to calculate a step size, possibly at every step.
 */
template <typename StateT, typename IntegratorT = math::RungeKutta4<StateT>>
class Body final {
 public:
  using StateType = StateT;
  using VectorType = typename StateType::ValueType;
  using ScalarType = typename StateType::ScalarType;
  using IntegratorType = IntegratorT;
  using ComputePartialsType = math::ComputePartials<StateType>;

  static constexpr size_t state_depth() { return StateType::depth(); }

  static constexpr ScalarType DEFAULT_STEP_SIZE{0.001};

 private:
  ScalarType t_{0};
  StateType state_;
  IntegratorType integrator_;

 public:
  constexpr Body(ScalarType initial_time, StateType initial_state,
                 const ComputePartialsType& compute_partials)
      : t_(initial_time), state_(initial_state), integrator_(compute_partials) {}

  constexpr Body(StateType initial_state, const ComputePartialsType& compute_partials)
      : state_(initial_state), integrator_(compute_partials) {}

  constexpr const StateType& state() const { return state_; }

  /**
   * Set the state of the system to the new_time using the given step_size. A default step_size
   * is provided; see code below for details. Note that it is possible to evolve to a time before
   * the current time.
   */
  void goto_time(ScalarType new_time, ScalarType step_size = 0) {
    using std::abs;

    if (step_size == 0) {
      step_size = DEFAULT_STEP_SIZE;

      VLOG(5) << "Default step size used. step_size: " << step_size;
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

  void evolve(ScalarType time_increment, ScalarType step_size = 0) {
    goto_time(t_ + time_increment, step_size);
  }
};

}  // namespace ndyn::simulation
