/**
 * Geometric primitives and transformations using "plane-based" 3D PGA (Projective Geometric
 * Algebra) also known as Cl(3,0,1). In this form of the 3D PGA, planes are represented by grade 1
 * multivectors (that is, vectors) and points are represented by grade 3 multivectors (trivectors).
 * In this form, the intersect() method below represents the join of two primitives, and the span()
 * represents the meet.
 *
 * A primary alternative form exists where planes are trivectors and points are vectors. In this
 * alternative form, the span() method would be the join operation, and intersect() would be the
 * meet. Superficially, these two forms are functionally identical but with opposite geometric
 * interpretations.
 *
 * For more details on this setup, please see https://youtu.be/v-WG02ILMXA and
 * https://youtu.be/ichOiuBoBoQ. For the distinction between plane- and point-based PGA, this
 * article clarifies the different representation and operators:
 * https://discourse.bivector.net/t/join-and-meet-in-projective-geometric-algebra/660
 */

#pragma once

#include <cmath>

#include "math/multivector.h"

namespace ndyn::math {

template <typename ScalarT>
class Transform;

template <typename ScalarT>
class Primitive final {
 public:
  using ScalarType = ScalarT;

 private:
  // Our vector type is the 3D PGA.
  using VectorType = Multivector<ScalarType, 3, 0, 1>;

  static constexpr VectorType one_{VectorType{1}};
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
  // TODO(james): Explain why the canonical ordering of these basis vectors isn't sufficient;
  // describe what complexity they would introduce.
  // These bases are equivalent to Plucker coordinates.
  static constexpr VectorType e31_{e3_ * e1_};

  static constexpr VectorType e012_{e0_ * e1_ * e2_};
  static constexpr VectorType e013_{e0_ * e1_ * e3_};
  static constexpr VectorType e023_{e0_ * e2_ * e3_};
  static constexpr VectorType e123_{e1_ * e2_ * e3_};

  // Special basis combinations to represent points.
  // TODO(james): Explain why the canonical ordering of these basis vectors isn't sufficient;
  // describe what complexity they would introduce.
  // These bases are equivalent to homogenous coordinates for points.
  static constexpr VectorType e021_{e0_ * e2_ * e1_};
  static constexpr VectorType e032_{e0_ * e3_ * e2_};

  static constexpr VectorType e0123_{e0_ * e1_ * e2_ * e3_};

  VectorType vec_{};

  friend class Transform<ScalarType>;

  constexpr Primitive() = default;
  constexpr Primitive(const VectorType& v) : vec_(v) {}
  constexpr Primitive(VectorType&& v) : vec_(std::move(v)) {}

 public:
  constexpr Primitive(const Primitive&) = default;
  constexpr Primitive(Primitive&&) = default;

  constexpr Primitive& operator=(const Primitive&) = default;
  constexpr Primitive& operator=(Primitive&&) = default;

  constexpr bool operator==(const Primitive& rhs) { return vec_ == rhs.vec_; }
  constexpr bool operator!=(const Primitive& rhs) { return vec_ != rhs.vec_; }

  /**
   * Create a Primitive representing a point specified in Cartesian coordinates.
   */
  static constexpr Primitive point(const ScalarType& x, const ScalarType& y, const ScalarType& z) {
    return Primitive{x * e032_ + y * e013_ + z * e021_ + e123_};
  }

  /**
   * Create a Primitive representing the point at the origin.
   */
  static constexpr Primitive origin() { return Primitive{e123_}; }

  /**
   * Create Primitives for each of the 3 Cartesian axes.
   */
  static constexpr Primitive x_axis() { return intersect(point(1, 0, 0), origin()); }
  static constexpr Primitive y_axis() { return intersect(point(0, 1, 0), origin()); }
  static constexpr Primitive z_axis() { return intersect(point(0, 0, 1), origin()); }

  static constexpr Primitive xy_plane() { return intersect(x_axis, y_axis); }
  static constexpr Primitive yz_plane() { return intersect(y_axis, z_axis); }
  static constexpr Primitive zx_plane() { return intersect(z_axis, x_axis); }

  /**
   * Create a Primitive representing the intersection of two primitives. For two points, this
   * creates the line that joins them.
   */
  static constexpr Primitive intersect(const Primitive& p1, const Primitive& p2) {
    return p1.vec_.regress(p2.vec_);
  }

  /**
   * Create a Primitive representing the span (the outer product null-space) of two primitives. For
   * two planes, this creates the line where the two planes meet.
   *
   * This is the grade-raising form of multiplication.
   */
  static constexpr Primitive span(const Primitive& p1, const Primitive& p2) {
    return Primitive{p1.vec_.outer(p2.vec_)};
  }
};

template <typename ScalarT>
class Transform final {
 public:
  using ScalarType = ScalarT;
  using PrimitiveType = Primitive<ScalarType>;

 private:
  using VectorType = typename PrimitiveType::VectorType;

  // This creates the identity transformation by default.
  VectorType vec_{1.f};

  constexpr Transform() = default;
  constexpr Transform(const VectorType& v) : vec_(v) {}
  constexpr Transform(VectorType&& v) : vec_(std::move(v)) {}

 public:
  constexpr Transform(const Transform& rhs) = default;
  constexpr Transform(Transform&& rhs) = default;

  constexpr Transform& operator=(const Transform& rhs) = default;
  constexpr Transform& operator=(Transform&& rhs) = default;

  constexpr bool operator==(const Transform& rhs) const { return vec_ == rhs.vec_; }
  constexpr bool operator!=(const Transform& rhs) const { return vec_ != rhs.vec_; }

  /**
   * Compose a combination of this transform with another transform. The other transform is applied
   * first. This ordering is consistent with standard mathematical semantics for matrices,
   * transforms, operators, etc., where multiplication is used to combine the transformations:
   * (*this) * first.
   */
  constexpr Transform compose(const Transform& first) const { return Transform{vec_ * first.vec_}; }

  constexpr Transform operator*(const Transform& first) const { return compose(first); }

  constexpr PrimitiveType apply(const PrimitiveType& primitive) const {
    return PrimitiveType{vec_ * primitive.vec_};
  }

  constexpr PrimitiveType operator*(const PrimitiveType& primitive) const {
    return apply(primitive);
  }

  constexpr Transform reverse() const { return Transform{~vec_}; }

  constexpr Transform operator~() const { return reverse(); }

  /**
   * Create a Transform that does nothing.
   */
  static constexpr Transform identity() {
    // The default constructed Transform is the identity.
    return Transform{};
  }

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
