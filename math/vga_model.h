#pragma once

#include <cmath>

#include "base/except.h"
#include "math/algebra.h"
#include "math/geometry_model.h"
#include "math/multivector.h"

namespace ndyn::math {

// -------------------------------------------------------------------------
// VGAModel — points as grade-1 vectors
//
// Basis convention:
//   e1, e2, e3          — points / position vectors        (grade 1)
//   e12, e13, e23       — lines through the origin         (grade 2)
//   e123                — pseudoscalar                     (grade 3)
//
// The outer product is the natural join operation for constructing higher-grade
// elements from points: two points span a line, three points span a plane.
// Only rotations about axes through the origin are representable as versors.
// -------------------------------------------------------------------------

/**
 * Euclidean 3D geometry model with points embedded as grade-1 vectors.
 *
 * This is the natural unextended embedding of E³ into G(3,0,0). The outer product
 * constructs lines and planes directly from points. Only rotations about axes through
 * the origin are representable as versors; for rigid body motions including translation
 * use PGA G(3,0,1) instead.
 */
template <typename ScalarType>
class VGAModelGrade1 final {
 public:
  using Algebra = Algebra<ScalarType, 3, 0, 0>;
  using Scalar = ScalarType;
  using Multivector = Algebra::VectorType;

  /**
   * Constructs a point as a grade-1 vector x*e1 + y*e2 + z*e3.
   */
  Multivector point(Scalar x, Scalar y, Scalar z) const {
    return x * Multivector::template e<0>() + y * Multivector::template e<1>() +
           z * Multivector::template e<2>();
  }

  /**
   * In VGA all grade-1 vectors are indistinguishable from directions — there is
   * no homogeneous coordinate to set to zero. This method is provided for concept
   * conformance; it is identical to point() and the distinction carries no
   * geometric meaning in this model.
   */
  Multivector ideal_point(Scalar x, Scalar y, Scalar z) const { return point(x, y, z); }

  /**
   * Constructs the line through two points as their outer product, yielding a
   * grade-2 bivector. The result represents a line through the origin only; the
   * moment (positional) component present in PGA lines does not exist here.
   */
  Multivector line(const Multivector& p1, const Multivector& p2) const { return p1.outer(p2); }

  /**
   * Constructs the plane through three points as their outer product, yielding
   * a grade-3 trivector (a scalar multiple of the pseudoscalar). In G(3,0,0)
   * all planes pass through the origin, so this encodes only the plane's normal.
   */
  Multivector plane(const Multivector& p1, const Multivector& p2, const Multivector& p3) const {
    return p1.outer(p2).outer(p3);
  }

  /**
   * Constructs a rotor encoding rotation by theta (radians) about the unit
   * bivector axis B. B must be a unit bivector (B * ~B = 1). The rotor encodes
   * rotation about a line through the origin only.
   */
  Multivector rotor(const Multivector& B, Scalar theta) const {
    return Multivector::scalar(std::cos(theta / Scalar{2})) -
           Multivector::scalar(std::sin(theta / Scalar{2})) * B;
  }

  /**
   * Translations are not representable as versors in G(3,0,0). Use PGA G(3,0,1)
   * for rigid body motions that include translation.
   */
  Multivector translator(const Multivector&) const {
    except<std::logic_error>("VGA G(3,0,0) does not support translation as a versor.");
  }

  /**
   * Motors are not meaningful in G(3,0,0) since translation is not representable
   * as a versor. Use PGA G(3,0,1) for rigid body motors.
   */
  Multivector motor(const Multivector&, const Multivector&) const {
    except<std::logic_error>("VGA G(3,0,0) does not support motors.");
  }

  /**
   * Applies a general versor V to multivector X via V * X * V⁻¹.
   */
  Multivector apply(const Multivector& V, const Multivector& X) const {
    return V * X * V.inverse();
  }

  /**
   * Returns the join (span) of two primitives via the outer product. For two
   * grade-1 points this yields the line through them; for three points the
   * plane through them.
   */
  Multivector join(const Multivector& A, const Multivector& B) const { return A.outer(B); }

  /**
   * Returns the meet (intersection) of two primitives via the regressive product.
   * For two grade-2 lines through the origin this yields their point of
   * intersection (also through the origin).
   */
  Multivector meet(const Multivector& A, const Multivector& B) const { return A.regress(B); }
};

// -------------------------------------------------------------------------
// VGAModelGrade3 — points as grade-3 trivectors (dual embedding)
//
// Basis convention:
//   e123                — scalar multiple; encodes a point  (grade 3)
//   e12, e13, e23       — lines through the origin         (grade 2)
//   e1, e2, e3          — plane normals                    (grade 1)
//
// This is the Hodge-dual picture of VGAModelGrade1. Points are trivectors
// (dual vectors), and meet is the natural operation for constructing lower-
// grade elements from points. The roles of outer and regressive products are
// exchanged relative to VGAModelGrade1.
// -------------------------------------------------------------------------

/**
 * Euclidean 3D geometry model with points embedded as grade-3 trivectors.
 *
 * This is the Hodge-dual embedding of E³ into G(3,0,0), obtained by applying the
 * dual map to the grade-1 model. Meet is the natural operation for constructing
 * lines and planes from points. As in VGAModelGrade1, only rotations about axes
 * through the origin are representable as versors, and translations do not exist.
 */
class VGAModelGrade3 final {
 public:
  using Algebra = VGAAlgebra;
  using Scalar = typename VGAAlgebra::ScalarType;
  using Multivector = typename VGAAlgebra::VectorType;

  /**
   * Constructs a point as a grade-3 trivector, the dual of the grade-1 position
   * vector x*e1 + y*e2 + z*e3. The result is x*e23 - y*e13 + z*e12, with signs
   * determined by the duality convention of G(3,0,0).
   */
  constexpr Multivector point(Scalar x, Scalar y, Scalar z) const {
    return (x * Multivector::template e<0>() + y * Multivector::template e<1>() +
            z * Multivector::template e<2>())
        .dual();
  }

  /**
   * In G(3,0,0) ideal points are not distinguishable from finite points — there
   * is no homogeneous coordinate. Provided for concept conformance; identical
   * to point() in this model.
   */
  constexpr Multivector ideal_point(Scalar x, Scalar y, Scalar z) const { return point(x, y, z); }

  /**
   * Constructs the line through two grade-3 points via the regressive product,
   * yielding a grade-2 bivector. This is the dual counterpart of the outer
   * product used in VGAModelGrade1.
   */
  constexpr Multivector line(const Multivector& p1, const Multivector& p2) const {
    return p1.regress(p2);
  }

  /**
   * Constructs the plane through three grade-3 points via successive regressive
   * products, yielding a grade-1 vector encoding the plane normal.
   */
  constexpr Multivector plane(const Multivector& p1, const Multivector& p2,
                              const Multivector& p3) const {
    return p1.regress(p2).regress(p3);
  }

  /**
   * Constructs a rotor encoding rotation by theta (radians) about the unit
   * bivector axis B. Identical in form to VGAModelGrade1 — rotors live in the
   * even subalgebra regardless of the point embedding convention.
   */
  constexpr Multivector rotor(const Multivector& B, Scalar theta) const {
    return Multivector::scalar(std::cos(theta / Scalar{2})) -
           Multivector::scalar(std::sin(theta / Scalar{2})) * B;
  }

  /**
   * Translations are not representable as versors in G(3,0,0). Use PGA G(3,0,1)
   * for rigid body motions that include translation.
   */
  constexpr Multivector translator(const Multivector&) const {
    except<std::logic_error>("VGA G(3,0,0) does not support translation as a versor.");
  }

  /**
   * Motors are not meaningful in G(3,0,0) since translation is not representable
   * as a versor. Use PGA G(3,0,1) for rigid body motors.
   */
  constexpr Multivector motor(const Multivector&, const Multivector&) const {
    except<std::logic_error>("VGA G(3,0,0) does not support motors.");
  }

  /**
   * Applies a general versor V to multivector X via V * X * V⁻¹. Versors act
   * identically on the algebra regardless of the point embedding convention.
   */
  constexpr Multivector apply(const Multivector& V, const Multivector& X) const {
    return V * X * V.inverse();
  }

  /**
   * Returns the join (span) of two primitives. In the dual embedding the
   * regressive product constructs lower-grade elements from points, so join
   * between two grade-1 plane normals uses the outer product to span upward.
   */
  constexpr Multivector join(const Multivector& A, const Multivector& B) const {
    return A.outer(B);
  }

  /**
   * Returns the meet (intersection) of two primitives via the regressive product.
   * In the dual embedding this is the natural downward construction: two grade-3
   * points meet at a grade-2 line, two grade-2 lines meet at a grade-1 plane normal.
   */
  constexpr Multivector meet(const Multivector& A, const Multivector& B) const {
    return A.regress(B);
  }
};

static_assert(GeometryModel<VGAModelGrade1>,
              "VGAModelGrade1 does not satisfy the GeometryModel concept.");
static_assert(GeometryModel<VGAModelGrade3>,
              "VGAModelGrade3 does not satisfy the GeometryModel concept.");

}  // namespace ndyn::math
