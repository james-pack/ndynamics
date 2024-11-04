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
#include "math/multivector_utils.h"

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
  // We are using the 3D PGA.
  using AlgebraType = Algebra<ScalarType, 3, 0, 1>;

 private:
  using VectorType = typename AlgebraType::VectorType;

  static constexpr VectorType one_{VectorType{1}};
  // Note that these definitions are different from those in the Multivector. The Multivector puts
  // the zero bases last, rather than first. In some algebras, like the dual numbers, that is more
  // expected. But, in PGA, we usually have e0 as the basis with the zero quadratic form.
  static constexpr VectorType e0_{VectorType::template e<3>()};
  static constexpr VectorType e1_{VectorType::template e<0>()};
  static constexpr VectorType e2_{VectorType::template e<1>()};
  static constexpr VectorType e3_{VectorType::template e<2>()};

  static constexpr VectorType e01_{e0_ * e1_};
  static constexpr VectorType e02_{e0_ * e2_};
  static constexpr VectorType e03_{e0_ * e3_};
  static constexpr VectorType e12_{e1_ * e2_};
  static constexpr VectorType e13_{e1_ * e3_};
  static constexpr VectorType e23_{e2_ * e3_};

  // Special basis combination to help represent lines.
  static constexpr VectorType e31_{e3_ * e1_};

  static constexpr VectorType e012_{e0_ * e1_ * e2_};
  static constexpr VectorType e013_{e0_ * e1_ * e3_};
  static constexpr VectorType e023_{e0_ * e2_ * e3_};
  static constexpr VectorType e123_{e1_ * e2_ * e3_};

  // Special basis combinations to help represent points.
  static constexpr VectorType e021_{e0_ * e2_ * e1_};
  static constexpr VectorType e031_{e0_ * e3_ * e1_};
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

  constexpr Primitive normalized() const {
    using std::abs;
    return Primitive{vec_ / abs(vec_)};
  }

  // Arithmetic with scalars.
  constexpr Primitive operator+(const ScalarType& a) const { return Primitive{vec_ + a}; }
  constexpr Primitive operator-(const ScalarType& a) const { return Primitive{vec_ - a}; }
  constexpr Primitive operator*(const ScalarType& a) const { return Primitive{a * vec_}; }

  // Arithmetic with other primitives.
  constexpr Primitive operator+(const Primitive& rhs) const { return Primitive{vec_ + rhs.vec_}; }
  constexpr Primitive operator-(const Primitive& rhs) const { return Primitive{vec_ - rhs.vec_}; }
  constexpr Primitive operator*(const Primitive& rhs) const { return Primitive{vec_ * rhs.vec_}; }

  // Unary minus.
  constexpr Primitive operator-() const { return Primitive{-vec_}; }

  /**
   * Create a Primitive representing a point specified in Cartesian coordinates.
   *
   * This combination of basis vectors can be derived by implementing this method as:
   *    return Primitive{x * e1_ + y * e2_ + z * e3_ + e0_}.dual();
   * The implementation below uses a slightly more optimized form of this expression.
   *
   * For more details, see
   * https://geometry.mrao.cam.ac.uk/2020/06/euclidean-geometry-and-geometric-algebra/
   * or
   * https://youtu.be/v-WG02ILMXA?t=636
   */
  static constexpr Primitive point(const ScalarType& x, const ScalarType& y, const ScalarType& z) {
    return Primitive{x * e032_ + y * e013_ + z * e021_ + e123_};
  }

  /**
   * Create a Primitive representing a line.
   */
  static constexpr Primitive line(const ScalarType& x, const ScalarType& y, const ScalarType& z) {
    return Primitive{x * e23_ + y * e31_ + z * e12_};
  }

  /**
   * Create a Primitive representing a plane according to the equation ax + by + cz + d = 0.
   */
  static constexpr Primitive plane(const ScalarType& a, const ScalarType& b, const ScalarType& c,
                                   const ScalarType& d) {
    return Primitive{a * e1_ + b * e2_ + c * e3_ + d * e0_};
  }

  static constexpr Primitive x_axis() { return line(1, 0, 0); }
  static constexpr Primitive y_axis() { return line(0, 1, 0); }
  static constexpr Primitive z_axis() { return line(0, 0, 1); }

  /**
   * Create a Primitive representing the point at the origin.
   */
  static constexpr Primitive origin() { return Primitive{e123_}; }

  /**
   * Create a Primitive representing a zero k-vector.
   */
  static constexpr Primitive zero() { return {}; }

  static constexpr Primitive join(const Primitive& p1, const Primitive& p2) {
    return Primitive{p1.vec_ & p2.vec_};
  }

  static constexpr Primitive meet(const Primitive& p1, const Primitive& p2) {
    return Primitive{p1.vec_ ^ p2.vec_};
  }

  static constexpr bool is_parallel(const Primitive& p1, const Primitive& p2) {
    const auto projected{p1.vec_.inner(p2.vec_)};
    return square_magnitude(projected) == 1;
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

template <typename ScalarType>
constexpr Primitive<ScalarType> operator+(const ScalarType& a,
                                          const Primitive<ScalarType>& primitive) {
  return primitive + a;
}

template <typename ScalarType>
constexpr Primitive<ScalarType> operator-(const ScalarType& a,
                                          const Primitive<ScalarType>& primitive) {
  return -primitive + a;
}

template <typename ScalarType>
constexpr Primitive<ScalarType> operator*(const ScalarType& a,
                                          const Primitive<ScalarType>& primitive) {
  return primitive * a;
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

  using AlgebraType = typename PrimitiveType::AlgebraType;

 private:
  using VectorType = typename AlgebraType::VectorType;

  // This creates the identity transformation by default.
  VectorType vec_{1.f};

  constexpr Transform() = default;
  explicit constexpr Transform(const VectorType& v) : vec_(v / abs(v)) {}

 public:
  constexpr Transform(const Transform& rhs) = default;
  constexpr Transform(Transform&& rhs) = default;

  constexpr Transform& operator=(const Transform& rhs) = default;
  constexpr Transform& operator=(Transform&& rhs) = default;

  constexpr bool operator==(const Transform& rhs) const { return vec_ == rhs.vec_; }
  constexpr bool operator!=(const Transform& rhs) const { return vec_ != rhs.vec_; }

  constexpr const VectorType& raw_vector() const { return vec_; }

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
  static constexpr Transform rotate(const PrimitiveType& axis, const ScalarType& angle) {
    using std::cos;
    using std::sin;
    return Transform{cos(angle / 2) - sin(angle / 2) * axis.normalized().vec_};
  }

  /**
   * Create a Transform that is a translation in the direction by a certain distance.
   */
  static constexpr Transform translate(const PrimitiveType& direction, const ScalarType& distance) {
    return Transform{static_cast<ScalarType>(1) -
                     distance / 2 * PrimitiveType::e0_ * direction.normalized().vec_};
  }
};

template <typename ScalarType>
std::string to_string(const Transform<ScalarType>& p) {
  using std::to_string;
  return to_string(p.raw_vector());
}

template <typename ScalarType>
std::ostream& operator<<(std::ostream& os, const Transform<ScalarType>& p) {
  using std::to_string;
  os << to_string(p);
  return os;
}

}  // namespace ndyn::math
