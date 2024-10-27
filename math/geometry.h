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

#include <algorithm>
#include <cmath>
#include <iostream>
#include <string>

#include "math/multivector.h"

namespace ndyn::math {

template <typename ScalarT>
class Transform;

/**
 * Primitives represent points, lines, and planes as well as the transforms of these.
 */
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
  explicit constexpr Primitive(const VectorType& v) : vec_(v) {}
  explicit constexpr Primitive(VectorType&& v) : vec_(std::move(v)) {}

 public:
  constexpr Primitive(const Primitive&) = default;
  constexpr Primitive(Primitive&&) = default;

  constexpr Primitive& operator=(const Primitive&) = default;
  constexpr Primitive& operator=(Primitive&&) = default;

  constexpr bool operator==(const Primitive& rhs) const { return vec_ == rhs.vec_; }
  constexpr bool operator!=(const Primitive& rhs) const { return vec_ != rhs.vec_; }

  constexpr bool is_point() const {
    constexpr std::array<size_t, 4> allowed_bases{7 /*e021*/, 11 /*e013*/, 13 /*e032*/,
                                                  14 /*e123*/};
    for (size_t i = 0; i < vec_.bases_count(); ++i) {
      const bool allowed{std::find(allowed_bases.begin(), allowed_bases.end(), i) !=
                         allowed_bases.end()};
      if (!allowed && vec_.basis(i) != 0) {
        return false;
      }
    }
    return true;
  }

  constexpr bool is_line() const {
    constexpr std::array<size_t, 6> allowed_bases{3 /*e01*/, 5 /*e02*/,  6 /*e12*/,
                                                  9 /*e03*/, 10 /*e13*/, 12 /*e23*/};
    for (size_t i = 0; i < vec_.bases_count(); ++i) {
      const bool allowed{std::find(allowed_bases.begin(), allowed_bases.end(), i) !=
                         allowed_bases.end()};
      if (!allowed && vec_.basis(i) != 0) {
        return false;
      }
    }
    return true;
  }

  constexpr bool is_plane() const {
    constexpr std::array<size_t, 4> allowed_bases{1 /*e0*/, 2 /*e1*/, 4 /*e2*/, 8 /*e3*/};
    for (size_t i = 0; i < vec_.bases_count(); ++i) {
      const bool allowed{std::find(allowed_bases.begin(), allowed_bases.end(), i) !=
                         allowed_bases.end()};
      if (!allowed && vec_.basis(i) != 0) {
        return false;
      }
    }
    return true;
  }

  constexpr const VectorType& raw_vector() const { return vec_; }

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
  static constexpr Primitive x_axis() { return join(point(1, 0, 0), origin()); }
  static constexpr Primitive y_axis() { return join(point(0, 1, 0), origin()); }
  static constexpr Primitive z_axis() { return join(point(0, 0, 1), origin()); }

  static constexpr Primitive xy_plane() { return join(x_axis, y_axis); }
  static constexpr Primitive yz_plane() { return join(y_axis, z_axis); }
  static constexpr Primitive zx_plane() { return join(z_axis, x_axis); }

  /**
   * Create a Primitive representing the intersection of two primitives. For two points, this
   * creates the line that joins them.
   */
  static constexpr Primitive join(const Primitive& p1, const Primitive& p2) {
    return Primitive{p1.vec_ & p2.vec_};
  }

  /**
   * Create a Primitive representing the span (the outer product null-space) of two primitives. For
   * two planes, this creates the line where the two planes meet.
   *
   * This is the grade-raising form of multiplication.
   */
  static constexpr Primitive meet(const Primitive& p1, const Primitive& p2) {
    return Primitive{p1.vec_ ^ p2.vec_};
  }
};

template <typename ScalarType>
std::string to_string(const Primitive<ScalarType>& p) {
  using std::to_string;
  return to_string(p.raw_vector());
}

template <typename ScalarType>
std::ostream& operator<<(std::ostream& os, const Primitive<ScalarType>& p) {
  using std::to_string;
  os << to_string(p);
  return os;
}

/**
 * Transforms represent versors (also called motors) that can reflect, rotate, and translate
 * primitives. Applying a transform involves sandwiching the primitive in between the versor and its
 * reverse.
 *
 * For more information, see https://en.wikipedia.org/wiki/Geometric_algebra#Versor
 */
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
   * transforms, operators, etc., where multiplication on the left is used to combine the
   * transformations:
   * (*this) * first.
   */
  constexpr Transform compose(const Transform& first) const { return Transform{vec_ * first.vec_}; }

  constexpr Transform operator*(const Transform& first) const { return compose(first); }

  /**
   * Standard sandwich product to apply a transform to a primitive.
   */
  constexpr PrimitiveType apply(const PrimitiveType& primitive) const {
    return PrimitiveType{vec_ * primitive.vec_ * ~vec_};
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
