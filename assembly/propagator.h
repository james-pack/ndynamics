#pragma once

#include <vector>

#include "assembly/manifold.h"
#include "assembly/worldline.h"

namespace ndyn::assembly {

template <typename Geometry>
class Propagator {
 public:
  using Multivector = typename Geometry::Multivector;

  // Finds the event in the source's worldline that intersects
  // the past light-cone of the target_event.
  Multivector find_retarded_event(const Worldline<Geometry>& source_wl,
                                  const Multivector& target_event,
                                  const Manifold<Geometry>& manifold) const {
    double t_now = Geometry::extract_time(target_event);
    double c = manifold.coordinate_c(t_now);

    // Solve: dist(target_event, source_state(t_ret)) = c * (t_now - t_ret)
    // For now, we assume a simple root-finding or iterative approximation
    double t_ret = t_now;  // Simplified: would be the result of the solver

    auto state_at_ret = source_wl.get_state_at(t_ret);
    // Map the local origin to the world frame via the pose motor (element 0)
    return state_at_ret.template element<0>() * Geometry::origin() *
           ~state_at_ret.template element<0>();
  }
};

}  // namespace ndyn::assembly
