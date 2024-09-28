#pragma once

#include "math/integrators.h"
#include "math/state.h"

namespace ndyn::simulation {

enum class Coordinates : uint8_t {
  CARTESIAN,
  POLAR,  // Includes cylindrical, if in 3+ dimensions.
  SPHERICAL,
};

/**
 * Representation of single particles or objects that evolve according to a differential equation.
 *
 * Areas for future abstraction:
 * - Include the concept of an external potential. The problem is that an external potential's frame
 * of reference is different from the Body's. For now, this concept is implicit in the definition of
 * the compute_partials functions.
 * - We should include the ability to calculate a step size, possibly at every step.
 * - For now, we are using the same integrator (4th order Runge-Kutta) for all Bodies. That makes
 * sense until we need to handle more rigid or chaotic systems.
 */
template <Coordinates COORDINATES, typename VectorT, size_t STATE_DEPTH,
          typename IntegratorT =
              math::RungeKutta4<typename VectorT::ScalarType, VectorT, STATE_DEPTH>>
class Body final {
 public:
  using VectorType = VectorT;
  using ScalarType = typename VectorType::ScalarType;
  using StateType = math::StateT<VectorType, STATE_DEPTH>;
  using IntegratorType = IntegratorT;
  using ComputePartialsType = math::ComputePartials<VectorType, STATE_DEPTH>;

  static constexpr size_t state_depth() { return STATE_DEPTH; }
  static constexpr Coordinates coordinates() { return COORDINATES; }

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

  constexpr const VectorType& position() const { return state_.template element<0>(); }
  constexpr const VectorType& velocity() const { return state_.template element<1>(); }
  constexpr const VectorType& acceleration() const { return state_.template element<2>(); }

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