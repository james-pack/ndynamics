#pragma once

#include <functional>
#include <vector>

#include "assembly/worldline.h"
#include "math/abs.h"
#include "math/state.h"

namespace ndyn::assembly {

template <typename Geometry>
class Manifold final {
 public:
  using Multivector = typename Geometry::Multivector;
  using ScalarType = typename Geometry::ScalarType;
  using WorldlineType = Worldline<Geometry>;
  using LightSpeedFunc = std::function<ScalarType(ScalarType)>;

  static constexpr ScalarType TOLERANCE{Geometry::Algebra::EPSILON};

 private:
  LightSpeedFunc c_func_;

  /**
   * Finds the retarded time (emission time) using Brent's Method.
   *
   * This solver finds the root of the light-cone intersection equation:
   * f(t) = spatial_distance(source(t), target(t_now)) - speed_of_light * (t_now - t)
   */
  // TODO(james): Fix this solver. The speed of light is currently fixed in these calculations,
  // rather than using c_func_ to recompute at each iteration. Likely, the entire method is
  // structurally incorrect and needs to be rethought.
  ScalarType solve_retardation(const WorldlineType& source_worldline,
                               const Multivector& target_pose, ScalarType t_now) const {
    using math::abs;
    const ScalarType speed_of_light{c_func_(t_now)};
    const Multivector target_position{target_pose * Geometry::origin() * ~target_pose};

    // The objective function: we are looking for f(t_retarded) == 0
    auto light_travel_error = [&](ScalarType t_test) {
      const auto state{source_worldline.get_state_at(t_test)};
      const Multivector source_pose{state.template element<0>()};
      const Multivector source_position{source_pose * Geometry::origin() * ~source_pose};

      const ScalarType spatial_distance{Geometry::distance(source_position, target_position)};
      const ScalarType time_delay{t_now - t_test};

      return spatial_distance - (speed_of_light * time_delay);
    };

    // Bracket the search between the oldest available history and the current time.
    ScalarType lower_bound{source_worldline.oldest_time()};
    ScalarType upper_bound{t_now};

    ScalarType f_lower{light_travel_error(lower_bound)};
    ScalarType f_upper{light_travel_error(upper_bound)};

    // If the signs are the same, the signal hasn't reached the target yet.
    if (f_lower * f_upper > 0) {
      return lower_bound;
    }

    // Brent's Method variables
    ScalarType best_estimate{upper_bound};
    ScalarType prev_best{lower_bound};
    ScalarType contrivance_point{lower_bound};  // 'c' in standard Brent notation

    ScalarType f_best{f_upper};
    ScalarType f_prev{f_lower};
    ScalarType f_contrivance{f_lower};

    ScalarType interpolation_step{0};
    ScalarType last_step_size{0};
    bool use_bisection{true};

    for (int iteration{0}; iteration < 64; ++iteration) {
      // Check convergence
      if (abs(f_best) < TOLERANCE || abs(best_estimate - prev_best) < TOLERANCE) {
        return best_estimate;
      }

      // Inverse Quadratic Interpolation or Secant Method
      if (f_best != f_prev && f_best != f_contrivance && f_prev != f_contrivance) {
        // Inverse Quadratic
        interpolation_step =
            (prev_best * f_best * f_contrivance) / ((f_prev - f_best) * (f_prev - f_contrivance)) +
            (best_estimate * f_prev * f_contrivance) /
                ((f_best - f_prev) * (f_best - f_contrivance)) +
            (contrivance_point * f_prev * f_best) /
                ((f_contrivance - f_prev) * (f_contrivance - f_best));
      } else {
        // Secant
        interpolation_step =
            best_estimate - f_best * (best_estimate - prev_best) / (f_best - f_prev);
      }

      // Evaluate if the proposed step is acceptable
      const ScalarType delta_est_prev{abs(best_estimate - prev_best)};
      const ScalarType delta_prev_contrivance{abs(prev_best - contrivance_point)};

      const bool out_of_bounds = (interpolation_step < (3 * prev_best + best_estimate) / 4 ||
                                  interpolation_step > best_estimate);
      const bool too_slow_iq =
          (use_bisection && abs(interpolation_step - best_estimate) >= delta_est_prev / 2);
      const bool too_slow_secant =
          (!use_bisection && abs(interpolation_step - best_estimate) >= delta_prev_contrivance / 2);

      if (out_of_bounds || too_slow_iq || too_slow_secant) {
        interpolation_step = (prev_best + best_estimate) / 2;
        use_bisection = true;
      } else {
        use_bisection = false;
      }

      const ScalarType f_new{light_travel_error(interpolation_step)};

      // Update the bracket
      contrivance_point = prev_best;
      f_contrivance = f_prev;
      last_step_size = abs(best_estimate - prev_best);

      if (f_prev * f_new < 0) {
        best_estimate = interpolation_step;
        f_best = f_new;
      } else {
        prev_best = interpolation_step;
        f_prev = f_new;
      }

      if (abs(f_prev) < abs(f_best)) {
        std::swap(prev_best, best_estimate);
        std::swap(f_prev, f_best);
      }
    }

    return best_estimate;
  }

 public:
  explicit Manifold(LightSpeedFunc c_func) : c_func_{std::move(c_func)} {}

  /**
   * Resolves retardation and provides the transition motor M.
   * M captures the relative pose shift and the parallel transport
   * across the geometry defined by the current speed of light.
   */
  Multivector transport(const WorldlineType& source_wl, const Multivector& target_pose) const {
    const ScalarType t_now{Geometry::extract_time(target_pose)};
    const ScalarType t_ret{solve_retardation(source_wl, target_pose, t_now)};

    const auto retarded_state{source_wl.get_state_at(t_ret)};
    const Multivector source_pose{retarded_state.template element<0>()};

    // The motor M moves the source frame to the target frame.
    return target_pose * (~source_pose);
  }

  /**
   * Exposed the speed of light for testing as well as logging and visualization.
   */
  ScalarType calculate_speed_of_light(ScalarType t) const { return c_func_(t); }
};

}  // namespace ndyn::assembly
