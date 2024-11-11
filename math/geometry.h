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

/**
 * Primitives represent points, lines, and planes as well as the transforms of these.
 */
template <typename AlgebraT>
class Geometry3D final {
 public:
  using AlgebraType = AlgebraT;
  static_assert(
      AlgebraType::POSITIVE_BASES >= 3,
      "Geometry3D requires a 3 or more dimensional algebra with at least one zero basis.");
  static_assert(
      AlgebraType::ZERO_BASES >= 1,
      "Geometry3D requires a 3 or more dimensional algebra with at least one zero basis.");

  using ScalarType = typename AlgebraType::ScalarType;
  using VectorType = typename AlgebraType::VectorType;

  static constexpr VectorType one{VectorType{1}};
  static constexpr VectorType zero{VectorType{}};

  static constexpr VectorType e0{VectorType::template e<3>()};
  static constexpr VectorType e1{VectorType::template e<0>()};
  static constexpr VectorType e2{VectorType::template e<1>()};
  static constexpr VectorType e3{VectorType::template e<2>()};

  static constexpr VectorType e01{e0 * e1};
  static constexpr VectorType e02{e0 * e2};
  static constexpr VectorType e03{e0 * e3};
  static constexpr VectorType e12{e1 * e2};
  static constexpr VectorType e13{e1 * e3};
  static constexpr VectorType e23{e2 * e3};

  // Special basis combination to help represent lines.
  static constexpr VectorType e31{e3 * e1};

  static constexpr VectorType e012{e0 * e1 * e2};
  static constexpr VectorType e013{e0 * e1 * e3};
  static constexpr VectorType e023{e0 * e2 * e3};
  static constexpr VectorType e123{e1 * e2 * e3};

  // Special basis combinations to help represent points.
  static constexpr VectorType e021{e0 * e2 * e1};
  static constexpr VectorType e031{e0 * e3 * e1};
  static constexpr VectorType e032{e0 * e3 * e2};

  static constexpr VectorType e0123{e0 * e1 * e2 * e3};

  static constexpr bool is_point(const VectorType& v) {
    constexpr std::array<size_t, 4> allowed_bases{7 /*e021*/, 11 /*e013*/, 13 /*e032*/,
                                                  14 /*e123*/};
    for (size_t i = 0; i < AlgebraType::bases_count(); ++i) {
      const bool allowed{std::find(allowed_bases.begin(), allowed_bases.end(), i) !=
                         allowed_bases.end()};
      if (!allowed && v.basis(i) != 0) {
        return false;
      }
    }
    return true;
  }

  static constexpr bool is_join_line(const VectorType& v) {
    // TODO(james): Better understand join and meet lines and update this method and the
    // is_meet_line() method.
    constexpr std::array<size_t, 6> allowed_bases{3, 5, 6, 9, 10, 12};
    for (size_t i = 0; i < AlgebraType::bases_count(); ++i) {
      if (v.basis(i) != 0) {
        const bool allowed{std::find(allowed_bases.begin(), allowed_bases.end(), i) !=
                           allowed_bases.end()};
        if (!allowed) {
          LOG(INFO) << "Non-join line coefficient is on basis i: " << i;
          return false;
        }
      }
    }
    return true;
  }

  static constexpr bool is_meet_line(const VectorType& v) {
    constexpr std::array<size_t, 6> allowed_bases{3, 5, 6, 9, 10, 12};
    for (size_t i = 0; i < AlgebraType::bases_count(); ++i) {
      const bool allowed{std::find(allowed_bases.begin(), allowed_bases.end(), i) !=
                         allowed_bases.end()};
      if (!allowed && v.basis(i) != 0) {
        return false;
      }
    }
    return true;
  }

  static constexpr bool is_plane(const VectorType& v) {
    constexpr std::array<size_t, 4> allowed_bases{1 /*e0*/, 2 /*e1*/, 4 /*e2*/, 8 /*e3*/};
    for (size_t i = 0; i < AlgebraType::bases_count(); ++i) {
      const bool allowed{std::find(allowed_bases.begin(), allowed_bases.end(), i) !=
                         allowed_bases.end()};
      if (!allowed && v.basis(i) != 0) {
        return false;
      }
    }
    return true;
  }

  /**
   * Create a vector representing a point specified in Cartesian coordinates.
   *
   * This combination of basis vectors can be derived by implementing this method as:
   *    return Primitive{x * e1 + y * e2 + z * e3 + e0}.dual();
   * The implementation below uses a slightly more optimized form of this expression.
   *
   * For more details, see
   * https://geometry.mrao.cam.ac.uk/2020/06/euclidean-geometry-and-geometric-algebra/
   * or
   * https://youtu.be/v-WG02ILMXA?t=636
   */
  static constexpr VectorType point(const ScalarType& x, const ScalarType& y, const ScalarType& z) {
    return x * e032 + y * e013 + z * e021 + e123;
  }

  /**
   * Create a vector representing a line where two planes meet.
   */
  static constexpr VectorType meet_line(const ScalarType& x, const ScalarType& y,
                                        const ScalarType& z) {
    return x * e23 + y * e31 + z * e12;
  }

  /**
   * Create a vector representing a plane according to the equation ax + by + cz + d = 0.
   */
  static constexpr VectorType plane(const ScalarType& a, const ScalarType& b, const ScalarType& c,
                                    const ScalarType& d) {
    return a * e1 + b * e2 + c * e3 + d * e0;
  }

  static constexpr VectorType origin{e123};
  static constexpr VectorType x_axis{meet_line(1, 0, 0)};
  static constexpr VectorType y_axis{meet_line(0, 1, 0)};
  static constexpr VectorType z_axis{meet_line(0, 0, 1)};

  static constexpr VectorType join(const VectorType& v1, const VectorType& v2) { return v1 & v2; }

  static constexpr VectorType meet(const VectorType& v1, const VectorType& v2) { return v1 ^ v2; }

  static constexpr bool is_parallel(const VectorType& v1, const VectorType& v2) {
    const auto projected{v1.inner(v2)};
    return square_magnitude(projected) == 1;
  }

  /**
   * The methods below generate versors (also called motors) that can reflect, rotate, and translate
   * primitives. Applying a versor involves sandwiching the primitive in between the versor and its
   * reverse.
   *
   * For more information, see https://en.wikipedia.org/wiki/Geometric_algebra#Versor
   */

  /**
   * Create a versor that represents an identity transform.
   */
  static constexpr VectorType identity_transform{1};

  /**
   * Create a versor that is a reflection across the vector v.
   */
  static constexpr VectorType reflect(const VectorType& v);

  /**
   * Create a versor that is a rotation about the axis by a certain angle. Note that the axis
   * does not need to be a line but may be any Primitive.
   */
  static constexpr VectorType rotate(const VectorType& axis, const ScalarType& angle) {
    using std::cos;
    using std::sin;
    return cos(angle / 2) - sin(angle / 2) * axis.normalized();
  }

  /**
   * Create a versor that is a translation in the direction by a certain distance.
   */
  static constexpr VectorType translate(const VectorType& direction, const ScalarType& distance) {
    return static_cast<ScalarType>(1) - distance / 2 * e0 * direction.normalized();
  }

  /**
   * Standard sandwich product to apply a transform to another vector.
   */
  static constexpr VectorType apply(const VectorType& transform, const VectorType& v) {
    return transform * v * ~transform;
  }
};

}  // namespace ndyn::math
