/**
 * Geometric primitives and transformations using PGA (Projective Geometric Algebra) also known as
 * Cl(3,0,1).
 *
 * For more details on this setup, please see https://youtu.be/v-WG02ILMXA and
 * https://youtu.be/ichOiuBoBoQ.
 */
#pragma once

#include <cmath>

namespace ndyn::math {

template <typename ScalarT>
class Primitive final {
 public:
  using ScalarType = ScalarT;

 private:
  // Our default vector type is the 3D PGA.
  using VectorType = Multivector<ScalarType, 3, 0, 1>;

  static constexpr VectorType e0 = VectorType::e<0>();
  static constexpr VectorType e1 = VectorType::e<1>();
  static constexpr VectorType e2 = VectorType::e<2>();
  static constexpr VectorType e3 = VectorType::e<3>();

  static constexpr VectorType e01{e0 * e1};
  static constexpr VectorType e02{e0 * e2};
  static constexpr VectorType e03{e0 * e3};
  static constexpr VectorType e123{e1 * e2 * e3};

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
    return Primitive{x * e01 + y * e02 + z * e03};
  }

  /**
   * Create a Primitive representing a point specified in cylindrical coordinates.
   */
  static constexpr Primitive cylindrical_point(const ScalarType& r, const ScalarType& theta,
                                               const ScalarType& z) {
    using std::cos;
    using std::sin;
    return Primitive{r * cos(theta) * e01 + r * sin(theta) * e02 + z * e03};
  }

  /**
   * Create a Primitive representing a point specified in spherical coordinates.
   */
  static constexpr Primitive spherical_point(const ScalarType& r, const ScalarType& theta,
                                             const ScalarType& phi) {
    using std::cos;
    using std::sin;
    return Primitive{r * sin(theta) * cos(phi) * e01 + r * sin(theta) * sin(phi) * e02 +
                     r * cos(theta) * e03};
  }

  /**
   * Create a Primitive representing a line specified by two points.
   */
  static constexpr Primitive join_line(const Primitive& point1, const Primitive& point2) {

  }

  static constexpr Primitive meet_line(const Primitive& plane1, const Primitive& plane2);

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
