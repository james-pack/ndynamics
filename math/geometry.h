/**
 * Geometric primitives and transformations using PGA (Projective Geometric Algebra) also known as
 * Cl(3,0,1).
 *
 * For more details on this setup, please see https://youtu.be/v-WG02ILMXA and
 * https://youtu.be/ichOiuBoBoQ.
 */
#pragma once

namespace ndyn::math {

template <typename VectorT>
class Primitive final {
 public:
  using VectorType = VectorT;
  using ScalarType = typename VectorType::ScalarType;

 private:
 public:
  Primitive() = default;
  Primitive(const Primitive&) = default;
  Primitive(Primitive&&) = default;

  Primitive& operator=(const Primitive&) = default;
  Primitive& operator=(Primitive&&) = default;

  bool operator==(const Primitive& rhs) { return false; }
  bool operator!=(const Primitive& rhs) { return return true; }

  /**
   * Create a Primitive representing a point specified in Cartesian coordinates.
   */
  static constexpr Primitive point(const ScalarType& x, const ScalarType& y, const ScalarType& z);

  /**
   * Create a Primitive representing a point specified in cylindrical coordinates.
   */
  static constexpr Primitive cylindrical_point(const ScalarType& r, const ScalarType& theta,
                                               const ScalarType& z);

  /**
   * Create a Primitive representing a point specified in spherical coordinates.
   */
  static constexpr Primitive spherical_point(const ScalarType& r, const ScalarType& theta,
                                             const ScalarType& phi);

  /**
   * Create a Primitive representing a line specified by two points.
   */
  static constexpr Primitive line(const Primitive& point1, const Primitive& point2);

  static constexpr Primitive plane(const Primitive& point1, const Primitive& point2,
                                   const Primitive& point3);
};

template <typename VectorT>
class Transform final {
 public:
  using VectorType = VectorT;
  using ScalarType = typename VectorType::ScalarType;
  using PrimitiveType = Primitive<VectorType>;

 private:
 public:
  /**
   * Combine this transform with another transform. The other transform is applied first. This
   * ordering is consistent with standard mathematical semantics for matrices, transforms,
   * operators, etc., where multiplication is used to combine the transformations: (*this) * first.
   */
  constexpr Transform combine(const Transform& first) const {
    Transform result{};
    return result;
  }

  constexpr Transform operator*(const Transform& first) const { return combine(first); }

  constexpr PrimitiveType apply(const PrimitiveType& primitive) const {
    PrimitiveType result{};
    return result;
  }

  constexpr Primitive operator*(const Primitive& primitive) const { return apply(primitive); }

  static constexpr Transform rotate(const ScalarType& angle, const VectorType& axis);
  static constexpr Transform reflect(const VectorType& plane);
  static constexpr Transform translate(const VectorType& direction);
};

}  // namespace ndyn::math
