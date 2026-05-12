#pragma once

#include "math/conformal_geometry.h"
#include "math/geometry_model.h"
#include "math/vector_space_geometry.h"

namespace ndyn::math {

template <typename Scalar = DefaultScalarType>
using Vga2dGeometry = VectorSpaceGeometryType<2, Scalar>;

template <typename Scalar = DefaultScalarType>
using Vga3dGeometry = VectorSpaceGeometryType<3, Scalar>;

template <typename Scalar = DefaultScalarType>
using Vga4dGeometry = VectorSpaceGeometryType<4, Scalar>;

template <typename Scalar = DefaultScalarType>
using Cga2dGeometry = ConformalGeometryType<Vga2dGeometry<Scalar>, Scalar>;
static_assert(std::is_same_v<Cga2dGeometry<>::Scalar, Vga2dGeometry<>::Scalar>);

template <typename Scalar = DefaultScalarType>
using Cga3dGeometry = ConformalGeometryType<Vga3dGeometry<Scalar>, Scalar>;

template <typename Scalar = DefaultScalarType>
using Cga4dGeometry = ConformalGeometryType<Vga4dGeometry<Scalar>, Scalar>;

static_assert(HasGenericBases<Vga2dGeometry<>>);
static_assert(HasGenericBases<Vga3dGeometry<>>);
static_assert(HasGenericBases<Vga4dGeometry<>>);

static_assert(VectorSpaceGeometryModel<Vga2dGeometry<>>);
static_assert(VectorSpaceGeometryModel<Vga3dGeometry<>>);
static_assert(VectorSpaceGeometryModel<Vga4dGeometry<>>);

static_assert(HasGenericBases<Cga2dGeometry<>>);
static_assert(HasGenericBases<Cga3dGeometry<>>);
static_assert(HasGenericBases<Cga4dGeometry<>>);

static_assert(ConformalGeometryModel<Cga2dGeometry<>>);
static_assert(ConformalGeometryModel<Cga3dGeometry<>>);
static_assert(ConformalGeometryModel<Cga4dGeometry<>>);

}  // namespace ndyn::math
