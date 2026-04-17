#pragma once

#include <algorithm>
#include <vector>

#include "assembly/field.h"
#include "assembly/manifold.h"
#include "assembly/particle.h"
#include "math/geometry_model.h"
#include "math/state.h"

namespace ndyn::assembly {

/**
 * Manages the algebraic connection between a Source (Parent) and a Receiver (Leaf).
 *
 * This class resolves the transformation between two coordinate frames separated by both
 * a hierarchical tree (spatial assembly) and a relativistic manifold (causal delay).
 *
 * Why Motors?
 * In Geometric Algebra (GA), specifically the Displacement Algebra (Cl(5,1) or similar),
 * "Motors" are used to represent the pose. Unlike matrices, Motors are unified operators
 * that handle both rotation and translation simultaneously via the sandwich product
 * (M * x * ~M). They are computationally efficient, avoid gimbal lock, and naturally
 * represent the "Exponential Map" of the Lie Algebra of the Euclidean/Poincaré group.
 * Reference: https://en.wikipedia.org/wiki/Motor_(algebra)
 *
 * Frame Composition
 * Frame changes are performed via operator composition. If M1 maps A->B and M2 maps B->C,
 * then M3 = M2 * M1 maps A->C. Because the reversion (~M) is the inverse for these
 * operators, the sandwich product provides a consistent way to reframe any multivector
 * (positions, velocities, or fields) across the hierarchy.
 */
template <math::ConformalGeometryModel Geometry>
class Connection final {
 public:
  using Multivector = typename Geometry::Multivector;
  using ScalarType = typename Geometry::ScalarType;
  using StateType = math::State<Geometry, 2>;

 private:
  const Manifold<Geometry>* manifold_;
  const Particle<Geometry>* leaf_;
  // parents_[0] is the leaf's immediate parent; parents_.back() is the root.
  std::vector<const Particle<Geometry>*> parents_;

 public:
  /**
   * Variadic constructor for a hierarchical chain.
   *
   * hierarchy_pack is the chain of particles from the Root down to the Parent.
   *
   * The pack is stored in reverse order (Leaf-Parent to Root) to allow efficient
   * left-multiplication when computing the world pose.
   */
  template <typename... Args>
  Connection(const Manifold<Geometry>& manifold, const Particle<Geometry>& leaf,
             const Args&... hierarchy_pack)
      : manifold_{&manifold}, leaf_{&leaf} {
    parents_.reserve(sizeof...(Args));
    const Particle<Geometry>* temporary_pack[] = {(&hierarchy_pack)...};
    for (int i = sizeof...(Args) - 1; i >= 0; --i) {
      parents_.push_back(temporary_pack[i]);
    }
  }

  Connection(const Particle<Geometry>& distal_parent, const Connection& initial)
      : manifold_{initial.manifold_}, leaf_{initial.leaf_}, parents_{initial.parents_} {
    parents_.push_back(&distal_parent);
  }

  /**
   * Resolves the cumulative world-frame transformation of the parent particle.
   *
   * We initialize the world_pose to 1 (the multiplicative identity motor), representing
   * the root of the manifold's global coordinate system. By iterating through the
   * hierarchy from the leaf's parent up to the root and left-multiplying each local
   * motor, we recursively "wrap" the geometry in its parents' reference frames.
   * This builds the unique transition map that lifts a multivector from the parent's
   * local fiber into the base manifold's global frame.
   */
  Multivector compute_parent_world_pose() const noexcept {
    Multivector world_pose{static_cast<ScalarType>(1)};
    for (const auto* parent : parents_) {
      world_pose = parent->current_state().template element<0>() * world_pose;
    }
    return world_pose;
  }

  /**
   * Computes the total operator M that maps the Parent's local frame to the Leaf's.
   *
   * This is the "core" of the Fiber Bundle logic. It reconciles three distinct
   * transformations into a single operator:
   *
   * 1. **Parent World Pose**: Maps the Parent's local geometry into the Global Manifold.
   * 2. **Parallel Transport**: Bridges the gap through the curved/retarded manifold.
   * In a curved space (like mapping Cartesian to Spherical), a vector doesn't
   * stay "pointed the same way" as it moves; Parallel Transport is the rule that
   * defines how to keep it "straight" relative to the manifold's geometry.
   * Reference: https://en.wikipedia.org/wiki/Parallel_transport
   * 3. **Leaf World Pose (Inverted)**: Maps the Global Manifold back into the
   * Leaf's local coordinate system.
   *
   * Returns the transition motor M = (~leaf_world) * M_transport * parent_world
   */
  Multivector compute_total_motor() const noexcept {
    const Multivector leaf_world_pose{leaf_->current_state().template element<0>()};
    const Multivector parent_world_pose{compute_parent_world_pose()};

    // transport() handles the light-cone intersection (retardation) and the
    // manifold's geometric curvature between the two world-events.
    const Multivector M_transport{
        manifold_->transport(parents_.front()->worldline(), leaf_world_pose)};

    // The composition maps: Parent_Local -> World -> (Transport) -> World -> Leaf_Local
    return (~leaf_world_pose) * M_transport * parent_world_pose;
  }

  /**
   * @brief Transforms an arbitrary Multivector from Parent frame to Leaf frame.
   */
  Multivector transform(const Multivector& pose) const noexcept {
    const Multivector M{compute_total_motor()};
    return M * pose * (~M);
  }

  /**
   * @brief Evaluates the force interaction across the Connection.
   * * The Field is evaluated at the Leaf's position as seen by the Parent.
   * The resulting acceleration (a Bivector) is then transported back to
   * the Leaf's local frame for integration.
   * Reference: https://en.wikipedia.org/wiki/Bivector
   */
  Multivector calculate_influence(const Field<Geometry>& field) const noexcept {
    const Multivector coupling_in_leaf{leaf_->coupling(field)};

    if (Geometry::magnitude_squared(coupling_in_leaf) < Geometry::Algebra::EPSILON) {
      return {};
    }

    const Multivector M{compute_total_motor()};
    const Multivector M_rev{~M};

    const Multivector leaf_local_pose{leaf_->current_state().template element<0>()};

    // Map the Leaf's pose and coupling into the Parent's frame for evaluation.
    const Multivector leaf_in_parent_frame{M_rev * leaf_local_pose * M};
    const Multivector coupling_in_parent_frame{M_rev * coupling_in_leaf * M};

    const Multivector parent_accel{
        field.evaluate_at(leaf_in_parent_frame, coupling_in_parent_frame)};

    // Bring the force/acceleration back to the Leaf's frame.
    return M * parent_accel * M_rev;
  }
};

}  // namespace ndyn::assembly
