#pragma once

namespace ndyn::assembly {

template <typename Geometry>
class Field {
 public:
  using Multivector = typename Geometry::Multivector;

  virtual ~Field() = default;

  /**
   * Given a spacetime event (X) and a coupling Multivector (Q),
   * returns the acceleration bivector in the local frame.
   * Q can contain Scalar (Mass/Charge), Bivector (Spin/Moment), etc.
   */
  virtual Multivector evaluate_at(const Multivector& event,
                                  const Multivector& coupling) const noexcept = 0;
};

}  // namespace ndyn::assembly
