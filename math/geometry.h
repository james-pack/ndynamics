/**
 * Geometric primitives and transformations using 3D PGA (Projective Geometric Algebra) also known
 * as Cl(3,0,1).
 *
 * For more details on this setup, please see https://youtu.be/v-WG02ILMXA and
 * https://youtu.be/ichOiuBoBoQ.
 */
#pragma once

#include <cmath>

#include "math/multivector.h"

namespace ndyn::math {

template <typename ScalarT>
class Primitive final {
 public:
  using ScalarType = ScalarT;

  // Our vector type is the 3D PGA.
  using VectorType = Multivector<ScalarType, 3, 0, 1>;

 private:
  static constexpr VectorType e0_{VectorType::template e<0>()};
  static constexpr VectorType e1_{VectorType::template e<1>()};
  static constexpr VectorType e2_{VectorType::template e<2>()};
  static constexpr VectorType e3_{VectorType::template e<3>()};

  static constexpr VectorType e01_{e0_ * e1_};
  static constexpr VectorType e02_{e0_ * e2_};
  static constexpr VectorType e03_{e0_ * e3_};
  static constexpr VectorType e12_{e1_ * e2_};
  static constexpr VectorType e13_{e1_ * e3_};
  static constexpr VectorType e23_{e2_ * e3_};

  // Special basis combinations to represent lines.
  // TODO(james): Explain why the canonical ordering of these basis vectors aren't sufficient;
  // describe what complexity they would introduce.
  // These bases are equivalent to Plucker coordinates.
  static constexpr VectorType e31_{e3_ * e1_};

  static constexpr VectorType e012_{e0_ * e1_ * e2_};
  static constexpr VectorType e013_{e0_ * e1_ * e3_};
  static constexpr VectorType e023_{e0_ * e2_ * e3_};
  static constexpr VectorType e123_{e1_ * e2_ * e3_};

  // Special basis combinations to represent points.
  // TODO(james): Explain why the canonical ordering of these basis vectors aren't sufficient;
  // describe what complexity they would introduce.
  // These bases are equivalent to homogenous coordinates for points.
  static constexpr VectorType e021_{e0_ * e2_ * e1_};
  static constexpr VectorType e032_{e0_ * e3_ * e2_};

  static constexpr VectorType e0123_{e0_ * e1_ * e2_ * e3_};

  VectorType p_{};

  Primitive(const VectorType& p) : p_(p) {}
  Primitive(VectorType&& p) : p_(std::move(p)) {}

 public:
  Primitive() = default;
  Primitive(const Primitive&) = default;
  Primitive(Primitive&&) = default;

  Primitive& operator=(const Primitive&) = default;
  Primitive& operator=(Primitive&&) = default;

  bool operator==(const Primitive& rhs) { return p_ == rhs.p_; }
  bool operator!=(const Primitive& rhs) { return p_ != rhs.p_; }

  /**
   * Create a Primitive representing a point specified in Cartesian coordinates.
   */
  static constexpr Primitive point(const ScalarType& x, const ScalarType& y, const ScalarType& z) {
    return Primitive{x * e032_ + y * e013_ + z * e021_ + e123_};
  }

  /**
   * Create a Primitive representing a point specified in cylindrical coordinates.
   */
  static constexpr Primitive cylindrical_point(const ScalarType& r, const ScalarType& theta,
                                               const ScalarType& z) {
    using std::cos;
    using std::sin;
    return Primitive{r * cos(theta) * e032_ + r * sin(theta) * e013_ + z * e021_ + e123_};
  }

  /**
   * Create a Primitive representing a point specified in spherical coordinates.
   */
  static constexpr Primitive spherical_point(const ScalarType& r, const ScalarType& theta,
                                             const ScalarType& phi) {
    using std::cos;
    using std::sin;
    return Primitive{r * sin(theta) * cos(phi) * e032_ + r * sin(theta) * sin(phi) * e013_ +
                     r * cos(theta) * e021_ + e123_};
  }

  /**
   * Create a Primitive representing a line specified by two points.
   */
  static constexpr Primitive join_line(const Primitive& point1, const Primitive& point2) {
    return point1.inner(point2);
  }

  static constexpr Primitive meet_line(const Primitive& plane1, const Primitive& plane2);

  static constexpr Primitive plane(const Primitive& point1, const Primitive& point2,
                                   const Primitive& point3);
};

template <typename ScalarT>
class Transform final {
 public:
  using ScalarType = ScalarT;
  using PrimitiveType = Primitive<ScalarType>;
  using VectorType = typename PrimitiveType::VectorType;

 private:
 public:
  /**
   * Compose a combination of this transform with another transform. The other transform is applied
   * first. This ordering is consistent with standard mathematical semantics for matrices,
   * transforms, operators, etc., where multiplication is used to combine the transformations:
   * (*this) * first.
   */
  constexpr Transform compose(const Transform& first) const {
    Transform result{};
    return result;
  }

  constexpr Transform operator*(const Transform& first) const { return combine(first); }

  constexpr PrimitiveType apply(const PrimitiveType& primitive) const {
    PrimitiveType result{};
    return result;
  }

  constexpr PrimitiveType operator*(const PrimitiveType& primitive) const {
    return apply(primitive);
  }

  /**
   * Create a Transform that does nothing.
   */
  static constexpr Transform identity() { return Transform{}; }

  /**
   * Create a Transform that is a reflection across the Primitive p.
   */
  static constexpr Transform reflect(const PrimitiveType& p);

  /**
   * Create a Transform that is a rotation about the axis by a certain angle. Note that the axis
   * does not need to be a line but may be any Primitive.
   */
  static constexpr Transform rotate(const PrimitiveType& axis, const ScalarType& angle);

  /**
   * Create a Transform that is a translation in the direction by a certain distance.
   */
  static constexpr Transform translate(const PrimitiveType& direction, const ScalarType& distance);
};

}  // namespace ndyn::math
