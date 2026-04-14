#pragma once

#include <cstddef>
#include <cstdint>

namespace ndyn::math {

template <typename AlgebraT>
class Multivector;

template <typename ScalarT, size_t POSITIVE_BASES, size_t NEGATIVE_BASES, size_t ZERO_BASES>
class Algebra final {
 private:
  // The Algebra class is meant to be a collection of types and properties describing the algebra.
  // It is not instantiable.
  Algebra() = delete;

 public:
  using ScalarType = ScalarT;
  using VectorType = Multivector<Algebra>;

  static constexpr size_t NUM_POSITIVE_BASES{POSITIVE_BASES};
  static constexpr size_t NUM_NEGATIVE_BASES{NEGATIVE_BASES};
  static constexpr size_t NUM_ZERO_BASES{ZERO_BASES};

  static constexpr size_t NUM_BASIS_VECTORS{NUM_POSITIVE_BASES + NUM_NEGATIVE_BASES +
                                            NUM_ZERO_BASES};
  static constexpr size_t NUM_BASIS_BLADES{1UL << NUM_BASIS_VECTORS};

  static constexpr ScalarType EPSILON{1e-6};
};

using DefaultScalarType = float;

// Common algebras.
template <typename T = DefaultScalarType>
using Scalar = Algebra<T, 0, 0, 0>;

template <typename T = DefaultScalarType>
using Complex = Algebra<T, 0, 1, 0>;

template <typename T = DefaultScalarType>
using Dual = Algebra<T, 0, 0, 1>;

template <typename T = DefaultScalarType>
using SplitComplex = Algebra<T, 1, 0, 0>;

// VGA 2D is a standard ("vanilla") 2D vectorspace geometric algebra. It is used in non-relativistic
// physics and engineering applications.
template <typename T = DefaultScalarType>
using Vga2d = Algebra<T, 2, 0, 0>;

// VGA is a standard ("vanilla") 3D vectorspace geometric algebra, also known as 3D Euclidean
// geometric algebra. It is used in non-relativistic physics and engineering applications.
template <typename T = DefaultScalarType>
using Vga = Algebra<T, 3, 0, 0>;

// PGA 2D is a 2D vectorspace geometric algebra with an additional zero dimension. It is used in
// computer graphics, non-relativistic physics, and engineering applications.
template <typename T = DefaultScalarType>
using Pga2d = Algebra<T, 2, 0, 1>;

// PGA is a 3D vectorspace geometric algebra with an additional zero dimension. It is used in
// computer graphics, non-relativistic physics, and engineering applications.
template <typename T = DefaultScalarType>
using Pga = Algebra<T, 3, 0, 1>;

// CGA is a 3D vectorspace geometric algebra with two additional dimensions. It can model lines that
// do not pass through the origin as well as translations.
template <typename T = DefaultScalarType>
using Cga = Algebra<T, 4, 1, 0>;

// TCGA, time-extended CGA, is a 4D vectorspace (presumably, time plus 3 spatial dimensions)
// geometric algebra with two additional dimensions. It can model lines that do not pass through the
// origin as well as translations.
template <typename T = DefaultScalarType>
using Tcga = Algebra<T, 5, 1, 0>;

// The spacetime algebra is primarily used in relativistic physics applications and research.
// Note that we assume Cl(1, 3) as the spacetime algebra here, rather than
// Cl(3, 1).
template <typename T = DefaultScalarType>
using Spacetime = Algebra<T, 1, 3, 0>;

// The other expression of the spacetime algebra.
template <typename T = DefaultScalarType>
using SpacetimePrime = Algebra<T, 3, 1, 0>;

// The conformal spacetime algebra is a conformal form of STA. It includes the two conformal
// dimensions to model rotations and translations in the STA.
template <typename T = DefaultScalarType>
using Csta = Algebra<T, 2, 4, 0>;

}  // namespace ndyn::math
