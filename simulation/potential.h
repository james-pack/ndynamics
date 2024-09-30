#pragma once

#include "math/state.h"

namespace ndyn::simulation {

template <typename VectorT, typename UnitSetT>
class Potential {
 public:
  using VectorType = VectorT;
  using ScalarType = typename math::ScalarTypes<VectorType>::ScalarType;
  using Units = UnitSetT;

  Potential() = default;
  virtual ~Potential() = default;

  virtual VectorT at(const VectorT& position) const = 0;

  virtual void goto_time(ScalarType new_time, ScalarType step_size = 0) = 0;

  virtual void evolve(ScalarType time_increment, ScalarType step_size = 0) = 0;
};

template <typename VectorT, typename UnitSetT>
class UniformPotential final : public Potential<VectorT, UnitSetT> {
 private:
  VectorT value_{};

 public:
  using VectorType = VectorT;
  using ScalarType = typename math::ScalarTypes<VectorType>::ScalarType;
  using Units = UnitSetT;

  // Uniform zero potential.
  UniformPotential() = default;

  // Uniform potential of the given value.
  UniformPotential(const VectorT& value) : value_(value) {}

  VectorT at(const VectorT& position) const override { return value_; }

  void goto_time(ScalarType new_time, ScalarType step_size = 0) override {
    // Do nothing. A UniformPotential does not change over time.
  }

  void evolve(ScalarType time_increment, ScalarType step_size = 0) override {
    // Do nothing. A UniformPotential does not change over time.
  }
};

}  // namespace ndyn::simulation
