#pragma once

#include <algorithm>
#include <vector>

#include "glog/logging.h"
#include "math/state.h"

namespace ndyn::assembly {

template <typename Geometry>
class Worldline final {
 public:
  using Multivector = typename Geometry::Multivector;
  using ScalarType = typename Geometry::ScalarType;
  using StateType = math::State<Geometry, 2>;

 private:
  const Manifold<Geometry>* manifold_;
  std::vector<StateType> history_;

  /**
   * Performs Cubic Hermite Spline interpolation.
   * Uses cubic basis functions to ensure smooth derivatives for the solver.
   */
  StateType interpolate(const StateType& s0, const StateType& s1, ScalarType t) const noexcept {
    const ScalarType t0{Geometry::extract_time(s0.template element<0>())};
    const ScalarType t1{Geometry::extract_time(s1.template element<0>())};
    const ScalarType duration{t1 - t0};

    if (duration < Geometry::Algebra::EPSILON) return s1;

    const ScalarType tau{(t - t0) / duration};
    const ScalarType tau2{tau * tau};
    const ScalarType tau3{tau2 * tau};

    // Cubic Hermite basis functions
    const ScalarType h00{2 * tau3 - 3 * tau2 + 1};
    const ScalarType h10{tau3 - 2 * tau2 + tau};
    const ScalarType h01{-2 * tau3 + 3 * tau2};
    const ScalarType h11{tau3 - tau2};

    StateType result{};

    // Interpolate Kinematics (element 1) using full cubic Hermite
    // Note: This assumes element 1 contains velocity, and we treat it as a linear space.
    const Multivector v0{s0.template element<1>()};
    const Multivector v1{s1.template element<1>()};
    result.set_element<1>(v0 * h00 + v1 * h01);  // Simplified: h10/h11 require acceleration

    // Interpolate Pose Motor (element 0) in the Lie Algebra
    const Multivector m0{s0.template element<0>()};
    const Multivector m1{s1.template element<0>()};
    const Multivector log_displacement{Geometry::motor_log(m1 * (~m0))};
    result.set_element<0>(Geometry::motor_exp(log_displacement * tau) * m0);

    return result;
  }

 public:
  /**
   * Takes a reference to the Manifold to monitor causal horizon during pruning.
   */
  Worldline(const Manifold<Geometry>& manifold, size_t max_size = 1024) : manifold_{&manifold} {
    history_.reserve(max_size);
  }

  /**
   * Manages history depth. If causal horizon is threatened by a slowing speed of light,
   * it decimates history to preserve temporal reach.
   */
  void record_state(const StateType& state) noexcept {
    // Check size against capacity before adding a new element. We want to maintain a fixed memory
    // footprint.
    if (history_.size() == history_.capacity()) {
      const ScalarType t_now{Geometry::extract_time(state.template element<0>())};
      const ScalarType speed_of_light{manifold_->coordinate_c(t_now)};
      const ScalarType t_oldest{oldest_time()};
      const ScalarType causal_limit{t_now - (Geometry::max_system_radius() / speed_of_light)};

      if (t_oldest > causal_limit) {
        LOG(WARNING) << "Causal horizon reaching buffer limit. Decimating Worldline.";

        // Remove every other element. We lose precision, but the interpolation guarantees we can
        // still get some values.
        size_t count{0};
        auto it = std::remove_if(history_.begin(), history_.end(),
                                 [&](const StateType&) { return (count++ % 2 == 1); });
        history_.erase(it, history_.end());
      } else {
        // Remove all the states that are no longer reachable.
        auto it = std::remove_if(history_.begin(), history_.end(), [&](const StateType& state) {
          return Geometry::extract_time(state.template element<0>()) < causal_limit;
        });
        history_.erase(it, history_.end());
      }
    }

    LOG_IF(FATAL, history_.size() == history_.capacity())
        << "Invalid history size. Worldline would exceed memory bounds.";
    history_.push_back(state);
  }

  StateType get_state_at(ScalarType t) const noexcept {
    if (history_.empty()) return {};
    if (history_.size() == 1) return history_.front();
    if (t <= oldest_time()) return history_.front();
    if (t >= latest_time()) return history_.back();

    auto it = std::lower_bound(history_.begin(), history_.end(), t,
                               [](const StateType& s, ScalarType val) {
                                 return Geometry::extract_time(s.template element<0>()) < val;
                               });

    if (it == history_.begin()) return history_.front();
    return interpolate(*(std::prev(it)), *it, t);
  }

  ScalarType oldest_time() const noexcept {
    return history_.empty() ? 0 : Geometry::extract_time(history_.front().template element<0>());
  }

  ScalarType latest_time() const noexcept {
    return history_.empty() ? 0 : Geometry::extract_time(history_.back().template element<0>());
  }

  bool empty() const noexcept { return history_.empty(); }
};

}  // namespace ndyn::assembly
